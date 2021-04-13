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

#include "debug.h"
#include "i18n.h"
#include "plugin-manager.h"
#include "utils.h"

#include <QByteArray>
#include <QApplication>
#include <QProcessEnvironment>
#include <QQmlContext>
#include <QUrl>
#include <QQuickView>
#include <QtGlobal>
#include <QtQml>
#include <QtQml/QQmlDebuggingEnabler>
static QQmlDebuggingEnabler debuggingEnabler(false);

using namespace LomiriSystemSettings;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QByteArray mountPoint = qEnvironmentVariableIsSet("SNAP") ? qgetenv("SNAP") : "";
    bool isSnap = !mountPoint.isEmpty();

    // Ensure printing environment is correct.
    qputenv("QT_PRINTER_MODULE", "cupsprintersupport");

    /* read environment variables */
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    if (environment.contains(QLatin1String("SS_LOGGING_LEVEL"))) {
        bool isOk;
        int value = environment.value(
            QLatin1String("SS_LOGGING_LEVEL")).toInt(&isOk);
        if (isOk)
            setLoggingLevel(value);
    }

    initTr(I18N_DOMAIN, nullptr);
    /* HACK: force the theme until lp #1098578 is fixed */
    QIcon::setThemeName("suru");

    /* Parse the commandline options to see if we've been given a panel to load,
     * and other options for the panel.
     */
    QString defaultPlugin;
    QVariantMap pluginOptions;
    parsePluginOptions(app.arguments(), defaultPlugin, pluginOptions);

    QQuickView view;
    Utilities utils;
    QObject::connect(view.engine(), SIGNAL(quit()), &app, SLOT(quit()),
                     Qt::QueuedConnection);
    qmlRegisterType<QAbstractItemModel>();
    qmlRegisterType<LomiriSystemSettings::PluginManager>("SystemSettings", 1, 0, "PluginManager");
    view.engine()->rootContext()->setContextProperty("Utilities", &utils);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.engine()->addImportPath(mountPoint + PLUGIN_PRIVATE_MODULE_DIR);
    view.engine()->addImportPath(mountPoint + PLUGIN_QML_DIR);
    view.rootContext()->setContextProperty("defaultPlugin", defaultPlugin);
    view.rootContext()->setContextProperty("mountPoint", mountPoint);
    view.rootContext()->setContextProperty("isSnap", isSnap);
    view.rootContext()->setContextProperty("i18nDirectory", mountPoint + I18N_DIRECTORY);
    view.rootContext()->setContextProperty("pluginOptions", pluginOptions);
    view.rootContext()->setContextProperty("view", &view);
    view.setSource(QUrl("qrc:/qml/MainWindow.qml"));
    view.show();

    return app.exec();
}
