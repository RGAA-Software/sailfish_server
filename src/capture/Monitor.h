#pragma once

//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <memory>
#include <string>
#include <shtypes.h>
#include <shellscalingapi.h>

#pragma comment(lib, "Shcore.lib")

namespace rgaa 
{

    typedef struct _Monitor {
        bool primary;
        int width;
        int height;
        int left;
        int top;
        int right;
        int bottom;
        char name[CCHDEVICENAME];
    } Monitor;
    typedef std::shared_ptr<Monitor> MonitorPtr;

    typedef struct _MonitorInfo {
        MONITORINFO* monitors;
        Monitor* mons;
        int size;
    } ScanMonitorInfo;

    static void ReleaseMonitors(ScanMonitorInfo* info)
    {
        if (!info || !info->monitors) {
            return;
        }
        free(info->monitors);
    }

    class MonitorDetector {
    public:

        static MonitorDetector* Instance() {
            static MonitorDetector m;
            return &m;
        }

        void Detect();

        MonitorPtr GetMonitorByIndex(int idx);
        MonitorPtr GetMonitorByName(const std::string& name);
        MonitorPtr GetPrimaryMonitor();
		std::vector<MonitorPtr> GetMonitors();
		int GetMonitorCount();

    public:

        std::vector<MonitorPtr>    monitors;

    };
}