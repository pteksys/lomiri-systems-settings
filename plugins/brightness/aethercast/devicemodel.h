/*
 * Copyright (C) 2016 Canonical Ltd.
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
 * Ken VanDine <ken.vandine@canonical.com>
 */

#ifndef AETHERCAST_DEVICE_MODEL_H
#define AETHERCAST_DEVICE_MODEL_H

#include <QByteArray>
#include <QHash>
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
#include "aethercast_device.h"
#include "aethercast_manager.h"

class DeviceModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    explicit DeviceModel(QDBusConnection &dbus, QObject *parent = 0);
    ~DeviceModel();

    enum Roles
    {
      // Qt::DisplayRole holds device name
      TypeRole = Qt::UserRole,
      AddressRole,
      StateRole,
      LastRole = StateRole
    };

    // implemented virtual methods from QAbstractTableModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int,QByteArray> roleNames() const;

    QSharedPointer<Device> getDeviceFromAddress(const QString &address);
    QSharedPointer<Device> getDeviceFromPath(const QString &path);

Q_SIGNALS:
    void countChanged(int count);

private:
    QDBusConnection m_dbus;
    DBusObjectManagerInterface m_aethercastManager;

    void setProperties(const QMap<QString,QVariant> &properties);
    void updateProperty(const QString &key, const QVariant &value);

    QList<QSharedPointer<Device> > m_devices;
    void getManagedObjects();
    void updateDevices();
    void addDevice(QSharedPointer<Device> &device);
    void addDevice(const QString &objectPath, const QVariantMap &properties);
    void removeRow(int i);
    int findRowFromAddress(const QString &address) const;
    void emitRowChanged(int row);

private Q_SLOTS:
    void slotInterfacesAdded(const QDBusObjectPath &objectPath, InterfaceList ifacesAndProps);
    void slotInterfacesRemoved(const QDBusObjectPath &objectPath, const QStringList &interfaces);
    void slotAdapterPropertiesChanged(const QString &interface, const QVariantMap &changedProperties,
                                      const QStringList &invalidatedProperties);
    void slotRemoveFinished(QDBusPendingCallWatcher *call);
    void slotPropertyChanged(const QString &key, const QDBusVariant &value);
    void slotDeviceChanged();
};

class DeviceFilter: public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    DeviceFilter() {}
    virtual ~DeviceFilter() {}

    void filterOnStates(Device::States);

Q_SIGNALS:
    void countChanged(int count);

protected:
    virtual bool filterAcceptsRow(int, const QModelIndex&) const;
    virtual bool lessThan(const QModelIndex&, const QModelIndex&) const;

private:
    Device::States m_states = Device::State::Idle;
    bool m_statesEnabled = false;

};

#endif // AETHERCAST_DEVICE_MODEL_H
