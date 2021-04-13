/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.4
import Lomiri.Components 1.3

Rectangle {
    anchors.fill: parent
    color: "#333130"
    Label {
        id: backgroundText

        anchors.fill: parent
        verticalAlignment: Text.AlignVCenter
        text: i18n.tr("Clear")
        fontSize: "medium"
        color: "#e8e1d0"
        font.bold: true
    }

    states: [
        State {
            name: "SwipingRight"
            PropertyChanges {
                target: backgroundText
                anchors.rightMargin: units.gu(3)
                anchors.leftMargin: 0
                horizontalAlignment: Text.AlignRight

            }
        },
        State {
            name: "SwipingLeft"
            PropertyChanges {
                target: backgroundText
                anchors.rightMargin: 0
                anchors.leftMargin: units.gu(3)
                horizontalAlignment: Text.AlignLeft
            }
        }
    ]
}
