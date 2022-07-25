#!/bin/bash

#-----------------------------------------------------------------------------#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG
#-----------------------------------------------------------------------------#

separator='***********************************************************************'

function echo_spaces(){
  local cnt=0
  while [[ $cnt -lt ${1} ]]; do
    echo -n ' '
    cnt=$((cnt+1))
  done
}

function echo_title(){
  local title_length=$(expr length "$1")
  local line_length=$(expr length "${separator}")

  local spaces_nr=$((${line_length}-${title_length}-2))
  local spaces_left=$((${spaces_nr}/2))
  local spaces_right=$((${spaces_nr}-${spaces_left}))

  echo "$separator"

  echo -n '*'
  echo_spaces $spaces_left
  echo -n $1
  echo_spaces $spaces_right
  echo '*'
  
  echo "$separator"

}

function eval_result(){
  if [[ $? == 0 ]]; then
    echo -e "\033[36mOK\033[0m"
    echo
  else
    echo -e "\033[31mFAILED\033[0m"
  fi
}

