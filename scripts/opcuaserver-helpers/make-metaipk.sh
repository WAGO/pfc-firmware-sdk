#!/usr/bin/env bash

# Build ipkg for installing opcuaserver on pfc firmware
#
# WAGO Kontakttechnik GmbH & Co. KG

PTXPROJ=$(pwd)

if [ -z "$PTXDIST_WORKSPACE" ]; then
  PTXDIST_WORKSPACE="${PTXDIST_WORKSPACE:-../../../ptxproj}"
  PTXDIST_PLATFORMDIR=$PTXDIST_WORKSPACE/platform-wago-pfcXXX
  echo !!! enveroment not set
  echo PTXDIST_WORKSPACE=$PTXDIST_WORKSPACE
  echo PTXDIST_PLATFORMDIR=$PTXDIST_PLATFORMDIR
fi

if echo "$PTXPROJ" | grep -q "/wago_intern/opcua/server"; then
BUILD_DIR=bin/
else
BUILD_DIR=$PTXDIST_PLATFORMDIR/packages/
fi

FW_VERSION=$(cat  $PTXDIST_WORKSPACE/projectroot/etc/REVISIONS | grep "FIRMWARE=" | cut -d= -f2 | sed -e 's/(/_/g;s/)//g' )


OPCUA_IPK=$(basename $(find $PTXDIST_PLATFORMDIR/packages -name "opcuaserver_*_armhf.ipk"| sed -e 's/.ipk//g'))
if [ "$OPCUA_IPK" = "" ]; then
  echo !!! OPCUA_IPK not found
  exit 1
fi
OPCUA_IPK_VERSION=$(echo $OPCUA_IPK|sed -e 's/opcuaserver_//' -e 's/_armhf//')

ABSEIL_CPP_IPK=$(ls $PTXDIST_PLATFORMDIR/packages | grep abseil-cpp_ | cut -f2)
if [ "$ABSEIL_CPP_IPK" = "" ]; then
  echo !!! ABSEIL_CPP_IPK not found
  exit 1
fi

C_ARES_IPK=$(ls $PTXDIST_PLATFORMDIR/packages | grep c-ares_ | cut -f2 )
if [ "$C_ARES_IPK" = "" ]; then
  echo !!! C_ARES_IPK not found
  exit 1
fi

PROTOBUF_IPK=$(ls $PTXDIST_PLATFORMDIR/packages | grep protobuf_ | cut -f2 )
if [ "$PROTOBUF_IPK" = "" ]; then
  echo !!! PROTOBUF_IPK not found
  exit 1
fi

GRPC_IPK=$(ls $PTXDIST_PLATFORMDIR/packages | grep grpc_ | cut -f2 )
if [ "$GRPC_IPK" = "" ]; then
  echo !!! GRPC_IPK not found
  exit 1
fi

FLEX_IPK=$(ls $PTXDIST_PLATFORMDIR/packages | grep flex_ | cut -f2 )
if [ "$FLEX_IPK" = "" ]; then
  echo !!! FLEX_IPK not found
  exit 1
fi

PAM_IPK=$(ls $PTXDIST_PLATFORMDIR/packages | grep pam_ | cut -f2 )
if [ "$PAM_IPK" = "" ]; then
  echo !!! PAM_IPK not found
  exit 1
fi
OPCUACSDK_IPK=$(ls $PTXDIST_PLATFORMDIR/packages | grep opcuacsdk_ | cut -f2 )
if [ "$OPCUACSDK_IPK" = "" ]; then
  echo !!! OPCUACSDK_IPK not found
  exit 1
fi

CDS3_TSCVAREXPORT_IPK=$(ls $PTXDIST_PLATFORMDIR/packages | grep cds3-tscvarexport_ | cut -f2 )
if [ "$CDS3_TSCVAREXPORT_IPK" = "" ]; then
  echo !!! CDS3_TSCVAREXPORT_IPK not found
  exit 1
fi

