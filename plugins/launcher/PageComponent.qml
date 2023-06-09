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

import GSettings 1.0
import QtQuick 2.12
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Ubuntu.Components 1.3
import Ubuntu.Settings.Components 0.1 as USC
import Ubuntu.Settings.Menus 0.1 as Menus
import Ubuntu.SystemSettings.Launcher 1.0

ItemPage {
    id: root
    objectName: "launcherPage"
    title: i18n.tr("Launcher")
    flickable: flick

    /* Some settings are only displayed when a large display is available since desktop mode
    isn't really used much on smaller screens. */
    property bool largeScreenAvailable: {
        var currentScreen = LauncherPanelPlugin.getCurrentScreenNumber();
        for (var i=0; i < LauncherPanelPlugin.screens; i++) {
            if (LauncherPanelPlugin.screenGeometry(i).width > units.gu(90)) {
                return true;
            }
        }
        return false; // No large screens.
    }

    Flickable {
        id: flick
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ?
            Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

        Column {
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: units.gu(1)

            SettingsItemTitle {
                text: i18n.tr("On desktop mode:")
                objectName: "largeScreenLabel"
                visible: largeScreenAvailable
            }

            SettingsListItems.Standard {
                id: alwaysShowLauncher
                objectName: "alwaysShowLauncher"
                text: i18n.tr("Always show the launcher")
                layout.subtitle.text: i18n.tr("Videos and full-screen games may hide it temporarily.")
                visible: largeScreenAvailable

                Switch {
                    id: alwaysShowLauncherSwitch
                    objectName: "alwaysShowLauncherSwitch"
                    checked: !unity8Settings.autohideLauncher
                    onTriggered: unity8Settings.autohideLauncher = !checked
                }
            }

            Label {
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: units.gu(2)
                }
                text: i18n.tr("Icon size:")
                height: units.gu(6)
                verticalAlignment: Text.AlignVCenter
            }

            ListItem {

                Button {
                    id: widthResetButton
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: units.gu(2)

                    action: Action {
                        text: i18n.tr("Reset")
                        onTriggered: {
                            unity8Settings.schema.reset("launcherWidth")
                        }
                    }
                }

                Slider {
                    id: iconWidth
                    objectName: "iconWidth"
                    function formatValue(v) { return v.toFixed(2) }
                    minimumValue: 6
                    maximumValue: 12
                    value: unity8Settings.launcherWidth
                    live: true

                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: widthResetButton.right
                        right: parent.right
                        margins: units.gu(2)
                    }

                    property real serverValue: unity8Settings.launcherWidth
                    USC.ServerPropertySynchroniser {
                        userTarget: iconWidth
                        userProperty: "value"
                        serverTarget: iconWidth
                        serverProperty: "serverValue"
                        maximumWaitBufferInterval: 16

                        onSyncTriggered: unity8Settings.launcherWidth = value
                    }
                }
            }
        }
    }

    GSettings {
        id: unity8Settings
        objectName: "unity8Settings"
        schema.id: "com.canonical.Unity8"
    }
}
