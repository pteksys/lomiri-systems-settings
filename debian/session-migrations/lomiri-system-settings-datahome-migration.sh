#!/bin/sh -euf

# Copyright (C) 2023 UBports Foundation
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; version 3.
#
# This program is distributed in the hope that it will be useful, but WITHOUTa
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Authored by: Ratchanan Srirattanamet <ratchanan@ubports.com>

XDG_DATA_HOME=${XDG_DATA_HOME:-${HOME}/.local/share}

SOURCE_DIR="${XDG_DATA_HOME}/ubuntu-system-settings/"
DEST_DIR="${XDG_DATA_HOME}/lomiri-system-settings/"

if ! [ -e "$SOURCE_DIR" ]; then
    echo "${SOURCE_DIR} doesn't exist. Nothing to migrate."
    exit 0
fi

# session-migration says we should be idempotent. We simply check if the target
# directory exists as to not overwrite the new data.
if [ -e "$DEST_DIR" ]; then
    echo "${DEST_DIR} exists. Perhaps the migration has already" \
         "happened, and/or the user has modified additional files."
    exit 0
fi

# Copy files using "cp -a" to preserve as much as possible. Won't wipe the
# old data, in case the migration goes wrong.

cp -a "$SOURCE_DIR" "$DEST_DIR"

echo "Migrated data for lomiri-system-settings successfully."
