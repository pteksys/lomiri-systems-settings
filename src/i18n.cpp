/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@canonical.com>
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

#define NO_TR_OVERRIDE
#include "i18n.h"

#include <libintl.h>

namespace LomiriSystemSettings {

void initTr(const char *domain, const char *localeDir)
{
    bindtextdomain(domain, localeDir);
    textdomain(domain);
}

QString _(const char *text, const char *domain)
{
    return QString::fromUtf8(dgettext(domain, text));
}

} // namespace

