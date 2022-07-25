#!/bin/bash

function usage {
    printf 'usage: %s [PTXDIST_WORKSPACE] [DEST_FOLDER] [MODEM_FW_1_PATH] [MODEM_FW_2_PATH] ...\n' "$0"
}

function download_modem_fw_files {
	DEST=$1
	shift 1
	AMOUNT=$#
	
	for FW_FILE in $@; do
		FILE_NAME=$(echo $FW_FILE | rev | cut -d/ -f1 | rev)
		FILE_NAME_MD5=$(echo ${FILE_NAME}.md5)
	
		$PTXDIST_WORKSPACE/scripts/wago/artifactory.sh fetch \
			"$FW_FILE" \
			"$DEST/$FILE_NAME" \
			"$DEST/$FILE_NAME_MD5"
		$PTXDIST_WORKSPACE/scripts/production-helpers/check_md5sum.sh \
			"$DEST/$FILE_NAME_MD5" \
			"$DEST/$FILE_NAME"
		rm "$DEST/$FILE_NAME_MD5"
	done
}

function main {
	if [[ "$#" -ge "3" ]]; then
		PTXDIST_WORKSPACE=$1
		shift 1
		download_modem_fw_files "$@"
	else
		usage
		exit 1
	fi
}

SCRIPTDIR=$(dirname $0)
main "$@" && \
trap - SIGTERM SIGINT EXIT