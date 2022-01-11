#!/bin/bash
#
# Copyright (C) 2010 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

ptxd_exist() {
    for file in "${@}"; do
	if [ ! -e "${file}" ]; then
	    echo -e "\nError: file not found: ${file}\n"
	    return 1
	fi
    done
}
export -f ptxd_exist

ptxd_install_error() {
    echo Error: "$@"
    exit 1
}
export -f ptxd_install_error

#
# ptxd_install_getent_id
#
# convert usr or grp into numeric value
#
# $1: "usr", "grp"
#
ptxd_install_getent_id() {
    local key="${1}"
    local db id

    case "${key}" in
	usr)
	    id="user id"
	    db="/etc/passwd"
	    ;;
	grp)
	    id="group id"
	    db="/etc/group"
	    ;;
    esac

    eval ${key}_name='\(${!key}\)'

    if ! ptxd_get_alternative "projectroot" "${db}"; then
	ptxd_bailout "

  Unable to find '${db}'.

"
    fi

    local line
    if ! line="$(grep -e "^${!key}" "${ptxd_reply}")"; then
	ptxd_bailout "

${id} '${!key}' not found in '$(ptxd_print_path ${ptxd_reply})' for:
  '${dst:-${dir}}'

"
    fi
    local orig_IFS="${IFS}"
    local IFS=":"
    set -- $line
    IFS="${orig_IFS}"
    eval ${key}="${3}"
}
export -f ptxd_install_getent_id

#
# ptxd_install_resolve_usr_grp
#
# convert usr/grp that contain names into numeric values
#
ptxd_install_resolve_usr_grp() {
    if [ -n "$usr" ] && ! [ 0 -le "$usr" ] 2>/dev/null; then
	ptxd_install_getent_id usr || return
    else
	unset usr_name
    fi
    if [ -n "$grp" ] && ! [ 0 -le "$grp" ] 2>/dev/null; then
	ptxd_install_getent_id grp || return
    else
	unset grp_name
    fi
}
export -f ptxd_install_resolve_usr_grp

