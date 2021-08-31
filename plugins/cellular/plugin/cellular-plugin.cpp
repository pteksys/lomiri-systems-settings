/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
 *
 * Contact: Ken VanDine <ken.vandine@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cellular-plugin.h"

#include <QDebug>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QProcessEnvironment>
#include <QtDBus>
#include <LomiriSystemSettings/ItemBase>

using namespace LomiriSystemSettings;

class CellularItem: public ItemBase
{
    Q_OBJECT

public:
    explicit CellularItem(const QVariantMap &staticData, QObject *parent = 0);
    void setVisibility(bool visible);
};


CellularItem::CellularItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent)
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (env.contains(QLatin1String("LSS_SHOW_ALL_UI"))) {
        QString showAllS = env.value("LSS_SHOW_ALL_UI", QString());

        if(!showAllS.isEmpty()) {
            setVisibility(true);
            return;
        }
    }

    bool supportedDevice(false);

    QDBusInterface m_NetStatusPropertiesIface(
            "com.lomiri.connectivity1",
            "/com/lomiri/connectivity1/NetworkingStatus",
            "org.freedesktop.DBus.Properties",
            QDBusConnection::sessionBus());
    QDBusPendingReply<QVariant> modemReply = m_NetStatusPropertiesIface.call(
        "Get", "com.lomiri.connectivity1.NetworkingStatus", "ModemAvailable");
    modemReply.waitForFinished();
    if (modemReply.isValid()) {
        supportedDevice = modemReply.argumentAt<0>().toBool();
    }

    setVisibility(supportedDevice);
}

void CellularItem::setVisibility(bool visible)
{
    setVisible(visible);
}

ItemBase *CellularPlugin::createItem(const QVariantMap &staticData,
                                 QObject *parent)
{
    return new CellularItem(staticData, parent);
}

#include "cellular-plugin.moc"
