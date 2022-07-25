#!/bin/dash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

FILE="$1"

FIRMWARE_MAJOR_LONG="$2"
FIRMWARE_MINOR_LONG="$3"
FIRMWARE_BUGFIX_LONG="$4"
FIRMWARE_REVISION_LONG="${FIRMWARE_MAJOR_LONG}${FIRMWARE_MINOR_LONG}${FIRMWARE_BUGFIX_LONG}"
FIRMWARE_INDEX="$5"
FIRMWARE_MAJOR="${FIRMWARE_MAJOR_LONG#0*}"
FIRMWARE_MINOR="${FIRMWARE_MINOR_LONG#0*}"
FIRMWARE_BUGFIX="${FIRMWARE_BUGFIX_LONG#0*}"
FIRMWARE_BASE="${FIRMWARE_MAJOR}.${FIRMWARE_MINOR}"
FIRMWARE_REVISION="${FIRMWARE_BASE}.${FIRMWARE_BUGFIX}"
RAUC_UPDATEFILE="$(basename "$6")"
PLATFORM="$7"

cat <<EOF > $FILE
<?xml version="1.0" encoding="utf-8"?>
<!-- Caution! Elements and attributes in this file are case sensitive! -->
<FirmwareUpdateFile StructureVersion="2.0" Revision="1">
  <FirmwareDescription Revision="${FIRMWARE_REVISION}" ReleaseIndex="${FIRMWARE_INDEX}">
  <AssociatedFiles>
    <File RefID="RAUC-File" Type="rauc" Name="$RAUC_UPDATEFILE"/>
  </AssociatedFiles>
  </FirmwareDescription>
   <ArticleList>
    <Article OrderNo="0768-3301" GroupRef="PFC_ADV-Common"/>
  </ArticleList>
  <GroupList>
    <Group RefID="PFC_ADV-Common">
      <Upgrade>
        <VersionList>
          <VersionRange SoftwareRevision="3.0.0-${FIRMWARE_BASE}.99"/>
        </VersionList>
      </Upgrade>
      <Downgrade>
        <VersionList>
          <VersionRange SoftwareRevision="3.0.0-99.99.99"/>
        </VersionList>
      </Downgrade>
    </Group>
  </GroupList>
</FirmwareUpdateFile>
EOF

exit 0