ptxd_install_setup_global() {
    local -a nfsroot_dirs

    nfsroot_dirs=("${ptx_nfsroot}" ${pkg_nfsroot_dirs})

    # all dirs
    dirs=("${nfsroot_dirs[@]}" "${pkg_xpkg_tmp}")

    # nfs root dirs
    # no setuid/setguid bit here
    ndirs=("${nfsroot_dirs[@]}")

    # package dirs
    # this goes into the ipkg, thus full file modes here
    pdirs=("${pkg_xpkg_tmp}")

    # strip dirs
    sdirs=("${nfsroot_dirs[@]}" "${pkg_xpkg_tmp}")

    # dirs with separate debug files
    ddirs=()
    if [ "$(ptxd_get_ptxconf PTXCONF_TARGET_DEBUG_OFF)" != "y" ]; then
	ddirs=("${nfsroot_dirs[@]}")
	if [ "$(ptxd_get_ptxconf PTXCONF_DEBUG_PACKAGES)" = "y" ]; then
	    ddirs[${#ddirs[@]}]="${pkg_xpkg_dbg_tmp}"
	fi
    fi
}
export -f ptxd_install_setup_global

ptxd_install_lock() {
    local lockfile
    mkdir -p "${PTXDIST_TEMPDIR}/locks"

    if [ -n "${dst}" ]; then
	lockfile="${PTXDIST_TEMPDIR}/locks/${dst//\//-}"
    else
	lockfile="${PTXDIST_TEMPDIR}/locks/other"
    fi
    if [ -n "${dst_lock}" ]; then
	ptxd_bailout "dst_lock must not be set when ptxd_install_lock() is called"
    fi
    exec {dst_lock}>"${lockfile}" &&
    flock "${dst_lock}"
}
export -f ptxd_install_lock

ptxd_install_unlock() {
    local ret=$?

    if [ -z "${dst_lock}" ]; then
	ptxd_bailout "dst_lock must be set when ptxd_install_unlock() is called"
    fi
    exec {dst_lock}>&-
    unset dst_lock
    # propagate the last error after unlocking
    return "${ret}"
}
export -f ptxd_install_unlock

ptxd_install_setup() {
    case "${dst}" in
	/bin/*|/sbin/*|/lib/*) dst="/usr${dst}" ;;
	/*|"") ;;
	*) ptxd_bailout "${FUNCNAME[${#FUNCNAME[@]}-5]}: 'dst' must be an absolute path!" ;;
    esac

    mod_nfs="$(printf "0%o" $(( 0${mod} & ~06000 )))" &&
    mod_rw="$(printf "0%o" $(( 0${mod} | 0200 )))" &&

    #
    # mangle user/group
    #
    ptxd_install_resolve_usr_grp &&

    #
    # lock destination to avoid parallel building issues
    #
    ptxd_install_lock
}
export -f ptxd_install_setup

ptxd_install_setup_src_list() {
    if [ "${cmd}" = "alternative" -o "${cmd}" = "config" ]; then
	#
	# if pkg_dir is empty we'll have some some empty entries in
	# the array, but that's no problem for the "-e" below.
	#
	local -a ptxd_reply
	ptxd_get_alternative_list projectroot "${1}"
	list=( \
	    "${ptxd_reply[@]}" \
	    "${pkg_pkg_dir:+${pkg_pkg_dir}${1}}" \
	    "${pkg_dir:+${pkg_dir}${1}}" \
	    )
    else
	list=( \
	    "${1}${PTXDIST_PLATFORMSUFFIX}" \
	    "${1}" \
	    )
    fi
}
export -f ptxd_install_setup_src_list

ptxd_install_setup_src() {
    local -a list
    local legacy_src

    if [ "${src}" = "-" -a -n "${dst}" ]; then
	src="${pkg_pkg_dir}${dst}"
	gdb_src="${pkg_pkg_dir}/usr/share/gdb/auto-load${dst}-gdb"
    elif [ "${src#${pkg_pkg_dir}}" != "${src}" ]; then
	gdb_src="${pkg_pkg_dir}/usr/share/gdb/auto-load${src#${pkg_pkg_dir}}-gdb"
    fi

    ptxd_install_setup || return

    legacy_src="${src#/usr}"
    if [ \( "${cmd}" = "alternative" -o "${cmd}" = "config" \) -a "${legacy_src}" != "${src}" ]; then
	pkg_dir= ptxd_install_setup_src_list "${legacy_src}"
	if ptxd_get_path "${list[@]}"; then
	    local tmp
	    echo -e "\nFound file for '${dst}' in these legacy locations:\n"
	    for tmp in "${ptxd_reply[@]}"; do
		echo "$(ptxd_print_path "${tmp}")"
	    done
	    echo
	    ptxd_bailout "They must be moved to the corresponding locations in '/usr'"
	fi
    fi

    ptxd_install_setup_src_list "${src}"

    # Since the dependency to the source files is dynamic we store
    # the dependency information in a dependency file that can be
    # included in the make files itself.
    # We depend on the first available file, which is the one that will
    # be used. If one with a higher priority is created, the dependency
    # will cause the package to be recreated.
    local deprule=""
    for src in "${list[@]}"; do
	# don't provide dependencies for files in PTXDIST_PLATFORMDIR.
	if [ "${src}" == "${src#${PTXDIST_PLATFORMDIR}}" -a -n "${src}" ]; then
		deprule="${deprule} ${src}"
	fi
    done
    if [ -n "${deprule}" ]; then
	deprule="${ptx_state_dir}/${pkg_stamp}: \$(firstword \$(wildcard ${deprule}))"
	# Make the deps rule robust for varying installation paths, and
	# make the deps rules file more readable.
	deprule=${deprule//${PTXDIST_TOPDIR}/\$(PTXDIST_TOPDIR)}
	deprule=${deprule//${PTXDIST_WORKSPACE}/\$(PTXDIST_WORKSPACE)}
	echo "${deprule}" >> ${pkg_xpkg_install_deps}
    fi
    if ptxd_get_path "${list[@]}"; then
	src="${ptxd_reply}"
	return
    fi

    echo -e "\nNo suitable file '${dst}' could be found in any of these locations:"
    local orig_IFS="${IFS}"
    local IFS="
"
    echo -e "${list[*]}\n"
    IFS="${orig_IFS}"
    return 1
}
export -f ptxd_install_setup_src

ptxd_install_virtfs() {
    local mod_virtfs="$(( 0${mod} | ${mod_type} ))"
    local d dir file

    if [ "${PTXCONF_SETUP_NFS_VIRTFS}" != "y" ]; then
	return
    fi

    for d in "${ndirs[@]/%/${dst}}"; do
	dir="${d%/*}/.virtfs_metadata"
	file="${dir}/${d##*/}"&&
	mkdir -p "${dir}" &&
	cat <<- EOF > "${file}"
	virtfs.uid=${usr}
	virtfs.gid=${grp}
	virtfs.mode=${mod_virtfs}
	EOF
	if [ -n "${major}" -a -n "${minor}" ]; then
	    local rdev=$[ ${major} << 8 | ${minor} ] &&
	    echo "virtfs.rdev=${rdev}" >> "${file}"
	fi || break
    done
}
export -f ptxd_install_virtfs

ptxd_install_dir_impl() {
    local mod_type=0040000

    if [ "${dst}" != "" ]; then
	ptxd_ensure_dir "${dst%/*}"
    fi &&

    if [ "${1}" != "keep" ]; then
	install -m "${mod_nfs}" -d "${ndirs[@]/%/${dst}}" &&
	install -m "${mod}" -o "${usr}" -g "${grp}" -d "${pdirs[@]/%/${dst}}"
    else
	# don't overwrite existing permissions
	mkdir -p "${dirs[@]/%/${dst}}"
    fi &&

    ptxd_install_virtfs
}
export -f ptxd_install_dir_impl

ptxd_ensure_dir() {
    local dst="$1"
    local usr="0"
    local grp="0"
    local mod="0755"
    local mod_nfs="0755"
    local mod_rw="0755"
    local dir no_skip dst_lock

    for dir in "${ndirs[@]/%/${dst}}"; do
	if [ ! -d "${dir}" -o ! -e "${dir%/*}/.virtfs_metadata/${dir##*/}" ]; then
	    no_skip=1
	    break
	fi
    done
    if [ "${no_skip}" != 1 ]; then
	# just create the rest and continue if virtfs data already exists
	#  but don't overwrite existing permissions
	mkdir -p "${dirs[@]/%/${dst}}" &&
	return
    fi &&
    ptxd_install_lock &&
    ptxd_install_dir_impl keep
    ptxd_install_unlock
}
export -f ptxd_ensure_dir

ptxd_install_dir() {
    local sep="$(echo -e "\x1F")"
    local dst="${1%/}"
    local usr="$2"
    local grp="$3"
    local mod="$4"
    local mod_nfs mod_rw dst_lock

    ptxd_install_setup &&
    echo "\
install directory:
  dir=${dst}
  owner=${usr} ${usr_name}
  group=${grp} ${grp_name}
  permissions=${mod}
" &&

    ptxd_install_dir_impl &&
    echo "d${sep}${dst}${sep}${usr}${sep}${grp}${sep}${mod}" >> "${pkg_xpkg_perms}" ||
    ptxd_install_error "install_dir failed!"
    ptxd_install_unlock
}
export -f ptxd_install_dir

ptxd_extract_build_id() {
    # accept only the first build-id
    ${CROSS_READELF} -n ${src} | awk 'BEGIN { FS=":\\s*" } /Build ID/{ print $2; exit }'
}
export -f ptxd_extract_build_id

export ptxd_install_file_objcopy_args="--only-keep-debug --compress-debug-sections"

ptxd_install_file_extract_debug() {
    local dst="${1}"
    local tmp="$(mktemp -u "${PTXDIST_TEMPDIR}/debug.XXXXX")"
    local dbg dir
    local bid=$(ptxd_extract_build_id)

    if [ "${#ddirs[*]}" -eq 0 ]; then
	return
    fi

    if [ -z "${bid}" ]; then
	dbg="${dst%/*}/.debug/.${dst##*/}.dbg"
    else
	local path_component=${bid::-38}
	local name_component=${bid:2:38}
	dbg="/usr/lib/debug/.build-id/${path_component}/${name_component}.debug"
    fi

    # this can fail if objcopy does not support compressing debug sections or
    # is compiled without zlib support
    "${CROSS_OBJCOPY}" ${ptxd_install_file_objcopy_args} "${src}" "${tmp}" |&
	grep -q "\(unrecognized option\|unable to initialize com*press status\)"
    local -a status=( "${PIPESTATUS[@]}" )
    if [ ${status[0]} -ne 0 ]; then
	if [ ${status[1]} -eq 0 ]; then
	    ptxd_install_file_objcopy_args="--only-keep-debug"
	    "${CROSS_OBJCOPY}" ${ptxd_install_file_objcopy_args} "${src}" "${tmp}"
	else
	    # do it again to see the error message
	    "${CROSS_OBJCOPY}" ${ptxd_install_file_objcopy_args} "${src}" "${tmp}"
	fi
    fi &&
    echo "  debug file: ${dbg}" &&
    for dir in "${ddirs[@]}"; do
	if [ -n "${bid}" -o -e "${dir}${dst}" ]; then
	    install -D -m 644 "${tmp}" "${dir}/${dbg}"
	fi &&
	if [ -z "${bid}" -a -e "${dir}${dst}" ]; then
	    "${CROSS_OBJCOPY}" --add-gnu-debuglink "${dir}/${dbg}" "${dir}${dst}"
	fi
    done &&
    rm "${tmp}"
}
export -f ptxd_install_file_extract_debug

#
# $1: file to strip
#
# $strip: k for kernel modules
#         y for normal executables and libraries
#
#
ptxd_install_file_strip() {
    local -a strip_cmd files
    local dst="${1}"
    local i file

    case "${strip:-y}" in
	k) strip_cmd=( "${CROSS_STRIP}" --strip-debug -R .GCC.command.line ) ;;
	y) strip_cmd=( "${CROSS_STRIP}" -R .note -R .comment -R .GCC.command.line ) ;;
    esac

    files=( "${sdirs[@]/%/${dst}}" )
    install -d "${files[0]%/*}" &&
    "${strip_cmd[@]}" -o "${files[0]}" "${src}" &&
    for (( i=1 ; ${i} < ${#files[@]} ; i=$[i+1] )); do
	install -m "${mod_rw}" -D "${files[0]}" "${files[${i}]}" || return
    done &&

    files=( "${dirs[@]/%/${dst}}" ) &&
    for file in "${files[@]}"; do
	if [ ! -e "${file}" ]; then
	    install -m "${mod_rw}" -D "${src}" "${file}" || return
	fi
    done &&

    if [ "${strip}" != "k" ]; then
	ptxd_install_file_extract_debug "${dst}" || return
    fi
}
export -f ptxd_install_file_strip


ptxd_install_file_impl() {
    local sep="$(echo -e "\x1F")"
    local src="$1"
    local dst="$2"
    local usr="$3"
    local grp="$4"
    local mod="$5"
    local strip="$6"
    local mod_type=0100000
    local mod_nfs mod_rw dst_lock
    local gdb_src

    ptxd_install_setup_src &&
    echo "\
install ${cmd}:
  src=$(ptxd_print_path "${src}")
  dst=${dst}
  owner=${usr} ${usr_name}
  group=${grp} ${grp_name}
  permissions=${mod}" &&

    ptxd_exist "${src}" &&
    rm -f "${dirs[@]/%/${dst}}" &&

    # check if src is a link
    if [ -L "${src}" ]; then
	local old="${src}"
	src="$(readlink -f "${src}")" &&
	if [ "${src}" = /dev/null ]; then
	    echo "'${old}' is a link to '/dev/null', skipping file."
	    echo
	    return
	fi &&
	ptxd_pedantic "file '$(ptxd_print_path "${old}")' is a link" &&
	echo "using '$(ptxd_print_path "${src}")' instead"
    fi &&

    if [ -z "${strip}" ]; then
	if ! "${CROSS_READELF}" -h "${src}" &> /dev/null; then
	    strip="n"
	else
	    strip="y"
	fi
    fi &&

    ptxd_ensure_dir "${dst%/*}" &&

    case "${strip}" in
	0|n|no|N|NO)
	    for d in "${dirs[@]/%/${dst}}"; do
		install -m "${mod_rw}" -D "${src}" "${d}" || return
	    done
	    ;;
	y|k)
	    ptxd_install_file_strip "${dst}"
	    ;;
	*)
	    if [ "${strip:0:1}" = "/" ] && \
		[ "${cmd}" = "alternative" -o "${cmd}" = "config" ]; then
		ptxd_bailout "