WAGO_NG_PLUGIN_OPCUA_IPK=$(ls $PTXDIST_PLATFORMDIR/packages | grep wbm-ng-plugin-opcua_ | cut -f2 )
if [ "$WAGO_NG_PLUGIN_OPCUA_IPK" = "" ]; then
  echo !!! WAGO_NG_PLUGIN_OPCUA_IPK not found
  exit 1
fi


PACKAGES="$WAGO_NG_PLUGIN_OPCUA_IPK $ABSEIL_CPP_IPK $C_ARES_IPK $PROTOBUF_IPK $GRPC_IPK $FLEX_IPK $PAM_IPK $OPCUACSDK_IPK $CDS3_TSCVAREXPORT_IPK ${OPCUA_IPK}.ipk"
IPKGNAME="opcuarepo_${OPCUA_IPK_VERSION}_FW${FW_VERSION}.ipk"

#--------------------------------------
# Cleanup function
#--------------------------------------
function cleanup
{
  rm -rf ${BUILD_DIR}opc-ipkg-src ${BUILD_DIR}opc-ipkg
}


mkdir -m 700 -p ${BUILD_DIR}opc-ipkg-src/data/root
mkdir -p ${BUILD_DIR}opc-ipkg-src/data/root/packages
mkdir -p ${BUILD_DIR}opc-ipkg-src/control

echo PTXPROJ=$PTXPROJ
echo PACKAGES=$PACKAGES
echo IPKGNAME=$IPKGNAME
echo OPCUA_IPK_VERSION=$OPCUA_IPK_VERSION

#---------------------------------------------------------
#get ipk files stored in this meta packet
#---------------------------------------------------------
for PACKAGE in $PACKAGES; do
  if [ ! -f "$PTXDIST_PLATFORMDIR/packages/$PACKAGE" ]; then
    echo "ERROR Could not find package $PTXDIST_PLATFORMDIR/packages/$PACKAGE"
    cleanup
    exit 1
  fi
  echo "INFO  get package $PACKAGE"
  cp "$PTXDIST_PLATFORMDIR/packages/$PACKAGE" ${BUILD_DIR}opc-ipkg-src/data/root/packages
done
#---------------------------------------------------------
#create repository file
#---------------------------------------------------------
#
echo '#!/usr/bin/env bash' > ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo 'set -e' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo '' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo 'pkg_dir=$1' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo ' ' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo ' if [ -z $pkg_dir ] || [ ! -d $pkg_dir ]; then' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo '   echo "Usage: ipkg-make-index <package_directory>" >&2' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo '   exit 1' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo ' fi' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo ' ' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo ' which md5sum >/dev/null 2>&1 || alias md5sum=md5' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo ' empty=1' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo ' ' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo ' for pkg in `find $pkg_dir -name '\''*.ipk'\'' | sort`; do' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo '   empty=' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo '   name="${pkg##*/}"' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo '   name="${name%%_*}"' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo ' # [[ "$name" = "kernel" ]] && continue' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo ' # [[ "$name" = "libc" ]] && continue' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo '   echo "Generating index for package $pkg" >&2' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo '   file_size=$(ls -l $pkg | awk '\''{print $5}'\'')' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo '   md5sum=$(md5sum $pkg | awk '\''{print $1}'\'')' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo '   sha256sumvar=$(sha256sum $pkg | awk '\''{print $1}'\'')' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo '   # Take pains to make variable value sed-safe' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo '   sed_safe_pkg=`echo $pkg | sed -e '\''s/^\.\///g'\'' -e '\''s/\\//\\\\\\//g'\''`' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo '   ar x $pkg ./control.tar.gz ' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo '   tar xzf ./control.tar.gz ./control' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo ' cat ./control | sed -e "s/^Description:/Filename: $sed_safe_pkg\\' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo 'Size: $file_size\\' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo 'MD5Sum: $md5sum\\' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo 'SHA256sum: $sha256sumvar\\' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo 'Description:/"' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo 'rm -f ./control.tar.gz' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo 'rm -f ./control' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo '   echo ""' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo ' done' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo ' [ -n "$empty" ] && echo' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
echo ' exit 0' >> ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh
chmod +x ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh

