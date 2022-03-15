#!/bin/bash
#
# Copyright (C) 2012 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# find and prepare the config file and any includes
#
# returns:
# 0 if a config file and all includes are ready
# 1 otherwise
#
# array "genimage_configs" contains all config files starting with the
# requested file
#
ptxd_make_image_genimage_config() {
    local cfg tmp inc
    local -a includes
    ptxd_get_alternative config/images "${1}" || ptxd_bailout "could not find config file ${1}"
    cfg="${ptxd_reply}"
    tmp="${configdir}/$(basename "${1}")"

    eval \
	"${image_env}" \
	IMAGE="$(basename "${image_image}")" \
	ptxd_replace_magic "${cfg}" > "${tmp}" &&

    genimage_configs[${#genimage_configs[@]}]="${tmp}" &&
    ptxd_get_alternative_list config/images "${1}"
    echo "${image_image}: \$(firstword \$(wildcard ${ptxd_reply[*]}))" >> "${pkg_genimage_deps}"

    includes=( $(sed -n "s/.*\<include(['\"]\(.*\)['\"]).*/\1/p" "${tmp}") ) &&
    sed  -i "s:\(.*\<include(['\"]\)\(.*\)\(['\"]).*\):\1${configdir}/\2\3:" "${tmp}" &&
    for inc in "${includes[@]}"; do
	ptxd_make_image_genimage_config "${inc}"
    done
    if [ "${PTXDIST_VERBOSE}" -ne 0 ]; then
	echo "'$(ptxd_print_path ${cfg})' after substitutions:"
	echo "----------------"
	cat "${tmp}"
	echo "----------------"
	echo
    fi
    cp "${tmp}" "$PTXDIST_PLATFORMDIR/images/$(basename "$image_image").config"
}
export -f ptxd_make_image_genimage_config

#
# extract ipkg an generate a tgz image
#
ptxd_make_image_genimage_impl() {
    local tmpdir configdir file
    local -a genimage_configs
    tmpdir="$(mktemp -d "${PTXDIST_TEMPDIR}/genimage.XXXXXX")"
    configdir="$(mktemp -d "${PTXDIST_TEMPDIR}/genimage-config.XXXXXX")"

    ptxd_make_image_init &&
    pkg_genimage_deps="${ptx_state_dir}/${pkg_pkg}.deps" &&
    rm -f "${pkg_genimage_deps}" &&
    ptxd_make_image_genimage_config "${1}" &&

    rm -rf "${pkg_dir}" &&
    install -m 755 -d "${pkg_dir}" &&
    for file in ${image_files}; do
	ptxd_make_extract_archive "${file}" "${pkg_dir}"
    done &&
    if [ -n "${image_pkgs}" ]; then
	ptxd_get_ipkg_files ${image_pkgs} &&
	ptxd_make_image_extract_xpkg_files "${pkg_dir}"
    fi &&

    ptxd_eval \
	"${image_env}" \
	genimage \
	--loglevel $((PTXDIST_VERBOSE+1)) \
	--rootpath "${pkg_dir}" \
	--tmppath "${tmpdir}" \
	--outputpath "$(dirname "${image_image}")" \
	--inputpath "${ptx_image_dir}" \
	--config "${genimage_configs}" &&

    rm -r "${pkg_dir}"
}
export -f ptxd_make_image_genimage_impl

ptxd_make_image_genimage() {
    fakeroot ptxd_make_image_genimage_impl "${1}"
}
export -f ptxd_make_image_genimage
