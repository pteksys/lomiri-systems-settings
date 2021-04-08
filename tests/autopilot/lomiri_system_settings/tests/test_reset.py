# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from __future__ import absolute_import

from time import sleep

from autopilot.matchers import Eventually
from gi.repository import Gio, GLib
from testtools.matchers import Contains, Equals

from lomiri_system_settings.tests import ResetBaseTestCase
from lomiri_system_settings.utils.i18n import ugettext as _


class ResetTestCase(ResetBaseTestCase):
    """Tests for Reset Page"""

    def set_unity_launcher(self, gsettings, key, value):
        gsettings.set_value(key, value)
        # wait for gsettings
        sleep(1)

    def test_reset_page_title_is_correct(self):
        """Checks whether Reset page is available"""
        self.assertThat(
            self.reset_page.title,
            Equals(_('Reset device')))

    def test_reset_launcher(self):
        gsettings = Gio.Settings.new('com.canonical.Unity.Launcher')

        favorites = gsettings.get_value('favorites')
        gsettings.set_value(
            'favorites',
            GLib.Variant('as', ['application://nautilus.desktop']))
        self.addCleanup(
            self.set_unity_launcher, gsettings, 'favorites', favorites)

        items = gsettings.get_value('items')
        gsettings.set_value(
            'items', GLib.Variant('as', ['application:///dialer-app.desktop']))
        self.addCleanup(
            self.set_unity_launcher, gsettings, 'items', items)

        self.reset_page.reset_launcher()
        self.assertThat(
            lambda: gsettings.get_value('favorites'),
            Eventually(Equals(gsettings.get_default_value('favorites'))))

        self.assertThat(
            lambda: gsettings.get_value('items'),
            Eventually(Equals(gsettings.get_default_value('items'))))

    def test_factory_reset(self):
        self.reset_page.erase_and_reset_everything()
        self.assertThat(
            lambda: str(self.sys_mock.GetCalls()),
            Eventually(Contains('FactoryReset')))
