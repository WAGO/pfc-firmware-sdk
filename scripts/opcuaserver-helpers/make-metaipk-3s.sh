#!/usr/bin/env bash

# Build ipkg for installing 3s opcuaserver on pfc firmware
#
# WAGO GmbH & Co. KG

PTXPROJ=$(pwd)

if [ -z "$PTXDIST_WORKSPACE" ]; then
  PTXDIST_WORKSPACE="${PTXDIST_WORKSPACE:-../../../../ptxproj}"
  PTXDIST_PLATFORMDIR=$PTXDIST_WORKSPACE/platform-wago-pfcXXX
  echo !!! environment not set
  echo PTXDIST_WORKSPACE=$PTXDIST_WORKSPACE
  echo PTXDIST_PLATFORMDIR=$PTXDIST_PLATFORMDIR
fi

if echo "$PTXPROJ" | grep -q "/wago_intern/opcua/server_3s_ipk"; then
  BUILD_DIR="bin"
else
  BUILD_DIR="$PTXDIST_PLATFORMDIR/packages/opcuaserver_3s_ipk"
fi

#--------------------------------------
# Collect data
#--------------------------------------
# path
SRC_PATH="build-target/codesys-3/cmp"
DST_PATH_LIB="usr/lib"
DST_PATH_CMP="$DST_PATH_LIB/cds3-custom-components"
DATA_DIR="$BUILD_DIR/data/tmp_libs"
NAME="opcuaserver_cds3"

# print path
echo "INFO path BUILD_DIR=           $BUILD_DIR"
echo "INFO path PTXPROJ=             $PTXPROJ"
echo "INFO path PTXDIST_PLATFORMDIR: $PTXDIST_PLATFORMDIR"
echo "INFO path DST_PATH_LIB:        $DST_PATH_LIB"
echo "INFO path DST_PATH_CMP:        $DST_PATH_CMP"
echo "INFO path DATA_DIR:            $DATA_DIR"
echo "INFO path SRC_PATH:            $SRC_PATH"

# search libs
LIB_CODESYS_OPCUA=$(ls $PTXDIST_PLATFORMDIR/$SRC_PATH | grep libCmpOPCUA | cut -f2 )
if [ "$LIB_CODESYS_OPCUA" = "" ]; then
  echo !!! libCmpOPCUA* not found
  exit 1
fi

# create folder
mkdir -p "$DATA_DIR/$DST_PATH_LIB"
mkdir -p "$DATA_DIR/$DST_PATH_CMP"
if [ ! -d "$DATA_DIR/$DST_PATH_LIB" ] && [ ! -d "$DATA_DIR/$DST_PATH_CMP" ]; then
  echo !!! folder not found
  exit 1
fi

# copy libs and install links
for LIB in $LIB_CODESYS_OPCUA; do
  # copy lib
  cp -a "$PTXDIST_PLATFORMDIR/$SRC_PATH/$LIB" "$DATA_DIR/$DST_PATH_LIB/$LIB"
  if [[ ! -L "$DATA_DIR/$DST_PATH_LIB/$LIB" && ! -f "$DATA_DIR/$DST_PATH_LIB/$LIB" ]]; then
    echo !!! copy fail
    exit 1
  fi
  # install link
  if [[ "$LIB" == *".so" ]]; then
    ln -s "/$DST_PATH_LIB/$LIB" "$DATA_DIR/$DST_PATH_CMP/$LIB"
    if [[ ! -L "$DATA_DIR/$DST_PATH_CMP/$LIB" ]]; then
      echo !!! install link fail
      exit 1
    fi
  fi
done

# infos
FW_VERSION=$(cat  $PTXDIST_WORKSPACE/projectroot/etc/REVISIONS | grep "FIRMWARE=" | cut -d= -f2 | sed -e 's/(/_/g;s/)//g' )
PROJECT_NAME_FOR_IPK=$(cat selected_ptxconfig | grep "PTXCONF_PROJECT=" | cut -d= -f2 | sed 's/"//g')
OPCUA_IPK_VERSION=$(ls $DATA_DIR/$DST_PATH_LIB | grep libCmpOPCUAServer.so. |sed 's/libCmpOPCUAServer.so.//')
IPKGNAME="${NAME}_${OPCUA_IPK_VERSION}_${PROJECT_NAME_FOR_IPK}_FW${FW_VERSION}.ipk"

#--------------------------------------
# Cleanup function
#--------------------------------------
function cleanup
{
  rm -rf ${BUILD_DIR}
}

#--------------------------------------
# Create folder
#--------------------------------------
mkdir -m 700 -p ${BUILD_DIR}/data/root
mkdir -p ${BUILD_DIR}/control

#--------------------------------------
# Print infos
#--------------------------------------
#echo "INFO firmware version: $FW_VERSION"
#echo "INFO server version:   $OPCUA_IPK_VERSION"
#echo "INFO ipkname:          $IPKGNAME"

#---------------------------------------------------------
#create control file
#---------------------------------------------------------
echo "Package: opcuaserver_3s"                              > ${BUILD_DIR}/control/control
echo "Priority: optional"                                  >> ${BUILD_DIR}/control/control
echo "Version:  $OPCUA_IPK_VERSION"                        >> ${BUILD_DIR}/control/control
echo "Section:    base"                                    >> ${BUILD_DIR}/control/control
echo "Architecture: armhf"                                 >> ${BUILD_DIR}/control/control
echo "Maintainer: \"WAGO GmbH & Co. KG \" " >> ${BUILD_DIR}/control/control
echo "Depends: "                                           >> ${BUILD_DIR}/control/control
echo "Source: "                                            >> ${BUILD_DIR}/control/control
echo "Description:  Installation for 3s opcuaserver. This packet is a meta packet including all parts need for server." >> ${BUILD_DIR}/control/control
echo "              This packet is build for Firmware Version $FW_VERSION" >> ${BUILD_DIR}/control/control

