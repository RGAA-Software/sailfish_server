#include "monitor.h"

#include <iostream>

namespace rgaa 
{

	static int callback_count = 0;

	static void GetMonitorSize(MONITORINFO* info, int* width, int* height)
	{
		*width = info->rcMonitor.right - info->rcMonitor.left;
		*height = info->rcMonitor.bottom - info->rcMonitor.top;
	}

	static void PrintMonitors(ScanMonitorInfo* info)
	{
		if (!info || !info->monitors)
		{
			return;
		}

		for (int i = 0; i < info->size; i++)
		{
			MONITORINFO mi = info->monitors[i];

			int is_primary_screen = mi.dwFlags;
			RECT monitor_region = mi.rcMonitor;

			int width = 0;
			int height = 0;
			GetMonitorSize(&mi, &width, &height);

			Monitor monitor_custom = info->mons[i];

//			printf("Monitor : %d, name : %s is primary : %d. resolution : %d x %d. left : %d, top : %d, right : %d, bottom : %d, scaled width : %d, scaled height : %d\n", i,
//				monitor_custom.name, is_primary_screen, width, height, monitor_region.left, monitor_region.top, monitor_region.right,
//				monitor_region.bottom, monitor_region, monitor_custom.scaled_width, monitor_custom.scaled_height);
		}
	}


	static BOOL CALLBACK EnumMonitor(HMONITOR handle, HDC hdc, LPRECT rect, LPARAM param) {
		ScanMonitorInfo* info = (ScanMonitorInfo*)param;
		MONITORINFOEXA mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(handle, &mi);
		info->monitors[callback_count] = mi;

		Monitor mon;
		GetMonitorSize(&mi, &mon.width, &mon.height);
		RECT region = mi.rcMonitor;
		mon.left = region.left;
		mon.top = region.top;
		mon.right = region.right;
		mon.bottom = region.bottom;
		mon.primary = mi.dwFlags;

		memset(mon.name, 0, sizeof(mon.name));
		memcpy(mon.name, mi.szDevice, sizeof(mi.szDevice));

		MONITOR_DPI_TYPE mdt = MDT_EFFECTIVE_DPI;
		UINT dpiX = 0;
		UINT dpiY = 0;
		GetDpiForMonitor(handle, mdt, &dpiX, &dpiY);

		//printf("dpiX : %d, dpiY : %d", dpiX, dpiY);

		DEVICE_SCALE_FACTOR factor = DEVICE_SCALE_FACTOR_INVALID;
		GetScaleFactorForMonitor(handle, &factor);
		if (factor != DEVICE_SCALE_FACTOR_INVALID) {
			mon.scale = factor;
			mon.scale_factor = factor / 100.0f;
			mon.scaled_width = (mon.right - mon.left) / mon.scale_factor;
			mon.scaled_height = (mon.bottom - mon.top) / mon.scale_factor;
			//printf("scale : %d, scale factor : %f, sw %d, sh%d \n", factor, mon.scale_factor, mon.scaled_width, mon.scaled_height);
		}

		info->mons[callback_count] = mon;
		callback_count++;
		return true;
	}

	static ScanMonitorInfo DectectMonitors()
	{
		ScanMonitorInfo info;

		int numbers = GetSystemMetrics(SM_CMONITORS);
		info.size = numbers;
		info.monitors = (MONITORINFO*)malloc(info.size * sizeof(MONITORINFO));
		info.mons = (Monitor*)malloc(info.size * sizeof(Monitor));
		EnumDisplayMonitors(NULL, NULL, EnumMonitor, (LPARAM)&info);

		PrintMonitors(&info);
		return info;
	}

	void MonitorDetector::Detect() {
		callback_count = 0;
		monitors.clear();

		DPI_AWARENESS_CONTEXT oldContext = GetThreadDpiAwarenessContext();
		DPI_AWARENESS oldDpiAwareness = GetAwarenessFromDpiAwarenessContext(oldContext);

		ScanMonitorInfo mi = DectectMonitors();
		if (mi.size <= 0) {
			return;
		}

		for (int i = 0; i < mi.size; i++) {
			auto m = std::make_shared<Monitor>();
			memcpy(m.get(), &mi.mons[i], sizeof(Monitor));
			monitors.push_back(m);
		}

		ReleaseMonitors(&mi);
	}

	MonitorPtr MonitorDetector::GetMonitorByIndex(int idx) {
		if (idx >= monitors.size()) {
			return nullptr;
		}
		return monitors.at(idx);
	}

	MonitorPtr MonitorDetector::GetMonitorByName(const std::string& name) {
		for (auto& mon : monitors) {
			if (std::string(mon->name) == name) {
				return mon;
			}
		}
		return nullptr;
	}

	MonitorPtr MonitorDetector::GetPrimaryMonitor() {
		for (auto& m : monitors) {
			if (m->primary) {
				return m;
			}
		}
		return nullptr;
	}

	std::vector<MonitorPtr> MonitorDetector::GetMonitors() {
		return monitors;
	}

	int MonitorDetector::GetMonitorCount() {
		return monitors.size();
	}

}