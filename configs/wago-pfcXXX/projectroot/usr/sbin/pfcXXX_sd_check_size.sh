#!/bin/bash

#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#

#
# An SD card will very likely be larger than the internal flash storage.
# Before copying SD card's contents to internal flash, we will have to check if
# the operation will succeed.
#
# Note that, because we copy from ext3 to ubifs, in some edge cases we will be
# unable to determine if the amount of free space is enough to finish the copy
# operation.



# Set "1" to enable, "0" to disable
PRINT_DEBUG=0

# Set nand threshold value for warning (default 5% for firmware)
WARNING_PERCENTAGE="${WARNING_PERCENTAGE:-5}"


function print_debug
{
  if [[ "$PRINT_DEBUG" = "1" ]]; then
    return 0
  else
    return 1
  fi
}

function exit_handler
{
  local lastline=$1
  local lasterr=$2
  local lastcmd=$3
  
  #
  # Don't handle "expected" errors in common handler
  #

  case $lasterr in
    $SUCCESS);;
    $NARROW_SPACE_WARNING);;
    $NOT_ENOUGH_SPACE_ERROR);;
    *)
      _common_exit_handler $lastline $lasterr $lastcmd
      ;;
  esac

  exit $lasterr
}


function check_size_NAND
{
  __resultvar=$1
  local result

  local source_location=$2
  
  # should be a ubifs volume name
  local target_location=$3
 
  local target_location_size_bytes=""

  for vol_nr in ${ubifs_volume_numbers[@]}; do
  
    if [[ "${ubifs_volume_labels[${vol_nr}]}" == "${target_location}" ]]; then
      target_location_size_bytes=$(( ${ubifs_volume_sizes[${vol_nr}]} * 128 * 1024 )) # size in LEBs * 128KB
    fi

  done

  if [[ "${target_location_size_bytes}" == "" ]]; then
    result=${INVALID_PARAMETER}
    return
  fi

  if print_debug; then echo "Check size if source location \"${source_location}\" would fit" \
                            "into target location \"${target_location}\""; fi
  source_location_size_bytes=$(du --apparent-size --bytes --summarize --one-file-system \
                               ${source_location} | awk '{print $1}')
  if print_debug; then echo "full source bytes:            $source_location_size_bytes"; fi

  # special case root file system /
  if [[ ${source_location} = "/" ]]; then

    # Sub bytes for "/home"
    home_size_bytes=$(du --apparent-size --bytes --summarize --one-file-system "/home/" | awk '{print $1}')
    if print_debug; then echo "home bytes:                   $home_size_bytes"; fi
    source_location_size_bytes=$(expr $source_location_size_bytes - $home_size_bytes)

    if print_debug; then echo "source bytes without /home:   $source_location_size_bytes"; fi
  fi

  # Take the result * 0.8 because of the compression used by ubifs (rather conservative factor)
  # Empirical observation: ~70MB root partition contents on sd -> ~38MB on ubifs@nand
  source_location_size_bytes=$((${source_location_size_bytes} * 8 / 10))

  if print_debug; then echo "source bytes rated (80%):     $source_location_size_bytes"; fi
  if print_debug; then echo "target bytes:                 $target_location_size_bytes"; fi

  if [[ ${source_location_size_bytes} -lt ${target_location_size_bytes} ]]; then

    difference=$((${target_location_size_bytes} - ${source_location_size_bytes}))
    if print_debug; then echo "free space difference:        $difference"; fi

    # integer division, error of <20 bytes possible
    buffer_percentage_of_target=$((${target_location_size_bytes} * ${WARNING_PERCENTAGE} / 100))
    if print_debug; then echo "$WARNING_PERCENTAGE% of target size bytes:      $buffer_percentage_of_target"; fi

    if [[ ${difference} -lt ${buffer_percentage_of_target} ]]; then
      if print_debug; then echo "result: warning"; fi
      result=${NARROW_SPACE_WARNING}
    else
      if print_debug; then echo "result: ok"; fi
      result=${SUCCESS}
    fi
  else
    if print_debug; then echo "result: error"; fi

    local source_string
    case $source_location in
      '/')      source_string="root file system";;
      '/home')  source_string="home directory";;
      *)        source_string=$source_location;;
    esac

    result=${NOT_ENOUGH_SPACE_ERROR}
  fi

  eval $__resultvar="'$result'"  
}

