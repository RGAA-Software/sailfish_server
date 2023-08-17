#pragma once

#include "pch.h"
#include <optional>
#include <atlbase.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Monitor.h"
#include "Capture.h"

namespace rgaa {

    class Data;

    class GraphicsCapture : public Capture {
    public:

        static bool IsGraphicsCaptureSupported();

        GraphicsCapture(const std::shared_ptr<Context>& ctx, const CaptureResultType& crt);
        ~GraphicsCapture();

		bool Init() override;
		bool CaptureNextFrame() override;
        void Exit() override;

        bool IsCursorEnabled() { CheckClosed(); return m_session.IsCursorCaptureEnabled(); }
        void SetCursorEnabled(bool value) { CheckClosed(); m_session.IsCursorCaptureEnabled(value); }
        bool IsBorderRequired() { CheckClosed(); return m_session.IsBorderRequired(); }
        void IsBorderRequired(bool value) { CheckClosed(); m_session.IsBorderRequired(value); }
        winrt::Windows::Graphics::Capture::GraphicsCaptureItem CaptureItem() { return m_item; }

        void SetPixelFormat(winrt::Windows::Graphics::DirectX::DirectXPixelFormat pixelFormat)
        {
            CheckClosed();
            auto newFormat = std::optional(pixelFormat);
            m_pixelFormatUpdate.exchange(newFormat);
        }

    private:
        void OnFrameArrived(
            winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool const& sender,
            winrt::Windows::Foundation::IInspectable const& args);

        inline void CheckClosed()
        {
            if (m_closed.load() == true)
            {
                throw winrt::hresult_error(RO_E_CLOSED);
            }
        }

        void ResizeSwapChain();
        bool TryResizeSwapChain(winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame const& frame);
        bool TryUpdatePixelFormat();

    private:
        winrt::Windows::Graphics::Capture::GraphicsCaptureItem m_item{ nullptr };
        winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool m_framePool{ nullptr };
        winrt::Windows::Graphics::Capture::GraphicsCaptureSession m_session{ nullptr };
        winrt::Windows::Graphics::SizeInt32 m_lastSize;

        winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice m_device{ nullptr };
        winrt::com_ptr<IDXGISwapChain1> m_swapChain{ nullptr };
        winrt::com_ptr<ID3D11DeviceContext> m_d3dContext{ nullptr };
        winrt::com_ptr<ID3D11Device> m_d3ddevice{ nullptr };
        winrt::Windows::Graphics::DirectX::DirectXPixelFormat m_pixelFormat;

        std::atomic<std::optional<winrt::Windows::Graphics::DirectX::DirectXPixelFormat>> m_pixelFormatUpdate{ std::nullopt };

        std::atomic<bool> m_closed = false;
        std::atomic<bool> m_captureNextImage = false;

        CComPtr<ID3D11Texture2D> shared_texture = nullptr; 
        uint64_t frame_index = 0;

        std::shared_ptr<Monitor> capture_monitor = nullptr;

        int64_t frame_index_ = 0;

        std::shared_ptr<Data> yuv_frame_data_ = nullptr;

    };

}