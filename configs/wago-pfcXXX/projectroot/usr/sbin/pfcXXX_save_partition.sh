#!/bin/bash

#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
#-----------------------------------------------------------------------------#

. /etc/config-tools/board_specific_defines
. "$(dirname $0)/${BOARD_ID}_commons"

modeLabel="$1"
filename="$2"
destinationDir="$3"
statusFile="${4:-}"

# Generates a newline separated list of file names on stdout
# This list contains all files and directories
# that shall be excluded from the tar
GenIgnoredFileList()
{
  # /home is ignored because it's backed up separately via "codesys" command
  IGNORED_DIRS="./lost+found ./log/* ./proc/* ./sys/* ./home/* ./media/* ./mnt/* ./tmp/* ./run/* ./var/run/* ./var/log/* ./var/lock/* ./var/tmp/*"
  PRESERVED_DEV_FILES="dev/console\\|dev/zero\\|dev/null"
  test -z "$statusFile" || IGNORED_DIRS="${IGNORED_DIRS} ${statusFile} ${statusFile}.tmp"

  (
    # copy ignored patterns to the list (everything we ignore except /dev)
    echo "$IGNORED_DIRS"
    # copy /dev/* to the list
    cd / && find ./dev/ -mindepth 1 -maxdepth 1
 ) | sed "\,$PRESERVED_DEV_FILES,d; s/ /\\n/g"
}

# print the estimated total size of uncompressed backup data on stdout
totalSize()
{
  case "${modeLabel}" in
  codesys)
    echo "$((1024 * $(du -xs /home | awk '{print $1}') ))"
    ;;
  rootfs)
    echo "$((1024 * $(du -xs / | awk '{print $1}') ))"
    ;;
  *)
    # Not reached. In theory.
    # Anyway, avoid division by 0
    echo "1"
  esac
}

# print backup data as tar on stdout
backupData()
{
  # exec 2>/dev/null
  case "${modeLabel}" in
    codesys)
      tar cpf - -C /home ./
      ;;
    rootfs)
      IGNORED_FILE_LIST=/tmp/ignored_file_list.txt
      GenIgnoredFileList > "${IGNORED_FILE_LIST}"
      tar cpf - -C / --exclude-from="${IGNORED_FILE_LIST}" ./
      rm -f "${IGNORED_FILE_LIST}"
      ;;
  esac
}

# Display progress bar, zip and optionally encrypt the
# backup data to stdout.
backupPackEncrypt()
{
  (
    if [ -x "$(which pv)" ]; then
      # show progress bar
      case "${modeLabel}" in
        codesys) label=CodeSys ;;
        rootfs) label=System ;;
      esac

      totalSize="$(totalSize)"
      backupData | pv -N "$label" -s "$totalSize" -p -t -w 70
    else
      backupData
    fi
  ) | gzip -1 | $ENCRYPT
}

# Disable standard error handler to pass error to firmware_backup
trap - EXIT
# Backup file may contain sensitive data.
umask 137

dest="${destinationDir}/${filename}.tgz$ENC_EXT"
: > "$dest"
chgrp www "$dest" 2>/dev/null ||:

if [ -n "$statusFile" ]; then
  # Execute backup in the background and regularily update
  # status file for WBM
  backupPackEncrypt > "${dest}" &
  pid="$!"
  while [ -d "/proc/$pid" ]; do
    size="$(ls -s "$dest")"
    echo "Creating archive: size ${size% *} bytes." > "${statusFile}.tmp"
    chgrp www "${statusFile}.tmp" && mv "${statusFile}.tmp" "${statusFile}"
    sleep 1
  done

  wait "$pid"
else
  backupPackEncrypt > "${dest}"
fi
