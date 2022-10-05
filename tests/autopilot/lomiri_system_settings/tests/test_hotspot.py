# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical Ltd.
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from autopilot.matchers import Eventually
from testtools.matchers import Equals

from lomiri_system_settings.tests import HotspotBaseTestCase

from lomiri_system_settings.tests.connectivity import (
    PRIV_IFACE as CTV_PRIV_IFACE, NETS_IFACE as CTV_NETS_IFACE
)


class HotspotSetupTestCase(HotspotBaseTestCase):

    connectivity_parameters = {
        'HotspotEnabled': False,
        'HotspotStored': False,
        'WifiEnabled': True,
        'HotspotSwitchEnabled': True
    }

    def test_setup(self):
        ssid = 'bar'
        password = 'zomgzomg'
        config = {'ssid': ssid, 'password': password}

        self.hotspot_page.setup_hotspot(config)

        # Assert that the switch is on.
        self.assertTrue(self.hotspot_page.get_hotspot_status())

        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotEnabled'),
            Eventually(Equals(True))
        )

        self.assertThat(
            lambda: bytearray(
                self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotSsid')
            ).decode('UTF-8'),
            Eventually(Equals(ssid))
        )

        self.assertThat(
            lambda: self.ctv_private.Get(CTV_PRIV_IFACE, 'HotspotPassword'),
            Eventually(Equals(password))
        )

        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotStored'),
            Eventually(Equals(True))
        )

    def test_insecure_setup(self):
        ssid = 'bar'
        auth = 'none'
        config = {'ssid': ssid, 'auth': auth}

        self.hotspot_page.setup_hotspot(config)

        # Assert that the switch is on.
        self.assertTrue(self.hotspot_page.get_hotspot_status())

        self.assertThat(
            lambda: self.ctv_private.Get(CTV_PRIV_IFACE, 'HotspotAuth'),
            Eventually(Equals(auth))
        )

        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotStored'),
            Eventually(Equals(True))
        )


class HotspotExistsTestCase(HotspotBaseTestCase):

    connectivity_parameters = {
        'HotspotStored': True,
        'WifiEnabled': True,
        'HotspotSwitchEnabled': True
    }

    def test_enabling(self):
        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotEnabled'),
            Eventually(Equals(False))
        )

        self.hotspot_page.enable_hotspot()

        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotEnabled'),
            Eventually(Equals(True))
        )

    def test_changing(self):
        ssid = 'bar'
        config = {'ssid': ssid}
        self.hotspot_page.setup_hotspot(config)

        self.assertThat(
            lambda: bytearray(
                self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotSsid')
            ).decode('UTF-8'),
            Eventually(Equals(ssid))
        )


class HotspotRunningTestCase(HotspotBaseTestCase):

    connectivity_parameters = {
        'HotspotStored': True,
        'HotspotEnabled': True,
        'WifiEnabled': True,
        'HotspotSwitchEnabled': True
    }

    def test_disabling(self):

        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotEnabled'),
            Eventually(Equals(True))
        )

        self.hotspot_page.disable_hotspot()

        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotEnabled'),
            Eventually(Equals(False))
        )


class HotspotChangeNoWiFiTestCase(HotspotBaseTestCase):

    connectivity_parameters = {
        'HotspotStored': True,
        'HotspotEnabled': False,
        'WifiEnabled': False,
        'HotspotSwitchEnabled': True
    }

    def test_enabling(self):

        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotEnabled'),
            Eventually(Equals(False))
        )

        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'WifiEnabled'),
            Eventually(Equals(False))
        )

        self.hotspot_page.enable_hotspot()

        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotEnabled'),
            Eventually(Equals(True))
        )

        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'WifiEnabled'),
            Eventually(Equals(True))
        )


class HotspotSetupNoWiFiTestCase(HotspotBaseTestCase):

    connectivity_parameters = {
        'HotspotStored': False,
        'HotspotEnabled': False,
        'WifiEnabled': False,
        'HotspotSwitchEnabled': True
    }

    def test_setup(self):
        ssid = 'bar'
        config = {'ssid': ssid}

        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotStored'),
            Eventually(Equals(False))
        )

        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'WifiEnabled'),
            Eventually(Equals(False))
        )

        self.hotspot_page.setup_hotspot(config)

        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotStored'),
            Eventually(Equals(True))
        )

        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'HotspotEnabled'),
            Eventually(Equals(True))
        )

        self.assertThat(
            lambda: self.ctv_nets.Get(CTV_NETS_IFACE, 'WifiEnabled'),
            Eventually(Equals(True))
        )


class HotspotChangeInFlightModeTestCase(HotspotBaseTestCase):

    connectivity_parameters = {
        'HotspotStored': True,
        'HotspotEnabled': False,
        'WifiEnabled': True,
        'FlightMode': True,
        'HotspotSwitchEnabled': False
    }

    def test_switch_disabled(self):
        self.assertFalse(self.hotspot_page.get_hotspot_possible())


class HotspotSetupInFlightModeTestCase(
        HotspotBaseTestCase):

    connectivity_parameters = {
        'HotspotStored': False,
        'HotspotEnabled': False,
        'WifiEnabled': True,
        'FlightMode': True,
        'HotspotSwitchEnabled': False
    }

    def test_setup_disabled(self):
        setup = self.hotspot_page.select_single(
            objectName='hotspotSetupButton'
        )
        self.assertFalse(setup.enabled)
        self.assertFalse(self.hotspot_page.get_hotspot_possible())