the 6th parameter of 'install_${cmd}' is strip, not the destination.
Usually, just remove the 6th parameter and everything works fine.
"
	    fi
	    ptxd_bailout "${FUNCNAME}: invalid value for strip ('${strip}')"
	    ;;
    esac &&

    if [ "${#ddirs[*]}" -gt 0 -a -n "${gdb_src}" ]; then
	local gdb_file
	local ddir="${dst%/*}"
	for gdb_file in $(ls "${gdb_src}".* 2>/dev/null); do
	    local gdb_dst="${ddir}/${gdb_file##*/}"
	    echo "  debug file: ${gdb_dst}" &&
	    for d in "${ddirs[@]/%/${gdb_dst}}"; do
		install -m 644 -D "${gdb_file}" "${d}" || break
	    done
	done
    fi &&
    echo "" &&

    # now change to requested permissions
    chmod "${mod_nfs}" "${ndirs[@]/%/${dst}}" &&
    chmod "${mod}"     "${pdirs[@]/%/${dst}}" &&

    # now change to requested user and group
    chown "${usr}:${grp}" "${pdirs[@]/%/${dst}}" &&

    ptxd_install_virtfs &&

    echo "f${sep}${dst}${sep}${usr}${sep}${grp}${sep}${mod}" >> "${pkg_xpkg_perms}"
    ptxd_install_unlock
}
export -f ptxd_install_file_impl

