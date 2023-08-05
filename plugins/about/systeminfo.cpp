/*
 * Copyright (C) 2022 Guido Berhoerster <guido+ubports@berhoerster.name>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QSysInfo>
#include <fstream>
#include <sstream>
#include <unistd.h>

#ifdef ENABLE_DEVICEINFO
#include <deviceinfo.h>
#endif

#include "systeminfo.h"
#include "storageabout.h"

SystemInfo::SystemInfo(QObject *parent) : QObject(parent)
{
}

SystemInfo::~SystemInfo()
{
}

QString SystemInfo::buildAbi()
{
    return QSysInfo::buildAbi();
}

QString SystemInfo::cpuArch()
{
    return QSysInfo::currentCpuArchitecture();
}

QString SystemInfo::kernelVersion()
{
    return QSysInfo::kernelVersion();
}

static std::string dmi_devicename()
{
    std::string str1;
    std::string str2;
    std::ifstream dmi_vendor("/sys/devices/virtual/dmi/id/sys_vendor");
    std::ifstream dmi_model("/sys/devices/virtual/dmi/id/product_name");

    std::getline(dmi_vendor, str1);
    std::getline(dmi_model, str2);
    return str1 + " " + str2;
}

static std::string devicetree_devicename()
{
    std::string str;
    std::ifstream devicetree_model("/proc/device-tree/model");

    std::getline(devicetree_model, str);
    return str;
}

QString SystemInfo::prettyName()
{
    QString str = "unknown";
    auto storageAbout = new StorageAbout();
#ifdef ENABLE_DEVICEINFO
    DeviceInfo info;
    str = QString::fromStdString(info.prettyName());
#endif
    if (str.toStdString() == "unknown"
        || str.toStdString() == "Generic device"
        || str.toStdString() == "Generic Halium device"
        || str.toStdString() == "Generic linux device") {
    // Fallback if we happen to stumble onto default values.
        if (!storageAbout->vendorString().isEmpty()) {
            return storageAbout->vendorString();
        } else if (access("/sys/devices/virtual/dmi", F_OK) != -1) {
            return QString::fromStdString(dmi_devicename());
        } else {
            return QString::fromStdString(devicetree_devicename());
        }
    }
    return str;
}
