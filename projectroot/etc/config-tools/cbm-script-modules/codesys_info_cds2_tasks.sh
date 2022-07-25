# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

PARENT=$(GetParentMenuName $BASH_SOURCE)

function MainCODESYSTasks
#
# Show window with information about CODESYS project tasks
#
{
  # loop until user wants to go back to main menu
  local quit=$FALSE
  local selection
  while [ "$quit" = "$FALSE" ]; do

    # check if there is at least one task existing (getting cycle count will return empty string, if task is not existing)
    local cycleCount=`./get_rts_info task 0 count`
    if [ -z $cycleCount ]; then
      ./wdialog "--msgbox" "$TITLE" "Tasks" " " "No Tasks existing" " "
      quit=TRUE
    else

      # create selection menu about all existing tasks
      local taskNo=0
      local menuString=""
      ifsBackup=$IFS
      IFS=$'\t'
      while [ -n "$cycleCount" ]; do
        menuString=`printf "%s$IFS$(($taskNo + 1)). Task $taskNo" "$menuString"`
        taskNo=$(($taskNo + 1));
        cycleCount=`./get_rts_info task $taskNo count`
      done

      # show menu
      WdialogWrapper "--menu" selection \
                "$TITLE" \
                "Tasks" \
                "0. Back to ${PARENT} Menu" \
                $menuString
      IFS=$ifsBackup

      # if selection value is valid
      if (( $selection > 0 )) && (( $selection <= $taskNo )); then

        ShowEvaluateDataWindow "Task"

        # get the informations about the requested task and show them in a message window
        local taskNo=$(($selection - 1))
        local name=`./get_rts_info task $taskNo name`
        local id=`./get_rts_info task $taskNo id`
        local cycleCount=`./get_rts_info task $taskNo count`
        local cycleTime=`./get_rts_info task $taskNo cycletime`
        local cycleTimeMin=`./get_rts_info task $taskNo cycletime-min`
        local cycleTimeMax=`./get_rts_info task $taskNo cycletime-max`
        local cycleTimeAcc=`./get_rts_info task $taskNo cycletime-acc`
        local mode=`./get_rts_info task $taskNo mode`
        local status=`./get_rts_info task $taskNo status`
        local priority=`./get_rts_info task $taskNo priority`
        local intervalUnit=`./get_rts_info task $taskNo interval-unit`
        local interval=`./get_rts_info task $taskNo interval`
#        local event=`./get_rts_info task $taskNo event`
#        local functionPointer=`./get_rts_info task $taskNo function-pointer`
#        local functionIndex=`./get_rts_info task $taskNo function-index`

        ./wdialog "--msgbox" \
                  "$TITLE " \
                  "Task $taskNo: $name, ID: $id" \
                  "Cycle count..............$cycleCount" \
                  "Cycletime (mcsec)........$cycleTime" \
                  "Cycletime min (mcsec)....$cycleTimeMin" \
                  "Cycletime max (mcsec)....$cycleTimeMax" \
                  "Cycletime avg (mcsec)....$cycleTimeAcc" \
                  "Status...................$status" \
                  "Mode.....................$mode" \
                  "Priority.................$priority" \
                  "Interval ($intervalUnit)..........$interval" #\
#                  "Event....................$event" \
#                  "Function pointer.........$functionPointer" \
#                  "Function index...........$functionIndex"

      # Quit or something else was selected -> end loop and get back to superior menu
      else
        quit=$TRUE
      fi

    fi
  done
}

MainCODESYSTasks
