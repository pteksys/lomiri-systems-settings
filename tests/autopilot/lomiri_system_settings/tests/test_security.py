# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2014-2016 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

from gi.repository import Gio
from time import sleep
from testtools.matchers import Equals, NotEquals
from autopilot.matchers import Eventually

from lomiri_system_settings.tests import (
    SecurityBaseTestCase,
    SIM_IFACE)

from lomiri_system_settings.utils.i18n import ugettext as _
from lomiriuitoolkit import emulators as toolkit_emulators


class SecurityTestCase(SecurityBaseTestCase):
    """ Tests for Security Page """

    def setUp(self):
        super(SecurityTestCase, self).setUp()
        prps = self.security_page.get_properties()
        self.use_powerd = prps['usePowerd']
        if self.use_powerd:
            gsettings = Gio.Settings.new('com.lomiri.touch.system')
            prev = gsettings.get_uint('activity-timeout')
            self.addCleanup(
                self.set_prev_activity_timeout, gsettings, prev)
        else:
            gsettings = Gio.Settings.new('org.gnome.desktop.session')
            prev = gsettings.get_uint('idle-delay')
            self.addCleanup(
                self.set_prev_idle_delay, gsettings, prev)

    def set_prev_idle_delay(self, gsettings, prev):
        gsettings.set_uint('idle-delay', prev)
        self.assertThat(
            lambda: int(gsettings.get_uint('idle-delay')),
            Eventually(Equals(prev))
        )

    def set_prev_activity_timeout(self, gsettings, prev):
        gsettings.set_uint('activity-timeout', prev)
        self.assertThat(
            lambda: int(gsettings.get_uint('activity-timeout')),
            Eventually(Equals(prev))
        )

    def _get_activity_timeout(self):
        if self.use_powerd:
            gsettings = Gio.Settings.new('com.lomiri.touch.system')
            prev = gsettings.get_uint('activity-timeout')
            return prev
        else:
            gsettings = Gio.Settings.new('org.gnome.desktop.session')
            prev = gsettings.get_uint('idle-delay')
            return prev

    def _get_dim_timeout(self):
        if self.use_powerd:
            gsettings = Gio.Settings.new('com.lomiri.touch.system')
            prev = gsettings.get_uint('dim-timeout')
            return prev
        else:
            return None

    def _go_to_phone_lock(self):
        selector = self.security_page.select_single(
            objectName='lockingControl'
        )
        self.main_view.scroll_to_and_click(selector)

    def _go_to_sleep_values(self):
        self._go_to_phone_lock()
        selector = self.main_view.select_single(
            objectName='lockTimeout'
        )
        self.main_view.scroll_to_and_click(selector)

    def _get_sleep_selector(self):
        self._go_to_sleep_values()
        sleep_values_page = self.main_view.select_single(
            objectName='sleepValues'
        )
        self.assertThat(
            sleep_values_page,
            NotEquals(None)
        )
        self._go_to_sleep_values()
        sleep_values_page = self.main_view.select_single(
            objectName='sleepValues'
        )
        return sleep_values_page.select_single(
            toolkit_emulators.ItemSelector,
            objectName='sleepSelector'
        )

    def test_security_page(self):
        """ Checks whether Security page is available """
        self.assertThat(
            self.security_page,
            NotEquals(None)
        )
        self.assertThat(
            self.security_page.title,
            Equals(_('Security & Privacy'))
        )

    def test_locking_control_value(self):
        self._go_to_phone_lock()
        actTimeout = self._get_activity_timeout()
        dimTimeout = self._get_dim_timeout()
        activityTimeout = self.main_view.select_single(
            objectName='lockTimeout').value
        if actTimeout is 0:
            self.assertEquals(activityTimeout, ('Manually'))
        elif actTimeout is 60:
            self.assertEquals(
                activityTimeout,
                ('{:d} minute').format(int(actTimeout/60)))
            if dimTimeout:
                self.assertEquals(dimTimeout, actTimeout - 10)
        else:
            self.assertEquals(
                activityTimeout,
                ('After {:d} minutes').format(int(actTimeout/60)))
            if dimTimeout:
                self.assertEquals(dimTimeout, actTimeout - 10)

    def test_phone_lock_page(self):
        self._go_to_phone_lock()
        phone_lock_page = self.main_view.select_single(
            objectName='phoneLockingPage'
        )
        self.assertThat(
            phone_lock_page,
            NotEquals(None)
        )
        self.assertThat(
            phone_lock_page.title,
            Equals(_('Locking and unlocking'))
        )

    def test_lock_security_focus_on_entry(self):
        self._go_to_phone_lock()

        phone_lock_page = self.main_view.select_single(
            objectName='phoneLockingPage')
        selector = phone_lock_page.select_single(objectName='lockSecurity')
        self.main_view.scroll_to_and_click(selector)

        lock_security_page = self.main_view.wait_select_single(
            objectName='lockSecurityPage')

        # Find the selected security method.
        unlock_methods = ['method_swipe', 'method_code', 'method_phrase']
        selected_method = None
        for m in unlock_methods:
            if lock_security_page.wait_select_single(objectName=m).selected:
                selected_method = m

        # If swipe is the selected security, we trigger the dialog by
        # changing the security to method_code
        if selected_method == 'method_swipe':
            dialog_trigger = lock_security_page.wait_select_single(
                objectName='method_code')
            input_selector = 'newInput'
        else:
            # If the security is anything besides swipe, we trigger the current
            # pass dialog by changing the security to swipe.
            dialog_trigger = lock_security_page.wait_select_single(
                objectName='method_swipe')
            input_selector = 'currentInput'

        # Trigger dialog.
        self.main_view.scroll_to_and_click(dialog_trigger)

        # Find the text input.
        dialog = self.main_view.wait_select_single(
            objectName='changeSecurityDialog')
        text_input = dialog.wait_select_single(
            objectName=input_selector)
        self.assertTrue(text_input.focus)

    def test_phone_lock_value(self):
        self._go_to_phone_lock()
        phone_lock_page = self.main_view.select_single(
            objectName='phoneLockingPage'
        )
        actTimeout = self._get_activity_timeout()
        activityTimeout = phone_lock_page.select_single(
            objectName='lockTimeout').value
        if actTimeout is 0:
            self.assertEquals(activityTimeout, ('Never'))
        elif actTimeout is 60:
            self.assertEquals(
                activityTimeout,
                ('After {:d} minute').format(int(actTimeout/60))
            )
        else:
            self.assertEquals(
                activityTimeout,
                ('After {:d} minutes').format(int(actTimeout/60))
            )

    def test_idle_never_timeout(self):
        selector = self._get_sleep_selector()
        to_select = selector.select_single(
            'OptionSelectorDelegate', text='Never')
        self.main_view.pointing_device.click_object(to_select)
        to_select.selected.wait_for(True)
        sleep(1)
        actTimeout = self._get_activity_timeout()
        self.assertEquals(actTimeout, 0)
        selected_delegate = selector.select_single(
            'OptionSelectorDelegate', selected=True)
        self.assertEquals(selected_delegate.text, 'Never')

    def test_idle_change_timeout(self):
        selector = self._get_sleep_selector()
        to_select = selector.select_single(
            'OptionSelectorDelegate', text='After 4 minutes')
        self.main_view.pointing_device.click_object(to_select)
        to_select.selected.wait_for(True)
        sleep(1)
        actTimeout = self._get_activity_timeout()
        self.assertEquals(actTimeout, 240)
        selected_delegate = selector.select_single(
            'OptionSelectorDelegate', selected=True)
        self.assertEquals(selected_delegate.text, 'After 4 minutes')

    def test_sim_pin_control_value(self):
        self.assertEqual('none', self.modem_0.Get(SIM_IFACE, 'PinRequired'))
        self.assertEqual(['pin'], self.modem_0.Get(SIM_IFACE, 'LockedPins'))

        sim_pin_value = self.security_page.select_single(
            objectName='simControl').value

        self.assertThat(
            sim_pin_value,
            Equals(_('On'))
        )