ptxd_install_ln() {
    local src="$1"
    local dst="$2"
    local usr="${3:-0}"
    local grp="${4:-0}"
    local mod="0777"
    local mod_type=0120000
    local mod_nfs mod_rw rel dst_lock

    ptxd_install_setup &&
    echo "\
install link:
  src=${src}
  dst=${dst}
" &&

    case "${src}" in
	/*)
	    if [ "${PTXCONF_SETUP_NFS_REL_SYMLINK}" = "y" ]; then
		rel="$(dirname "${dst}" | sed -e 's,/[^/]*,/..,g' -e 's,^/,,')"
	    fi
	    ;;
	*)  ;;
    esac &&

    rm -f "${dirs[@]/%/${dst}}" &&
    ptxd_ensure_dir "${dst%/*}" &&
    for d in "${ndirs[@]/%/${dst}}"; do
	ln -s "${rel}${src}" "${d}" || return
    done &&
    for d in "${pdirs[@]/%/${dst}}"; do
	ln -s "${src}" "${d}" || return
    done &&

    ptxd_install_virtfs &&

    chown --no-dereference "${usr}:${grp}" "${dirs[@]/%/${dst}}"
    ptxd_install_unlock
}
export -f ptxd_install_ln

ptxd_install_mknod() {
    local sep="$(echo -e "\x1F")"
    local dst="$1"
    local usr="$2"
    local grp="$3"
    local mod="$4"
    local type="$5"
    local major="$6"
    local minor="$7"
    local mod_nfs mod_rw mod_type dst_lock

    case "${type}" in
	c) mod_type=0020000 ;;
	b) mod_type=0060000 ;;
	p) mod_type=0010000 ;;
    esac &&

    ptxd_install_setup &&
    echo "\
install device node:
  owner=${usr}
  group=${grp}
  permissions=${mod}
  type=${type}
  major=${major}
  minor=${minor}
  name=${dst}
" &&

    rm -f "${pdirs[@]/%/${dst}}" &&
    ptxd_ensure_dir "${dst%/*}" &&
    for d in "${pdirs[@]/%/${dst}}"; do
	mknod -m "${mod}" "${d}" "${type}" ${major} ${minor} || return
    done &&
    touch "${ndirs[@]/%/${dst}}" &&
    chown "${usr}:${grp}" "${pdirs[@]/%/${dst}}" &&

    ptxd_install_virtfs &&

    echo "n${sep}${dst}${sep}${usr}${sep}${grp}${sep}${mod}${sep}${type}${sep}${major}${sep}${minor}" >> "${pkg_xpkg_perms}"
    ptxd_install_unlock
}
export -f ptxd_install_mknod

ptxd_install_alternative() {
    local cmd="alternative"
    local src="${1}"
    local dst="${2}"
    shift 2
    ptxd_install_file_impl "${src}" "${dst:-${src}}" "${@}" ||
    ptxd_install_error "install_alternative failed!"
}
export -f ptxd_install_alternative

ptxd_install_config() {
    local cmd="config"
    local src="${1}"
    local dst="${2}"
    shift 2
    ptxd_install_file_impl "${src}" "${dst:-${src}}" "${@}" &&
    # this is a config file add it to conffiles so it's handled correctly
    echo "${dst:-${src}}" >> "${pkg_xpkg_conffiles}" &&
    chown 0:0 "${pkg_xpkg_conffiles}" ||
    ptxd_install_error "install_config failed!"
}
export -f ptxd_install_config

ptxd_install_file() {
    local cmd="file"
    ptxd_install_file_impl "$@" ||
    ptxd_install_error "install_file failed!"
}
export -f ptxd_install_file

ptxd_install_link() {
    ptxd_install_ln "$@" ||
    ptxd_install_error "install_link failed!"
}
export -f ptxd_install_link

ptxd_install_node() {
    ptxd_install_mknod "$@" ||
    ptxd_install_error "install_node failed!"
}
export -f ptxd_install_node

ptxd_install_replace() {
    local dst="$1"
    local placeholder="$2"
    local value="$3"
    local mod_nfs mod_rw dst_lock

    ptxd_install_setup &&
    echo "\
install replace:
  file=${dst}
  '${placeholder}' -> '${value}'
" &&

    ptxd_exist "${dirs[@]/%/${dst}}" &&
    sed -i -e "s,${placeholder//,/\\,},${value//,/\\,},g" "${dirs[@]/%/${dst}}" ||

    ptxd_install_error "install_replace failed!"
    ptxd_install_unlock
}
export -f ptxd_install_replace

ptxd_install_script_replace() {
    local dst="$1"
    local placeholder="$2"
    local value="$3"

    ptxd_install_setup &&
    echo "\
install script replace:
  script=${dst}
  '${placeholder}' -> '${value}'
" &&

    ptxd_exist "${pkg_xpkg_control_dir}/${dst}" &&
    sed -i -e "s,${placeholder},${value},g" "${pkg_xpkg_control_dir}/${dst}" ||

    ptxd_install_error "install_script_replace failed!"
    ptxd_install_unlock
}
export -f ptxd_install_script_replace

ptxd_install_replace_figlet() {
    local dst="$1"
    local placeholder="$2"
    local value="$3"
    local escapemode="$4"
    local mod_nfs mod_rw dst_lock

    ptxd_install_setup &&
    echo "\
install replace figlet:
  file=${dst}
  '${placeholder}' -> '\$(figlet ${value})'
" &&

    ptxd_exist "${dirs[@]/%/${dst}}" &&
    ptxd_figlet_helper() {
	local value="$1"
	local escapemode="$2"
	figlet -d "${PTXDIST_SYSROOT_HOST}/share/figlet" -- "${value}" | \
	case "$escapemode" in
	    # /etc/issue needs each backslash quoted by another backslash. As
	    # the string is interpreted by the shell once more below, another
	    # level of quoting is needed such that every \ in the output of
	    # figlet needs to be replaced by \\\\. As a \ in sed needs to be
	    # quoted, too, this results in eight backslashes in the replacement
	    # string.
	    etcissue)	sed 's,\\,\\\\\\\\,g';;
	    *)		;;
	esac | \
	awk '{ if ($0 !~ "^ *$") printf("%s\\n", $0) }'  # newlines for sed
    } &&
    figlet="$(ptxd_figlet_helper "$value" "$escapemode")" &&
    sed -i -e "s#${placeholder}#${figlet}#g" "${dirs[@]/%/${dst}}" ||

    ptxd_install_error "install_replace failed!"
    ptxd_install_unlock
}
export -f ptxd_install_replace_figlet

ptxd_install_generic() {
    local file="$1"
    local dst="$2"
    local usr="$3"
    local grp="$4"
    local strip="$5"

    local -a stat
    local orig_IFS="${IFS}"
    local IFS=":"
    stat=( $(stat -c "%u:%g:0%a:0x%t:0x%T:%F" "${file}") ) &&
    IFS="${orig_IFS}"
    local usr="${usr:-${stat[0]}}" &&
    local grp="${grp:-${stat[1]}}" &&
    local mod="${stat[2]}" &&
    local major="${stat[3]}" &&
    local minor="${stat[4]}" &&
    local type="${stat[5]}" &&

    case "${type}" in
	"directory")
	    ptxd_install_dir "${dst}" "${usr}" "${grp}" "${mod}"
	    ;;
	"character special file")
	    ptxd_install_mknod "${dst}" "${usr}" "${grp}" "${mod}" c "${major}" "${minor}"
	    ;;
	"block special file")
	    ptxd_install_mknod "${dst}" "${usr}" "${grp}" "${mod}" b "${major}" "${minor}"
	    ;;
	"symbolic link")
	    local src="$(readlink "${file}")" &&
	    ptxd_install_ln "${src}" "${dst}" "${usr}" "${grp}"
	    ;;
	"regular file"|"regular empty file")
	    ptxd_install_file "${file}" "${dst}" "${usr}" "${grp}" "${mod}" "${strip}"
	    ;;
	*)
	    echo "Error: File type '${type}' unkown!"
	    return 1
	    ;;
    esac
}
export -f ptxd_install_generic

ptxd_install_find() {
    local src="${1%/}"
    local dst="${2%/}"
    dst=${dst:-/}
    local usr="${3#-}"
    local grp="${4#-}"
    local strip="${5}"
    local mod_nfs mod_rw dst_lock
    local gdb_src
    if [ -z "${glob}" ]; then
	local -a glob=( "-o" "-print" )
    fi

    ptxd_install_setup_src &&
    if [ -L "${src}" -a "$(readlink -f "${src}")" = /dev/null ]; then
	echo "'${src}' is a link to '/dev/null', skipping."
	echo
	return
    fi &&
    test -d "${src}"
    ptxd_install_unlock &&

    find "${src}" ! -path "${src}" -a \( \
		-path "*/.svn" -prune -o -path "*/.git" -prune -o \
		-path "*/.pc" -prune -o -path "*/CVS" -prune \
		"${glob[@]}" \) | while read file; do
	local dst_file="${dst}${file#${src}}"
	ptxd_install_generic "${file}" "${dst_file}" "${usr}" "${grp}" "${strip}" || return
    done
    check_pipe_status
}
export -f ptxd_install_find

