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

#ifndef SYSTEM_SETTINGS_UTILS_H
#define SYSTEM_SETTINGS_UTILS_H

#include <QSettings>
#include <QStringList>
#include <QVariantMap>

namespace LomiriSystemSettings {

void parsePluginOptions(const QStringList &arguments, QString &defaultPlugin,
                        QVariantMap &pluginOptions);

class Utilities : public QObject {
    Q_OBJECT

public:
    explicit Utilities(QObject *parent = 0);
    Q_INVOKABLE QString formatSize(quint64) const;
    Q_INVOKABLE QString mapUrl(const QString &source);
    static QString getDestinationUrl(const QString &source);
};

} // namespace

#endif // SYSTEM_SETTINGS_UTILS_H
