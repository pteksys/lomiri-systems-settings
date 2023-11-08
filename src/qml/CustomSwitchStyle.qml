/*
 * Copyright 2015 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.4
import Lomiri.Components 1.3

Item {
    id: switchStyle

    property real thumbPadding: 4

    implicitWidth: units.gu(7)
    implicitHeight: units.gu(4)
    LayoutMirroring.enabled: false
    LayoutMirroring.childrenInherit: true

    property color checkedBackgroundColor: "#4a54da"
    property color secondaryCheckedBackgroundColor: "#8726c1"
    property color thumbColor: "white"

    Rectangle {
        id: background
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: switchStyle.checkedBackgroundColor }
            GradientStop { position: 1.0; color: switchStyle.secondaryCheckedBackgroundColor }
        }
        clip: true
        radius: height / 2

        Rectangle {
            id: thumb
            states: [
                State {
                    name: "checked"
                    when: styledItem.checked
                    AnchorChanges {
                        target: thumb
                        anchors.left: undefined
                        anchors.right: parent.right
                    }
                }
            ]

            transitions: [
                // Avoid animations on width changes (during initialization)
                // by explicitly setting from and to for the Transitions.
                Transition {
                    from: "*"
                    to: "checked"
                    reversible: true
                    enabled: animated
                    AnchorAnimation {
                        targets: thumb
                        duration: LomiriAnimation.FastDuration
                        easing: LomiriAnimation.StandardEasing
                    }
                    ColorAnimation {
                        target: thumb
                        properties: "backgroundColor"
                        duration: LomiriAnimation.FastDuration
                        easing: LomiriAnimation.StandardEasing
                    }
                }
            ]

            color: switchStyle.thumbColor
            width: (background.width - switchStyle.thumbPadding * 3.0) / 2.0
            height: width
            radius: width / 2
            anchors {
                top: parent.top
                bottom: parent.bottom
                left: parent.left
                topMargin: switchStyle.thumbPadding
                bottomMargin: switchStyle.thumbPadding
                leftMargin: switchStyle.thumbPadding
                rightMargin: switchStyle.thumbPadding
            }
        }
    }
}
