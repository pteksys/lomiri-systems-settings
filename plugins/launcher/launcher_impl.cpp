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

#include "launcher_impl.h"

#include <QApplication>
#include <QGuiApplication>
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
#include <QScreen>
#endif

LauncherPanelPluginImpl::LauncherPanelPluginImpl(QObject *parent)
    : LauncherPanelPlugin(parent)
{
    auto app = (QApplication*) QGuiApplication::instance();
    m_desktopWidget = app->desktop();
    connect(m_desktopWidget, SIGNAL(screenCountChanged(int)),
            this, SIGNAL(screensChanged(int)));
}

LauncherPanelPluginImpl::~LauncherPanelPluginImpl()
{
    disconnect(m_desktopWidget, SIGNAL(screenCountChanged(int)),
               this, SIGNAL(screensChanged(int)));
}

QRect LauncherPanelPluginImpl::screenGeometry(const int &screen) const
{
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    return m_desktopWidget->screenGeometry(screen);
#else
    return QGuiApplication::screens().at(screen)->geometry(); ;
#endif
}

int LauncherPanelPluginImpl::getCurrentScreenNumber() const
{
    return m_desktopWidget->screenNumber(m_desktopWidget);
}

int LauncherPanelPluginImpl::screens() const
{
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    return m_desktopWidget->screenCount();
#else
     return QGuiApplication::screens().count();
#endif
}
