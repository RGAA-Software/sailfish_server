#include "GraphicsCapture.h"
#include <iostream>
#include <combaseapi.h>
#include <atlbase.h>
#include <fstream>
#include <iostream>

#include <libyuv.h>

#include "pch.h"
#include "rgaa_common/RData.h"
#include "rgaa_common/RTime.h"
#include "rgaa_common/RLog.h"
#include "rgaa_common/RCloser.h"
#include "CursorCapture.h"
#include "Monitor.h"
#include "messages.pb.h"
#include "MonitorList.h"
#include "CapturedFrame.h"

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Windows::Foundation::Numerics;
    using namespace Windows::Graphics;
    using namespace Windows::Graphics::Capture;
    using namespace Windows::Graphics::DirectX;
    using namespace Windows::Graphics::DirectX::Direct3D11;
    using namespace Windows::System;
}

namespace util
{
	using namespace robmikh::common::desktop;
	using namespace robmikh::common::uwp;
}

namespace rgaa {

    bool GraphicsCapture::IsGraphicsCaptureSupported() {
        auto isCaptureSupported = winrt::Windows::Graphics::Capture::GraphicsCaptureSession::IsSupported();
        if (!isCaptureSupported) {
            LOG_ERROR("Graphics API to capture NOT supported.");
            return false;
        }
        return true;
    }

    GraphicsCapture::GraphicsCapture(const std::shared_ptr<Context> &ctx, const CaptureResultType &crt) : Capture(ctx,
                                                                                                                  crt) {
        //void* handle, const std::string& name
    //    if (type == WindowCaptureType::kWindow) {
    //        HWND window = (HWND)handle;
    //        m_item = util::CreateCaptureItemForWindow(window);
    //    }
    //    else if (type == WindowCaptureType::kMonitor) {
    //        HMONITOR monitor = (HMONITOR)handle;
    //        m_item = util::CreateCaptureItemForMonitor(monitor);
    //    }

        auto monitor_list = MonitorList::Make();
        auto monitors = monitor_list->GetCurrentMonitors();
        MonitorInfo info;
        for (auto &mi: monitors) {
            //if (capture_name == name) {
            info.DisplayName = mi.DisplayName;
            info.MonitorHandle = mi.MonitorHandle;
            break;
            //}
        }

//        if (!info.MonitorHandle) {
//            LOGE("Not find monitor for : {}", "FIRST");
//            return;
//        }

        //
//        info.MonitorHandle = nullptr;

        HMONITOR monitor = (HMONITOR)info.MonitorHandle;
        m_item = util::CreateCaptureItemForMonitor(monitor);

    //    capture_monitor = MonitorDetector::Instance()->GetMonitorByName(name);
    //    if (!capture_monitor) {
    //        LOG_ERROR("Can't find screen monitor : %s", name.c_str());
    //    }
    }

    bool GraphicsCapture::Init() {
        auto d3dDevice = util::CreateD3DDevice();
        auto dxgiDevice = d3dDevice.as<IDXGIDevice>();
        m_device = CreateDirect3DDevice(dxgiDevice.get());
        //m_device = device;
        m_pixelFormat = winrt::Windows::Graphics::DirectX::DirectXPixelFormat::R8G8B8A8UIntNormalized;

        //auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(m_device);
        d3dDevice->GetImmediateContext(m_d3dContext.put());

        m_swapChain = util::CreateDXGISwapChain(d3dDevice, static_cast<uint32_t>(m_item.Size().Width),
                                                static_cast<uint32_t>(m_item.Size().Height),
                                                static_cast<DXGI_FORMAT>(m_pixelFormat), 2);

        // Creating our frame pool with 'Create' instead of 'CreateFreeThreaded'
        // means that the frame pool's FrameArrived event is called on the thread
        // the frame pool was created on. This also means that the creating thread
        // must have a DispatcherQueue. If you use this method, it's best not to do
        // it on the UI thread.
        m_framePool = winrt::Direct3D11CaptureFramePool::CreateFreeThreaded(m_device, m_pixelFormat, 2, m_item.Size());
        //m_framePool = winrt::Direct3D11CaptureFramePool::CreateFreeThreaded(m_device, m_pixelFormat, 1, m_item.Size());
        m_session = m_framePool.CreateCaptureSession(m_item);
        m_session.IsCursorCaptureEnabled(false);
        m_lastSize = m_item.Size();
        m_framePool.FrameArrived({this, &GraphicsCapture::OnFrameArrived});

        SetCursorEnabled(true);

        // start
        CheckClosed();
        m_session.StartCapture();

        return true;
    }

