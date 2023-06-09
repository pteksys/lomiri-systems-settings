/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
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
 *
 */

#include "launcher_impl.h"
#include "launcher-plugin.h"

#include <SystemSettings/ItemBase>
#include <QObject>
#include <QProcessEnvironment>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QScopedPointer>
#include <QUrl>

using namespace SystemSettings;

class LauncherItem: public ItemBase
{
    Q_OBJECT

public:
    explicit LauncherItem(const QVariantMap &staticData, QObject *parent = 0);
    void setVisibility(bool visible);
};

LauncherItem::LauncherItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent)
{
    // Unconditionally show if USS_SHOW_ALL_UI is set.
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (env.contains(QLatin1String("USS_SHOW_ALL_UI"))) {
        QString showAllS = env.value("USS_SHOW_ALL_UI", QString());

        if(!showAllS.isEmpty()) {
            setVisibility(true);
            return;
        }
    }

    setVisibility(true);
}

void LauncherItem::setVisibility(bool visible)
{
    setVisible(visible);
}

ItemBase *LauncherPlugin::createItem(const QVariantMap &staticData,
                                     QObject *parent)
{
    return new LauncherItem(staticData, parent);
}

#include "launcher-plugin.moc"
