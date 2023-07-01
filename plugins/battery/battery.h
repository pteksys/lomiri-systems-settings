/*
 * Copyright (C) 2013 Canonical Ltd.
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

#include <libupower-glib/upower.h>

class Battery : public QObject
{
    Q_OBJECT

    Q_PROPERTY( double batteryLevel
                READ batteryLevel
                NOTIFY batteryLevelChanged)

    Q_PROPERTY( State state
                READ state
                NOTIFY stateChanged)

    Q_PROPERTY( int lastFullCharge
                READ lastFullCharge
                NOTIFY lastFullChargeChanged)

public:
    enum State {
        Unknown = UP_DEVICE_STATE_UNKNOWN,
        Charging = UP_DEVICE_STATE_CHARGING,
        Discharging = UP_DEVICE_STATE_DISCHARGING,
        Empty = UP_DEVICE_STATE_EMPTY,
        FullyCharged = UP_DEVICE_STATE_FULLY_CHARGED,
        PendingCharge = UP_DEVICE_STATE_PENDING_CHARGE,
        PendingDischarge = UP_DEVICE_STATE_PENDING_DISCHARGE,
    };
    Q_ENUM(State)

    explicit Battery(UpDevice * device, QObject *parent = 0);
    ~Battery();
    double batteryLevel() const;
    State state() const;
    int lastFullCharge() const;

    Q_INVOKABLE QVariantList getHistory(const int timespan, const int resolution);

Q_SIGNALS:
    void batteryLevelChanged();
    void stateChanged();
    void lastFullChargeChanged();

private:
    UpDevice *m_device;
    int m_lastFullCharge = 0;

    void getLastFullCharge();

    static void handleNotifyPercentage(GObject* , GParamSpec* , gpointer user_data);
    static void handleNotifyState(GObject* , GParamSpec* , gpointer user_data);
};

class BatteryBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY( bool powerdRunning
                READ powerdRunning
                CONSTANT)

    Q_PROPERTY( Battery * primaryBattery
                READ primaryBattery
                CONSTANT)

public:
    explicit BatteryBackend(QObject *parent = 0);
    ~BatteryBackend();
    bool powerdRunning() const;
    Battery * primaryBattery() const;

private:
    QDBusConnection m_systemBusConnection;
    QDBusInterface m_powerdIface;
    bool m_powerdRunning;
    Battery * m_primaryBattery;

    void choosePrimaryDevice();
};

#endif // BATTERY_H