cd ${BUILD_DIR}opc-ipkg-src/data/root/packages
./ipkg-make-index.sh ./ > Packages
gzip Packages

cd $PTXPROJ
rm ${BUILD_DIR}opc-ipkg-src/data/root/packages/ipkg-make-index.sh

#---------------------------------------------------------
#create control file
#---------------------------------------------------------
echo "Package: opcua_repo" > ${BUILD_DIR}opc-ipkg-src/control/control
echo "Priority: optional" >> ${BUILD_DIR}opc-ipkg-src/control/control
echo "Version:  $OPCUA_IPK_VERSION" >> ${BUILD_DIR}opc-ipkg-src/control/control
echo "Section:    base" >> ${BUILD_DIR}opc-ipkg-src/control/control
echo "Architecture: armhf" >> ${BUILD_DIR}opc-ipkg-src/control/control
echo "Maintainer: \"WAGO Kontakttechnik GmbH & Co. KG (jobst.wellensiek@wago.com)\" ">> ${BUILD_DIR}opc-ipkg-src/control/control
echo "Depends: ">> ${BUILD_DIR}opc-ipkg-src/control/control
echo "Source: ">> ${BUILD_DIR}opc-ipkg-src/control/control
echo "Description:  Installation for WAGO opcuaserver. This packet is a meta packet including all parts need for server." >> ${BUILD_DIR}opc-ipkg-src/control/control
echo "              This packet is build for Firmware Version $FW_VERSION" >> ${BUILD_DIR}opc-ipkg-src/control/control
echo "              The following packets are included." >> ${BUILD_DIR}opc-ipkg-src/control/control
for PACKAGE in $PACKAGES; do
  echo "              $PACKAGE" >> ${BUILD_DIR}opc-ipkg-src/control/control
done
echo "              The packet install a local repo only. Use opkg update and opkg install opcuaserver to install server." >> ${BUILD_DIR}opc-ipkg-src/control/control
#---------------------------------------------------------
#create postinst file
#---------------------------------------------------------
echo "#!/bin/bash" > ${BUILD_DIR}opc-ipkg-src/control/postinst
echo "#" >> ${BUILD_DIR}opc-ipkg-src/control/postinst
echo "# WAGO Kontakttechnik GmbH & Co. KG" >> ${BUILD_DIR}opc-ipkg-src/control/postinst
echo "" >> ${BUILD_DIR}opc-ipkg-src/control/postinst

echo 'sed -i -e "s/src ptxdist http:\/\/www.example.org\/ptxdist\/PFC\/dists\/PFCPFC-trunk/src\/gz local file:\/\/\/root\/packages/g" /etc/opkg/opkg.conf' >> ${BUILD_DIR}opc-ipkg-src/control/postinst
echo "rm -f /usr/lib/libCmpOPCUA*"  >> ${BUILD_DIR}opc-ipkg-src/control/postinst
echo "rm -f /usr/lib/cds3-custom-components/libCmpOPCUA*"  >> ${BUILD_DIR}opc-ipkg-src/control/postinst

echo "source /etc/profile &>/dev/null" >> ${BUILD_DIR}opc-ipkg-src/control/postinst
echo "function start_update" >> ${BUILD_DIR}opc-ipkg-src/control/postinst
echo "{" >> ${BUILD_DIR}opc-ipkg-src/control/postinst
echo "    local PID_OF_IPKG=\$1" >> ${BUILD_DIR}opc-ipkg-src/control/postinst
echo "    while ps -p \$PID_OF_IPKG > /dev/null" >> ${BUILD_DIR}opc-ipkg-src/control/postinst
echo "    do" >> ${BUILD_DIR}opc-ipkg-src/control/postinst
echo "        usleep 100000" >> ${BUILD_DIR}opc-ipkg-src/control/postinst
echo "    done" >> ${BUILD_DIR}opc-ipkg-src/control/postinst
echo "    export PREPARE_MODE=@PREPARE_MODE@" >> ${BUILD_DIR}opc-ipkg-src/control/postinst
echo "    /root/install_opcuaserver.sh 2>&1 |  tee -a /var/log/opcua.log" >> ${BUILD_DIR}opc-ipkg-src/control/postinst
echo "    sync" >> ${BUILD_DIR}opc-ipkg-src/control/postinst
echo "}" >> ${BUILD_DIR}opc-ipkg-src/control/postinst
echo "" >> ${BUILD_DIR}opc-ipkg-src/control/postinst
echo "PID_OF_IPKG=\$(pidof opkg)" >> ${BUILD_DIR}opc-ipkg-src/control/postinst
echo "start_update \$PID_OF_IPKG &" >> ${BUILD_DIR}opc-ipkg-src/control/postinst


