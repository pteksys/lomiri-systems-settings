/*
 * Copyright (C) 2013 Canonical Ltd
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
 * Łukasz 'sil2100' Zemczak <lukasz.zemczak@canonical.com>
 *
*/

import QtQuick 2.12
import Ubuntu.Components 1.3

Item {
    id: overlayImage

    property string source

    Image {
        anchors.fill: parent
        source: overlayImage.source
        sourceSize.width: parent.width
        sourceSize.height: parent.height
        fillMode: Image.PreserveAspectCrop
        verticalAlignment: Image.AlignTop
        smooth: true
    }
}
