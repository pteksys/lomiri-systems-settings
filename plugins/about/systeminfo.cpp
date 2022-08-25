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

#include "systeminfo.h"

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
