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

#pragma once

#include <QObject>
#include <QtDBus>

/**
 * Communication between system-settings and repowerd.
 */

class GesturesDbusHelper final : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isDT2WSupported
        READ isDT2WSupported
        NOTIFY isDT2WSupportedChanged)

public:
    explicit GesturesDbusHelper(QObject *parent = nullptr);
    ~GesturesDbusHelper() {};

public Q_SLOTS:
    bool isSupported();

    bool isDT2WSupported();
    bool getDT2WEnabled();
    void setDT2WEnabled(bool value);

Q_SIGNALS:
    void isDT2WSupportedChanged();

private Q_SLOTS:
    void handleDT2WEnabledChanged(bool enabled);
    void handleDT2WEnabledDone();
    void handleDT2WEnabledError(QDBusError error);

private:
    QDBusInterface* m_unityScreenInterface = nullptr;

    bool m_dt2wSupported = false;
};
