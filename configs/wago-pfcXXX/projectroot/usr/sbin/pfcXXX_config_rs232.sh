#!/bin/bash

#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#

. /etc/config-tools/board_specific_defines; . $(dirname $0)/${BOARD_ID}_commons

function ConnectLinuxToRS232
#
# Write accordings strings to config-files or remove the concerning comment-characters and induce linux to read the 
# new configuration about using the RS232-interface.
#
{
  status=$SUCCESS
  #echo "Connect Linux to RS232"

  # check whether inittab exists
  if [ ! -f $INITTAB ]; then
    status=$CONFIG_FILE_MISSING
    ReportError $CONFIG_FILE_MISSING "($INITTAB)"
    SetLastError "Error while execution (can't write linux-config-file)"
  else

    # change kernel command line (just for sure - the setting should already be set when
    # changing the owner file)
    /usr/sbin/eeprom-xsection -W -s 1 > /dev/null
    status=$?

    if [ "$SUCCESS" = $status ]; then

      # change file /etc/inittab - remove the comment-character in front of line
      sudo $SED -i 's/^#'"$INITTAB_CONSOLE_STRING"'/'"$INITTAB_CONSOLE_STRING"'/g' $INITTAB
      if [ $? != $SUCCESS ]; then
        status=$WRITE_FILE_ERROR
        ReportError $status "($INITTAB)"
        SetLastError "Error while connecting Linux"
      else

        # write buffered filesystem-blocks to disk
        sync

        # send a signal to init-process so that it uses the new configuration
        sudo $KILL -HUP 1
        if [ $? != "$SUCCESS" ]; then
          status=$SHELL_ERROR
          ReportError $status "(kill -HUP 1)"
          SetLastError "Error while connecting Linux"
        fi
      fi
    fi
  fi

  return $status
}


function DisconnectLinuxFromRS232
#
# Remove or comment out according strings from config-files and induce linux to read the 
# new configuration about not to use the RS232-interface.
#
{
  status=$SUCCESS
  #echo "Disconnect Linux from RS232"

  # check whether inittab exists
  if [ ! -f $INITTAB ]; then
    status=$CONFIG_FILE_MISSING
    ReportError $CONFIG_FILE_MISSING "($INITTAB)"
    SetLastError "Error while execution (can't write linux-config-file)"
  else

    # change kernel command line (just for sure - is already done when changing the owner file)

    /usr/sbin/eeprom-xsection -W -s 0 > /dev/null
    status=$?

    # now change inittab
    
    if [ $status != $SUCCESS ]; then
      status=$WRITE_FILE_ERROR
      ReportError $status "Kernel command line"
      SetLastError "Error while disconnecting Linux"
    else

      # write buffered filesystem-blocks to disk
      sync

      # change file /etc/inittab - add comment-character in front of line
      sudo $SED -i 's/^'"$INITTAB_CONSOLE_STRING"'/#'"$INITTAB_CONSOLE_STRING"'/g' $INITTAB
      if [ $? != $SUCCESS ]; then
        status=$WRITE_FILE_ERROR
        ReportError $status "($INITTAB)"
        SetLastError "Error while disconnecting Linux"
      else
  
        # write buffered filesystem-blocks to disk
        sync

        # kill shell, if it is running on RS232 (because someone has already logged in there)
        # get process-list, filter line with "ttyS0" and look after "-bash" (= name of shell-process)
        local processList=`ps aux`
        local ttyString=`echo "$processList" | egrep ${SERIAL_CONSOLE_NAME} 2> /dev/null`
        
        set +o errexit
        echo $ttyString | grep "\-bash" > /dev/null 2> /dev/null
        status=$?
        set -o errexit

        if [ "$SUCCESS" = "$status" ]; then

          # get PID of the shell on RS232 (out of the string from the process-list) and stop it immediately (friendly ask doesn't pay...)
          #echo "kill shell on RS232"
          shellPID=`echo $ttyString | cut -d\  -f2`
          #echo "shellPID:"$shellPID
          sudo $KILL -9 $shellPID
          if [ "$SUCCESS" != "$?" ]; then
            status=$SHELL_ERROR
            ReportError $status "(kill shell on RS232)"
            SetLastError "Error while disconnecting Linux"
          fi
        else # no login shell on serial console
          status=$SUCCESS
        fi

        if [ "$SUCCESS" = "$status" ]; then

          # send a signal to init-process so that it uses the new configuration
          sudo $KILL -HUP 1 
          if [ $? != "$SUCCESS" ]; then
            status=$SHELL_ERROR
            ReportError $status "(kill -HUP 1)"
            SetLastError "Error while disconnecting Linux"
          fi
        fi

      fi
    fi
  fi

  return $status
}