echo "" >> ${BUILD_DIR}opc-ipkg-src/control/postinst
echo "exit 0" >> ${BUILD_DIR}opc-ipkg-src/control/postinst
echo "" >> ${BUILD_DIR}opc-ipkg-src/control/postinst

chmod +x ${BUILD_DIR}opc-ipkg-src/control/postinst

#---------------------------------------------------------
#create install_opcuaserver.sh
#---------------------------------------------------------
echo "#!/bin/sh"                                        > ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo ""                                                 >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "/etc/init.d/runtime stop"                         >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "ORDER=\$(/etc/config-tools/get_typelabel_value -n ORDER)" >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "CHECK_ORDER_NUMBER=\${ORDER:10:3}"                >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "if [ \"810\" = \"\$CHECK_ORDER_NUMBER\" ] || [ \"820\" = \"\$CHECK_ORDER_NUMBER\" ]; then"  >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "    opkg remove cds3-tsciobacnet"                 >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "    opkg remove cds3-iodrvbacnet"                 >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "    opkg remove libbacnet"                        >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "    opkg remove libbacnetstack"                   >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "    opkg remove wbm-ng-plugin-bacnet"             >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "    opkg remove --force-depends libbacnetconfig"  >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "    opkg remove profinetdemo"                     >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "    opkg remove cds3-tscsyspn"                    >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "    opkg remove cds3-iodrvpn"                     >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "    opkg remove libprofinet"                      >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "    opkg remove screen"                           >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "    opkg remove ppp"                              >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "    opkg clean"                                   >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "fi"                                               >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo ""                                                 >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "opkg remove opcuaserver"                          >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "opkg remove opcuacsdk"                            >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "opkg remove cds3-tscvarexport"                    >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "opkg remove grpc"                                 >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "opkg remove protobuf"                             >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "opkg remove c-ares"                               >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "opkg remove abseil-cpp"                           >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "opkg remove wbm-ng-plugin-opcua"                  >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "opkg clean"                                       >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo ""                                                 >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "opkg update"                                      >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "opkg install opcuaserver"                         >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "opkg remove opcua_repo"                           >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo "/etc/init.d/runtime start"                        >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
echo ""                                                 >> ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh
chmod +x ${BUILD_DIR}opc-ipkg-src/data/root/install_opcuaserver.sh

#---------------------------------------------------------
#create files to store in ipk
#---------------------------------------------------------
mkdir -p ${BUILD_DIR}opc-ipkg
tar --owner=root --group=root -C ${BUILD_DIR}opc-ipkg-src/control -czf ${BUILD_DIR}opc-ipkg/control.tar.gz .
tar --owner=root --group=root -C ${BUILD_DIR}opc-ipkg-src/data -czf ${BUILD_DIR}opc-ipkg/data.tar.gz .
echo "2.0" > ${BUILD_DIR}opc-ipkg/debian-binary



#---------------------------------------------------------
#create ipk file
#---------------------------------------------------------
rm -f $IPKGNAME
cd ${BUILD_DIR}opc-ipkg
echo "OPKG  create package $IPKGNAME"
ar r ../$IPKGNAME debian-binary data.tar.gz control.tar.gz 2> /dev/null
cd $PTXPROJ



if echo "$PTXPROJ" | grep -q "/wago_intern/opcua/server"; then
echo cleanup not done
#cleanup
else
#echo no cleanup
cleanup
fi

exit 0
