/*
 * Copyright (C) 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Ken VanDine <ken.vandine@canonical.com>
 *
*/

import QtQuick 2.12
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Lomiri.Components 1.3

ItemPage {
    id: preview
    anchors.fill: parent

    property string uri

    // whether an image was just imported from e.g. contentHub
    property bool imported: false
    signal save
    property Item headerStyle: header.__styleInstance ?
                                   header.__styleInstance : null

    Component.onCompleted: {
        /* change the header text color to make it more readable over the background */
        if (headerStyle.hasOwnProperty("textColor"))
            headerStyle.textColor = Theme.palette.selected.foregroundText;
    }

    Component.onDestruction: {
        if (headerStyle.hasOwnProperty("textColor"))
            headerStyle.textColor = Theme.palette.selected.backgroundText;
    }

    // keep the ItemPage components title, which is a predefined property, to avoid errors about translations
    title: pageHeader.title
    // override the ItemPage components header to allow actions in the header
    header: PageHeader {
        id: pageHeader
        title: i18n.tr("Preview")

        trailingActionBar { actions: [
                    Action {
                        text: i18n.tr("Set")
                        iconName: "tick"
                        onTriggered: {
                            save();
                            pageStack.removePages(preview);
                        }
                    }]
                }
    }

    Image {
        id: previewImage
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        source: uri
        autoTransform: true
        sourceSize.height: height
        sourceSize.width: 0
        fillMode: Image.PreserveAspectCrop
    }
}
