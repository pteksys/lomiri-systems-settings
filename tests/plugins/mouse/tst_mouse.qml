/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
 *
 * Contact: Ken VanDine <ken.vandine@canonical.com>
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
import QtQuick 2.4
import QtTest 1.0
import Lomiri.Components 1.3
import Ubuntu.Test 0.1

import Source 1.0

Item {
    id: testRoot
    width: units.gu(50)
    height: units.gu(90)

    TapArea {
        id: area
        height: 48
        width: height
        doubleTapSpeed: 200
    }

    UbuntuTestCase {
        name: "MouseTapAreaTest"
        id: test1
        when: windowShown

        function init() {
            tryCompare(area.button, "enabled", true);
        }
        function test_double_click_success() {
            mouseClick(area);
            wait(10);
            mouseClick(area);
            compare(area.message, i18n.tr("Double-clicked"));
        }
        function test_double_click_fail() {
            var timer = findInvisibleChild(testRoot, "clickTimer");
            mouseClick(area);
            tryCompare(timer, "running", false);
            mouseClick(area);
            compare(area.message, i18n.tr("Not fast enough"));
        }
        function test_double_click_safety() {
            mouseClick(area);
            wait(220);
            mouseClick(area);
            verify(!area.button.enabled);
        }
    }
}
