//
// Created by RGAA on 2023/8/16.
//

#include "IPUtil.h"

#include "rgaa_common/RLog.h"

#ifdef _OS_WINDOWS_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iphlpapi.h>
#include <iptypes.h>
#include <cstdlib>
#include <cstring>
#include <map>

#pragma comment(lib, "IPHLPAPI.lib")

#endif


namespace rgaa {

    std::map<std::string, IPNetworkType> IPUtil::ScanIPs() {

        std::map<std::string, IPNetworkType> ips;

#ifdef _OS_WINDOWS_
        //https://developer.aliyun.com/article/272274
        DWORD dwRetVal = 0;
        auto pAdapterInfo = (IP_ADAPTER_INFO *) malloc(sizeof(IP_ADAPTER_INFO));
        ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

        if(GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS) {
            GlobalFree(pAdapterInfo);
            pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen);
        }

        if((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
            while (pAdapterInfo) {
                if(strstr(pAdapterInfo->Description,"PCI") != nullptr
                   || (IF_TYPE_IEEE80211 == pAdapterInfo->Type && strstr(pAdapterInfo->Description, "802") != nullptr))
                {
                    PIP_ADDR_STRING addr = &(pAdapterInfo->IpAddressList);
                    do {
                        if (IF_TYPE_IEEE80211 == pAdapterInfo->Type) {
                            ips.insert(std::make_pair(addr->IpAddress.String, IPNetworkType::kWireless));
                        }
                        else {
                            ips.insert(std::make_pair(addr->IpAddress.String, IPNetworkType::kWired));
                        }
                        addr = addr->Next;
                    }
                    while (addr);
                }
                pAdapterInfo = pAdapterInfo->Next;
            }
        }
        else {
            LOGE("GetAdaptersInfo failed with error: {}", dwRetVal);
        }

        if(pAdapterInfo) {
            GlobalFree(pAdapterInfo);
        }
#endif

        return ips;
    }

}