    bool GraphicsCapture::CaptureNextFrame() {

        std::this_thread::sleep_for(std::chrono::microseconds(16));

        return false;
    }

    //bool GraphicsCapture::StartCapture() {
    //    CheckClosed();
    //    m_session.StartCapture();
    //    return true;
    //}

    void GraphicsCapture::Exit() {
        auto expected = false;
        if (m_closed.compare_exchange_strong(expected, true)) {
            m_session.Close();
            m_framePool.Close();

            m_swapChain = nullptr;
            m_framePool = nullptr;
            m_session = nullptr;
            m_item = nullptr;
        }
    }

    void GraphicsCapture::ResizeSwapChain() {
        winrt::check_hresult(m_swapChain->ResizeBuffers(2, static_cast<uint32_t>(m_lastSize.Width),
                                                        static_cast<uint32_t>(m_lastSize.Height),
                                                        static_cast<DXGI_FORMAT>(m_pixelFormat), 0));
    }

    bool GraphicsCapture::TryResizeSwapChain(winrt::Direct3D11CaptureFrame const &frame) {
        auto const contentSize = frame.ContentSize();
        if ((contentSize.Width != m_lastSize.Width) ||
            (contentSize.Height != m_lastSize.Height)) {
            // The thing we have been capturing has changed size, resize the swap chain to match.
            m_lastSize = contentSize;
            ResizeSwapChain();
            return true;
        }
        return false;
    }

    bool GraphicsCapture::TryUpdatePixelFormat() {
        auto newFormat = m_pixelFormatUpdate.exchange(std::nullopt);
        if (newFormat.has_value()) {
            auto pixelFormat = newFormat.value();
            if (pixelFormat != m_pixelFormat) {
                m_pixelFormat = pixelFormat;
                ResizeSwapChain();
                return true;
            }
        }
        return false;
    }

    uint64_t last_time = rgaa::GetCurrentTimestamp();
    static int fps = 0;

