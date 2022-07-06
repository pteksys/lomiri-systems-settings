/*
 * Copyright (C) 2022 UBports Foundation
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QStringList>
#include <QDBusReply>
#include <QtDebug>
#include <QDBusInterface>
#include <QDBusReply>

#include "gestures_dbushelper.h"

/* DBus definitions for interacting with Unity Screen, living in repowerd */
const QString UNITY_SCREEN_INTERFACE = QStringLiteral("com.canonical.Unity.Screen");
const QString UNITY_SCREEN_PATH = QStringLiteral("/com/canonical/Unity/Screen");
const QString UNITY_SCREEN_SERVICE = QStringLiteral("com.canonical.Unity.Screen");

/* DBus methods for interacting with DoubleTapToWake (DT2W) */
const QString UNITY_SCREEN_METHOD_DT2W_SUPPORTED = QStringLiteral("getDoubleTapToWakeSupported");
const QString UNITY_SCREEN_METHOD_DT2W_GET_ENABLED = QStringLiteral("getDoubleTapToWakeEnabled");
const QString UNITY_SCREEN_METHOD_DT2W_SET_ENABLED = QStringLiteral("setDoubleTapToWakeEnabled");

GesturesDbusHelper::GesturesDbusHelper(QObject *parent) : QObject(parent)
{
    this->m_unityScreenInterface = new QDBusInterface(
        UNITY_SCREEN_SERVICE,
        UNITY_SCREEN_PATH,
        UNITY_SCREEN_INTERFACE,
        QDBusConnection::systemBus(),
        this);

    this->m_dt2wSupported = isDT2WSupported();

    if (this->m_dt2wSupported) {
        qDebug() << Q_FUNC_INFO << "DT2W enabled:" << getDT2WEnabled();
    } else {
        qDebug() << Q_FUNC_INFO << "DT2W is not supported";
    }
}

bool GesturesDbusHelper::isSupported()
{
    /* If any item is supported, return true to show the settings entry.
     * Unsupported entries are automatically hidden. */
    return isDT2WSupported();
}

bool GesturesDbusHelper::isDT2WSupported()
{
    QDBusReply<bool> dt2wSupportedValue =
        this->m_unityScreenInterface->call(UNITY_SCREEN_METHOD_DT2W_SUPPORTED);
    return dt2wSupportedValue.isValid() && dt2wSupportedValue.value();
}

bool GesturesDbusHelper::getDT2WEnabled()
{
    QDBusReply<bool> dt2wEnabledValue =
        this->m_unityScreenInterface->call(UNITY_SCREEN_METHOD_DT2W_GET_ENABLED);
    return dt2wEnabledValue.isValid() && dt2wEnabledValue.value();
}

void GesturesDbusHelper::setDT2WEnabled(bool value)
{
    QVariantList args;
    args.append(QVariant(value));

    this->m_unityScreenInterface->callWithCallback(UNITY_SCREEN_METHOD_DT2W_SET_ENABLED,
                                                    args, this,
                                                    SLOT(handleDT2WEnabledDone()),
                                                    SLOT(handleDT2WEnabledError(QDBusError)));
}

void GesturesDbusHelper::handleDT2WEnabledDone()
{
    qDebug() << "DT2W enable state changed";
}

void GesturesDbusHelper::handleDT2WEnabledError(QDBusError error)
{
    qWarning() << "Failed to change DT2W enable state," << error.message();
}
