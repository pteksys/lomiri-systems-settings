/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015-2016 Canonical Ltd.
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
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Lomiri.Components 1.3


Column {
    anchors {
        left: parent.left
        right: parent.right
        leftMargin: 10
        rightMargin: 10
    }
    spacing: units.gu(1)

    property string category
    property string categoryName

    objectName: "categoryGrid-" + category

    SettingsItemTitle {
        id: header
        text: categoryName
        visible: repeater.count > 0
    }

    AdaptiveContainer {
        id: container
        anchors {
            left: parent.left
            right: parent.right
        }
        layout: "column"
        gridItemWidth: units.gu(12)
        gridColumnSpacing: units.gu(1)
        gridRowSpacing: units.gu(3)

        Behavior on y { LomiriNumberAnimation {}}
        Behavior on height { LomiriNumberAnimation {}}

        Repeater {
            id: repeater

            visible: false // AdaptiveContainer must ignore the Repeater
            model: pluginManager.itemModel(category)

            onCountChanged: {
                // when doing search, the doLayout is called too early.
                // force re-rendering of items
                container.doLayout()
            }

            delegate: Loader {
                id: loader
                property string layout: ""
                sourceComponent: model.item.entryComponent
                visible: model.item.visible
                active: model.item.visible
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: container.border.width
                    rightMargin: container.border.width
                }
                Connections {
                    ignoreUnknownSignals: true
                    target: loader.item
                    function onClicked() {
                        var pageComponent = model.item.pageComponent
                        if (pageComponent) {
                            Haptics.play();
                            loadPluginByName(model.item.baseName);
                        }
                    }
                }
                Binding {
                    target: loader.item
                    property: "color"
                    value: theme.palette.highlighted.background
                    when: currentPlugin == model.item.baseName && apl.columns > 1
                }
                Binding {
                    target: loader.item
                    property: "color"
                    value: "transparent"
                    when: currentPlugin != model.item.baseName || apl.columns == 1
                }
                Binding {
                    target: loader.item
                    property: "layout"
                    value: loader.layout
                }
                Binding {
                    target: loader.item
                    property: "showDivider"
                    value: index != repeater.count-1
                }
                Binding {
                    target: loader.item
                    property: "layout.title.color"
                    value: "white"
                }
                Binding {
                    target: loader.item
                    property: "divider.colorFrom"
                    value: "#3d4cc9"
                }
                Binding {
                    target: loader.item
                    property: "divider.colorTo"
                    value: "#3d4cc9"
                }
                Binding {
                    target: loader.item
                    property: "highlightWhenPressed"
                    value: false
                }

                Component {
                    id: customSwitchStyle
                    CustomSwitchStyle {}
                }

                onStatusChanged: {
                    if (loader.status == Loader.Ready) {
                        console.log(model.item.baseName)
                        console.log(loader.item.text)
                        console.log(loader.item.children.length)

                        if(!loader.item.children.length) {
                            return
                        }

                        for(var i = 0; i < loader.item.children.length; i++) {
                            console.log(loader.item.children[i])
                            console.log(loader.item.children[i].children.length)
                            for(var j = 0; j < loader.item.children[i].children.length; j++) {
                                console.log("    " + loader.item.children[i].children[j])
                                console.log("    " + loader.item.children[i].children[j].children.length)
                                for(var k = 0; k < loader.item.children[i].children[j].children.length; k++) {
                                    console.log("        " + loader.item.children[i].children[j].children[k])
                                    console.log("        " + loader.item.children[i].children[j].children[k].style)
                                    if(loader.item.children[i].children[j].children[k].style !== undefined) {
                                        loader.item.children[i].children[j].children[k].style = customSwitchStyle
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    ListItem {
        divider {
            visible: true
            colorFrom: "#EEEEEE"
            colorTo: "#EEEEEE"
        }
        visible: header.visible && container.layout == "grid"
        height: divider.height
    }
}
