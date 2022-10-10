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

#ifndef USS_BLUETOOTH_AGENT_H
#define USS_BLUETOOTH_AGENT_H

#include <QObject>
#include <QMap>
#include <QSharedPointer>

#include <QtDBus>

#include "device.h"
#include "devicemodel.h"

class Agent: public QObject, protected QDBusContext
{
    Q_OBJECT

public:
    Agent(QDBusConnection connection, DeviceModel &devices, QObject *parent=0):
        QObject(parent), m_connection(connection), m_devices(devices) {}
    virtual ~Agent() {}
    Q_INVOKABLE void confirmPasskey(uint tag, bool confirmed);
    Q_INVOKABLE void providePasskey(uint tag, bool provided, uint passkey);
    Q_INVOKABLE void providePinCode(uint tag, bool provided, QString pinCode);
    Q_INVOKABLE void displayPinCodeCallback(uint tag);
    Q_INVOKABLE void displayPasskeyCallback(uint tag);
    Q_INVOKABLE void authorizationRequestCallback(uint tag, bool allow);

public Q_SLOTS: // received from the system's bluez service
    void Cancel();
    void DisplayPinCode(const QDBusObjectPath &path, QString pincode);
    void DisplayPasskey(const QDBusObjectPath &path, uint passkey, ushort entered);
    void Release();
    void RequestConfirmation(const QDBusObjectPath &path, uint passkey);
    uint RequestPasskey(const QDBusObjectPath &path);
    QString RequestPinCode(const QDBusObjectPath &path);
    void RequestAuthorization(const QDBusObjectPath &path);

Q_SIGNALS:
    void pinCodeNeeded(int tag, Device* device);
    void passkeyNeeded(int tag, Device* device);
    void passkeyConfirmationNeeded(int tag, Device* device, QString passkey);
    void displayPinCodeNeeded(Device* device, QString pincode);
    void displayPasskeyNeeded(Device* device, QString passkey, ushort entered);
    void releaseNeeded();
    void cancelNeeded();
    void authorizationRequested(int tag, Device* device);

private:
    Q_DISABLE_COPY(Agent)

    QDBusConnection m_connection;
    DeviceModel &m_devices;
    QMap<uint,QDBusMessage> m_delayedReplies;
    uint m_tag = 1;

    void cancel(QDBusMessage msg, const char *functionName);
    void reject(QDBusMessage msg, const char *functionName);

    QSharedPointer<Device> findOrCreateDevice(const QDBusObjectPath &path);
};

Q_DECLARE_METATYPE(Agent*)


#endif // USS_BLUETOOTH_AGENT_H
