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

#include <QString>
#include <QNetworkInterface>

namespace rgaa {

    static bool is_virtual_network_card_or_loopback(QString str_card_name) {
        if (-1 != str_card_name.indexOf("VMware")
            || -1 != str_card_name.indexOf("Loopback")
            || -1 != str_card_name.indexOf("VirtualBox")
                )
            return true;
        return false;
    }

    // 0 - wireless , 1 - wire
    static void get_ips(std::map<std::string, IPNetworkType>& map_ip) {
        QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
        QList<QNetworkAddressEntry> entry;
        foreach(QNetworkInterface inter, interfaces) {
            if (is_virtual_network_card_or_loopback(inter.humanReadableName())) {
                continue;
            }

            if (inter.flags() & (QNetworkInterface::IsUp | QNetworkInterface::IsRunning)) {
                entry = inter.addressEntries();
                int cnt = entry.size() - 1;
                for (int i = 1; i <= cnt; ++i) {
                    if (entry.at(i).ip().protocol() == QAbstractSocket::IPv4Protocol) {
                        if (-1 != inter.name().indexOf("wireless")) {
                            map_ip.insert({entry.at(i).ip().toString().toStdString(), IPNetworkType::kWireless});
                        }
                        else if (-1 != inter.name().indexOf("ethernet")) {
                            map_ip.insert({entry.at(i).ip().toString().toStdString(), IPNetworkType::kWired});
                        }
                    }
                }
                entry.clear();
            }
        }
    }

    std::map<std::string, IPNetworkType> IPUtil::ScanIPs() {
        std::map<std::string, IPNetworkType> ips;
        get_ips(ips);
        return ips;
    }

}
