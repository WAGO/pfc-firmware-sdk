# #!/bin/bash
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG
# 
# source dirs.sh $2
# 
# if [ ! -d $ROOT_DIR ]
# then
#   mkdir $ROOT_DIR
# fi
# 
# if [ ! -d $ROOT_SCRIPT_DIR ]
# then
#   mkdir $ROOT_SCRIPT_DIR
# fi
# 
# if [ ! -d $XML_DIR ]
# then
#   mkdir $XML_DIR
# fi
# 
# if [ ! -d $HEADER_DIR ]
# then
#   mkdir $HEADER_DIR
# fi
# 
# if [ ! -d $HOST_DIR ]
# then
#   mkdir $HOST_DIR
#    
# fi
# 
# 
# cp $ROOT_SCRIPT_DIR/diagnostic.dtd $HOST_DIR/
# cp $ROOT_SCRIPT_DIR/diagnostics.dtd $HOST_DIR/
# cp $ROOT_SCRIPT_DIR/diagnostics.dtd $XML_DIR/
# 
# # cp diags_template.xml diags.xml
# # rm ./header/*.h
# # for i in $XML_DIR/*.xml
# # do 
# #   sed -e "/<\/diagnostics>/r $i" -e //N diags.xml > tmp.xml 
# #   cp tmp.xml diags.xml
# #   
# #   HEADER_NAME="$HEADER_DIR/`basename ${i%.xml}.h`"
# #   xsltproc -o $HEADER_NAME  create_header.xslt $i
# # done
# # 
# # xsltproc -o diagnostics.csv  create_csv.xslt diags.xml
# 
# $ROOT_SCRIPT_DIR/xslt_make_clean.sh $1 $2
# 
# if [ -f $HOST_DIR/diagnostics.xml ]
#   then
#   xsltproc -o $ROOT_DIR/etc/diagnostics.conf $ROOT_SCRIPT_DIR/create_system_xml.xslt $HOST_DIR/diagnostics.xml
#   fi
