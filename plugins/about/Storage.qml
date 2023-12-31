/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
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

import GSettings 1.0
import QtQuick 2.12
import QtSystemInfo 5.0
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Lomiri.Components 1.3
import Lomiri.SystemSettings.StorageAbout 1.0

ItemPage {
    id: storagePage
    objectName: "storagePage"
    title: i18n.tr("Storage")

    Column {
        anchors.centerIn: parent
        visible: progress.running
        spacing: units.gu(2)
        Label {
            anchors {
                left: parent.left
                right: parent.right
            }
            horizontalAlignment: Text.AlignHCenter
            text: i18n.tr("Scanning")
        }
        ActivityIndicator {
            id: progress
            visible: running
            running: !pageLoader.visible
        }
    }

    Loader {
        id: pageLoader
        anchors.fill: parent
        asynchronous: true
        visible: status == Loader.Ready
        sourceComponent: Item {
            anchors.fill: parent
            property var allDrives: {
                var drives = ["/"] // Always consider /
                var paths = [backendInfo.getDevicePath("/")]
                var systemDrives = backendInfo.mountedVolumes
                for (var i = 0; i < systemDrives.length; i++) {
                    var drive = systemDrives[i]
                    var path = backendInfo.getDevicePath(drive)
                    if (paths.indexOf(path) == -1 && // Haven't seen this device before
                        path.charAt(0) === "/") { // Has a real mount point
                        drives.push(drive)
                        paths.push(path)
                    }
                }
                return drives
            }
            property real diskSpace: {
                var space = 0
                for (var i = 0; i < allDrives.length; i++) {
                    space += backendInfo.getTotalSpace(allDrives[i])
                }
                return space
            }
            /* Limit the free space to the user available one (see bug #1374134) */
            property real freediskSpace: {
                return backendInfo.getFreeSpace("/home")
            }
            property real userdata: backendInfo.getTotalSpace("/userdata")

            property real usedBySystem: userdata -
                                        freediskSpace -
                                        backendInfo.homeSize -
                                        backendInfo.totalClickSize
            // Files in the home direcoty but not in any of the other categories
            property real otherSize: userdata -
                                     freediskSpace -
                                     usedBySystem -
                                     clickAndAppDataSize -
                                     backendInfo.moviesSize -
                                     backendInfo.audioSize -
                                     backendInfo.picturesSize -
                                     backendInfo.documentsSize -
                                     backendInfo.downloadsSize -
                                     backendInfo.anboxSize -
                                     backendInfo.libertineSize
            property real clickAndAppDataSize: backendInfo.totalClickSize +
                                               backendInfo.appCacheSize +
                                               backendInfo.appConfigSize +
                                               backendInfo.appDataSize -
                                               backendInfo.libertineSize
            property real reserved: diskSpace - userdata
            //TODO: Let's consider use unified colors in a ¿file?
            property variant spaceColors: [
                "#b05738",
                "#f67936",
                "#f8ae46",
                "#edb55f",
                "#d1c35f",
                "#9dd93f",
                "#82ce5f",
                "#76a8b6",
                "#8891c1",
                "#a77eaa",
                "#cb556c"]
            property variant spaceLabels: [
                i18n.tr("System Reserved"),
                i18n.tr("Used by system"),
                i18n.tr("Videos"),
                i18n.tr("Audio"),
                i18n.tr("Pictures"),
                i18n.tr("Documents"),
                i18n.tr("Downloads"),
                i18n.tr("Anbox"),
                i18n.tr("Libertine"),
                i18n.tr("Other files"),
                i18n.tr("Used by apps")]
            property variant spaceValues: [
                reserved, // used by the partition layout
                usedBySystem, // Used by Ubuntu
                backendInfo.moviesSize,
                backendInfo.audioSize,
                backendInfo.picturesSize,
                backendInfo.documentsSize,
                backendInfo.downloadsSize,
                backendInfo.anboxSize,
                backendInfo.libertineSize,
                otherSize, //Other Files
                clickAndAppDataSize]
            property variant spaceObjectNames: [
                "systemReservedItem",
                "usedBySystemItem",
                "moviesItem",
                "audioItem",
                "picturesItem",
                "documentsItem",
                "downloadsItem",
                "anboxItem",
                "libertineItem",
                "otherFilesItem",
                "usedByAppsItem"]

            GSettings {
                id: settingsId
                schema.id: "com.lomiri.lomiri-system-settings"
            }

            LomiriStorageAboutPanel {
                id: backendInfo
                property bool ready: false
                // All of these events come simultaneously
                onMoviesSizeChanged: ready = true
                Component.onCompleted: populateSizes()
                sortRole: settingsId.storageSortByName ?
                              ClickRoles.DisplayNameRole :
                              ClickRoles.InstalledSizeRole

            }

            Flickable {
            id: scrollWidget
            anchors.fill: parent
            contentHeight: columnId.height

            Component.onCompleted: storagePage.flickable = scrollWidget

            Column {
                id: columnId
                anchors.left: parent.left
                anchors.right: parent.right

                SettingsListItems.SingleValue {
                    id: diskItem
                    objectName: "diskItem"
                    text: i18n.tr("Total storage")
                    value: Utilities.formatSize(diskSpace)
                    showDivider: false
                }

                StorageBar {
                    ready: backendInfo.ready
                }

                Repeater {
                    model: spaceColors

                    StorageItem {
                        objectName: spaceObjectNames[index]
                        colorName: modelData
                        label: spaceLabels[index]
                        value: spaceValues[index]
                        ready: backendInfo.ready
                    }
                }

                StorageItem {
                    objectName: "storageItem"
                    colorName: theme.palette.normal.foreground
                    label: i18n.tr("Free space")
                    value: freediskSpace
                    ready: backendInfo.ready
                }

                ListItem {
                    objectName: "installedAppsItemSelector"
                    height: layout.height + (divider.visible ? divider.height : 0)
                    divider.visible: false
                    SlotsLayout {
                        id: layout
                        mainSlot: OptionSelector {
                            id: valueSelect
                            width: parent.width - 2 * (layout.padding.leading + layout.padding.trailing)
                            model: [i18n.tr("By name"), i18n.tr("By size")]
                            onSelectedIndexChanged:
                                settingsId.storageSortByName = (selectedIndex == 0)
                                                               // 0 → by name, 1 → by size
                        }
                    }
                }

                Binding {
                    target: valueSelect
                    property: 'selectedIndex'
                    value: (backendInfo.sortRole === ClickRoles.DisplayNameRole) ?
                            0 :
                            1
                }

                ListView {
                    objectName: "installedAppsListView"
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: childrenRect.height
                    /* Deactivate the listview flicking, we want to scroll on the
                     * column */
                    interactive: false
                    model: backendInfo.clickList
                    delegate: ListItem {
                        objectName: "appItem" + displayName
                        height: appItemLayout.height + (divider.visible ? divider.height : 0)

                        ListItemLayout {
                            id: appItemLayout
                            title.text: displayName
                            height: units.gu(6)

                            IconWithFallback {
                                SlotsLayout.position: SlotsLayout.First
                                height: units.gu(4)
                                source: iconPath
                                fallbackSource: "image://theme/clear"
                            }
                            Label {
                                SlotsLayout.position: SlotsLayout.Last
                                horizontalAlignment: Text.AlignRight
                                text: installedSize ?
                                        Utilities.formatSize(installedSize) :
                                        i18n.tr("N/A")
                            }
                        }
                    }
                }
            }
        }
        }
    }
}
