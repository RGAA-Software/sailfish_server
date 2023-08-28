//
// Created by RGAA on 2023/8/10.
//

#include "DDACapture.h"

#ifdef _OS_WINDOWS_

#include <format>
#include <iostream>
#include <fstream>

#include <libyuv.h>

#include "CapturedFrame.h"
#include "settings/Settings.h"
#include "rgaa_common/RData.h"
#include "rgaa_common/RLog.h"
#include "rgaa_common/RCloser.h"
#include "rgaa_common/RTime.h"
#include "context/Context.h"

#include "CursorCapture.h"

#pragma comment(lib, "Dxgi.lib")
#pragma comment(lib, "D3D11.lib")

#undef min
#undef max

namespace rgaa {

    OutputDuplication::~OutputDuplication() {
        if (duplication_) {
            duplication_->Release();
            duplication_ = nullptr;
        }
    }

    DDACapture::DDACapture(const std::shared_ptr<Context>& ctx, const CaptureResultType& crt) : Capture(ctx, crt) {

    }

    DDACapture::~DDACapture() {

    }

    bool DDACapture::Init() {
        Capture::Init();

        HRESULT hr = S_OK;
        
        D3D_DRIVER_TYPE driver_types[] = {
                D3D_DRIVER_TYPE_HARDWARE,
                D3D_DRIVER_TYPE_WARP,
                D3D_DRIVER_TYPE_REFERENCE,
        };
        auto num_drivers = ARRAYSIZE(driver_types);

        // Feature levels supported
        D3D_FEATURE_LEVEL feature_levels[] = {
                D3D_FEATURE_LEVEL_11_0,
                D3D_FEATURE_LEVEL_10_1,
                D3D_FEATURE_LEVEL_10_0,
                D3D_FEATURE_LEVEL_9_1};
        auto num_feature_levels = ARRAYSIZE(feature_levels);

        D3D_FEATURE_LEVEL feature_level;

        // Create device
        for (size_t i = 0; i < num_drivers; i++) {
            hr = D3D11CreateDevice(nullptr, driver_types[i], nullptr, 0, feature_levels, (UINT) num_feature_levels,
                                   D3D11_SDK_VERSION, &d3d_device, &feature_level, &d3d_device_context);
            if (SUCCEEDED(hr))
                break;
        }
        if (FAILED(hr)) {
            LOGE("D3D11CreateDevice failed: {0:x}", hr);
            return false;
        }

        IDXGIDevice* dxgi_device = nullptr;
        hr = d3d_device->QueryInterface(__uuidof(IDXGIDevice), (void**) &dxgi_device);
        if (FAILED(hr)) {
            LOGE("Query IDXGIDevice failed: {0:x}", hr);
            return false;
        }

        IDXGIAdapter* dxgi_adapter = nullptr;
        hr = dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**) &dxgi_adapter);
        dxgi_device->Release();
        dxgi_device = nullptr;
        if (FAILED(hr)) {
            LOGE("Get IDXGIAdapter failed: {0:x}", hr);
            return false;
        }

        UINT i = 0;
        IDXGIOutput* output = nullptr;
        std::vector<IDXGIOutput*> total_outputs;
        while(dxgi_adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND) {
            if (!output) {
                continue;
            }
            total_outputs.push_back(output);
            ++i;
        }

        int op_idx = 0;
        for (const auto op : total_outputs) {
            auto output_dup = std::make_shared<OutputDuplication>();
            // 1
            DXGI_OUTPUT_DESC desc;
            op->GetDesc(&desc);

            // 2
            // QI for Output 1
            IDXGIOutput1* dxgi_output = nullptr;
            hr = op->QueryInterface(__uuidof(dxgi_output), (void**) &dxgi_output);
            op->Release();

            if (FAILED(hr)) {
                auto msg = std::format("dxgiOutput->QueryInterface failed: {}", hr);
                std::cout << msg << std::endl;
                continue;
            }

            // Create desktop duplication
            hr = dxgi_output->DuplicateOutput(d3d_device, &output_dup->duplication_);
            dxgi_output->Release();
            dxgi_output = nullptr;
            if (FAILED(hr)) {
                if (hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE) {
                    LOGE("Too many desktop recorders already active");
                }
                else {
                    LOGE("DuplicationOutput failed : {0:x}", hr);
                }
                continue;
            }

            output_dup->dup_index_ = op_idx++;

            output_duplications_.push_back(output_dup);
        }
        return true;
    }

    bool DDACapture::CaptureNextFrame() {
        Capture::CaptureNextFrame();

        if (!context_->HasConnectedPeer()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(17));
            return true;
        }

        if (exit_) {
            exit_already_processed_ = true;
            return false;
        }

        auto capture_monitor_type = settings_->GetCaptureMonitorType();
        if (capture_monitor_type == CaptureMonitorType::kAll) {
            for (const auto& out_dup : output_duplications_) {
                CaptureNextFrameInternal(out_dup, 3);
            }
        }
        else if (capture_monitor_type == CaptureMonitorType::kSingle) {
            int target_monitor_idx = capture_monitor_idx_;
            if (target_monitor_idx >= output_duplications_.size()) {
                target_monitor_idx = 0;
            }

            auto out_dup = output_duplications_.at(target_monitor_idx);
            CaptureNextFrameInternal(out_dup, 3);
        }
        return true;
    }

    void DDACapture::Exit() {
        Capture::Exit();

        int wait_times = 0;
        while (!exit_already_processed_ && wait_times < 5) {
            wait_times++;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        LOGI("DDA exit [CaptureNextFrame]...");

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
        LOGI("Duplications released...");

        if (d3d_device_context) {
            d3d_device_context->Release();
            LOGI("D3D device context released...");
        }

        if (d3d_device) {
            d3d_device->Release();
            LOGI("D3D device released...");
        }

        if (cpu_side_texture_) {
            cpu_side_texture_->Release();
            LOGI("D3D staging texture released...");
        }
        LOGI("DDACapture exit...");
    }

    int DDACapture::GetCaptureCount() {
        return output_duplications_.size();
    }

    static uint64_t last_cbk_time_ = 0;

    int DDACapture::CaptureNextFrameInternal(const std::shared_ptr<OutputDuplication>& out_dup, int timeout) {
        auto begin = std::chrono::high_resolution_clock::now();
        auto dxgi_dup = out_dup->duplication_;
        auto dxgi_dup_release = Closer::Make([dxgi_dup](){
            dxgi_dup->ReleaseFrame();
        });

        HRESULT hr;
        bool use_cache = false;
        ID3D11Texture2D* gpu_side_texture = nullptr;
        IDXGIResource* desk_res = nullptr;
        DXGI_OUTDUPL_FRAME_INFO frameInfo;
        hr = dxgi_dup->AcquireNextFrame(timeout, &frameInfo, &desk_res);
        if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
            if (cached_textures_.find(out_dup->dup_index_) == cached_textures_.end()) {
                LOGI("Not find cached texture...");
                return 0;
            }
            use_cache = true;
        }
        else if (FAILED(hr)) {
            // perhaps shutdown and reinitialize
            auto msg = std::format("Acquire failed: {}", hr);
            std::cout << msg << std::endl;
            return hr;
        }

        auto capture_time = GetCurrentTimestamp();

        if (!use_cache) {
            hr = desk_res->QueryInterface(__uuidof(ID3D11Texture2D), (void **) &gpu_side_texture);
            desk_res->Release();
            desk_res = nullptr;

            if (FAILED(hr) || !gpu_side_texture) {
                return hr;
            }
            auto gpu_texture_release = Closer::Make([gpu_side_texture]() {
                gpu_side_texture->Release();
            });
        }

        // 1. if encode by raw data, capture => map => convert to i420 , then encode it.
        if (capture_result_type_ == CaptureResultType::kRawI420) {

            auto func_get_desc = [=]() -> D3D11_TEXTURE2D_DESC {
                D3D11_TEXTURE2D_DESC desc;
                gpu_side_texture->GetDesc(&desc);

                desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
                desc.Usage = D3D11_USAGE_STAGING;
                desc.BindFlags = 0;
                desc.MiscFlags = 0;
                return desc;
            };

            if (cached_textures_.find(out_dup->dup_index_) == cached_textures_.end()) {
                ID3D11Texture2D* texture = nullptr;
                auto desc = func_get_desc();
                hr = d3d_device->CreateTexture2D(&desc, nullptr, &texture);
                if (SUCCEEDED(hr)) {
                    cached_textures_[out_dup->dup_index_] = texture;
                }
            }

            if (!cpu_side_texture_) {
                auto desc = func_get_desc();
                hr = d3d_device->CreateTexture2D(&desc, nullptr, &cpu_side_texture_);
                if (FAILED(hr)) {
                    return hr;
                }
            }

            if (use_cache) {
                auto cached_texture = cached_textures_[out_dup->dup_index_];
                d3d_device_context->CopyResource(cpu_side_texture_, cached_texture);
            } else {
                d3d_device_context->CopyResource(cpu_side_texture_, gpu_side_texture);
                if (cached_textures_.find(out_dup->dup_index_) != cached_textures_.end()) {
                    d3d_device_context->CopyResource(cached_textures_[out_dup->dup_index_], gpu_side_texture);
                }
            }

            D3D11_MAPPED_SUBRESOURCE sr;
            hr = d3d_device_context->Map(cpu_side_texture_, 0, D3D11_MAP_READ, 0, &sr);
            if (FAILED(hr)) {
                return hr;
            }
            auto cpu_texture_release = Closer::Make([this]() {
                d3d_device_context->Unmap(cpu_side_texture_, 0);
            });

            D3D11_TEXTURE2D_DESC desc;
            cpu_side_texture_->GetDesc(&desc);
            auto width = desc.Width;
            auto height = desc.Height;

            out_dup->frame_index_++;

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

            auto cp_frame = std::make_shared<CapturedFrame>();
            cp_frame->capture_type_ = CaptureType::kRawData;
            cp_frame->raw_data_ = Data::Make((char *) yuv_frame_data.data(), desc.Width * desc.Height * 4);
            cp_frame->frame_index_ = out_dup->frame_index_;
            cp_frame->frame_width_ = desc.Width;
            cp_frame->frame_height_ = desc.Height;
            cp_frame->dup_index_ = out_dup->dup_index_;
            cp_frame->captured_time_ = capture_time;

            if (captured_cbk_ && !exit_) {
                captured_cbk_(cp_frame);

                auto current_time = GetCurrentTimestamp();
                auto diff = current_time - last_cbk_time_;
                last_cbk_time_ = current_time;
                //LOGI("{} - IN CBK - {}", out_dup->frame_index_, diff);
            }

        }
        else if (capture_result_type_ == CaptureResultType::kTexture2D) {
            // 2. capture to Texture2d, capture => texture2d to NVENC => encode it by hardware without copy and convert
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin).count();
        //LOGI("{} - DDA - {}", out_dup->frame_index_, diff);
        return 0;
    }

}

#endif
