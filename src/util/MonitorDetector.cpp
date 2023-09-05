#include "MonitorDetector.h"

#include "rgaa_common/RLog.h"
#include "rgaa_common/RString.h"

namespace rgaa {

    void MonitorDetector::DetectMonitors() {
        infos.clear();

        CComPtr<IDXGIFactory1> factory;
        HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void **) (&factory));
        if (FAILED(hr)) {
            return;
        }
        int max_devices = 8;

        for (int dev = 0; dev < max_devices; dev++) {
            CComPtr<IDXGIAdapter1> tmp_adapter;
            if (factory->EnumAdapters1(dev, &tmp_adapter) != S_OK) {
                break;
            }

            MonitorInfo info;
            DXGI_ADAPTER_DESC adapter_desc;
            tmp_adapter->GetDesc(&adapter_desc);
            info.HighPart = adapter_desc.AdapterLuid.HighPart;
            info.LowPart = adapter_desc.AdapterLuid.LowPart;

            for (int i = 0; i < max_devices; i++) {
                CComPtr<IDXGIOutput> dxgi_output;
                auto r = tmp_adapter->EnumOutputs(i, &dxgi_output);
                if (dxgi_output && SUCCEEDED(r)) {
                    DXGI_OUTPUT_DESC output_desc;
                    dxgi_output->GetDesc(&output_desc);

                    info.display_name = ToUTF8(output_desc.DeviceName);
                    info.rect = output_desc.DesktopCoordinates;
                    info.width = info.rect.right - info.rect.left;
                    info.height = info.rect.bottom - info.rect.top;
                    infos.push_back(info);
                }
            }
        }
    }

    std::vector<MonitorInfo> MonitorDetector::GetMonitors() {
        return infos;
    }

    void MonitorDetector::Dump() {
        for (auto &info: infos) {
            LOGI("Monitor name : {}, resolution : {} x {}, left top : ", info.display_name.c_str(),
                 info.width, info.height, info.rect.left, info.rect.top);
        }
    }

}