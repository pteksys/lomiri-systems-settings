/*
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

import QtQuick 2.12
import SystemSettings 1.0
import Lomiri.Components 1.3
import Lomiri.Components.ListItems 1.3 as ListItem
import Lomiri.SystemSettings.Gestures 1.0

ItemPage {
    id: root
    title: i18n.tr("Gestures")
    flickable: gesturesFlickable

    Flickable {
        id: gesturesFlickable
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height
        boundsBehavior: (contentHeight > root.height) ?
                            Flickable.DragAndOvershootBounds :
                            Flickable.StopAtBounds
        /* Workaround https://bugreports.qt-project.org/browse/QTBUG-31905 */
        flickableDirection: Flickable.VerticalFlick

        Column {
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.Standard {
                id: doubleTapToWake
                text: i18n.tr("Tap to wake")
                visible: DbusHelper.isDT2WSupported
                control: Switch {
                    id: doubleTapToWakeSwitch
                    objectName: "doubleTapToWake"
                    checked: DbusHelper.getDT2WEnabled()
                    SlotsLayout.position: SlotsLayout.Trailing
                    onCheckedChanged: {
                        DbusHelper.setDT2WEnabled(checked)
                    }
                }
                showDivider: false
            }

            Label {
                anchors {
                    left:parent.left
                    right:parent.right
                    leftMargin: units.gu(2)
                    rightMargin: units.gu(2)
                }
                text: i18n.tr("Double-tap anywhere on the screen to wake device.")
                wrapMode: Text.WordWrap
                visible: doubleTapToWake.visible
            }

            ListItem.Standard {
                id: divider
                height: divider.height + units.gu(2)
                visible: doubleTapToWake.visible
                showDivider: true
            }
        }
    }
}
