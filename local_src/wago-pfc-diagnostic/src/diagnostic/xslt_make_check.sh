#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

source $(dirname $0)/xslt_lock.sh

ROOT_DIR=$2/src/diagnostic
ROOT_SCRIPT_DIR=$2/src/diagnostic
XML_DIR=$ROOT_DIR/xml
HEADER_DIR=$ROOT_DIR/header
HOST_DIR=$ROOT_DIR/host

echo "Hello XSLT: $ROOT_DIR"

if [ ! "$3" = "recursive" ]
then
  echo "PING"
  if [ -f $XML_DIR/`basename $1` ]
  then
    rm $XML_DIR/`basename $1`
    rm $HOST_DIR/diagnostics.xml
    for i in $XML_DIR/*.xml
    do
      if [ -f $i ]
      then
        $0 $i $2 recursive
      fi
    done
  fi
fi

cp $ROOT_SCRIPT_DIR/diagnostics.dtd `dirname $1`/

xmlstarlet validate -E $1
if [ $? -ne 0 ] 
then
  echo "XML-Lint Error"
  exit 1
fi

echo "<!DOCTYPE diagnostic SYSTEM \"diagnostic.dtd\">" > $HOST_DIR/temp.xml
echo "<diagnostic>" >> $HOST_DIR/temp.xml

if [ -f $HOST_DIR/diagnostics.xml ]
then
  xsltproc $ROOT_SCRIPT_DIR/create_host_xml.xslt $HOST_DIR/diagnostics.xml >> $HOST_DIR/temp.xml
fi

xsltproc $ROOT_SCRIPT_DIR/create_host_xml.xslt $1 >> $HOST_DIR/temp.xml
if [ $? -ne 0 ] 
then
  echo "XML-Parser Error"
  exit 1
fi

echo "</diagnostic>" >> $HOST_DIR/temp.xml

cp $ROOT_SCRIPT_DIR/diagnostic.dtd $HOST_DIR


xmlstarlet validate -E $HOST_DIR/temp.xml
if [ $? -ne 0 ] 
then
  echo "XML-Lint Error at second check"
  exit 1
fi

cp $HOST_DIR/temp.xml $HOST_DIR/diagnostics.xml
echo $1
cp $1 $XML_DIR/