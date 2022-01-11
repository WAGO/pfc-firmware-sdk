# In 2017, Pengutronix added a check to sort out certain packages when running the patch-in step.
# Packages with a PKG_URL starting with "file://" or "lndir://" are automatically excluded from patching,
# which is only mentioned to the user with a simple echo print.
# However, since many of our own packages use the "file://" URL and some need to be patched, this new part
# of ptxd_make_world_patchin_apply() is skipped.
#
# Details: https://git.pengutronix.de/cgit/ptxdist/commit/scripts/lib/ptxd_make_world_patchin.sh?id=8b488811b76976c12f6372e19a820fb7bc66841f

ptxd_make_world_patchin_apply()
{
    local \
        tmp_patch_series \
        pkg_patch_series \
        pkg_patch_tool

#    if [[ "${pkg_url}" =~ ^file:// || "${pkg_url}" =~ ^lndir:// ]]; then
#        local url="$(ptxd_file_url_path "${pkg_url}")"
#        # local directories are not intended to be patched
#        if [ -d "${url}" ]; then
#           echo "Local source directory detected, skipping patch-in step"
#           return
#        fi
#    fi

    ptxd_make_world_patchin_apply_init || return
    if [ -z "${pkg_patch_dir}" ]; then
        return
    fi &&

    if [ "${pkg_patch_tool}" = "git" ]; then
        ptxd_make_world_patchin_apply_git_init || return
    fi &&

    #
    # the primary reference is the ".ptxdist" folder in the pkg_patchin_dir:
    # these files might be existent:
    #
    # patches   link, pointing to the dir containing the pkg's patches
    # series    usually link, pointing to the used series file
    #           if no series file is supplied and the folder
    #           containing the patches is not writable, this will be a
    #           file
    #
    if [ -e "${pkg_patchin_dir}/.ptxdist" ]; then
        ptxd_bailout "pkg_patchin_dir '${pkg_patchin_dir}' already contains a '.ptxdist' folder"
    fi &&
    mkdir "${pkg_patchin_dir}/.ptxdist" &&

    #
    # create a ".ptxdist/patches" link pointing to the directory
    # containing the patches
    #
    ln -s "${pkg_patch_dir}" "${pkg_patchin_dir}/.ptxdist/patches" &&

    tmp_patch_series="${pkg_patchin_dir}/.ptxdist/series" &&

    # link series file - if not available create it
    if [ ! -e "${pkg_patch_series}" ]; then
        #
        # look for patches (and archives) and put into series file
        # (the "sed" removes "./" from find's output)
        #
        pushd "${pkg_patch_dir}/" >/dev/null &&
        find \
            -name "*.diff" -o \
            -name "*.patch" -o \
            -name "*.xz" -o \
            -name "*.bz2" -o \
            -name "*.gz" | \
            sed -e "s:^[.]/::" | sort > \
            "${tmp_patch_series}" &&
        popd > /dev/null

        # no patches found
        if [ \! -s "${tmp_patch_series}" ]; then
            rm -f "${tmp_patch_series}" &&
            unset pkg_patch_series
        else
            ptxd_pedantic "series file for '$(ptxd_print_path "${pkg_patch_dir}")' is missing" &&

            # if writable, create series file next to the patches
            if [ -w "${pkg_patch_dir}/" ]; then
                mv "${tmp_patch_series}" "${pkg_patch_series}" &&
                ln -s "${pkg_patch_series}" "${tmp_patch_series}"
            fi
        fi
    else
        ln -s "${pkg_patch_series}" "${tmp_patch_series}"

        #
        # check for non existing patches
        #
        # Some tools like "git" skip non existing patches without an
        # error. In ptxdist we consider this a fatal error.
        #
        local patch para junk
        while read patch tmp; do
            case "${patch}" in
                ""|"#"*) continue ;;    # skip empty lines and comments
                *) ;;
            esac

            case "${para}" in
                ""|"#"*) ;;             # no para or comment
                -p*) ;;
                *) ptxd_bailout "invalid parameter to patch '${patch}' in series file '${pkg_patch_series}'"
            esac

            if [ \! -f "${pkg_patchin_dir}/.ptxdist/patches/${patch}" ]; then
                ptxd_bailout "cannot find patch: '${patch}' specified in series file '${pkg_patch_series}'"
            fi
            
        done < "${pkg_patchin_dir}/.ptxdist/series" &&
        unset patch para junk
    fi || return

    #
    # setup convenience links
    #
    # series
    if [ -e "${pkg_patchin_dir}/.ptxdist/series" ]; then
        if [ -e "${pkg_patchin_dir}/series" ]; then
            ptxd_bailout "there's a 'series' file in the pkg_patchin_dir"
        fi
        ln -sf ".ptxdist/series" "${pkg_patchin_dir}/series"
    fi &&

    # patches
    if [ \! -e "${pkg_patchin_dir}/patches" ]; then
        ln -sf ".ptxdist/patches" "${pkg_patchin_dir}/patches"
    fi || return

    echo
    echo "pkg_patch_dir:     '$(ptxd_print_path "${pkg_patch_dir:-<none>}")'"
    echo "pkg_patch_series:  '$(ptxd_print_path "${pkg_patch_series:-<none>}")'"
    echo

    # apply patches if series file is available
    if [ -n "${pkg_patch_series}" ]; then
        echo    "patchin: ${pkg_patch_tool}: apply '$(ptxd_print_path ${pkg_patch_series})'"
        "ptxd_make_world_patchin_apply_${pkg_patch_tool}" || return
        echo -e "patchin: ${pkg_patch_tool}: done\n"
    fi
}
