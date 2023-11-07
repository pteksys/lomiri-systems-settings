/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
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

import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Lomiri.Components 1.3
import Lomiri.Components.Themes.Ambiance 1.3

MainView {
    id: main
    // on phone, this prevent the app from starting with 2 columns and then resize to 1 column
    implicitWidth: Screen.width >= units.gu(90) ? units.gu(140) : units.gu(45)
    implicitHeight: units.gu(90)
    applicationName: "lomiri-system-settings"
    objectName: "systemSettingsMainView"
    automaticOrientation: true
    anchorToKeyboard: true
    property var pluginManager: PluginManager {}
    property string currentPlugin: ""

    /* Workaround for lp:1648801, i.e. APL does not support a placeholder,
    so we implement it here. */
    property string placeholderPlugin: "about"

    function loadPluginByName(pluginName, pluginOptions) {
        var plugin = pluginManager.getByName(pluginName)
        var opts = { plugin: plugin,
                     pluginManager: pluginManager }

        if (pluginOptions)
            opts.pluginOptions = pluginOptions

        if (plugin) {
            // Got a valid plugin name - load it
            var pageComponent = plugin.pageComponent
            var page;
            if (pageComponent) {
                apl.removePages(apl.primaryPage);
                page = apl.addComponentToNextColumnSync(
                    apl.primaryPage, pageComponent, opts
                );
                currentPlugin = pluginName;
                page.Component.destruction.connect(function () {
                    if (currentPlugin == this.baseName) {
                        currentPlugin = "";
                    }
                }.bind(plugin))
            }
            return true
        } else {
            // Invalid plugin
            console.log("Plugin " + pluginName + " does not exist.")
            return false
        }
    }

    Component.onCompleted: {
        i18n.domain = "lomiri-system-settings"
        i18n.bindtextdomain("lomiri-system-settings", i18nDirectory)

        if (defaultPlugin) {
            if (!loadPluginByName(defaultPlugin, pluginOptions))
                Qt.quit()
        } else if (apl.columns > 1) {
            loadPluginByName(placeholderPlugin);
            aplConnections.target = apl;
        }
    }

    Connections {
        id: aplConnections
        ignoreUnknownSignals: true
        onColumnsChanged: {
            var columns = target.columns;
            if (columns > 1 && !currentPlugin) {
                loadPluginByName(placeholderPlugin);
            } else if (columns == 1 && currentPlugin == placeholderPlugin) {
                apl.removePages(apl.primaryPage);
            }
        }
    }

    Connections {
        target: UriHandler
        onOpened: {
            var url = String(uris);
            url = Utilities.mapUrl(url);
            var panelAndOptions = url.replace("settings:///system/", "")
            var optionIndex = panelAndOptions.indexOf("?")
            var panel = optionIndex > -1 ?
                        panelAndOptions.substring(0, optionIndex) :
                        panelAndOptions
            var urlParams = {}
            // Parse URL options
            // From http://stackoverflow.com/a/2880929
            if (optionIndex > -1) { // Got options
                var match,
                    // Regex for replacing addition symbol with a space
                    pl     = /\+/g,
                    search = /([^&=]+)=?([^&]*)/g,
                    decode = function (s) {
                        return decodeURIComponent(s.replace(pl, " "))
                    }
                while (match = search.exec(
                           panelAndOptions.substring(optionIndex + 1)))
                       urlParams[decode(match[1])] = decode(match[2])

                loadPluginByName(panel, urlParams)
            } else {
                loadPluginByName(panel)
            }
        }
    }

    USSAdaptivePageLayout {
        id: apl
        objectName: "apl"
        anchors.fill: parent
        primaryPage: mainPage
        layouts: [
            PageColumnsLayout {
                when: width >= units.gu(90)
                PageColumn {
                    minimumWidth: units.gu(40)
                    maximumWidth: units.gu(50)
                    preferredWidth: units.gu(50)
                }
                PageColumn {
                    fillWidth: true
                }
            }
        ]

        Page {
            id: mainPage
            objectName: "systemSettingsPage"
            visible: false
            header: standardHeader

            Rectangle {
                anchors.fill: parent
                color: "#4868d5"
            }

            PageHeader {
                id: standardHeader
                objectName: "standardHeader"
                // turn off automatic height
                // so when in APL the height doesn't change
                automaticHeight: false
                visible: mainPage.header === standardHeader
                title: i18n.tr("System Settings")
                flickable: mainFlickable

                StyleHints {
                    foregroundColor: "white"
                    backgroundColor: "#4868d5"
                    dividerColor: "#4868d5"
                }

                style: PageHeaderStyle {
                    contentHeight: 100
                }
            }

            Flickable {
                id: mainFlickable
                anchors.fill: parent
                contentHeight: contentItem.childrenRect.height
                boundsBehavior: (contentHeight > mainPage.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds
                /* Set the direction to workaround https://bugreports.qt-project.org/browse/QTBUG-31905
                   otherwise the UI might end up in a situation where scrolling doesn't work */
                flickableDirection: Flickable.VerticalFlick

                Column {
                    anchors.left: parent.left
                    anchors.right: parent.right

                    Item {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        height: 40

                        Rectangle {
                            anchors.fill: parent
                            anchors.leftMargin: 10
                            anchors.rightMargin: 10
                            color: "transparent"
                            border.color: "white"
                            border.width: 2
                            radius: 10

                            RowLayout {
                                anchors.fill: parent

                                Item {
                                    Layout.preferredWidth: 10
                                }

                                Image {
                                    Layout.preferredHeight: parent.height / 3
                                    Layout.preferredWidth: height
                                    source: "qrc:/qml/magnifyingglass.svg"
                                }

                                TextField {
                                    Layout.fillWidth: true
                                    Layout.alignment: Qt.AlignVCenter

                                    id: searchField
                                    objectName: "searchField"
                                    inputMethodHints: Qt.ImhNoPredictiveText
                                    onDisplayTextChanged: pluginManager.filter = displayText
                                    hasClearButton: false

                                    color: "white"
                                    style: TextFieldStyle {
                                        background: Item {
                                            Label {
                                                anchors.fill: parent
                                                anchors.leftMargin: 10
                                                color: "white"
                                                text: i18n.tr("Search")
                                                verticalAlignment: Qt.AlignVCenter
                                                visible: searchField.text == ""
                                            }
                                        }
                                    }
                                }

                                Item {
                                    Layout.preferredWidth: 10
                                }
                            }
                        }
                    }

                    UncategorizedItemsView {
                        model: pluginManager.itemModel("uncategorized-top")
                    }

                    CategorySection {
                        category: "network"
                        categoryName: i18n.tr("Controls")
                    }

                    CategorySection {
                        category: "personal"
                        categoryName: i18n.tr("General")
                    }

                    CategorySection {
                        category: "system"
                        categoryName: i18n.tr("System")
                    }

                    UncategorizedItemsView {
                        model: pluginManager.itemModel("uncategorized-bottom")
                    }
                }
            }
        }
    }
}
