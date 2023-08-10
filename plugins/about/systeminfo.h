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

#pragma once

#include <QObject>

class SystemInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString buildAbi READ buildAbi CONSTANT)
    Q_PROPERTY(QString cpuArch READ cpuArch CONSTANT)
    Q_PROPERTY(QString kernelVersion READ kernelVersion CONSTANT)
    Q_PROPERTY(QString prettyName READ prettyName CONSTANT)

public:
    SystemInfo(QObject *parent = 0);
    ~SystemInfo();
    QString buildAbi();
    QString cpuArch();
    QString kernelVersion();
    QString prettyName();
};