function ConnectCoDeSysToRS232
{
  local status=$SUCCESS
  
  ln -fs /dev/serial /dev/$SERIAL_CONSOLE_NAME_CDS3

  return $status
}

function DisconnectCoDeSysFromRS232
{
  local status=$SUCCESS

  if [ -h /dev/$SERIAL_CONSOLE_NAME_CDS3 ]
  then
    rm /dev/$SERIAL_CONSOLE_NAME_CDS3
  fi

  return $status
}

function DisconnectRS232
#
# Disconnect the actual user of the serial interface.
#
{
  local actualOwner=`${CONFIG_TOOLS_PATH}get_coupler_details RS232-owner`
  local status=$SUCCESS

  # instruct actual owner to free the interface
  case "$actualOwner" in

    # None means CoDeSys libs might use the serial interface.
    # We have to inform it that they have to stop doing it.
    None)               DisconnectCoDeSysFromRS232
                        status=$?;;

    $OWNER_LINUX)       DisconnectLinuxFromRS232
                        status=$?;;

 esac

  return $status
}

ConnectUserToRS232()
#
# Connect new user to use the RS232-Interface.
#
{
  local newOwner=$1
 
  # write new owner to config-filefile
  echo -e $newOwner"\c" > $RS232_OWNER_FILENAME
  if [ "$SUCCESS" != "$?" ]; then
    status=$WRITE_FILE_ERROR
    ReportError $WRITE_FILE_ERROR "($RS232_OWNER_FILENAME)"
    SetLastError "Error while execution"
  else
    
    case "$newOwner" in

      # None means CoDeSys libs may use serial interface.
      # We have to inform CoDeSys about it.
      None)               ConnectCoDeSysToRS232
                          status=$?;;

      $OWNER_LINUX)       ConnectLinuxToRS232
                          status=$?;;

      *)                  status=$INVALID_PARAMETER
                          ReportError $status "($newOwner)";;
    esac

    sync
  fi

  return $status
}

function RS232SetMode
{
  local actualOwner=$(${CONFIG_TOOLS_PATH}get_coupler_details RS232-owner)
  local newOwner=$(cat ${RS232_OWNER_FILENAME})
  
  # if a value for owner is denoted and new value differs from old value -> connect new user to RS232
  #if [ -n "$newOwner" ] && [ "$newOwner" != "$actualOwner" ]; then
  if [ -n "$newOwner" ]; then
    # check for invalid input-parameter
    if [ "None" != "$newOwner" ] && [ "$OWNER_LINUX" != "$newOwner" ] ; then
      status=$INVALID_PARAMETER
      ReportError $status "($newOwner)"
      SetLastError "RS232-owner invalid"
    else
      if [ "$newOwner" != "$actualOwner" ]; then
        # disconnect actual user ...
        DisconnectRS232
        status=$?

        # wait a while to let CoDeSys do the work
        sleep 1

        # ... and connect the new one
        if [ "$SUCCESS" == "$status" ]; then
          ConnectUserToRS232 $newOwner
          status=$?
        fi
      elif [ "None" == "$newOwner" ]; then
        ConnectUserToRS232 $newOwner
      fi
    fi
  fi

}

function RS232EditOwnerFile
{
  local newOwner=$(GetParameterValue $OWNER $*)
  case $newOwner in
    Linux)
      # change kernel command line here: it will only affect the kernel cmdline on next boot
      /usr/sbin/eeprom-xsection -W -s 1 > /dev/null && echo $newOwner > $RS232_OWNER_FILENAME
      sync
      ;;
    None)
      # change kernel command line here (see comment above)
      /usr/sbin/eeprom-xsection -W -s 0 > /dev/null && echo $newOwner > $RS232_OWNER_FILENAME
      sync
      ;;
    *)
      return $INVALID_PARAMETER
      ;;
  esac
}


###################### MAIN ########################

status=$SUCCESS

# show help-text if requested
if [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
  echo
  echo "* Change the owner of the serial interface *"
  echo
  echo "Usage: config_RS232 < owner=RS232-owner-value >"
  echo "       config_RS232 < start >"
  echo "\"start\": to be used at boot up; script takes the settings for RS232 from configuration-file"
  echo "RS232-owner: None | Linux: value that will be set when calling \"$0 start\" (see above)." 
  exit $status
fi

case "$(GetBootMode)" in
  *overwrite)
    SetLastError "Cannot set RS232 owner when the kernel uses a custom command line."
    exit $LAST_ERROR_SET
    ;;
esac

case $@ in

  start)
    RS232SetMode 
    ;;

  *)
    RS232EditOwnerFile $@
    ;;

esac

exit $status
