#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

command=$1 # (compile, clean, word)
XML_FILE=$2
TARGET_DIRECTORY=$3
SYSROOT_DIRECTORY=$4
#PTXDIST_WORKSPACE=$5 

source "$(dirname "$0")/xslt_lock.sh"

ROOT_DIR=${TARGET_DIRECTORY}/src/diagnostic
ROOT_SCRIPT_DIR=${TARGET_DIRECTORY}/src/diagnostic
XML_DIR=$ROOT_DIR/xml
HEADER_DIR=${SYSROOT_DIRECTORY}/usr/include/diagnostic
HOST_DIR=$ROOT_DIR/host
TARGET_DIR=$ROOT_DIR/etc
#PROJECTROOT=${PTXDIST_WORKSPACE}/projectroot/etc/specific
PROJECTROOT=${SYSROOT_DIRECTORY}/etc/specific
FILE_NAME=$(basename "${XML_FILE%.xml}")
#PTXWSPACE_DIR=${PTXDIST_WORKSPACE}
PROVENANCE_FILE=$XML_DIR/provenance.txt

check_xml() {
  local fileName=$1

  cp "${ROOT_SCRIPT_DIR}/diagnostics.dtd" "$(dirname "${fileName}")/"

  if ! xmlstarlet validate -q -d "${ROOT_SCRIPT_DIR}/diagnostics.dtd" "${fileName}"
  then
    echo "XML-Lint Error"
    exit 1
  fi
  if ! xmlstarlet validate -q -d "${ROOT_SCRIPT_DIR}/diagnostic.dtd" "${fileName}"
  then
    echo "XML-Lint Error"
    exit 1
  fi
  echo "<?xml version=\"1.0\" encoding=\"UTF8\" ?>" > "${HOST_DIR}/temp.xml"
  echo "<!DOCTYPE diagnostic SYSTEM \"diagnostic.dtd\">" > "${HOST_DIR}/temp.xml"
  echo "<diagnostic>" >> "${HOST_DIR}/temp.xml"
  
  if [ -f "${HOST_DIR}/diagnostics.xml" ]
  then
    xsltproc "${ROOT_SCRIPT_DIR}/create_host_xml.xslt" "${HOST_DIR}/diagnostics.xml" >> "${HOST_DIR}/temp.xml"
  fi
  
  if ! xsltproc "${ROOT_SCRIPT_DIR}/create_host_xml.xslt" "${fileName}" >> "${HOST_DIR}/temp.xml"
  then
    echo "XML-Parser Error"
    exit 1
  fi
  
  echo "</diagnostic>" >> "${HOST_DIR}/temp.xml"
  
  cp "${ROOT_SCRIPT_DIR}/diagnostic.dtd" "${HOST_DIR}"
  
  if ! xmlstarlet validate -q -d "${ROOT_SCRIPT_DIR}/diagnostic.dtd" "${HOST_DIR}/temp.xml"
  then
    echo "XML-Lint Error at second check"
    exit 1
  fi
  
  #sleep 1
  
  mv "${HOST_DIR}/temp.xml" "${HOST_DIR}/diagnostics.xml"
  if [ ! -f "${PROVENANCE_FILE}" ] || ! grep "$(basename "${fileName}")" "${PROVENANCE_FILE}" >> /dev/null
  then
    echo "${fileName}" >> "${PROVENANCE_FILE}"
  fi
  cp -u "${fileName}" "${XML_DIR}/" &> /dev/null
}

check_for_file() {
  local fileName=$1

  if [ -f "${XML_DIR}/$(basename "${fileName}")" ]
  then
    rm "${XML_DIR}/$(basename "${fileName}")"
    rm "${HOST_DIR}/diagnostics.xml"
    for i in "${XML_DIR}"/*.xml
    do
      if [ -f "$i" ]
      then
        check_xml "$i"
      fi
    done
  fi
}

# Create directories, if they are not yet there
mkdir --parents "${ROOT_DIR}" "${ROOT_SCRIPT_DIR}" "${XML_DIR}" "${HEADER_DIR}" "${HOST_DIR}" "${TARGET_DIR}"

cp "${ROOT_SCRIPT_DIR}/diagnostic_host.dtd" "${HOST_DIR}/diagnostic.dtd"
cp "${ROOT_SCRIPT_DIR}/diagnostics.dtd" "${HOST_DIR}/"
cp "${ROOT_SCRIPT_DIR}/diagnostics.dtd" "${XML_DIR}/"

case ${command} in 
  compile)
    
    check_for_file "${XML_FILE}"
    check_xml "${XML_FILE}"
    if ! xsltproc -o "${HEADER_DIR}/$FILE_NAME.h" --stringparam path "${XML_FILE}" "${ROOT_SCRIPT_DIR}/create_header.xslt" "${XML_FILE}" >&2
    then
      echo "${HOST_DIR}/temp.xml:40: XML-Lint Error while creating header files" >&2
      exit 1
    fi
    ;;
  clean)
    check_for_file "${XML_FILE}"
    rm --force "${HEADER_DIR}/$FILE_NAME.h"
    rm --force "${TARGET_DIR}/diagnostics.conf"
    ;;
esac

for i in "${XML_DIR}"/*.xml
do
  if [ -f "$i" ]
  then
    HEADER_NAME="${HEADER_DIR}/$(basename "${i%.xml}".h)"
    if [ ! -f "${HEADER_NAME}" ]
    then
      if ! xsltproc -o "${HEADER_NAME}" --stringparam path "$i" "${ROOT_SCRIPT_DIR}/create_header.xslt" "$i" >&2
      then
        echo "${HOST_DIR}/temp.xml:40: XML-Lint Error while creating header files" >&2
        exit 1
      fi
    fi
  fi
done

if [ -f "${HOST_DIR}/diagnostics.xml" ]
then
  if xsltproc -o "${TARGET_DIR}/diagnostic.xml" "${ROOT_SCRIPT_DIR}/create_system_xml.xslt" "${HOST_DIR}/diagnostics.xml"
  then
    mkdir --parents "${PROJECTROOT}"
    cp "${TARGET_DIR}/diagnostic.xml" "${PROJECTROOT}/diagnostic.xml"
  fi
fi
