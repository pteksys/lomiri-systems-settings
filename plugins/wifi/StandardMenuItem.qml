/*
 * Copyright 2013-2016 Canonical Ltd.
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
 *
 * Authors:
 *      Nick Dedekind <nick.dedekind@canonical.com>
 */

import QtQuick 2.12
import Ubuntu.Components 1.3

FramedMenuItem {
    id: menuItem

    property bool checkable: false
    property bool checked: false

    signal activate()

    layout.subtitle.text: checked ? i18n.tr("Connected") : ""

    progressionVisible: checked
    onClicked: {
        if (!checked) {
            menuItem.activate();
        }
    }
}