ptxd_install_tree() {
    local cmd="file"
    local src="${1}"
    local dst="${2}"
    shift 2
    ptxd_install_find "${src}" "${dst}" "$@" ||
    ptxd_install_error "install_tree failed!"
}
export -f ptxd_install_tree

ptxd_install_glob() {
    local cmd="file"
    local src="${1}"
    local dst="${2}"
    set +B -f
    local yglob=( ${3} )
    local nglob=( ${4} )
    set -B +f
    local -a glob

    if [ -n "${3}" ]; then
	local pattern
	for pattern in "${yglob[@]}"; do
	    glob=( "${glob[@]}" "-o" "-path" "${pattern}" "-print" )
	done
    else
	glob=( "-o" "-print" )
    fi
    if [ -n "${4}" ]; then
	local pattern
	for pattern in "${nglob[@]}"; do
	    glob=( "-o" "-path" "${pattern}" "-prune" "${glob[@]}" )
	done
    fi
    shift 4

    ptxd_install_find "${src}" "${dst}" "$@" ||
    ptxd_install_error "install_glob failed!"
}
export -f ptxd_install_glob

ptxd_install_alternative_tree() {
    local cmd="alternative"
    local src="${1}"
    local dst="${2}"
    shift 2
    ptxd_install_find "${src}" "${dst:-${src}}" "$@" ||
    ptxd_install_error "install_alternative_tree failed!"
}
export -f ptxd_install_alternative_tree

