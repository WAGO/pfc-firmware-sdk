#!/bin/bash
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#-----------------------------------------------------------------------------#
# Script-name: blender.sh
#
# mixes given entries according the given sorting algorithm
# will be used for NTP and DNS entries
#
# Author: WAGO Kontakttechnik GmbH
#-----------------------------------------------------------------------------#


SEARCH_PRAM=$1
SEARCH_DYNAMIC_FILES_TEMPLATE=$2
STATIC_FILE=$3
DELIMITER=$4
SORT_ALGORITHM=$5

function printUsage ()
{
cat << EOF

Usage: $0 [SEARCH PARAM] [DYNAMIC_TEMPLATE] [STATIC_FILE] [DELIMITER] [SORTING_ALGORITHM]

Sorting and mixing all kinds of network parameters

    SEARCH_PRAM         Specifies what kind of entry to search for
    DYNAMIC_TEMPLATE    Template filename of dynamic data files. Which has to be
                        specified with the full path.
    STATIC_FILE         Location of data file which contains the static entries,
                        also has to be specified with the full path.
    DELIMITER           Specfies the separator character.
    SORTING_ALGORITHM   Name of the sorting algorithm that hast to be used:
                        dynSingleTopStaticBelow
                        xxx
                        dynTopStaticBelow
                        xxx

Example:
    $0 NTP_SERVER_ /tmp/dhcp-bootp-data-br /tmp/network_static_parameter '=' dynSingleTopStaticBelow


EOF

}

function getEntryWithDelimiter()
{
   local FILENAME=$1
   local DELIMITER=$2
   local RETURN=""
   local ENTRY=`grep $SEARCH_PRAM $FILENAME`

   if [ -n $DELIMITER ]; then
     while IFS=$2 read -r f1 f2
     do
         RETURN="$RETURN $f2"
     done <<< "$ENTRY"
   else
        RETURN=$ENTRY
   fi
   #remove quotes
   RETURN=`echo $RETURN | tr -d '\042'`
   echo $RETURN

}

#######
# Generation of the final list.
# It will sort the input array to the given sorting algorithm
# param $1 = input array 
# param $2 = count of array elements
# param $3 = desired sorting algorithm
# returns the sorted list
function generateList()
{
    local data=("${!1}") #magic for array as parameter
    local len=$2 #
    local sortingAlgo=$3
    local ret[0]=""
    local return_value=0

    case "$sortingAlgo" in
        #----------------------------------------------------
        dynSingleTopStaticBelow)
        local found_elements=1  #only for starting behaviour

        local field_row=0
        while [ $found_elements -gt 0 ]; do
            found_elements=0

            for i in $(seq 0 $len)
            do
                #seperate string with delimiter <space>
                dummy=(${data[$i]})
                entry=${dummy[$field_row]}
                if [ "$entry" ] && [ ${#entry} -gt 0 ] ; then
                    ret=("${ret[@]}" "$entry")
                    found_elements=$((found_elements+1))
                fi
            done
            field_row=$((field_row+1))
        done
        ;;
        #----------------------------------------------------
        dynTopStaticBelow)
        for i in $(seq 0 $len)
        do
                ret=("${ret[@]}" "${data[$i]}")
        done
        ;;
        #----------------------------------------------------
        *)
        ret="wrong sorting algorithm"
        return_value=1
    esac
    echo ${ret[@]}
    return $return_value
}
#------------------------------------------------------------

if [ "$#" -lt 5 ] ; then
    printUsage
    exit
fi

DYNAMIC_FILE_LIST=`ls $SEARCH_DYNAMIC_FILES_TEMPLATE* 2> /dev/null`
index=0

#Read dynamic files
for file in $DYNAMIC_FILE_LIST; do
    STORAGE_ARRAY[$index]=`getEntryWithDelimiter $file $DELIMITER`
    index=$((index + 1))
done

#Read static files
STORAGE_ARRAY[$index]=`getEntryWithDelimiter $STATIC_FILE $DELIMITER`
#genertate the sorted list
list=`generateList STORAGE_ARRAY[@] $index $SORT_ALGORITHM`

if [ "$?" -gt 0 ] ; then
        echo "ERROR: $list"
        exit 1
fi

#return the list
for ent in $list; do
        echo "$ent"
done

exit 0
