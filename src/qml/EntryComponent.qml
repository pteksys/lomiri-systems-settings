/*
 * This file is part of system-settings
 *
 * Copyright (C) 2020 UBports Foundation
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
import Lomiri.Components 1.3

Item {
    id: root

    property string layout: ""
    property string text: i18n.dtr(model.item.translations, model.displayName)
    property string iconSource: model.icon
    property var color: "transparent"

    signal clicked

    objectName: "entryComponent-" + model.item.baseName
    implicitHeight: childrenRect.height

    Component {
        id: listComponent
        EntryComponentList {
            text: root.text
            iconSource: root.iconSource
            color: root.color
            onClicked: root.clicked()
            Behavior on opacity { LomiriNumberAnimation {}}
        }
    }

    Component {
        id: gridComponent
        EntryComponentGrid {
            text: root.text
            iconSource: root.iconSource
            color: root.color
            onClicked: root.clicked()
            Behavior on opacity { LomiriNumberAnimation {}}
        }
    }

    Loader {
        width: parent.width
        active: root.layout !== ""
        sourceComponent: root.layout == "column" ? listComponent : gridComponent
    }
}
