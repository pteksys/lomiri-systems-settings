/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
 *
 * Contact: Sebastien Bacher <sebastien.bacher@canonical.com>
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
import Lomiri.Components 1.3
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Lomiri.SystemSettings.Sound 1.0
import Lomiri.Settings.Menus 0.1 as Menus
import Lomiri.Settings.Components 0.1 as LSC
import QMenuModel 1.0
import Lomiri.Connectivity 1.0

import "utilities.js" as Utilities

ItemPage {
    id: root

    objectName: "soundPage"

    title: i18n.tr("Sound")
    flickable: scrollWidget

    LomiriSoundPanel { id: backendInfo }

    GSettings {
        id: keyboardSettings

        schema.id: "com.lomiri.keyboard.maliit"
    }

    GSettings {
        id: soundSettings
        schema.id: "com.lomiri.touch.sound"
    }

    Connections {
        id: networkingStatus
        target: NetworkingStatus
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ?
                            Flickable.DragAndOvershootBounds :
                            Flickable.StopAtBounds
        /* Set the direction to workaround https://bugreports.qt-project.org/browse/QTBUG-31905
           otherwise the UI might end up in a situation where scrolling doesn't work */
        flickableDirection: Flickable.VerticalFlick

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            SettingsListItems.Standard {
                text: i18n.tr("Silent Mode")

                Switch {
                    id: silentModeSwitch
                    objectName: "silentMode"
                    property bool serverChecked: soundActionGroup.silentMode.state

                    LSC.ServerPropertySynchroniser {
                        userTarget: silentModeSwitch
                        userProperty: "checked"
                        serverTarget: silentModeSwitch
                        serverProperty: "serverChecked"

                        onSyncTriggered: function(arg) { soundActionGroup.silentMode.activate(arg) }
                    }
                }
            }

            Column  {
                anchors.left: parent.left
                anchors.right: parent.right
                visible: NetworkingStatus.modemAvailable

                SettingsItemTitle {
                    text: i18n.tr("Ringer volume:")
                }

                QDBusActionGroup {
                    id: soundActionGroup
                    busType: DBus.SessionBus
                    busName: "org.ayatana.indicator.sound"
                    objectPath: "/org/ayatana/indicator/sound"

                    property variant volume: action("volume")
                    property variant silentMode: action("silent-mode")
                    property variant highVolume: action("high-volume")

                    Component.onCompleted: start()
                }

                Menus.SliderMenu {
                    id: volumeSlider
                    objectName: "sliderMenu"
                    enabled: soundActionGroup.volume.state != null
                    minimumValue: 0.0
                    maximumValue: 1.0
                    minIcon: "image://theme/audio-volume-low-zero"
                    maxIcon: "image://theme/audio-volume-high"

                    property real serverValue: soundActionGroup.volume.state

                    LSC.ServerPropertySynchroniser {
                        userTarget: volumeSlider
                        userProperty: "value"
                        serverTarget: volumeSlider
                        serverProperty: "serverValue"
                        maximumWaitBufferInterval: 16

                        onSyncTriggered: soundActionGroup.volume.updateState(value);
                    }
                }

                SettingsListItems.Standard {
                    id: highVolumeWarning
                    visible: soundActionGroup.highVolume.state == true
                    color: theme.palette.normal.negative
                    text: i18n.tr("High volume can damage your hearing.")
                    divider.visible: false
                }

                // with highVolumeWarning not visible there would be no divider -> enforce a divider here
                SettingsListItems.Divider {}

                SettingsItemTitle {
                    text: i18n.tr("Phone calls:")
                }

                SettingsListItems.SingleValueProgression {
                    text: i18n.tr("Ringtone")
                    value: Utilities.buildDisplayName(backendInfo.incomingCallSound)
                    divider.visible: false

                    onClicked: pageStack.addPageToNextColumn(root,
                        Qt.resolvedUrl("SoundsList.qml"), {
                            title: i18n.tr("Ringtone"),
                            showStopButton: true,
                            soundType: 0,
                            soundsDir: "sounds/lomiri/ringtones/"
                        }
                    )
                }
                SettingsListItems.ThinDivider {}

                SettingsListItems.Standard {
                    CheckBox {
                        objectName: "callVibrate"
                        SlotsLayout.position: SlotsLayout.First
                        property bool serverChecked: backendInfo.incomingCallVibrate
                        onServerCheckedChanged: checked = serverChecked
                        Component.onCompleted: checked = serverChecked
                        onTriggered: backendInfo.incomingCallVibrate = checked
                    }
                    text: i18n.tr("Vibrate on ring")
                    divider.visible: false
                }
                SettingsListItems.ThinDivider {}

                SettingsListItems.Standard {
                    CheckBox {
                        objectName: "callVibrateSilentMode"
                        SlotsLayout.position: SlotsLayout.First
                        property bool serverChecked: backendInfo.incomingCallVibrateSilentMode
                        onServerCheckedChanged: checked = serverChecked
                        Component.onCompleted: checked = serverChecked
                        onTriggered: backendInfo.incomingCallVibrateSilentMode = checked
                    }
                    text: i18n.tr("Vibrate in Silent Mode")
                    divider.visible: false
                }
                SettingsListItems.ThinDivider {}

                SettingsListItems.Standard {
                    Switch {
                        objectName: "dialpadSounds"
                        property bool serverChecked: backendInfo.dialpadSoundsEnabled
                        onServerCheckedChanged: checked = serverChecked
                        Component.onCompleted: checked = serverChecked
                        onTriggered: backendInfo.dialpadSoundsEnabled = checked
                    }
                    text: i18n.tr("Dialpad tones")
                }
            }

            Column  {
                anchors.left: parent.left
                anchors.right: parent.right

                SettingsItemTitle {
                    text: i18n.tr("Messages:")
                }

                visible: !isSnap || showAllUI

                SettingsListItems.SingleValueProgression {
                    text: i18n.tr("Message sound")
                    value:Utilities.buildDisplayName(
                              backendInfo.incomingMessageSound)
                    divider.visible: false
                    onClicked: pageStack.addPageToNextColumn(root,
                        Qt.resolvedUrl("SoundsList.qml"), {
                            title: i18n.tr("Message sound"),
                            soundType: 1,
                            soundsDir:  "sounds/lomiri/notifications/"
                        }
                    )
                }
                SettingsListItems.ThinDivider {}

                SettingsListItems.Standard {
                    CheckBox {
                        objectName: "messageVibrate"
                        SlotsLayout.position: SlotsLayout.First
                        property bool serverChecked: backendInfo.incomingMessageVibrate
                        onServerCheckedChanged: checked = serverChecked
                        Component.onCompleted: checked = serverChecked
                        onTriggered: backendInfo.incomingMessageVibrate = checked
                    }
                    text: i18n.tr("Vibrate with message sound")
                    divider.visible: false
                }
                SettingsListItems.ThinDivider {}

                SettingsListItems.Standard {
                    CheckBox {
                        objectName: "messageVibrateSilentMode"
                        SlotsLayout.position: SlotsLayout.First
                        property bool serverChecked: backendInfo.incomingMessageVibrateSilentMode
                        onServerCheckedChanged: checked = serverChecked
                        Component.onCompleted: checked = serverChecked
                        onTriggered: backendInfo.incomingMessageVibrateSilentMode = checked
                    }
                    text: i18n.tr("Vibrate in Silent Mode")
                }
            }

            SettingsItemTitle {
                text: i18n.tr("Other sounds:")
            }

            SettingsListItems.Standard {
                text: i18n.tr("Keyboard sound")
                divider.visible: false

                Switch {
                    objectName: "keyboardSoundSwitch"
                    property bool serverChecked: keyboardSettings.keyPressFeedback
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: keyboardSettings.keyPressFeedback = checked
                }
            }
            SettingsListItems.ThinDivider {}

            SettingsListItems.Standard {
                id: lockSound

                Switch {
                    checked: false
                }
                text: i18n.tr("Lock sound")
                visible: showAllUI
                divider.visible: false
            }
            SettingsListItems.ThinDivider {
                visible: showAllUI
            }

            /* Add some artificial spacing here, once we update the QtQuick.Layouts plugin
             * to 1.3 we can replace this with Layout.bottomMargin(units.gu(2))
             */
            Item { height: 1; width: 1 }

            SettingsListItems.Standard {
                text: i18n.tr("Other vibrations")

                Switch {
                    objectName: "otherVibrateSwitch"
                    property bool serverChecked: backendInfo.otherVibrate
                    onServerCheckedChanged: checked = serverChecked
                    Component.onCompleted: checked = serverChecked
                    onTriggered: backendInfo.otherVibrate = checked
                }
            }
        }
    }
}
