# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

function ShowPLCRuntimeServicesMenu
{
  local menu_ressource=$(basename ${BASH_SOURCE[1]})
  local menu_ressource_quoted=${__XMLStarlet_QUOTE}${menu_ressource}${__XMLStarlet_QUOTE}

  
  local menu_lines_list
  # Each entry has the form 'bla.sh bla_state.sh' (if bla_state.sh exists)
  local menu_scripts_list

  local entry_cnt=0
  local quit="$FALSE"

  local selection

  local menu_lines_array
  local menu_scripts_array

  local menu_titles_array
  local state_scripts_array

  # Build ':'-separated lists of menu lines and sub-script/state script names.
  
  # Main menu is handled differently in that only top-level lines are evaluated;
  # first menu line is "Quit" instead of "Back"
  
  # NOTE: @ressource is unique; @text isn't!
  parent=$(${XMLSTARLET} sel -t -m "//simple_level[@ressource = ${menu_ressource_quoted}]/.." -v "@text" ${CONFIG_FILE})
  menu_name=$(${XMLSTARLET} sel -t -m "//simple_level[@ressource = ${menu_ressource_quoted}]" -v "@text" ${CONFIG_FILE})

  local  menu_entry_back="Back to $parent Menu"

  # Here is why we need to use a custom ShowGenericMenu variant: filter illegal menu lines (codesys2/e!runtime) if
  # they are not supported on the device
 
  local xpath_query=
  local possible_runtimes=$(/etc/config-tools/get_possible_runtimes) 
  case $possible_runtimes in
      "0 2 3") xpath_query="//simple_level[@ressource=${menu_ressource_quoted}]/simple_level";;
        "0 2") xpath_query="//simple_level[@ressource=${menu_ressource_quoted}]/simple_level[@uid != 'PORT_ERUNTIME']";;
        "0 3") xpath_query="//simple_level[@ressource=${menu_ressource_quoted}]/simple_level[@uid != 'PORT_CODESYS2']";;
          "*") xpath_query="//simple_level[@ressource=${menu_ressource_quoted}]/leaf_that_does_not_exist";; # show an empty menu

  esac

  menu_lines_list=$(${XMLSTARLET} sel -t -m "${xpath_query}" -v "@text" -o ":" ${CONFIG_FILE})
  menu_scripts_list=$(${XMLSTARLET} sel -t -m "${xpath_query}" \
                                               -v "@ressource" -i "@dyn_state_ressource" -o '@' -v "@dyn_state_ressource" -b -o ":" ${CONFIG_FILE})

  menu_lines_array[0]="0. ${menu_entry_back}"
  menu_scripts_array[0]="dummy_fn"

  # first put the script modules for each line to the array so that we can
  # reference them after the user makes his choice

  local ifsBackup=$IFS
  
  entry_cnt=1
  IFS=":"
  for entry in ${menu_scripts_list}; do
    menu_scripts_array[${entry_cnt}]="${SUBMENU_DIR}/"${entry};
    entry_cnt=$((${entry_cnt}+1));
  done
  IFS=${ifsBackup}

  # now separate menu_lines_list tuples (main_script@state_script) into different arrays

  entry_cnt=1
  IFS=":"
  for entry in ${menu_scripts_list}; do

    case "${entry}" in
      *@*)
        menu_scripts_array[${entry_cnt}]=${SUBMENU_DIR}/${entry%%@*}; # cut everything after '@'
        state_scripts_array[${entry_cnt}]=${SUBMENU_DIR}/${entry##*@}; # cut everything before '@'
        ;;
        *)
        menu_scripts_array[${entry_cnt}]=${SUBMENU_DIR}/${entry};
        state_scripts_array[${entry_cnt}]=""
        ;;
    esac

    entry_cnt=$((${entry_cnt}+1));
  done
  IFS=${ifsBackup}
 
  # create static menu lines

  IFS=":"

  entry_cnt=1
  for entry in ${menu_lines_list}; do
    
    if [[ "" == "${state_scripts_array[${entry_cnt}]}" ]]; then
      menu_lines_array[${entry_cnt}]="${entry_cnt}. ${entry}";
    else
      menu_lines_array[${entry_cnt}]=""
    fi

    entry_cnt=$((${entry_cnt}+1));

  done
  IFS=${ifsBackup}

  while [ "${quit}" = "$FALSE" ]; do
    
    # we have to rebuild the menu lines each time because of the dynamic lines,
    # i.e. FTP....enabled
    local ifsBackup=$IFS
    IFS=":"
  
    local state=""
    entry_cnt=1
    local state_script=""
    for entry in ${menu_lines_list}; do
      # Dynamic lines have mini-scripts defined to compute the dynamic part.
      # These mini-scripts are set in the XML config as dyn_state_ressource
      # attributes.  We have to look for these attributes, source them and append
      # the result to the line if available
    
      state_script=${state_scripts_array[${entry_cnt}]}
      if [[ "${state_script}" != "" ]]; then
        state=$(source ${state_script})
      fi

      menu_lines_array[${entry_cnt}]="${entry_cnt}. ${entry}${state}";

      state="" 
      entry_cnt=$((${entry_cnt}+1));
    done
    IFS=${ifsBackup}

    # show the menu

    WdialogWrapper "--menu" selection "${TITLE}" "${menu_name}" "${menu_lines_array[@]}"

    # evaluate the user's choice

    if [ ${selection} -eq 0 ]; then
      quit="$TRUE";
    elif [ ${selection} -lt ${#menu_scripts_array[@]} ]; then
      source ${menu_scripts_array[$selection]};
    else
      quit="$TRUE";
      errorText="Error in wdialog" 
    fi

    case "$option" in
      show_once)
        quit="$TRUE";;
    esac

  done
}

ShowEvaluateDataWindow "PLC Runtime Services"
ShowPLCRuntimeServicesMenu
ShowEvaluateDataWindow "Ports and Services"