ptxd_install_archive() {
    local archive="$1"
    shift

    if [ -L "${archive}" -a "$(readlink -f "${archive}")" = /dev/null ]; then
	echo "'${archive}' is a link to '/dev/null', skipping."
	echo
	return
    fi &&

    local dir="$(mktemp -d "${PTXDIST_TEMPDIR}/install_archive.XXXXXX")" &&

    ptxd_make_extract_archive "${archive}" "${dir}" &&
    ptxd_install_find "${dir}" "$@" &&

    rm -rf "${dir}" ||

    ptxd_install_error "install_archive failed!"
}
export -f ptxd_install_archive

#
# $1: path to spec file
#
# From linux/Documentation/filesystems/ramfs-rootfs-initramfs.txt:
#
#   file  <name> <location> <mode> <uid> <gid> [<hard links>]
#   dir   <name> <mode> <uid> <gid>
#   nod   <name> <mode> <uid> <gid> <dev_type> <maj> <min>
#   slink <name> <target> <mode> <uid> <gid>
#   pipe  <name> <mode> <uid> <gid>
#   sock  <name> <mode> <uid> <gid>
#
ptxd_install_spec() {
    local specfile="${1}"
    local type args
    local orig_IFS="${IFS}"

    ptxd_exist "${specfile}"

    while read type args; do
	set -- ${args}
	case "${type}" in
	    "file")
		local name="$1"
		local location="$2"
		local mode="$3"
		local uid="$4"
		local gid="$5"

		case "${location}" in
		    /*)
			ptxd_install_file "${location}" "${name}" "${uid}" "${gid}" "${mode}"
			;;
		    *)
			ptxd_install_alternative "/${location}" "${name}" "${uid}" "${gid}" "${mode}"
			;;
		esac
		;;

	    "dir")
		local name="$1"
		local mode="$2"
		local uid="$3"
		local gid="$4"

		ptxd_install_dir "${name}" "${uid}" "${gid}" "${mode}"
		;;

	    "nod")
		local name="$1"
		local mode="$2"
		local uid="$3"
		local gid="$4"
		local dev_type="$5"
		local maj="$6"
		local min="$7"

		ptxd_install_node "${name}" "${uid}" "${gid}" "${mode}" "${dev_type}" "${maj}" "${min}"
		;;

	    "slink")
		local name="$1"
		local target="$2"
		local mode="$3"
		local uid="$4"
		local gid="$5"

		ptxd_install_link "${name}" "${target}" "${uid}" "${gid}"
		;;

	    "pipe"|"sock")
		ptxd_install_error "${type} not supported: ${type} ${args}"
		;;

	    \#*|"")
		;;
	    *)
		ptxd_install_error "Unknown type ${type}"
		;;
	esac
    done < "${specfile}"
}
export -f ptxd_install_spec

ptxd_install_package() {
    local lib_dir=$(ptxd_get_lib_dir)

    for dir in "${pkg_pkg_dir}/"{,usr/}{bin,sbin,libexec}; do
	find "${dir}" \( -type f -o -type l \) \
		    -perm /111 2>/dev/null | while read file; do
	    ptxd_install_generic - "${file#${pkg_pkg_dir}}" ||
	    ptxd_install_error "install_package failed!"
	done
    done

    for dir in "${pkg_pkg_dir}/"{,usr/}${lib_dir}; do
	find "${dir}" \( -type f -o -type l \) \
		    -a -name "*.so*" 2>/dev/null | while read file; do
	    ptxd_install_generic - "${file#${pkg_pkg_dir}}" ||
	    ptxd_install_error "install_package failed!"
	done
    done
}
export -f ptxd_install_package

ptxd_install_shared() {
    local src="$1"
    local dst="$2"
    local usr="$3"
    local grp="$4"
    local mod="$5"
    local strip="${6:-y}"
    local filename="${src##*/}"

    ptxd_install_file "${src}" "${dst}/${filename}" "${usr}" "${grp}" "${mod}" "${strip}" &&

    find -H "${src%/*}" -maxdepth 1 -type l | while read file; do
	if [ "$(basename "$(readlink -f "${file}")")" = "${filename}" ]; then
	    local link="${dst}/${file##*/}"
	    ptxd_install_ln "${filename}" "${link}" "${usr}" "${grp}" || return
	fi
    done
}
export -f ptxd_install_shared

