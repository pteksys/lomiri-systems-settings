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

#include "utils.h"
#include "debug.h"

#include <glib.h>
#include <QStandardPaths>
#include <QUrlQuery>
#include <QUrlQuery>


typedef QPair<QString,QString> StringPair;

namespace LomiriSystemSettings {

void parsePluginOptions(const QStringList &arguments, QString &defaultPlugin,
                        QVariantMap &pluginOptions)
{
    for (int i = 1; i < arguments.count(); i++) {
        const QString &argument = arguments.at(i);
        if (argument.startsWith("settings://")) {
            QUrl urlArgument(Utilities::getDestinationUrl(argument));
            /* Find out which plugin is required. If the first component of the
             * path is "system", just skip it. */
            QStringList pathComponents =
                urlArgument.path().split('/', QString::SkipEmptyParts);
            int pluginIndex = 0;
            if (pathComponents.value(pluginIndex, "") == "system")
                pluginIndex++;
            defaultPlugin = pathComponents.value(pluginIndex, QString());
            /* Convert the query parameters into options for the plugin */
            QUrlQuery query(urlArgument);
            Q_FOREACH(const StringPair &pair, query.queryItems()) {
                pluginOptions.insert(pair.first, pair.second);
            }
        } else if (!argument.startsWith('-')) {
            defaultPlugin = argument;
        } else if (argument == "--option" && i + 1 < arguments.count()) {
            QStringList option = arguments.at(++i).split("=");
            // If no value is given, insert an empty string
            pluginOptions.insert(option.at(0), option.value(1, ""));
        }
    }
}

Utilities::Utilities(QObject *parent) :
    QObject(parent)
{
}

QString Utilities::formatSize(quint64 size) const
{
    guint64 g_size = size;

    gchar * formatted_size = g_format_size (g_size);
    QString q_formatted_size = QString::fromLocal8Bit(formatted_size);
    g_free (formatted_size);

    return q_formatted_size;
}

/*
 * This function makes getDestinationUrl invokable from QML.
 */
QString Utilities::mapUrl(const QString &source)
{
    return Utilities::getDestinationUrl(source);
}

/*
 * Returns a destination for the given source if the source has an entry
 * in url-map.ini (based on the first path component, excluding “system”).
 * If there were any query items on the source, these are appended to the
 * destination.

 * If the source had no entry, it's returned unchanged.
 */
QString Utilities::getDestinationUrl(const QString &source)
{
    QString mapFile = QStandardPaths::locate(
        QStandardPaths::GenericDataLocation,
        QStringLiteral("%1/%2").arg(MANIFEST_DIR, "url-map.ini")
    );
    if (Q_UNLIKELY(mapFile.isEmpty())) {
        qWarning() << "could not locate map file";
        return source;
    }

    // This method will be called from multiple threads, and QSettings
    // is reentrant, meaning each call to this function require its own
    // settings instance.
    QSettings map(mapFile, QSettings::IniFormat);
    map.sync();

    // If reading the map failed, return the source unchanged.
    if (map.status() != QSettings::NoError) {
        qWarning() << "reading url map failed: " << map.status();
        return source;
    }

    QUrl sourceUrl(source);
    QStringList pathComponents =
        sourceUrl.path().split('/', QString::SkipEmptyParts);

    int pluginIndex = 0;
    if (pathComponents.value(pluginIndex, "") == "system")
        pluginIndex++;
    QString key = pathComponents.value(pluginIndex, QString());

    map.beginGroup("sources");
    if (map.contains(key)) {
        QString destination = map.value(key, QVariant()).toString();

        // Copy any query items from the source to the destination
        if (sourceUrl.hasQuery()) {
            QUrl destinationUrl = QUrl(destination);
            QUrlQuery sQ(sourceUrl);
            QUrlQuery dQ(destinationUrl);

            // Insert all query items from source query to destination query.
            for (int i = 0; i < sQ.queryItems().size(); ++i) {
                const QPair<QString, QString> a(sQ.queryItems().at(i));
                dQ.addQueryItem(a.first, a.second);
            }

            destinationUrl.setQuery(dQ);
            destination = destinationUrl.toString();
        }
        return destination;
    }
    return source;
}

} // namespace
