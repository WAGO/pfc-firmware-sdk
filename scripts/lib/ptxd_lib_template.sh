ptxd_template_new_ct_build() {
    ptxd_template_read_name &&
    ptxd_template_read "enter version number" VERSION "0.1.0" &&
    export AUTHOR="WAGO GmbH \\\\\& Co. KG" &&
    ptxd_template_read_section "wago_specific" &&
    ptxd_template_read "enter base folder" BASE_FOLDER "wago_intern" &&
    if [[ ${BASE_FOLDER} != wago_intern* ]]; then export IS_WAGO_INTERN="#remove#"; else export IS_WAGO_INTERN=""; fi &&
    if [[ ${BASE_FOLDER} != local_src* ]]; then export IS_LOCAL_SRC="#remove#"; else export IS_LOCAL_SRC=""; fi &&
    ptxd_template_filter_ctbuild "${PACKAGE}" && 
    ptxd_template_write_rules &&
    crop_rules &&
    ptxd_template_write_src_ctbuild "${BASE_FOLDER}"
}
export -f ptxd_template_new_ct_build
ptxd_template_help_list[${#ptxd_template_help_list[@]}]="ct-build"
ptxd_template_help_list[${#ptxd_template_help_list[@]}]="create package based on ct-build"


ptxd_template_filter_ctbuild() {
    local PACKAGE="${1}"
    
    if grep --word-regexp "PTXCONF_${PACKAGE}" selected_ptxconfig >/dev/null; then
        echo
        local overwrite
        read -e -p "${PTXDIST_LOG_PROMPT}warning: A package ${PACKAGE} is already defined in selected_ptxconfig, continue anyway? [y/N] " overwrite
        case "${overwrite}" in
            y|Y) ;;
            *)
                echo "${PTXDIST_LOG_PROMPT}aborted."
                echo
                exit
            ;;
        esac
    fi
}

crop_rules() {
    local template_suffix
    echo
    for template_suffix in "make" "in"; do
        local filename="${PTXDIST_WORKSPACE}/rules/${class}${package_filename}.${template_suffix}"
        sed -i -e '/^#remove#.*/d' -e 's/#remove#.*//' "${filename}"
    done
}

ptxd_template_write_src_ctbuild() {
    local BASE_FOLDER="$1"
    local dst="${PTXDIST_WORKSPACE}/${BASE_FOLDER}/${package}"
    local template_src
    local template_dir

    if [ -d "${dst}" ]; then
      return
    fi

    echo
    local r
    read -e -p "${dst#${PTXDIST_WORKSPACE}/} does not exist, create? [Y/n] " r
    case "${r}" in
      y|Y|"") ;;
      *) return ;;
    esac

    template_src="$(ptxd_template_file "${action}")" &&
    mkdir -p "${dst}" &&
    tar -C "${template_src}" -cf - --exclude .svn . | \
    tar -C "${dst}" -xf - &&

    if [ ! -e "${dst}/wizard.sh" ]; then
      return
    fi &&
    template_dir=$(dirname "${template_src}") &&
    ( cd "${dst}" && bash wizard.sh "${package}" "${template_dir}") &&
    rm -f "${dst}/wizard.sh"
}
