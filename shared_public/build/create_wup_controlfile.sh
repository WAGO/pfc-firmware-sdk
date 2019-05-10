#!/bin/dash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

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


echo "<?xml version=\"1.0\" encoding=\"utf-8\"?>" > "$FILE"
echo "<!-- Caution! Elements and attributes in this file are case sensitive! -->" >> "$FILE"
echo "<FirmwareUpdateFile StructureVersion=\"1.0\" Revision=\"1\" System=\"PFC-Linux\">" >> "$FILE"
echo "  <FirmwareDescription Revision=\"${FIRMWARE_REVISION}\" ReleaseIndex=\"${FIRMWARE_INDEX}\">" >> "$FILE"
echo "    <AssociatedFiles>" >> "$FILE"
echo "      <File RefID=\"RAUC-File\" Type=\"rauc\" Name=\"$RAUC_UPDATEFILE\" TargetPath=\"/tmp/fwupdate/update_${FIRMWARE_INDEX}_${FIRMWARE_REVISION_LONG}.raucb\"/>" >> "$FILE"
echo "    </AssociatedFiles>" >> "$FILE"
echo "  </FirmwareDescription>" >> "$FILE"
echo "  <ArticleList>" >> "$FILE"

case "$PLATFORM" in

    wago-pfcXXX|wago-pfcXXX-hardened)
        echo "    <Article OrderNo=\"0750-8100\"                      GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8101\"                      GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8101/0000-0010\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8101/0025-0000\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8102\"                      GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8102/0025-0000\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8202\"                      GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8202/0000-0011\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8202/0000-0012\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8202/0000-0013\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8202/0000-0014\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8202/0000-0015\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8202/0000-0022\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8202/0025-0000\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8202/0025-0001\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8202/0025-0002\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8202/0040-0000\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8202/0040-0001\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8202/K000-0001\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8203\"                      GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8203/0025-0000\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8203/K000-0001\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8204\"                      GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8204/0025-0000\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8204/K013-1080\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8204/K013-1080/0000-0001\"  GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8206\"                      GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8206/0000-0012\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8206/0025-0000\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8206/0025-0001\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8206/0040-0000\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8206/0040-0001\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8207\"                      GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8207/0000-0022\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8207/0025-0000\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8207/0025-0001\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8207/K000-0002\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8208\"                      GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8208/0025-0000\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8208/0025-0001\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8212\"                      GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8212/0000-0100\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8212/0025-0000\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8212/0025-0001\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8212/0025-0002\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8212/0040-0010\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8213\"                      GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8213/0040-0010\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8214\"                      GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8215\"                      GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8215/K000-0001\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8216\"                      GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8216/0025-0000\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8216/0025-0001\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0750-8216/K000-0001\"            GroupRef=\"PFC-Common\"/>" >> "$FILE"
        ;;

    vtp-ctp)
        echo "    <Article OrderNo=\"0762-4101\"                      GroupRef=\"TP-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0762-4102\"                      GroupRef=\"TP-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0762-4103\"                      GroupRef=\"TP-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0762-4104\"                      GroupRef=\"TP-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0762-4201/8000-0001\"            GroupRef=\"TP-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0762-4202/8000-0001\"            GroupRef=\"TP-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0762-4203/8000-0001\"            GroupRef=\"TP-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0762-4204/8000-0001\"            GroupRef=\"TP-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0762-4301/8000-0002\"            GroupRef=\"TP-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0762-4302/8000-0002\"            GroupRef=\"TP-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0762-4303/8000-0002\"            GroupRef=\"TP-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0762-4304/8000-0002\"            GroupRef=\"TP-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0762-5203/8000-0001\"            GroupRef=\"TP-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0762-5204/8000-0001\"            GroupRef=\"TP-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0762-5303/8000-0002\"            GroupRef=\"TP-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0762-5304/8000-0002\"            GroupRef=\"TP-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0762-6201/8000-0001\"            GroupRef=\"TP-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0762-6202/8000-0001\"            GroupRef=\"TP-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0762-6203/8000-0001\"            GroupRef=\"TP-Common\"/>" >> "$FILE"
        echo "    <Article OrderNo=\"0762-6204/8000-0001\"            GroupRef=\"TP-Common\"/>" >> "$FILE"
        ;;

    *)
        echo "<!-- No supported articles found for platform \"$PLATFORM\"! -->" >> "$FILE"
        ;;
esac
echo "  </ArticleList>" >> "$FILE"
echo "  <GroupList>" >> "$FILE"

case "$PLATFORM" in
    wago-pfcXXX|wago-pfcXXX-hardened)
        echo "    <Group RefID=\"PFC-Common\">" >> "$FILE"
        echo "      <Upgrade>" >> "$FILE"
        echo "        <VersionList>" >> "$FILE"
        echo "          <!-- Currently we only allow Upgrade inside FW${FIRMWARE_INDEX} (${FIRMWARE_BASE}.x) -->" >> "$FILE"
        echo "          <VersionRange SoftwareRevision=\"${FIRMWARE_BASE}.0-${FIRMWARE_BASE}.99\"/>" >> "$FILE"
        echo "        </VersionList>" >> "$FILE"
        echo "      </Upgrade>" >> "$FILE"
        echo "      <Downgrade>" >> "$FILE"
        echo "        <VersionList>" >> "$FILE"
        echo "          <!-- Currently we only allow Downgrade from any version beginning with FW12 (3.0.x) -->" >> "$FILE"
        echo "          <VersionRange SoftwareRevision=\"3.0.0-99.99.99\"/>" >> "$FILE"
        echo "        </VersionList>" >> "$FILE"
        echo "      </Downgrade>" >> "$FILE"
        echo "    </Group>" >> "$FILE"
        ;;

    vtp-ctp)
        echo "    <Group RefID=\"TP-Common\">" >> "$FILE"
        echo "      <Upgrade>" >> "$FILE"
        echo "        <VersionList>" >> "$FILE"
        echo "          <!-- Currently we only allow Upgrade inside FW${FIRMWARE_INDEX} (${FIRMWARE_BASE}.x) -->" >> "$FILE"
        echo "          <VersionRange SoftwareRevision=\"${FIRMWARE_BASE}.0-${FIRMWARE_BASE}.99\"/>" >> "$FILE"
        echo "        </VersionList>" >> "$FILE"
        echo "      </Upgrade>" >> "$FILE"
        echo "      <Downgrade>" >> "$FILE"
        echo "        <VersionList>" >> "$FILE"
        echo "          <!-- Currently we only allow Downgrade from any version beginning with FW12 (3.0.x) -->" >> "$FILE"
        echo "          <VersionRange SoftwareRevision=\"3.0.0-99.99.99\"/>" >> "$FILE"
        echo "        </VersionList>" >> "$FILE"
        echo "      </Downgrade>" >> "$FILE"
        echo "    </Group>" >> "$FILE"
        ;;

    *)
        echo "<!-- No supported firmware found for platform \"$PLATFORM\"! -->" >> "$FILE"
        ;;
esac

echo "  </GroupList>" >> "$FILE"
echo "</FirmwareUpdateFile>" >> "$FILE"

exit 0
