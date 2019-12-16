#!/bin/bash
#
# Copyright (C) 2008, 2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

export PTX_KGEN_DIR="${PTXDIST_TEMPDIR}/kgen"

ptxd_kgen_awk()
{
    kgen_part="${kgen_part}" \
    gawk '
	BEGIN {
	    FS = ":##[[:space:]]*SECTION=|[[:space:]]*$";
	}

	{
	    file = $1;
	    section = $2;
	    pkg = gensub(/.*\//, "", "g", file);

	    if (!((section, pkg) in pkgs))
		pkgs[section, pkg] = file;
	}

	END {
	    n = asorti(pkgs, sorted);

	    for (i = 1; i <= n; i++) {
		split(sorted[i], sep, SUBSEP)

		file = pkgs[sorted[i]];
		section = sep[1];
		pkg = sep[2];

		output = "'"${PTX_KGEN_DIR}/generated/"'" section ".in";

#		print output ":", "source \"" file "\""
		print "source \"" file "\"" > output
	    }
	}
	'
}
export -f ptxd_kgen_awk


ptxd_kgen_generate_sections()
{
    local dir
    {
	for dir in "${kgen_dirs[@]}"; do
	    if [ \! -d "${dir}" ]; then
		continue
	    fi
	    # '! -name ".#*"' filters out emacs's lock files
	    find "${dir}/" -name "*.in" \! -name ".#*" -print0
	done
    } | {
	#
	# if there isn't any "SECTION" in any .in-file, grep has a
	# negative return value. but this is no error, so use
	# "|| true" here
	#
	xargs -r -0 -- \
	    grep -R -H -e "^##[[:space:]]*SECTION=" || true
    } | ptxd_kgen_awk
    check_pipe_status
}
export -f ptxd_kgen_generate_sections


ptxd_kgen()
{
    local kgen_part="${1}"
    local kgen_dirs

    if [ ${#} -ne 1 ]; then
	cat <<EOF

${PROMPT}error: '${FUNCNAME}' needs one parameter

EOF
	exit 1
    fi

    if [ -d "${PTX_KGEN_DIR}/generated" ]; then
	return
    fi

    # transmogrify part into subdir
    local oldIFS="$IFS"
    case "${kgen_part}" in
	ptx|platform) IFS=: kgen_dirs=( ${PTXDIST_PATH_RULES} ${PTXDIST_PATH_PLATFORMS} ) ;;
	board|user|collection) return 0 ;;
	*) cat <<EOF

${PROMPT}error: '${FUNCNAME}' doesn't support '${part}', yet

EOF
	    exit 1
    esac
    IFS="$oldIFS"

    mkdir -p "${PTX_KGEN_DIR}/generated" &&

    ptxd_kgen_generate_sections
}
export -f ptxd_kgen
