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

#ifndef SYSTEM_SETTINGS_PLUGIN_INTERFACE_H
#define SYSTEM_SETTINGS_PLUGIN_INTERFACE_H

#include <QObject>
#include <QVariantMap>

namespace LomiriSystemSettings {

class ItemBase;

class PluginInterface
{
public:
    virtual ItemBase *createItem(const QVariantMap &staticData,
                                 QObject *parent = 0) = 0;
};

class PluginInterface2: public PluginInterface
{
public:
    /* Return true if a reset has been performed */
    virtual bool reset() { return false; }
};

} // namespace

Q_DECLARE_INTERFACE(LomiriSystemSettings::PluginInterface,
                    "com.lomiri.SystemSettings.PluginInterface")
Q_DECLARE_INTERFACE(LomiriSystemSettings::PluginInterface2,
                    "com.lomiri.SystemSettings.PluginInterface/2.0")

#endif // SYSTEM_SETTINGS_PLUGIN_INTERFACE_H
