/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@canonical.com>
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

#ifndef SYSTEM_SETTINGS_TEST_PLUGIN_H
#define SYSTEM_SETTINGS_TEST_PLUGIN_H

#include <QObject>
#include <LomiriSystemSettings/PluginInterface>

class TestPlugin: public QObject, public LomiriSystemSettings::PluginInterface2
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.lomiri.SystemSettings.PluginInterface/2.0")
    Q_INTERFACES(LomiriSystemSettings::PluginInterface2)

public:
    TestPlugin();

    LomiriSystemSettings::ItemBase *createItem(const QVariantMap &staticData,
                                         QObject *parent = 0);
};

#endif // SYSTEM_SETTINGS_TEST_PLUGIN_H
