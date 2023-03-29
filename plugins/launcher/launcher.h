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
 */

#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QObject>
#include <QRect>

class LauncherPanelPlugin : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int screens READ screens NOTIFY screensChanged)
public:
    explicit LauncherPanelPlugin(QObject *parent = nullptr)
        : QObject(parent) {};
    virtual ~LauncherPanelPlugin() {};
    // Return index of screen at which USS is currently rendered.
    virtual int screens() const = 0;
public Q_SLOTS:
    virtual QRect screenGeometry(const int &screen = -1) const = 0;
    virtual int getCurrentScreenNumber() const = 0;
Q_SIGNALS:
    void screensChanged(int newCount);
};

#endif // LAUNCHER_H
