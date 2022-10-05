/*
 * Copyright (C) 2013-2015 Canonical Ltd.
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
 * Charles Kerr <charles.kerr@canonical.com>
 *
 */

#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QObject>

#include "agent.h"
#include "devicemodel.h"

#include "bluez_agent1adaptor.h"

class Bluetooth : public QObject
{
    Q_OBJECT

    Q_PROPERTY (QAbstractItemModel* connectedDevices
                READ getConnectedDevices
                CONSTANT)

    Q_PROPERTY (QAbstractItemModel* disconnectedDevices
                READ getDisconnectedDevices
                CONSTANT)

    Q_PROPERTY (QAbstractItemModel* autoconnectDevices
                READ getAutoconnectDevices
                CONSTANT)

    Q_PROPERTY (QObject * selectedDevice
                READ getSelectedDevice
                NOTIFY selectedDeviceChanged)

    Q_PROPERTY (QObject * agent
                READ getAgent
                CONSTANT)

    Q_PROPERTY (bool powered
                READ isPowered
                NOTIFY poweredChanged)

    Q_PROPERTY (bool discovering
                READ isDiscovering
                NOTIFY discoveringChanged)

    Q_PROPERTY (bool discoverable
                READ isDiscoverable
                NOTIFY discoverableChanged)

    Q_PROPERTY (QString adapterName
                READ adapterName
                NOTIFY adapterNameChanged)

    Q_PROPERTY (QString adapterAddress
                READ adapterAddress
                NOTIFY adapterAddressChanged)

Q_SIGNALS:
    void selectedDeviceChanged();
    void poweredChanged(bool powered);
    void discoveringChanged(bool isActive);
    void discoverableChanged(bool isActive);
    void devicePairingDone(Device *device, bool success);
    void adapterNameChanged();
    void adapterAddressChanged();

public:
    explicit Bluetooth(QObject *parent = nullptr);
    explicit Bluetooth(const QDBusConnection &dbus, QObject *parent = nullptr);
    ~Bluetooth() {}

    Q_INVOKABLE QString adapterName() const { return m_devices.adapterName(); }
    Q_INVOKABLE QString adapterAddress() const { return m_devices.adapterAddress(); }
    Q_INVOKABLE void setSelectedDevice(const QString &address);
    Q_INVOKABLE void connectDevice(const QString &address);
    Q_INVOKABLE void disconnectDevice();
    Q_INVOKABLE void removeDevice();
    Q_INVOKABLE void trySetDiscoverable(bool discoverable);
    Q_INVOKABLE void resetSelectedDevice();
    Q_INVOKABLE void blockDiscovery();
    Q_INVOKABLE void unblockDiscovery();
    Q_INVOKABLE void startDiscovery();
    Q_INVOKABLE void stopDiscovery();
    Q_INVOKABLE void toggleDiscovery();

public:
    Agent * getAgent();
    Device * getSelectedDevice();
    QAbstractItemModel * getConnectedDevices();
    QAbstractItemModel * getDisconnectedDevices();
    QAbstractItemModel * getAutoconnectDevices();

    bool isPowered() const { return m_devices.isPowered(); }
    bool isDiscovering() const { return m_devices.isDiscovering(); }
    bool isDiscoverable() const { return m_devices.isDiscoverable(); }

private:
    QDBusConnection m_dbus;
    DeviceModel m_devices;
    DeviceFilter m_connectedDevices;
    DeviceFilter m_disconnectedDevices;
    DeviceFilter m_autoconnectDevices;
    QSharedPointer<Device> m_selectedDevice;

    Agent m_agent;
};

#endif // BLUETOOTH_H

