#pragma once

#include <vector>
#include <string>
#include <iostream>

#include <d3d11.h>
#include <Dxgi1_6.h>
#include <atlbase.h>
#include <string>
#include <vector>
#include <comdef.h>
#include <Wbemidl.h>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <codecvt>

namespace rgaa {

    class MonitorInfo {
    public:
        DWORD LowPart = 0;
        LONG HighPart = 0;
        std::string display_name{};
        RECT rect{};
        int width = 0;
        int height = 0;
    };

    class MonitorDetector {
    public:

        static MonitorDetector *Instance() {
            static MonitorDetector detector;
            return &detector;
        }

        void DetectMonitors();
        std::vector<MonitorInfo> GetMonitors();
        void Dump();

    private:

        std::vector<MonitorInfo> infos;

    };

}