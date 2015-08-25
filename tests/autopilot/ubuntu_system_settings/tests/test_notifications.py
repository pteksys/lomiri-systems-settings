# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

import json
import os
import subprocess
import time

from testtools.matchers import Equals, NotEquals

from ubuntu_system_settings.tests import UbuntuSystemSettingsTestCase

from ubuntuuitoolkit import emulators as toolkit_emulators

""" Tests for Ubuntu System Settings """


def has_helper(package):
    """Return True if  package['hooks']['foo']['push-helper'] exists"""
    return any(package['hooks'][hook].get('push-helper', None)
               for hook in package['hooks'])


class NotificationsTestCases(UbuntuSystemSettingsTestCase):
    """ Tests for Search """

    def setUp(self):
        # Check legacy items: one for each file in legacy-helpers
        super(NotificationsTestCases, self).setUp()

    def test_item_counts(self):
        """ Checks whether the Notificatins category is available """
        try:
            legacy_count = len(
                os.listdir("/usr/lib/ubuntu-push-client/legacy-helpers/"))
        except os.FileNotFoundError:
            legacy_count = 0
        # Get output of click list --manifest, and parse for the rest
        packages = json.loads(
            subprocess.check_output(
                ['click', 'list', '--manifest']).decode('utf8'))
        click_count = len([x for x in packages if has_helper(x)])

        notif = self.main_view.select_single(
            objectName='entryComponent-notifications')
        self.assertThat(notif, NotEquals(None))
        self.main_view.pointing_device.click_object(notif)
        # Have to wait until the model loads
        time.sleep(1)
        notif_page = self.main_view.wait_select_single(
            objectName='systemNotificationsPage')
        items = notif_page.select_many(toolkit_emulators.Standard)
        self.assertThat(len(items), Equals(click_count + legacy_count))
