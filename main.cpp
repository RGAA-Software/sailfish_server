#include <memory>
#include <iostream>

#include "Application.h"
#include "rgaa_common/RDump.h"
#include "rgaa_common/RLog.h"
#include "ui/Workspace.h"
#include "context/Context.h"

#include <QApplication>

#include <Windows.h>
#include <iptypes.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "IPHLPAPI.lib")

using namespace rgaa;
//https://developer.aliyun.com/article/272274
void get_ip_list(QMap<QString, int> & map_ip)
{
    DWORD dwRetVal = 0;
    PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO *) malloc(sizeof(IP_ADAPTER_INFO));
    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

    if(GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS)
    {
        GlobalFree(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen);
    }

    if((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
    {
        while (pAdapterInfo)
        {
            if(strstr(pAdapterInfo->Description,"PCI") > 0
               || (IF_TYPE_IEEE80211 == pAdapterInfo->Type && 0 < strstr(pAdapterInfo->Description, "802")))
            {
                PIP_ADDR_STRING addr = &(pAdapterInfo->IpAddressList);
                do
                {
                    if (IF_TYPE_IEEE80211 == pAdapterInfo->Type)
                        map_ip.insert(addr->IpAddress.String, 0);
                    else
                        map_ip.insert(addr->IpAddress.String, 1);
                    addr = addr->Next;
                }
                while (addr);
            }
            pAdapterInfo = pAdapterInfo->Next;
        }
    }
    else
    {
        qDebug() << "GetAdaptersInfo failed with error: " << dwRetVal;
    }

    if(pAdapterInfo)
    {
        GlobalFree(pAdapterInfo);
    }
}

int main(int argc, char** argv) {

    CaptureDump();

    QApplication qApplication(argc, argv);

    Logger::Init("sailfish_server.log", false);

    auto context = std::make_shared<Context>();
    context->Init();

    Workspace workspace(context, nullptr);
    workspace.Show();

    return QApplication::exec();
}
