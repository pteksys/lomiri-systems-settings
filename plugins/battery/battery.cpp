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

#include "battery.h"
#include <glib.h>
#include <libupower-glib/upower.h>
#include <QtCore/QDebug>

BatteryBackend::BatteryBackend(QObject *parent) :
    QObject(parent),
    m_systemBusConnection (QDBusConnection::systemBus()),
    m_powerdIface ("com.lomiri.Repowerd",
                   "/com/lomiri/Repowerd",
                   "com.lomiri.Repowerd",
                   m_systemBusConnection),
    m_primaryBattery(nullptr)
{
    choosePrimaryDevice();

    m_powerdRunning = m_powerdIface.isValid();
}

bool BatteryBackend::powerdRunning() const
{
    return m_powerdRunning;
}

Battery * BatteryBackend::primaryBattery() const
{
    return m_primaryBattery;
}

/* FIXME: this logic could fall apart if there's >1 battery e.g. PinePhone w/ keyboard battery/ */
void BatteryBackend::choosePrimaryDevice() {
    UpClient *client;
    GPtrArray *devices;
    UpDeviceKind kind;
    UpDeviceTechnology technology;
    UpDevice *primaryDevice = nullptr;

    client = up_client_new();

#if !UP_CHECK_VERSION(0, 99, 0)
    gboolean returnIsOk;

    returnIsOk = up_client_enumerate_devices_sync(client, nullptr, nullptr);

    if(!returnIsOk)
        return;
#endif

    devices = up_client_get_devices2(client);

    for (uint i=0; i < devices->len; i++) {
        UpDevice *device;
        device = (UpDevice *)g_ptr_array_index(devices, i);
        g_object_get(device, "kind", &kind, nullptr);
        g_object_get(device, "technology", &technology, nullptr);
        if (kind == UP_DEVICE_KIND_BATTERY &&
            (technology == UP_DEVICE_TECHNOLOGY_LITHIUM_ION ||
             technology == UP_DEVICE_TECHNOLOGY_LITHIUM_POLYMER))
        {
            primaryDevice = device;
        }
    }

    if (primaryDevice) {
        m_primaryBattery = new Battery(
            static_cast<UpDevice *>(g_object_ref(primaryDevice)), this);
    }

    g_ptr_array_unref(devices);
    g_object_unref(client);
}

BatteryBackend::~BatteryBackend() {}

Battery::Battery(UpDevice * device, QObject *parent) :
    QObject(parent),
    m_device(device),
    m_lastFullCharge(0)
{
    getLastFullCharge();

    g_object_connect(m_device,
        "signal::notify::percentage", G_CALLBACK(&handleNotifyPercentage), this,
        "signal::notify::state",      G_CALLBACK(&handleNotifyState),      this,
        nullptr);
}

Battery::~Battery()
{
    g_signal_handlers_disconnect_by_data(m_device, this);
    g_object_unref(m_device);
}

double Battery::batteryLevel() const
{
    double percentage;
    g_object_get (m_device, "percentage", &percentage, nullptr);

    return percentage;
}

Battery::State Battery::state() const
{
   guint state;
   g_object_get (m_device, "state", &state, nullptr);

    /* Because we define the enum values to be the same as libupower-glib's
     * values, no conversion has to be done here. */
   return State(state);
}

int Battery::lastFullCharge() const
{
    return m_lastFullCharge;
}

void Battery::getLastFullCharge()
{
    GPtrArray *values = nullptr;
    gint32 offset = g_get_real_time() / 1000000;
    values = up_device_get_history_sync(m_device, "charge", 864000, 1000, nullptr, nullptr);

    if (values == nullptr) {
        qWarning() << "Can't get charge info";
        return;
    }

    double maxCapacity = 100.0;
    g_object_get (m_device, "capacity", &maxCapacity, nullptr);

    for (uint i=0; i < values->len; i++) {
        auto item = static_cast<UpHistoryItem *>(g_ptr_array_index(values, i));

        /* Getting the next point after full charge, since upower registers only on state changes,
           typically you get no data while the device is fully charged and plugged and you get a discharging
           one when you unplugged, that's when the charge stops */
        if (up_history_item_get_state(item) == UP_DEVICE_STATE_FULLY_CHARGED ||
                up_history_item_get_value(item) >= maxCapacity) {
            if (i < values->len-1) {
                UpHistoryItem *nextItem = (UpHistoryItem *) g_ptr_array_index(values, i+1);
                m_lastFullCharge = (int)((offset - (gint32) up_history_item_get_time(nextItem)));
                Q_EMIT(lastFullChargeChanged());
                g_ptr_array_unref (values);
                return;
            }
        }
    }
    g_ptr_array_unref (values);
}

/* TODO: refresh values over time for dynamic update */
QVariantList Battery::getHistory(const int timespan, const int resolution)
{
    GPtrArray *values = nullptr;
    gint32 offset = g_get_real_time() / 1000000;
    QVariantList listValues;
    QVariantMap listItem;
    gdouble currentValue = 0;

    values = up_device_get_history_sync(m_device, "charge", timespan, resolution, nullptr, nullptr);

    if (values == nullptr) {
        qWarning() << "Can't get charge info";
        return QVariantList();
    }

    for (uint i=values->len-1; i > 0; i--) {
        auto item = static_cast<UpHistoryItem *>(g_ptr_array_index(values, i));

        if (up_history_item_get_state(item) == UP_DEVICE_STATE_UNKNOWN)
            continue;

        /* TODO: find better way to filter out suspend/resume buggy values,
         * we get empty charge report when that happens, in practice batteries don't run flat often,
         * if charge was over 3% before it's likely a bug so we ignore the value */
        if (up_history_item_get_state(item) == UP_DEVICE_STATE_EMPTY && currentValue > 3)
            continue;

        /* Getting the next point after full charge, since upower registers only on state changes,
           typically you get no data while the device is fully charged and plugged and you get a discharging
           one when you unplugged, that's when the charge stops */
        if (up_history_item_get_state(item) == UP_DEVICE_STATE_FULLY_CHARGED ||
                up_history_item_get_value(item) == 100.0) {
            if (i > 1) {
                UpHistoryItem *nextItem = (UpHistoryItem *) g_ptr_array_index(values, i-1);
                m_lastFullCharge = (int)((offset - (gint32) up_history_item_get_time(nextItem)));
                Q_EMIT(lastFullChargeChanged());
            }
        }

        currentValue = up_history_item_get_value(item);
        listItem.insert("time",(offset - (gint32) up_history_item_get_time(item)));
        listItem.insert("value", currentValue);
        listValues += listItem;
    }

    /* Set an extra point, at the current time, with the previous value
     * that's to workaround https://bugs.freedesktop.org/show_bug.cgi?id=68711
     * otherwise a fully charged device lacks the flat full charge segment */
    listItem.insert("time", 0);
    listItem.insert("value", currentValue);
    listValues += listItem;

    g_ptr_array_unref (values);
    return listValues;
}

/* static */
void Battery::handleNotifyPercentage(GObject* , GParamSpec* , gpointer user_data)
{
    auto self = static_cast<Battery *>(user_data);
    Q_EMIT(self->batteryLevelChanged());
}

/* static */
void Battery::handleNotifyState(GObject* , GParamSpec* , gpointer user_data)
{
    auto self = static_cast<Battery *>(user_data);
    Q_EMIT(self->stateChanged());
}
