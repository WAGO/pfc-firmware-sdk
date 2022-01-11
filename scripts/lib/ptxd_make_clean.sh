#!/bin/bash
#
# Copyright (C) 2019 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

ptxd_make_clean() {
    local directory="${1}"

    if [ -e "${directory}" ]; then
	if [ -h "${directory}" -o ! -d "${directory}" ]; then
	    rm -f "${directory}"
	elif [ -n "${PTXDIST_FORCE}" ]; then
	    : # always delete with --force
	elif [[ -e "${ptx_make_target}" && "${ptx_make_target}" =~ .extract$ ]]; then
#	    if [ -z "${PTXCONF_SETUP_AUTO_EXTRACT}" ]; then
#		ptxd_bailout "Refusing to delete existing source" \
#		    "$(ptxd_print_path "${directory}")" \
#		    "Delete anyways with --force or  skip extract with --dirty."
#	    elif [ -z "${PTXDIST_FORCE}" -a -d "${directory}/.git" ]; then
            if [ -z "${PTXDIST_FORCE}" -a -d "${directory}/.git" ]; then
		ptxd_bailout "Refusing to delete existing source with git" \
		    "$(ptxd_print_path "${directory}")" \
		    "Delete anyways with --force or skip extract with --dirty."
	    fi
	elif [ -d "${directory}/.git" ]; then
	    ptxd_bailout "Refusing to delete git repository" \
		"$(ptxd_print_path "${directory}")" \
		"Delete anyways with --force."
	fi
	rm -rf "${directory}"
    fi
}
export -f ptxd_make_clean
