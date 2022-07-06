/*
 * Copyright (C) 2022 UBports Foundation
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

#include "gestures-plugin.h"

#include <QDebug>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QProcessEnvironment>
#include <QtDBus>
#include <SystemSettings/ItemBase>

#include "../gestures_dbushelper.h"

using namespace SystemSettings;

class GesturesItem: public ItemBase
{
    Q_OBJECT

public:
    explicit GesturesItem(const QVariantMap &staticData, QObject *parent = 0);
    void setVisibility(bool visible);

private:
    GesturesDbusHelper m_gesturesDbusHelper;
};


GesturesItem::GesturesItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent)
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (env.contains(QLatin1String("USS_SHOW_ALL_UI"))) {
        QString showAllS = env.value("USS_SHOW_ALL_UI", QString());

        if(!showAllS.isEmpty()) {
            setVisibility(true);
            return;
        }
    }

    const bool supportedDevice = this->m_gesturesDbusHelper.isSupported();
    setVisibility(supportedDevice);
}

void GesturesItem::setVisibility(bool visible)
{
    setVisible(visible);
}

ItemBase *GesturesPlugin::createItem(const QVariantMap &staticData,
                                 QObject *parent)
{
    return new GesturesItem(staticData, parent);
}

#include "gestures-plugin.moc"
