//
// Created by RGAA on 2023/8/10.
//

#ifndef SAILFISH_SERVER_DDACAPTURE_H
#define SAILFISH_SERVER_DDACAPTURE_H

#ifdef _OS_WINDOWS_

#include "Capture.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>

#include <map>
#include <vector>
#include <string>

namespace rgaa {

    class OutputDuplication {
    public:
        ~OutputDuplication();

    public:
        DXGI_OUTPUT_DESC desc_{};
        IDXGIOutputDuplication* duplication_ = nullptr;
        int64_t frame_index_ = 0;
        int dup_index_ = -1;
    };

    class DDACapture : public Capture {
    public:

        DDACapture(const std::shared_ptr<Context>& ctx, const CaptureResultType& crt);
        ~DDACapture() override;

        bool Init() override;
        bool CaptureNextFrame() override;
        void Exit() override;
        int GetCaptureCount() override;

    private:

        int CaptureNextFrameInternal(const std::shared_ptr<OutputDuplication>& out_dup, int timeout);

    private:

        ID3D11Device* d3d_device = nullptr;
        ID3D11DeviceContext* d3d_device_context = nullptr;
        std::vector<std::shared_ptr<OutputDuplication>> output_duplications_;
        std::map<int, ID3D11Texture2D*> cpu_side_textures_;
        std::map<int, ID3D11Texture2D*> cached_textures_;
        uint64_t last_cbk_time_ = 0;
        std::vector<uint8_t> yuv_frame_data_;
    };

}

#endif

#endif //SAILFISH_SERVER_DDACAPTURE_H
