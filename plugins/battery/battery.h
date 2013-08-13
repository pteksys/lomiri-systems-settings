/*
 * Copyright (C) 2013 Canonical Ltd
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
 * Authors:
 * Sebastien Bacher <sebastien.bacher@canonical.com>
 *
*/

#ifndef BATTERY_H
#define BATTERY_H

#include <QDBusInterface>
#include <QObject>
#include <QProcess>

#include <libupower-glib/upower.h>

class Battery : public QObject
{
    Q_OBJECT
    Q_PROPERTY( bool powerdRunning
                READ powerdRunning
                CONSTANT)

    Q_PROPERTY( QString deviceString
                READ deviceString
                CONSTANT)

public:
    explicit Battery(QObject *parent = 0);
    ~Battery();
    bool powerdRunning();
    QString deviceString();
    Q_INVOKABLE QVariantList getHistory(const QString &deviceString, const int timespan, const int resolution) const;

private:
    QDBusConnection m_systemBusConnection;
    QString m_objectPath;
    QDBusInterface m_powerdIface;
    bool m_powerdRunning;
    UpDevice *m_device;
};

#endif // BATTERY_H
