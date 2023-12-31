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
 * Iain Lane <iain.lane@canonical.com>
 *
*/

import QtQuick 2.12
import GSettings 1.0
import SystemSettings 1.0
import Lomiri.Content 1.3
import Lomiri.Components 1.3
import Lomiri.Components.ListItems 1.3 as ListItem
import SystemSettings.ListItems 1.0 as SettingsListItems
import Lomiri.Components.Popups 1.3
import Lomiri.SystemSettings.Background 1.0
import "utilities.js" as Utilities

ItemPage {
    id: mainPage
    objectName: "backgroundPage"
    flickable: sources
    title: i18n.tr("Background & Appearance")

    signal save (string uri)

    readonly property string welcomeBackground: backgroundPanel.backgroundFile

    property var activeTransfer

    function preview(props) {
        var page = pageStack.addFileToNextColumnSync(
            mainPage, Qt.resolvedUrl("Preview.qml"), props
        );
        selectedItemConnection.target = page;
    }

    // Action to import image
    Action {
        id: selectPeer
        // when action has been activated, push the picker on the stack
        onTriggered: pageStack.addPageToNextColumn(mainPage, picker)
    }

    // Action at cancel selection mode
    Action {
        id: noAction
        // empty action when cancel is pressed in selection mode
        // needed changes happen by State changes
    }

    // qml bindings for background stuff
    LomiriBackgroundPanel {
        id: backgroundPanel
    }

    GSettings {
        id: shellSettings
        schema.id: "com.lomiri.Shell"
    }

    Flickable {
        id: sources

        anchors {
            fill: parent
        }
        visible: true
        contentHeight: sourceColumn.height + sourceColumn.anchors.bottomMargin

        Column {
            id: sourceColumn
            anchors {
                left: parent.left
                right: parent.right
            }

            SettingsListItems.Standard {
                text: i18n.tr("Enable blur effects")
                Switch {
                    checked: shellSettings.enableBlur
                    onClicked: shellSettings.enableBlur = checked
                }
                showDivider: true
            }

            SettingsItemTitle {
                text: i18n.tr("Background image:")
            }

            WallpaperGrid {
                id: systemArtGrid
                objectName: "SystemArtGrid"
                anchors.left: parent.left
                anchors.right: parent.right
                columns: 3
                bgmodel: backgroundPanel.systemArt
                backgroundPanel: backgroundPanel
                title: i18n.tr("System art")
                current: welcomeBackground
                onSelected: preview({ uri: uri })
            }
            ListItem.ThinDivider { }

            WallpaperGrid {
                id: customGrid
                objectName: "customArtGrid"
                anchors.left: parent.left
                anchors.right: parent.right
                columns: 3
                bgmodel: backgroundPanel.customBackgrounds
                backgroundPanel: backgroundPanel
                title: i18n.tr("Custom")
                current: welcomeBackground
                editable: true
                isCustom: true
                onSelected: preview({ uri: uri })
            }
            ListItem.Divider { }
        }
    }

    Connections {
        id: contentHubConnection
        property var imageCallback
        target: activeTransfer ? activeTransfer : null
        onStateChanged: {
            if (activeTransfer.state === ContentTransfer.Charged) {
                if (activeTransfer.items.length > 0) {
                    var imageUrl = activeTransfer.items[0].url;
                    imageCallback(imageUrl);
                }
            }
        }
    }

    // set up connections
    Connections {
        id: selectedItemConnection
        onSave: {
            Utilities.setBackground(target.uri)
        }
        onStateChanged: {
            var trans = mainPage.activeTransfer;
            if (target.state === "saved") {
                save(target.uri);

                // if a transfer is done, clean up
                if (trans && trans.state === ContentTransfer.Collected) {
                    trans.state = ContentTransfer.Finalized;
                }
            }

            if ((target.state === "cancelled") &&
                (trans && trans.state === ContentTransfer.Collected)) {

                if (target.imported) {
                    // if we just did an import, remove the image if the user
                    // cancels
                    backgroundPanel.rmFile(target.uri);
                } else {
                    backgroundPanel.prepareBackgroundFile(target.uri, true);
                }
                trans.state = ContentTransfer.Finalized;
            }
        }
    }

    Page {
        id: picker
        visible: false
        header: PageHeader {
            title: i18n.tr("Choose application")
        }

        ContentStore {
            id: appStore
            scope: ContentScope.App
        }

        ContentPeerPicker {
            id: peerPicker
            visible: parent.visible
            handler: ContentHandler.Source
            contentType: ContentType.Pictures

            onPeerSelected: {
                pageStack.removePages(picker);
                // requests an active transfer from peer
                function startContentTransfer(callback) {
                    if (callback)
                        contentHubConnection.imageCallback = callback
                    var transfer = peer.request(appStore);
                    if (transfer !== null) {
                        mainPage.activeTransfer = transfer;
                    }
                }
                peer.selectionType = ContentTransfer.Single;
                // when peer has been selected, request a transfer, providing
                // a callback that pushes the preview stack
                startContentTransfer(function(uri) {
                    preview({ uri: uri, imported: true });
                });
            }

            onCancelPressed: pageStack.removePages(picker)
        }
    }

    ContentTransferHint {
        anchors.fill: parent
        activeTransfer: mainPage.activeTransfer
    }
}
