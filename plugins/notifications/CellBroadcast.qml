/*
 * This file is part of system-settings
 *
 * Copyright (C) 2022 UBports Foundation
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

import QtQuick 2.4
import GSettings 1.0
import Lomiri.Components 1.3
import MeeGo.QOfono 0.2
import SystemSettings 1.0


ItemPage {
    id: root
    objectName: "cellBroadcastSettings"

    title: i18n.tr("Emergency Broadcast")
    flickable: scrollWidget

    property var modemsSorted: []
    property var simNames: ["SIM 1", "SIM 2"]
    property bool multiSim: modemsSorted.length > 1
    property string modemPath: ""
    property var topics: []

    // topics == channels numbers
    readonly property var extreme_threats_topics: [4371,4372,4373,4374]
    readonly property var severe_threat_topics: [4375,4376,4377,4378]
    readonly property var amber_topics: [4379]
    readonly property var public_safety_topics: [4396]
    readonly property var test_topics: [4380,4381]

     header: PageHeader {
         title: i18n.tr("Emergency Broadcast")
         extension: Sections {
             model: root.simNames
             height: root.multiSim ? undefined : 0
             onSelectedIndexChanged: {
                 root.modemPath = modemsSorted[selectedIndex];
             }
         }
     }

     function existInRange(topic) {
         const sTopics = ofonoCellBroadcast.topics.split(',')
         let found = false
         for (const t of sTopics) {
             const r = t.split('-').map(Number);
             if (r.length === 1) {
                 if (topic === r[0]){
                     found = true
                 }
             } else {
                 if (topic >= r[0] && topic <= r[1]){
                     found = true
                 }
             }
         }
         return found
     }

     function isSubscribed(targetTopics) {
        return targetTopics.every(existInRange);
     }

     function updateTopics() {
        // ETWS alerts is mandatory and already enabled in ofono
        // CMAS_ALERT_PRESIDENTIAL_LEVEL too, cannot be opt-out
        var t = []
        if (extremeAlert.checked) {
            t = t.concat(extreme_threats_topics)
        }
        if (severeAlert.checked) {
            t = t.concat(severe_threat_topics)
        }
        if (amberAlert.checked) {
            t = t.concat(amber_topics)
        }
        if (publicSafetyAlert.checked) {
            t = t.concat(public_safety_topics)
        }
        if (testsAlert.checked) {
            t = t.concat(test_topics)
        }

        const newTopics = t.join(',')
        if (ofonoCellBroadcast.topics !== newTopics) {
            console.log('set topics to', newTopics)
            ofonoCellBroadcast.topics = newTopics
        }
     }

    OfonoManager {
        id: manager
        onModemsChanged: {
            root.modemsSorted = modems.slice(0).sort();
            root.modemPath = root.modemsSorted[0];
            console.log("root.modemsSorted",root.modemsSorted)
        }
    }

    OfonoCellBroadcast {
        id: ofonoCellBroadcast
        modemPath: root.modemPath
        onReportError: console.log('OfonoCellBroadcast Error:', errorString)

    }

    Label {
        visible: !ofonoCellBroadcast.valid
        anchors.centerIn: parent
        text: i18n.tr("Cell Broadcast is not available")
        elide: Label.ElideRight
        color: theme.palette.normal.baseText
    }

    Flickable {
        id: scrollWidget
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        contentWidth: parent.width
        contentHeight: selectorColumn.implicitHeight
        boundsBehavior: (contentHeight > height) ?
                            Flickable.DragAndOvershootBounds :
                            Flickable.StopAtBounds
        /* Set the direction to workaround https://bugreports.qt-project.org/browse/QTBUG-31905
           otherwise the UI might end up in a situation where scrolling doesn't work */
        flickableDirection: Flickable.VerticalFlick

        Column {
            id: selectorColumn
            visible: ofonoCellBroadcast.valid
            width: parent.width

            ListItem {
                height: cbEnableLayout.height + (divider.visible ? divider.height : 0)
                enabled: false // we can't set it to false ( bug or intentional ? )
                ListItemLayout {
                    id: cbEnableLayout
                    title.text: cbchecked.checked ? i18n.tr("Cell Broadcast Enabled") : i18n.tr("Cell Broadcast Disabled")
                    Switch {
                        id: cbchecked
                        checked: ofonoCellBroadcast.enabled
                        SlotsLayout.position: SlotsLayout.Trailing
                    }
                }
            }

            SettingsItemTitle {
                text: i18n.tr("Alerts:")
            }

            ListItem {
                height: children.height + (divider.visible ? divider.height : 0)
                ListItemLayout {
                    title.text: i18n.tr("Receive emergency notifications")
                    Switch {
                        id: mandatoryAlert
                        enabled: false // mandatory and hard setted in ofono
                        checked: true
                        SlotsLayout.position: SlotsLayout.Trailing
                    }
                }
            }

            ListItem {
                height: children.height + (divider.visible ? divider.height : 0)
                ListItemLayout {
                    title.text: i18n.tr("Extreme threats")
                    subtitle.text: i18n.tr("Extreme threats to life and property")
                    Switch {
                        id: extremeAlert
                        checked: isSubscribed(extreme_threats_topics)
                        SlotsLayout.position: SlotsLayout.Trailing
                        onClicked: updateTopics()
                    }
                }
            }

            ListItem {
                height: children.height + (divider.visible ? divider.height : 0)
                ListItemLayout {
                    title.text: i18n.tr("Severe threats")
                    subtitle.text: i18n.tr("Severe threats to life and property")
                    Switch {
                        id: severeAlert
                        checked: isSubscribed(severe_threat_topics)
                        SlotsLayout.position: SlotsLayout.Trailing
                        onClicked: updateTopics()
                    }
                }
            }

            ListItem {
                height: children.height + (divider.visible ? divider.height : 0)
                ListItemLayout {
                    title.text: i18n.tr("AMBER Alerts")
                    subtitle.text: i18n.tr("Child abduction emergency bulletins")
                    Switch {
                        id: amberAlert
                        checked: isSubscribed(amber_topics)
                        SlotsLayout.position: SlotsLayout.Trailing
                        onClicked: updateTopics()
                    }
                }
            }

            ListItem {
                height: children.height + (divider.visible ? divider.height : 0)
                ListItemLayout {
                    title.text: i18n.tr("Public safety messages")
                    subtitle.text: i18n.tr("Recommended actions to save life and property")
                    Switch {
                        id: publicSafetyAlert
                        checked: isSubscribed(public_safety_topics)
                        SlotsLayout.position: SlotsLayout.Trailing
                        onClicked: updateTopics()
                    }
                }
            }

            ListItem {
                height: children.height + (divider.visible ? divider.height : 0)
                ListItemLayout {
                    title.text: i18n.tr("Tests")
                    subtitle.text: i18n.tr("Tests alerts")
                    Switch {
                        id: testsAlert
                        checked: isSubscribed(test_topics)
                        SlotsLayout.position: SlotsLayout.Trailing
                        onClicked: updateTopics()
                    }
                }
            }

        }
    }
}
