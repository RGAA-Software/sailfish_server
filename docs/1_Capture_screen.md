#### 1. See [Here](https://learn.microsoft.com/en-us/windows/win32/direct3ddxgi/desktop-dup-api#updating-the-desktop-image-data) first, the official document of DDA

#### 2. There are 3 steps to do this
##### (Attention : If you want to encode with NVENC(Nvidia) or AMF(AMD), ignore converting to yuv)
- Init resources
- Capture RGBA/BGRA... image buffer -> Convert to yuv format 
- Exit

##### 2.1 Init resources, see comments in code
- See  src/capture/DDACapture.cpp
```c++
bool DDACapture::Init() {
   ...
    // 1. Create device[创建设备]
    for (size_t i = 0; i < num_drivers; i++) {
        hr = D3D11CreateDevice(nullptr, driver_types[i], nullptr, 0, feature_levels, (UINT) num_feature_levels,
                               D3D11_SDK_VERSION, &d3d_device, &feature_level, &d3d_device_context);
        if (SUCCEEDED(hr))
            break;
    }
    IDXGIDevice* dxgi_device = nullptr;
    hr = d3d_device->QueryInterface(__uuidof(IDXGIDevice), (void**) &dxgi_device);
    
    // 2. Get IDXGIAdapter => represent a GPU [代表一个显卡]
    IDXGIAdapter* dxgi_adapter = nullptr;
    hr = dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**) &dxgi_adapter);
    
    // 3. Enum monitor IDXGIOutput => represent a monitor/screen [代表一个屏幕]
    IDXGIOutput* output = nullptr;
    std::vector<IDXGIOutput*> total_outputs;
    while(dxgi_adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND) {
        if (!output) {
            continue;
        }
        total_outputs.push_back(output);
        ++i;
    }
    
    // 4. Get DuplicateOutput => for capturing image ! [下一步获取图像用它]
    for (const auto op : total_outputs) {
        IDXGIOutput1* dxgi_output = nullptr;
        hr = op->QueryInterface(__uuidof(dxgi_output), (void**) &dxgi_output);
        op->Release();
        // Create desktop duplication
        hr = dxgi_output->DuplicateOutput(d3d_device, &output_dup->duplication_);
        
    }
    return true;
}
```

##### 2.2 Capture RGBA/BGRA... image -> (Convert to yuv if needed)
- see src/capture/DDACapture.cpp
```c++
int DDACapture::CaptureNextFrameInternal(const std::shared_ptr<OutputDuplication>& out_dup, int timeout) {
    ...
    
    // 1. Try to obtain a frame
    ID3D11Texture2D* gpu_side_texture = nullptr;
    IDXGIResource* desk_res = nullptr;
    DXGI_OUTDUPL_FRAME_INFO frameInfo;
    hr = dxgi_dup->AcquireNextFrame(timeout, &frameInfo, &desk_res);
    if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
        // !!! IMPORTANT !!! => represent that the image in screen is motionless, your can use previous frame
        // 屏幕上的图像没变化，可以使用上一帧画面
    }
    
    // 2. Get the frame texture
    hr = desk_res->QueryInterface(__uuidof(ID3D11Texture2D), (void **) &gpu_side_texture);

    // 3. !!! Copy frame from GPU => CPU
    d3d_device_context->CopyResource(cpu_side_textures_[out_dup->dup_index_], gpu_side_texture);

    // 4. Read the frame to memory
    hr = d3d_device_context->Map(cpu_side_textures_[out_dup->dup_index_], 0, D3D11_MAP_READ, 0, &sr);
    
    // 5. Convert to I420(for FFmpeg encoding)
    std::vector<uint8_t> yuv_frame_data;
    yuv_frame_data.resize(4 * width * height);
    size_t pixel_size = width * height;

    const int uv_stride = width >> 1;
    uint8_t *y = yuv_frame_data.data();
    uint8_t *u = y + pixel_size;
    uint8_t *v = u + (pixel_size >> 2);

    if (DXGI_FORMAT_B8G8R8A8_UNORM == desc.Format) {
        libyuv::ARGBToI420((uint8_t *) sr.pData, sr.RowPitch, y, width, u, uv_stride, v, uv_stride, width, height);
    } else if (DXGI_FORMAT_R8G8B8A8_UNORM == desc.Format) {
        libyuv::ABGRToI420((uint8_t *) sr.pData, sr.RowPitch, y, width, u, uv_stride, v, uv_stride, width, height);
    } else {
        libyuv::ARGBToI420((uint8_t *) sr.pData, sr.RowPitch, y, width, u, uv_stride, v, uv_stride, width, height);
    }

    ...
    return 0;
}

```

##### 2.3 Release resources
```c++
void DDACapture::Exit() {
    Capture::Exit();

    for (auto& [_idx, tex] : cached_textures_) {
        if (tex) {
            tex->Release();
        }
    }

    for (auto& dup : output_duplications_) {
        if (dup->duplication_) {
            dup->duplication_->Release();
        }
    }

    if (d3d_device_context) {
        d3d_device_context->Release();
    }
    if (d3d_device) {
        d3d_device->Release();
    }
    for (auto& [_, texture] : cpu_side_textures_) {
        texture->Release();
    }

}
```