# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

function MainProfibus
#
# Processing of menu Main -> Profibus
# Show and change the permission flag
#
{
  ShowEvaluateDataWindow "PROFIBUS Set-Slave-Address-Service"

  local PROFIBUS_CONFIG="${CONFIG_TOOLS_PATH}/pbdp_config"

  # get the values for the several parameters
  local permissionFlagValue=`$PROFIBUS_CONFIG get ssa.user ssa_released`

  # loop until the user wants to quit
  local quit=$FALSE
  local selection
  local retUnused
  
  local state

  while [ "$quit" = "$FALSE" ]; do

    ShowEvaluateDataWindow "PROFIBUS Set-Slave-Address-Service"

    # evaluate state of server new after any changes (can switch to disabled due to errors)
    local permissionFlagValue=`$PROFIBUS_CONFIG get ssa.user ssa_released`

    case "$permissionFlagValue" in
      true)
        state="enabled";;
      false)
        state="disabled";;
    esac

    local slaveAddress=$($PROFIBUS_CONFIG get ssa.user slave_address)

    local menu_params_ssa=("Stored slave address.................$slaveAddress" 
                           "Permission to change slave address...$state")
    
    ./wdialog "--msgbox" "${TITLE}" "PROFIBUS Set-Slave-Address-Service" "${menu_params_ssa[@]}"
    
    WdialogWrapper "--menu" selection \
              "$TITLE" \
              "PROFIBUS DP Slave Configuration" \
              "0. Back to Main Menu" \
              "1. Reset address" 
    
    # analyse user selection and do to the according processing
    case "$selection" in
    
      0)  # Quit was selected -> end loop and get back to superior menu
          quit=$TRUE;;

      1)  # state was selected -> show selection-menu
          WdialogWrapper "--menu" selection \
                    "$TITLE" \
                    "Reset SSA settings to factory defaults?" \
                    "0. Back to PROFIBUS Configuration Menu" \
                    "1. Reset"
          
    
          # if a new state was selected - change it, get tconfig_sntp port=he actual value again and show possible errors
          if [[ "$selection" == "1" ]]; then 
            ShowProcessingDataWindow "PROFIBUS Reset SSA to factory settings"
            
            local factoryPermission=$($PROFIBUS_CONFIG get ssa.factory ssa_released) && \
            local factoryAddress=$($PROFIBUS_CONFIG get ssa.factory slave_address) && \
            $PROFIBUS_CONFIG set ssa.user ssa_released  $factoryPermission && \
            $PROFIBUS_CONFIG set ssa.user slave_address $factoryAddress 
            
            ShowLastError
          fi
          ;;
       
      *)  errorText="Error in wdialog"
          quit=TRUE;;

    esac
  done
}

# end of MainProfibus

MainProfibus