class SecuritySimPinLockedTestCase(SecurityBaseTestCase):
    """ Tests for Security Page with Locked SIM Pin """

    def setUp(self):
        super(SecuritySimPinLockedTestCase, self).setUp()
        self.modem_0.LockPin('pin', '2468')
        self.assertEqual(['pin'], self.modem_0.Get(SIM_IFACE, 'LockedPins'))

    def test_sim_pin_lock_control(self):
        sim_pin_page = self.security_page.go_to_sim_lock()

        switch = sim_pin_page.get_sim_pin_switch(0)
        locked = len(self.modem_0.Get(SIM_IFACE, 'LockedPins')) > 0
        self.assertEquals(locked, switch.checked)

    def test_sim_pin_lock_control_unlock(self):
        sim_pin_page = self.security_page.go_to_sim_lock()
        switch = sim_pin_page.get_sim_pin_switch(0)

        self.assertTrue(switch.checked)

        sim_pin_page.click_sim_pin_switch(0)

        lock_dialog = self.main_view.select_single(
            objectName='lockDialogComponent'
        )
        self.assertEqual(
            lock_dialog.title,
            _("Enter SIM PIN")
        )

        prev_input = self.main_view.select_single(
            objectName='prevInput'
        )
        submit_button = self.main_view.select_single(
            objectName='lockButton'
        )

        self.assertEqual(
            submit_button.text,
            _("Unlock")
        )

        self.assertFalse(
            submit_button.enabled
        )
        self.main_view.scroll_to_and_click(prev_input)
        prev_input.write("246")
        self.assertFalse(
            submit_button.enabled
        )
        prev_input.write("2468")

        self.assertTrue(
            submit_button.enabled
        )

        self.main_view.scroll_to_and_click(submit_button)

        self.assertFalse(switch.checked)

        locked = len(self.modem_0.Get(SIM_IFACE, 'LockedPins')) > 0
        self.assertEquals(locked, switch.checked)

    def test_sim_pin_lock_control_unlock_fail(self):
        sim_pin_page = self.security_page.go_to_sim_lock()
        switch = sim_pin_page.get_sim_pin_switch(0)

        self.assertTrue(
            len(self.modem_0.Get(SIM_IFACE, 'LockedPins')) > 0
        )
        self.assertTrue(switch.checked)

        sim_pin_page.click_sim_pin_switch(0)

        lock_dialog = self.main_view.select_single(
            objectName='lockDialogComponent'
        )
        self.assertEqual(
            lock_dialog.title,
            _("Enter SIM PIN")
        )

        prev_input = self.main_view.select_single(
            objectName='prevInput'
        )
        submit_button = self.main_view.select_single(
            objectName='lockButton'
        )

        self.assertThat(
            submit_button.text,
            Eventually(Equals(_("Unlock")))
        )

        self.assertFalse(
            submit_button.enabled
        )

        self.main_view.scroll_to_and_click(prev_input)
        prev_input.write("1234")

        self.assertTrue(
            submit_button.enabled
        )

        self.main_view.scroll_to_and_click(submit_button)

        self.assertTrue(
            len(self.modem_0.Get(SIM_IFACE, 'LockedPins')) > 0
        )


