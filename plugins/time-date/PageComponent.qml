/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
 *
 * Contact: Iain Lane <iain.lane@canonical.com>
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
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Lomiri.Components 1.3
import Lomiri.Components.ListItems 1.3 as ListItems
import Lomiri.Components.Popups 1.3
import Lomiri.SystemSettings.TimeDate 1.1

ItemPage {
    id: root
    title: i18n.tr("Time and Date")
    objectName: "timeDatePage"
    flickable: scrollWidget

    function getUTCOffset() {
        // We get the difference in minutes between UTC and our TZ (UTC - TZ)
        // but we want it in hours between our TZ and UTC (TZ - UTC), so divide
        // by -60 to invert and convert to hours.
        var offset = new Date().getTimezoneOffset() / -60
        var plus = offset >= 0 ? "+" : ""
        return "UTC" + plus + offset
    }

    LomiriTimeDatePanel {
        id: timeDatePanel
        onTimeZoneChanged: {
            // Inform the JS engine that the TZ has been updated
            Date.timeZoneUpdated()
            timeZone.value = getUTCOffset()
        }
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds
        flickableDirection: Flickable.VerticalFlick

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            SettingsItemTitle { text: i18n.tr ("Time zone:") }

            SettingsListItems.SingleValueProgression {
                objectName: "timeZone"
                id: timeZone
                text: timeDatePanel.timeZoneName
                value: getUTCOffset()
                onClicked: pageStack.addPageToNextColumn(root,
                    Qt.resolvedUrl("ChooseTimeZone.qml"), {
                        timeDatePanel: timeDatePanel
                    }
                )
            }

            SettingsItemTitle {
                text: i18n.tr ("Set the time and date:")
            }

            ListItems.ItemSelector {
                id: setTimeAutomatically
                objectName: "timeItemSelector"
                model: [ i18n.tr("Automatically") , i18n.tr("Manually")]
                expanded: true
                onSelectedIndexChanged: {
                    var useNTP = (selectedIndex === 0) // 0 = Automatically
                    timeDatePanel.useNTP = useNTP
                }
            }

            Binding {
                target: setTimeAutomatically
                property: "selectedIndex"
                value: timeDatePanel.useNTP ? 0 : 1
            }

            Timer {
                onTriggered: currentTime.text = Qt.formatDateTime(
                                new Date(),
                                Qt.DefaultLocaleLongDate)
                triggeredOnStart: true
                repeat: true
                running: true
            }

            Component {
                id: timePicker
                TimePicker {}
            }

            SettingsListItems.StandardProgression {
                id: currentTime
                objectName: "currentTime"
                progressionVisible: setTimeAutomatically.selectedIndex === 1 // Manually
                enabled: progressionVisible
                onClicked: {
                    Qt.inputMethod.hide()
                    var popupObj = PopupUtils.open(timePicker);
                    popupObj.accepted.connect(
                                function(hour, minute, second,
                                         day, month, year) {
                                    var newDate =  new Date(year,
                                                            month,
                                                            day,
                                                            hour,
                                                            minute,
                                                            second)
                                    // Milliseconds to microseconds
                                    timeDatePanel.setTime(newDate.getTime() * 1000)
                    })
                }
            }
        }
    }
}
