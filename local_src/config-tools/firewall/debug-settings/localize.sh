#!/usr/bin/env bash
# eclipse launch configurations contain absolute paths. Use this script to replace them by paths to the current workspace.

scripts_path=../../../../wago_intern/debugging/scripts

source $(dirname $0)/"${scripts_path}"/relocation.sh
localize_launch_configs "${scripts_path}"