    void GraphicsCapture::OnFrameArrived(winrt::Direct3D11CaptureFramePool const &sender, winrt::IInspectable const &) {
        auto swapChainResizedToFrame = false;

        fps++;
        uint64_t current_time = rgaa::GetCurrentTimestamp();
        if (current_time - last_time > 1000) {
            printf("FPS : %d \n", fps);
            last_time = current_time;
            fps = 0;
        }

        {
            auto frame = sender.TryGetNextFrame();
            swapChainResizedToFrame = TryResizeSwapChain(frame);

            winrt::com_ptr<ID3D11Texture2D> backBuffer;
            winrt::check_hresult(m_swapChain->GetBuffer(0, winrt::guid_of<ID3D11Texture2D>(), backBuffer.put_void()));
            auto surfaceTexture = GetDXGIInterfaceFromObject<ID3D11Texture2D>(frame.Surface());
            // copy surfaceTexture to backBuffer
            m_d3dContext->CopyResource(backBuffer.get(), surfaceTexture.get());

            if (!shared_texture) {
                HRESULT hr;

                D3D11_TEXTURE2D_DESC old_desc;
                surfaceTexture->GetDesc(&old_desc);

                D3D11_TEXTURE2D_DESC desc = {};
                desc.Width = old_desc.Width;
                desc.Height = old_desc.Height;
                desc.Format = old_desc.Format;
                desc.MipLevels = 1;
                desc.ArraySize = 1;
                desc.SampleDesc.Count = 1;
                desc.Usage = D3D11_USAGE_STAGING;//D3D11_USAGE_DEFAULT;//
                //desc.BindFlags = D3D11_BIND_RENDER_TARGET;// D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
                desc.BindFlags = 0;// D3D11_BIND_SHADER_RESOURCE; //D3D11_BIND_RENDER_TARGET;// D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
                desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;// | D3D11_CPU_ACCESS_WRITE;
                desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;

                printf("width : %d, height : %d, the old format is ; %d , new format : %d\n", desc.Width, desc.Height,
                       old_desc.Format, desc.Format);

                m_d3ddevice = GetDXGIInterfaceFromObject<ID3D11Device>(m_device);
                hr = m_d3ddevice->CreateTexture2D(&desc, nullptr, &shared_texture);
                if (FAILED(hr)) {
                    printf("create_d3d11_stage_surface: failed to create texture 0x%x \n", hr);
                    return;
                }
                LOGI("Create texture success");
            }

            if (shared_texture) {
                m_d3dContext->CopyResource(shared_texture, backBuffer.get());
                D3D11_TEXTURE2D_DESC desc;
                shared_texture->GetDesc(&desc);

                // -- Get Handle --
                HANDLE tex_handle = NULL;
                CComPtr<IDXGIResource> resource = nullptr;
                auto hr = shared_texture->QueryInterface(__uuidof(IDXGIResource), reinterpret_cast<void **>(&resource));
                if (SUCCEEDED(hr)) {
                    hr = resource->GetSharedHandle(&tex_handle);
                    if (FAILED(hr)) {
                        LOGE("GetSharedHandle Failed !!!");
                        return;
                    }
                } else {
                    LOGE("Query resource error : {}", hr);
                }
                if (!tex_handle) {
                    LOGE("Query texture handle failed !");
                    return;
                }

                auto captured_time = GetCurrentTimestamp();

                auto map_cvt_encode_time = RegionTimeCount::Make([](auto count) {
                    //LOGI("Map convert decode time count: {}", count);
                });

                if (capture_result_type_ == CaptureResultType::kRawI420) {
                    D3D11_MAPPED_SUBRESOURCE sr;
                    hr = m_d3dContext->Map(shared_texture, 0, D3D11_MAP_READ, 0, &sr);
                    if (FAILED(hr)) {
                        LOGE("Map texture failed : {0:x}", hr);
                        return;
                    }
                    auto cpu_texture_release = Closer::Make([this]() {
                        m_d3dContext->Unmap(shared_texture, 0);
                    });

                    auto width = (int)desc.Width;
                    auto height = (int)desc.Height;

                    // cursor ?

                    frame_index_++;
                    auto target_yuv_frame_size = (int)(1.5 * width * height);
                    if (!yuv_frame_data_ || yuv_frame_data_->Size() != target_yuv_frame_size) {
                        yuv_frame_data_ = Data::Make(nullptr, target_yuv_frame_size);
                    }

                    size_t pixel_size = width * height;

                    const int uv_stride = width >> 1;
                    auto y = (uint8_t*)yuv_frame_data_->DataAddr();
                    auto u = y + pixel_size;
                    auto v = u + (pixel_size >> 2);

                    if (DXGI_FORMAT_B8G8R8A8_UNORM == desc.Format) {
                        libyuv::ARGBToI420((uint8_t *) sr.pData, sr.RowPitch, y, width, u, uv_stride, v, uv_stride,
                                           width,
                                           height);
                    } else if (DXGI_FORMAT_R8G8B8A8_UNORM == desc.Format) {
                        libyuv::ABGRToI420((uint8_t *) sr.pData, sr.RowPitch, y, width, u, uv_stride, v, uv_stride,
                                           width,
                                           height);
                    } else {
                        libyuv::ARGBToI420((uint8_t *) sr.pData, sr.RowPitch, y, width, u, uv_stride, v, uv_stride,
                                           width,
                                           height);
                    }

                    auto cp_frame = std::make_shared<CapturedFrame>();
                    cp_frame->capture_type_ = CaptureType::kRawData;
                    cp_frame->raw_data_ = yuv_frame_data_;
                    cp_frame->frame_index_ = frame_index_;
                    cp_frame->frame_width_ = desc.Width;
                    cp_frame->frame_height_ = desc.Height;
                    cp_frame->dup_index_ = 0;
                    cp_frame->captured_time_ = captured_time;

                    if (captured_cbk_) {
                        captured_cbk_(cp_frame);
                    }

                }
                else if (capture_result_type_ == CaptureResultType::kTexture2D) {

                }

    //			auto ipc_message = IPCFrameMessage::MakeEmptyMessage();
    //			ipc_message->type = IPCMessageType::kSharedTextureHandle;
    //			ipc_message->sender = IPCMessageSender::kSenderClient;
    //			ipc_message->handle = (uint64_t)tex_handle;
    //			ipc_message->format = desc.Format;
    //			ipc_message->width = desc.Width;
    //			ipc_message->height = desc.Height;
    //			ipc_message->frame_index = frame_index++;
    //            capture_callback(ipc_message);
            }
        }

        DXGI_PRESENT_PARAMETERS presentParameters{};
        m_swapChain->Present1(1, 0, &presentParameters);

        swapChainResizedToFrame = swapChainResizedToFrame || TryUpdatePixelFormat();

        if (swapChainResizedToFrame) {
            m_framePool.Recreate(m_device, m_pixelFormat, 2, m_lastSize);
        }
    }

}