#---------------------------------------------------------
#create postinst file
#---------------------------------------------------------
echo "#!/bin/bash"                                > ${BUILD_DIR}/control/postinst
echo "#"                                         >> ${BUILD_DIR}/control/postinst
echo "# WAGO GmbH & Co. KG"       >> ${BUILD_DIR}/control/postinst
echo ""                                          >> ${BUILD_DIR}/control/postinst
echo "source /etc/profile &>/dev/null"           >> ${BUILD_DIR}/control/postinst
echo "function start_update"                     >> ${BUILD_DIR}/control/postinst
echo "{"                                         >> ${BUILD_DIR}/control/postinst
echo "    local PID_OF_IPKG=\$1"                 >> ${BUILD_DIR}/control/postinst
echo "    while ps -p \$PID_OF_IPKG > /dev/null" >> ${BUILD_DIR}/control/postinst
echo "    do"                                    >> ${BUILD_DIR}/control/postinst
echo "		    usleep 100000"                     >> ${BUILD_DIR}/control/postinst
echo "    done"                                  >> ${BUILD_DIR}/control/postinst
echo "    export PREPARE_MODE=@PREPARE_MODE@"    >> ${BUILD_DIR}/control/postinst
echo "    /root/install_3s_opcuaserver.sh 2>&1 |  tee -a /var/log/opcuaserver.log" >> ${BUILD_DIR}/control/postinst
echo "    sync"                                  >> ${BUILD_DIR}/control/postinst
echo "}"                                         >> ${BUILD_DIR}/control/postinst
echo ""                                          >> ${BUILD_DIR}/control/postinst
echo "PID_OF_IPKG=\$(pidof opkg)"                >> ${BUILD_DIR}/control/postinst
echo "start_update \$PID_OF_IPKG &"              >> ${BUILD_DIR}/control/postinst
echo ""                                          >> ${BUILD_DIR}/control/postinst
echo "exit 0"                                    >> ${BUILD_DIR}/control/postinst
echo ""                                          >> ${BUILD_DIR}/control/postinst

chmod +x ${BUILD_DIR}/control/postinst

#---------------------------------------------------------
#create install_3s_opcuaserver.sh
#---------------------------------------------------------
echo "#!/bin/sh"                                                   > ${BUILD_DIR}/data/root/install_3s_opcuaserver.sh
echo ""                                                           >> ${BUILD_DIR}/data/root/install_3s_opcuaserver.sh
echo "/etc/init.d/runtime stop"                                   >> ${BUILD_DIR}/data/root/install_3s_opcuaserver.sh
echo "/etc/init.d/opcua-server stop"                              >> ${BUILD_DIR}/data/root/install_3s_opcuaserver.sh
echo "opkg remove pp-opcua"                                       >> ${BUILD_DIR}/data/root/install_3s_opcuaserver.sh
echo "opkg remove opcuaclient"                                    >> ${BUILD_DIR}/data/root/install_3s_opcuaserver.sh
echo "opkg remove opcuaserver"                                    >> ${BUILD_DIR}/data/root/install_3s_opcuaserver.sh
echo "opkg remove opcuacsdk"                                      >> ${BUILD_DIR}/data/root/install_3s_opcuaserver.sh
echo "cp -au /tmp_libs/usr/lib/ /usr/"                            >> ${BUILD_DIR}/data/root/install_3s_opcuaserver.sh
echo "cp -au /tmp_libs/usr/lib/cds3-custom-components/ /usr/lib/" >> ${BUILD_DIR}/data/root/install_3s_opcuaserver.sh
echo "/etc/init.d/runtime start"                                  >> ${BUILD_DIR}/data/root/install_3s_opcuaserver.sh
echo "opkg remove opcuaserver_3s"                                 >> ${BUILD_DIR}/data/root/install_3s_opcuaserver.sh
echo ""                                                           >> ${BUILD_DIR}/data/root/install_3s_opcuaserver.sh
echo "exit 0"                                                     >> ${BUILD_DIR}/data/root/install_3s_opcuaserver.sh
echo ""                                                           >> ${BUILD_DIR}/data/root/install_3s_opcuaserver.sh

chmod +x ${BUILD_DIR}/data/root/install_3s_opcuaserver.sh

#---------------------------------------------------------
#create files to store in ipk
#---------------------------------------------------------
mkdir -p ${BUILD_DIR}/opc-ipkg
tar --owner=root --group=root -C ${BUILD_DIR}/control -czf ${BUILD_DIR}/opc-ipkg/control.tar.gz .
tar --owner=root --group=root -C ${BUILD_DIR}/data -czf ${BUILD_DIR}/opc-ipkg/data.tar.gz .
echo "2.0" > ${BUILD_DIR}/opc-ipkg/debian-binary

#---------------------------------------------------------
#create ipk file
#---------------------------------------------------------
rm -f $IPKGNAME
cd ${BUILD_DIR}/opc-ipkg
echo "INFO create OPKG package: $IPKGNAME"
ar r ../../$IPKGNAME debian-binary data.tar.gz control.tar.gz 2> /dev/null
cd $PTXPROJ

cleanup

exit 0