ptxd_install_lib() {
    local lib_dir=$(ptxd_get_lib_dir)
    local lib="$1"
    local root_dir="${2}"
    shift 2

    if [ -n "${root_dir}" ]; then
	root_dir="${root_dir%/}"

	if [[ "${root_dir}" != /* ]]; then
	    ptxd_install_error "ptxd_lib_install: root_dir '${root_dir}' must be absolute"
	fi
    fi

    local file="$(
	for dir in "${pkg_pkg_dir}"${root_dir}{/,/usr/}${lib_dir}; do
	    if [ -d "${dir}" ]; then
		find "${dir}" -type f -path "${dir}/${lib}.so*" ! -name "*.debug"
	    fi
	done | while read f; do
		grep -q '^INPUT(' "${f}" || echo "${f}"
	    done)"

    if [ ! -f "${file}" ]; then
	ptxd_install_error "ptxd_lib_install: cannot find library '${lib}'!"
    fi

    local dst="$(dirname "${file#${pkg_pkg_dir}}")" &&
    ptxd_install_shared "${file}" "${dst}" "${@}" ||
    ptxd_install_error "ptxd_install_lib failed!"
}
export -f ptxd_install_lib

ptxd_install_run() {
    local script="${pkg_xpkg_control_dir}/${1}"
    local dir
    local mod_nfs mod_rw dst_lock

    if [ -e "${script}" ]; then
	ptxd_install_setup &&
	echo "\
executing '${pkg_label}.${1}'
" &&
	for dir in "${ndirs[@]}"; do
	    DESTDIR="${dir}" /bin/sh "${script}"
	done
	ptxd_install_unlock
    fi ||
    ptxd_install_error "running '${1}' script failed!"
}
export -f ptxd_install_run

# In 2015, Pengutronix introduced the function ptxd_install_fixup_timestamps(), which sets the time stamp of
# all files within an ipk file to the first day of PTXdist's release month. The aim was to get more reproducible
# artifacts. As they admit, this is only a small step towards that target.
# Cf. https://www.mail-archive.com/ptxdist@pengutronix.de/msg10352.html
# As it is considered beneficial to be able to distinguish files that have been installed into the root image during
# PTXdist's build process from those that have been install via opkg in a later step, the time stamp is kept to
# build time by overwriting this shell function.
# Cf. also https://svreq01002.wago.local/polarion/#/project/kvp/workitem?id=SE-4999

ptxd_install_fixup_timestamps() {
  :
}
export -f ptxd_install_fixup_timestamps

ptxd_make_xpkg_pkg() {
    local pkg_xpkg_tmp="$1"
    local pkg_xpkg_dbg_tmp="$2"
    local pkg_xpkg_cmds="$3"
    local pkg_xpkg_perms="$4"
    local -a dirs ndirs pdirs sdirs ddirs

    ptxd_install_setup_global &&

    . "${pkg_xpkg_cmds}" &&

    ptxd_install_fixup_timestamps "${pkg_xpkg_tmp}"
}
export -f ptxd_make_xpkg_pkg
