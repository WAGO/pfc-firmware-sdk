#!/bin/bash

set -e

function _tree() {
    if command -v tree; then
        tree -a --matchdirs $1
    else
        find ${1} | sed -e "s/[^-][^\/]*\// |/g" -e "s/|\([^ ]\)/|-\1/"
    fi
}

HERE=$(pwd)

NAME=$1
if [ -z "$NAME" ]; then
	echo -n "project name: "
	read NAME
fi
NAME_UP=$(echo $NAME | tr '[a-z-]' '[A-Z_]')
name_c=$(echo $NAME | tr '-' '_')

UP_TO_PTXPROJ=".."
search_path=".."
while [[ ! -d "${search_path}/ptxproj" ]]; do
  UP_TO_PTXPROJ="..\/${UP_TO_PTXPROJ}"
  search_path="../${search_path}"
done

PWD=$(pwd)

for i in $(find -mindepth 1 -type f)\
; do
  sed -i -e "s/\@up_to_ptxproj\@/${UP_TO_PTXPROJ}/g" $i
  sed -i -e "s/\@name\@/${NAME}/g" $i
  sed -i -e "s/\@NAME\@/${NAME_UP}/g" $i
  sed -i -e "s/\@name_c\@/${name_c}/g" $i
  sed -i -e "s#\@pwd\@#${PWD}#g" $i
done

# rename files
mv "@name@.pc" "${NAME}.pc"
mv "src/@name@_API.cpp" "src/${NAME}_API.cpp"
mv "src/@name@_API.h" "src/${NAME}_API.h"
mv "src/@name@/@name@.c" "src/@name@/${NAME}.c"

# rename folders
mv "src/@name@" "src/${NAME}"

echo "Created project in $(pwd)"
cd .. && _tree ${NAME}
