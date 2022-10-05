/*
 * Copyright (C) 2013-2014 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Iain Lane <iain.lane@canonical.com>
 *
*/

#ifndef TIMEDATE_H
#define TIMEDATE_H

#include "timezonelocationmodel.h"

#include <QAbstractTableModel>
#include <QDBusInterface>
#include <QDBusServiceWatcher>
#include <QObject>
#include <QProcess>
#include <gio/gio.h>

class TimeDate : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QString timeZone
                READ timeZone
                NOTIFY timeZoneChanged)
    Q_PROPERTY (QString timeZoneName
                READ timeZoneName
                NOTIFY timeZoneChanged)
    Q_PROPERTY (QAbstractItemModel *timeZoneModel
                READ getTimeZoneModel
                CONSTANT)
    Q_PROPERTY (QString filter
                READ getFilter
                WRITE setFilter)
    Q_PROPERTY(bool useNTP
               READ useNTP
               WRITE setUseNTP
               NOTIFY useNTPChanged)
    Q_PROPERTY (bool listUpdating
                READ getListUpdating
                NOTIFY listUpdatingChanged)

public:
    explicit TimeDate(QObject *parent = 0);
    ~TimeDate();
    Q_INVOKABLE void setTimeZone (const QString &time_zone, const QString &time_zone_name = QString());
    QString timeZone();
    QString timeZoneName();
    bool useNTP();
    QAbstractItemModel *getTimeZoneModel();
    QString getFilter();
    void setFilter (QString &filter);
    void setUseNTP(bool enabled);
    Q_INVOKABLE void setTime (qlonglong new_time);
    bool getListUpdating();

public Q_SLOTS:
    void slotChanged(QString, QVariantMap, QStringList);
    void slotNameOwnerChanged(QString, QString, QString);

Q_SIGNALS:
    void timeZoneChanged();
    void timeZoneModelChanged();
    void useNTPChanged();
    void listUpdatingChanged();

private:
    bool m_useNTP;
    QString m_currentTimeZone;
    QString m_currentTimeZoneName;
    QDBusConnection m_systemBusConnection;
    QDBusServiceWatcher m_serviceWatcher;
    QDBusInterface m_timeDateInterface;
    GSettings *m_indicatorSettings;
    QString m_objectPath;
    TimeZoneLocationModel m_timeZoneModel;
    QString m_filter;
    void initializeTimeZone();
    bool getUseNTP();
    void setUpInterface();

};

#endif // TIMEDATE_H
