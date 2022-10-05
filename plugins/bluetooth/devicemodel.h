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
 */

#ifndef BLUETOOTH_DEVICE_MODEL_H
#define BLUETOOTH_DEVICE_MODEL_H

#include <QByteArray>
#include <QHash>
#include <QTimer>
#include <QList>
#include <QVariant>

#include <QAbstractListModel>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

#include "device.h"

#include "freedesktop_objectmanager.h"
#include "freedesktop_properties.h"
#include "bluez_adapter1.h"
#include "bluez_agentmanager1.h"

class DeviceModel: public QAbstractListModel
{
    Q_OBJECT

public:
    explicit DeviceModel(QDBusConnection &dbus, QObject *parent = 0);
    ~DeviceModel();

    enum Roles
    {
      // Qt::DisplayRole holds device name
      TypeRole = Qt::UserRole,
      IconRole,
      StrengthRole,
      ConnectionRole,
      AddressRole,
      TrustedRole,
      LastRole = TrustedRole
    };

    // implemented virtual methods from QAbstractTableModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int,QByteArray> roleNames() const;

    QSharedPointer<Device> getDeviceFromAddress(const QString &address);
    QSharedPointer<Device> getDeviceFromPath(const QString &path);
    QSharedPointer<Device> addDeviceFromPath(const QDBusObjectPath &path);
    QString adapterName() const { return m_adapterName; }
    QString adapterAddress() const { return m_adapterAddress; }

public:
    bool isPowered() const { return m_isPowered; }
    bool isDiscovering() const { return m_isDiscovering; }
    bool isDiscoverable() const { return m_isDiscoverable; }
    void removeDevice(const QString &path);
    void trySetDiscoverable(bool discoverable);
    void stopDiscovery();
    void startDiscovery();
    void toggleDiscovery();
    void blockDiscovery();
    void unblockDiscovery();

Q_SIGNALS:
    void poweredChanged(bool powered);
    void discoveringChanged(bool isDiscovering);
    void discoverableChanged(bool isDiscoverable);
    void devicePairingDone(Device *device, bool success);
    void adapterNameChanged();
    void adapterAddressChanged();

private:
    QDBusConnection m_dbus;
    DBusObjectManagerInterface m_bluezManager;
    BluezAgentManager1 m_bluezAgentManager;

    void setProperties(const QMap<QString,QVariant> &properties);
    void updateProperty(const QString &key, const QVariant &value);

    QString m_adapterName;
    QString m_adapterAddress;
    bool m_isPowered = false;
    bool m_isPairable = false;
    bool m_isDiscovering = false;
    bool m_isDiscoverable = false;
    QTimer m_discoveryTimer;
    QTimer m_discoverableTimer;
    unsigned int m_discoveryBlockCount;
    unsigned int m_activeDevices;
    bool m_anyDeviceActive;

    void restartDiscoveryTimer();
    void setDiscoverable(bool discoverable);
    void setPowered(bool powered);

    QScopedPointer<BluezAdapter1> m_bluezAdapter;
    QScopedPointer<FreeDesktopProperties> m_bluezAdapterProperties;

    void clearAdapter();
    void setAdapterFromPath(const QString &objectPath, const QVariantMap &properties);

    QList<QSharedPointer<Device> > m_devices;
    void updateDevices();
    QSharedPointer<Device> addDevice(QSharedPointer<Device> &device);
    QSharedPointer<Device> addDevice(const QString &objectPath, const QVariantMap &properties);
    void removeRow(int i);
    int findRowFromAddress(const QString &address) const;
    void emitRowChanged(int row);

    void setDiscovering(bool value);
    void setupAsDefaultAgent();

private Q_SLOTS:
    void slotInterfacesAdded(const QDBusObjectPath &objectPath, InterfaceList ifacesAndProps);
    void slotInterfacesRemoved(const QDBusObjectPath &objectPath, const QStringList &interfaces);
    void slotAdapterPropertiesChanged(const QString &interface, const QVariantMap &changedProperties,
                                      const QStringList &invalidatedProperties);
    void slotRemoveFinished(QDBusPendingCallWatcher *call);
    void slotPropertyChanged(const QString &key, const QDBusVariant &value);
    void slotDiscoveryTimeout();
    void slotEnableDiscoverable();
    void slotDeviceChanged();
    void slotDevicePairingDone(bool success);
    void slotDeviceConnectionChanged();
};

class DeviceFilter: public QSortFilterProxyModel
{
    Q_OBJECT

public:
    DeviceFilter() {}
    virtual ~DeviceFilter() {}
    void filterOnType(const QVector<Device::Type>);
    void filterOnConnections(Device::Connections);
    void filterOnTrusted(bool trusted);

protected:
    virtual bool filterAcceptsRow(int, const QModelIndex&) const;
    virtual bool lessThan(const QModelIndex&, const QModelIndex&) const;

private:
    QVector<Device::Type> m_types = QVector<Device::Type>();
    bool m_typeEnabled = false;
    Device::Connections m_connections = Device::Connection::Connected;
    bool m_connectionsEnabled = false;
    bool m_trustedEnabled = false;
    bool m_trustedFilter = false;
};

#endif // BLUETOOTH_DEVICE_MODEL_H
