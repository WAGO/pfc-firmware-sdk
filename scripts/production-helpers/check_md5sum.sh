#!/bin/bash

function usage {
    printf 'usage: %s [CHECKSUM_FILE] [TARGET_FILE]\n' "$0"
}

function check_md5 {
	if [ ! -f "$1" ]; then
		echo "checksum file does not exist"
		exit 1
	fi
	expected_checksum=$(cat "$1")

	echo "$expected_checksum" "$2" | md5sum -c -
}

function main
{
	if [[ "$#" == "2" ]]; then
		check_md5 "$@"
	else
		usage
		exit 1
	fi
}

main "$@" && \
trap - SIGTERM SIGINT EXIT