class SecuritySimPinUnlockedTestCase(SecurityBaseTestCase):
    """ Tests for Security Page with Unlocked SIM Pin """

    def setUp(self):
        super(SecuritySimPinUnlockedTestCase, self).setUp()
        self.modem_0.UnlockPin('pin', '2468')
        self.assertFalse(len(self.modem_0.Get(SIM_IFACE, 'LockedPins')) > 0)

    def test_sim_pin_lock_control_lock(self):
        sim_pin_page = self.security_page.go_to_sim_lock()
        switch = sim_pin_page.get_sim_pin_switch(0)

        self.assertFalse(switch.checked)

        sim_pin_page.click_sim_pin_switch(0)

        lock_dialog = self.main_view.select_single(
            objectName='lockDialogComponent'
        )
        self.assertEqual(
            lock_dialog.title,
            _("Enter Previous SIM PIN")
        )

        prev_input = self.main_view.select_single(
            objectName='prevInput'
        )
        submit_button = self.main_view.select_single(
            objectName='lockButton'
        )

        self.assertEqual(
            submit_button.text,
            _("Lock")
        )

        self.assertFalse(
            submit_button.enabled
        )
        self.main_view.scroll_to_and_click(prev_input)
        prev_input.write("246")
        self.assertFalse(
            submit_button.enabled
        )
        prev_input.write("2468")

        self.assertTrue(
            submit_button.enabled
        )

        self.main_view.scroll_to_and_click(submit_button)

        self.assertTrue(switch.checked)

        self.assertEqual(['pin'], self.modem_0.Get(SIM_IFACE, 'LockedPins'))
        locked = len(self.modem_0.Get(SIM_IFACE, 'LockedPins')) > 0
        self.assertEquals(locked, switch.checked)

    def test_sim_pin_lock_control_lock_fail(self):
        sim_pin_page = self.security_page.go_to_sim_lock()

        self.assertFalse(
            len(self.modem_0.Get(SIM_IFACE, 'LockedPins')) > 0
        )

        sim_pin_page.click_sim_pin_switch(0)

        lock_dialog = self.main_view.select_single(
            objectName='lockDialogComponent'
        )
        self.assertEqual(
            lock_dialog.title,
            _("Enter Previous SIM PIN")
        )

        prev_input = self.main_view.select_single(
            objectName='prevInput'
        )
        submit_button = self.main_view.select_single(
            objectName='lockButton'
        )

        self.assertThat(
            submit_button.text,
            Eventually(Equals(_("Lock")))
        )

        self.assertFalse(
            submit_button.enabled
        )
        self.main_view.scroll_to_and_click(prev_input)
        prev_input.write("1234")

        self.assertTrue(
            submit_button.enabled
        )

        self.main_view.scroll_to_and_click(submit_button)

        self.assertFalse(
            len(self.modem_0.Get(SIM_IFACE, 'LockedPins')) > 0
        )