function main_nand
{

    check_size_NAND exit_status / $G_UBIFS_ROOT1_LABEL
    if [[ $exit_status -ne $SUCCESS ]]; then
        exit ${exit_status}
    fi

    check_size_NAND exit_status /home $G_UBIFS_HOME_LABEL
    exit ${exit_status}
}

function main_emmc
{
    local size_on_sd
    local index=0

    local -a partition_mountpts=("${!1}")
    local -a partition_sizes=("${!2}")

    for directory in "${partition_mountpts[@]}"; do
        if [[ -n "${directory}" ]]; then
            size_on_sd=$(du --apparent-size \
                            --bytes \
                            --summarize \
                            --one-file-system \
                            "${directory}" | awk '{print $1}')

            # convert partition size to bytes; be conservative (use 1000 rather than 1024)
            if (( ${partition_sizes[${index}]} * 1000 * 1000 < size_on_sd )); then
                return "${NOT_ENOUGH_SPACE_ERROR}"
            fi
            if (( ${partition_sizes[${index}]} * 1000 * 1000 * (100 - ${WARNING_PERCENTAGE}) / 100 < size_on_sd )); then
                return "${NARROW_SPACE_WARNING}"
            fi 
        fi
        index=$((index+1))
    done

    return "${SUCCESS}"
}

function main_sd
{
    local sourceMedium=${1}
 
    case $sourceMedium in
        ${INTERNAL_FLASH_NAND})

            # mimic old behaviour: no size check (NAND is smaller than any SD card)
            return "${SUCCESS}"
            ;;
        ${INTERNAL_FLASH_EMMC})
            local required_space_on_sd_card_mb
            local size_of_sd_card_kb
            local sd_card_device

            sd_card_device=$(/etc/config-tools/get_device_data name "${SD_CARD}")

            required_space_on_sd_card_mb=$(/etc/config-tools/get_min_sd_card_size)
            size_of_sd_card_kb=$(/etc/config-tools/get_device_data size "${sd_card_device}")

            if (( required_space_on_sd_card_mb * 1024 < "${size_of_sd_card_kb}" )); then
                return "${SUCCESS}"
            else
                return "${NOT_ENOUGH_SPACE_ERROR}"
            fi

            ;;
    esac
}

function main
{
    local sourceMedium=${1}
    local destinationMedium=${2}

    exit_status="${SUCCESS}"

    case ${destinationMedium} in
        ${INTERNAL_FLASH_NAND}) 
            main_nand 
            exit_status="${?}"
            ;;
        ${INTERNAL_FLASH_EMMC}) 
            main_emmc "EMMC_PARTITION_MOUNTPTS[@]" "EMMC_PARTITION_SIZES[@]"
            exit_status="${?}"
            ;;
        ${SD_CARD})
            main_sd "${sourceMedium}"
            exit_status="${?}"
            ;;
           *) 
            exit_status="${ILLEGAL_ARGUMENT}"
            ;;
    esac

    exit "${exit_status}"
}

# Don't run main when the file is being sourced.
# Required for unit testing.
if [[ "$0" == "${BASH_SOURCE[0]}" ]]; then
    . /etc/config-tools/board_specific_defines
    . $(dirname $0)/${BOARD_ID}_commons
    . /etc/config-tools/config_tool_defines
    
    trap 'exit_handler ${LINENO} ${?} ${BASH_COMMAND}' EXIT
    
    main "$@"
fi
