#!/bin/bash

#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
#-----------------------------------------------------------------------------#

. /etc/config-tools/board_specific_defines
. $(dirname $0)/${BOARD_ID}_commons

#
# This slightly awkward construction allows to generate a list of files to
# exclude from backup which can be passed to tar after removing a couple of
# needed ones from the list
#

# /home is ignored because it's backed up separately via "codesys" command
IGNORED_DIRS="./lost+found"
EMPTY_DIRS="./log ./proc ./sys ./home ./media ./mnt ./tmp ./run ./var/run ./var/log ./var/lock ./var/tmp"
PRESERVED_DEV_FILES="dev/console dev/zero dev/null"

IGNORED_FILE_LIST=/tmp/ignored_file_list.txt

modeLabel=$1
filename=$2
destinationDir=$3
statusFile=${4:-""}

umask 137

function GenIgnoredFileList
{
  rm -f ${IGNORED_FILE_LIST}

  # copy ignored patterns to the list (everything we ignore except /dev)
  for dir in ${EMPTY_DIRS}; do
    echo "$dir/*" >> ${IGNORED_FILE_LIST}
  done

  for dir in ${IGNORED_DIRS}; do
    echo "$dir" >> ${IGNORED_FILE_LIST}
  done
  
  # now copy /dev/* to the list
  cd / && find ./dev/ -mindepth 1 -maxdepth 1 >> ${IGNORED_FILE_LIST}

  # remove basic nodes from the list
  for file in ${PRESERVED_DEV_FILES}; do
    sed -i "\,${file},d" ${IGNORED_FILE_LIST}
  done
}

status=$SUCCESS

case ${modeLabel} in
  "codesys")
    > ${destinationDir}/${filename}.tgz
    chgrp www ${destinationDir}/${filename}.tgz || true
    if [[ "$(which pv)" != "" ]]; then
      # show progress bar
      totalSize=$((1024 * $(du -xs /home | awk '{print $1}') ))
      { tar 2>/dev/null cp -C /home -f - ./ | \
      pv -N "codesys" -s $totalSize -p -t -w 70 | gzip > ${destinationDir}/${filename}.tgz ; } || exit 1
    else
      tar 2>/dev/null czpf ${destinationDir}/${filename}.tgz -C /home ./ || exit 1
    fi
    ;;
  "rootfs")

    GenIgnoredFileList

    status=$INTERNAL_ERROR 

    status=$SUCCESS

    if [[ "$statusFile" == "" ]]; then
        # cbm/console: show progress bar using pv

        totalSize=$((1024 * $(du -xs / | awk '{print $1}') ))
        # tar uses 512 bytes per file. We need to take it into account so that
        # the progress bar doesn't stop at 90-something %
        tarMetadataOverhead=$(( 512 * $(find / -xdev | wc -l) ))

        > ${destinationDir}/${filename}.tgz
        chgrp www ${destinationDir}/${filename}.tgz || true
        { tar 2>/dev/null cp -C / --exclude-from=${IGNORED_FILE_LIST} -f - ./ | \
            pv -N "system" -tps $(( $totalSize + $tarMetadataOverhead)) -w 70 | \
            gzip -1 > ${destinationDir}/${filename}.tgz; } || \
            status=$? 
    else
        # wbm: write status update to $statusFile
        > ${destinationDir}/${filename}.tgz
        chgrp www ${destinationDir}/${filename}.tgz || true
        tar 2>/dev/null cp -C / --exclude-from=${IGNORED_FILE_LIST} -f - ./ | \
            gzip -1 > ${destinationDir}/${filename}.tgz &
        gzipPid=$!

        while true; do
            sleep 5 

            gzipSize=$(stat ${destinationDir}/${filename}.tgz | awk '/Size: / {print $2}')
            if ! ps >& /dev/null -p $gzipPid; then # gzip is no longer running
                break
            fi
            rm $statusFile
            echo "Creating archive: size $gzipSize bytes." > $statusFile
            chgrp www $statusFile | true
        done

        wait $gzipPid
        status=$? 
    fi

    case $status in
    $SUCCESS) 
        ;;
    *)
        usedSpace=$(df | awk -v pattern=$destinationDir\$ 'match($0,pattern) {print $5}')
        if [[ "$usedSpace" == "100%" ]]; then
            status=$NOT_ENOUGH_SPACE_ERROR
        else
          status=$SHELL_ERROR
        fi 
        ;;
  esac
  ;;
esac

# Disable standard error handler to pass error to firmware_backup
trap - EXIT

exit $status
