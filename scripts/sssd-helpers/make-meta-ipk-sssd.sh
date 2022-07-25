#!/usr/bin/env bash

#
# Build meta-ipk for System Security Service Daemon (SSSD) for PFC firmware
#
# WAGO GmbH & Co. KG
#

set -o errexit
set -o errtrace
set -o nounset
set -o pipefail
#set -o xtrace

find_ipk ()
{
  find "${1}" -maxdepth 1 -iname "${2}"'_*.ipk' | grep '.*'
}

copy_deps ()
{
  local pkgs_dir; pkgs_dir="${1}"; shift
  local dst_dir; dst_dir="${1}"; shift
  local package;
  local ipk_path;

  for package in ${@}; do
    ipk_path="$(find_ipk "${pkgs_dir}" "${package}")"

    if [ -z "${ipk_path}" ]; then
      1>&2 echo 'Error: Could not find package "'"${package}"'"!'
      exit 1
    fi

    cp "${ipk_path}" "${dst_dir}"
  done
}

find_license ()
{
  find "${1}" -iname 'license.'"${2}"'_*.txt' | grep '.*'
}

copy_licenses ()
{
  local lic_dir; lic_dir="${1}"; shift
  local dst_dir; dst_dir="${1}"; shift
  local package;
  local lic_path;

  for package in ${@}; do
    lic_path="$(find_license "${lic_dir}" "${package}")"

    if [ -z "${lic_path}" ]; then
      1>&2 echo 'Error: Could not find license for package "'"${package}"'"!'
      exit 1
    fi

    cp "${lic_path}" "${dst_dir}"
  done
}

cleanup ()
{
  rm -rf "${SSSD_META_IPK_BUILD_DIR}"
}

make_ipk_skeleton ()
{
  local basedir; basedir="${1}"

  # common ipk folder structure
  mkdir "${basedir}/data"
  mkdir "${basedir}/control"

  echo '2.0' >"${basedir}/debian-binary"
}

build_ipk ()
{
  local basedir; basedir="${1}"
  local package_name; package_name="${2}"

  tar --numeric-owner --group=0 --owner=0 -C "${basedir}/control" -czf "${basedir}/control.tar.gz" .
  tar --numeric-owner --group=0 --owner=0 -C "${basedir}/data" -czf "${basedir}/data.tar.gz" .

  ar r "${basedir}/${package_name}" \
    "${basedir}/control.tar.gz" \
    "${basedir}/data.tar.gz" \
    "${basedir}/debian-binary" 2>/dev/null
}

# Arguments:
SSSD_REPO_VERSION=$1
FW_VERSION=$2
SSSD_BUILD_DIR=$3

# PTXdist defines:
PTXPROJ="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Local defines:
PTXDIST_PACKAGES_DIR="${PTXDIST_PLATFORMDIR}/packages/"
PTXDIST_LICENSES_DIR="${PTXDIST_WORKSPACE}/projectroot/usr/share/licenses/oss/"

SSSD_REPO_NAME='sssd_repo'
SSSD_REPO_FILE_NAME="${SSSD_REPO_NAME}_${SSSD_REPO_VERSION}_FW${FW_VERSION}.repo"

# dependencies which are needed and already in right order!
PACKAGES='attr acl ding-libs openldap libbsd talloc tevent tdb cmocka ldb keyutils krb5 p11-kit samba cyrus-sasl libunistring libpcre2 sssd'

# this is our working dir structure
SSSD_META_IPK_BUILD_DIR="${SSSD_BUILD_DIR}/sssd-meta-ipk"
SSSD_META_IPK_CONTROL_DIR="${SSSD_META_IPK_BUILD_DIR}/control"
SSSD_META_IPK_DATA_DIR="${SSSD_META_IPK_BUILD_DIR}/data"
SSSD_META_IPK_PACKAGES_DIR="${SSSD_META_IPK_DATA_DIR}/root/packages-sssd"
SSSD_META_IPK_LICENSES_DIR="${SSSD_META_IPK_DATA_DIR}/usr/share/licenses/oss"

# make build dir
mkdir -p "${SSSD_META_IPK_BUILD_DIR}"

make_ipk_skeleton "${SSSD_META_IPK_BUILD_DIR}"

# "data", e.g. contains *.ipks, will be copied to
# where we want while installing, this is packet
# specific
mkdir -m 700 -p "${SSSD_META_IPK_DATA_DIR}/root"
mkdir "${SSSD_META_IPK_PACKAGES_DIR}"
mkdir -p "${SSSD_META_IPK_LICENSES_DIR}"

# collect ipks and licenses
copy_deps "${PTXDIST_PACKAGES_DIR}" "${SSSD_META_IPK_PACKAGES_DIR}" ${PACKAGES}
copy_licenses "${PTXDIST_LICENSES_DIR}" "${SSSD_META_IPK_LICENSES_DIR}" ${PACKAGES}

cat <<-"EOF_IPKG_MAKE_INDEX_SH" >"${SSSD_META_IPK_PACKAGES_DIR}/ipkg-make-index.sh"
	#!/usr/bin/env bash
	
	set -e
	
	pkg_dir=$1
	
	if [ -z $pkg_dir ] || [ ! -d $pkg_dir ]; then
	  echo "Usage: ipkg-make-index <package_directory>" >&2
	  exit 1
	fi
	
	which md5sum >/dev/null 2>&1 || alias md5sum=md5
	empty=1
	
	for pkg in `find $pkg_dir -name '*.ipk' | sort`; do
	  empty=
	  name="${pkg##*/}"
	  name="${name%%_*}"
	  # [[ "$name" = "kernel" ]] && continue
	  # [[ "$name" = "libc" ]] && continue
	  echo "Generating index for package $pkg" >&2
	  file_size=$(stat -c "%s" $pkg)
	  md5sum=$(md5sum $pkg | awk '{print $1}')
	  sha256sumvar=$(sha256sum $pkg | awk '{print $1}')
	  # Take pains to make variable value sed-safe
	  sed_safe_pkg=`echo $pkg | sed -e 's/^\.\///g' -e 's/\\//\\\\\\//g'`
	  ar x $pkg ./control.tar.gz
	  tar xzf ./control.tar.gz ./control
	  cat ./control | sed -e "s/^Description:/Filename: $sed_safe_pkg\\
	Size: $file_size\\
	MD5Sum: $md5sum\\
	SHA256sum: $sha256sumvar\\
	Description:/"
	  rm -f ./control.tar.gz
	  rm -f ./control
	  echo ""
	done

	[ -n "$empty" ] && echo
	exit 0
EOF_IPKG_MAKE_INDEX_SH

chmod +x "${SSSD_META_IPK_PACKAGES_DIR}/ipkg-make-index.sh"

pushd "${SSSD_META_IPK_PACKAGES_DIR}" 1>/dev/null
./ipkg-make-index.sh ./ >Packages
gzip Packages
popd 1>/dev/null

rm ${SSSD_META_IPK_PACKAGES_DIR}/ipkg-make-index.sh

cat <<-EOF_CONTROL_FILE >"${SSSD_META_IPK_CONTROL_DIR}/control"
	Package: ${SSSD_REPO_NAME}
	Priority: optional
	Version:  ${SSSD_REPO_VERSION}
	Section:    base
	Architecture: armhf
	Maintainer: "WAGO GmbH & Co. KG"
	Depends:
	Source:
	Description:  Installation for sssd. This packet is a meta packet including all
              parts needed for System Security Services Daemon (SSSD).
              This packet is build for Firmware Version ${FW_VERSION}
              The following packets are included:

$(for package in ${PACKAGES}; do
  echo "                ${package}"
done)

	              The packet install a local repo only. Use opkg update and opkg install to install SSSD.
EOF_CONTROL_FILE

cat <<-EOF_POSTINST_FILE >"${SSSD_META_IPK_DATA_DIR}/root/install_sssd.sh"
	#!/usr/bin/env bash
	#
	# WAGO GmbH & Co. KG
	#
	
	set -o errexit
	set -o errtrace
	set -o nounset
	set -o pipefail
	#set -o xtrace
	
	PACKAGE_DEPENDENCIES='${PACKAGES}'
	
	is_installed ()
	{
	  opkg status "\${1}" | grep -q '^Status: .* installed'
	}
	
	install_sssd ()
	{
	  for package in \${PACKAGE_DEPENDENCIES}; do
	    is_installed "\${package}" || opkg install '/root/packages-sssd/'"\${package}"*.ipk
	  done
	}
	
	install_sssd
	opkg remove 'sssd_repo'
EOF_POSTINST_FILE

chmod +x ${SSSD_META_IPK_DATA_DIR}/root/install_sssd.sh

cat <<-EOF_POSTINST_FILE >"${SSSD_META_IPK_CONTROL_DIR}/postinst"
	#!/usr/bin/env bash
	#
	# WAGO GmbH & Co. KG
	#
	
	set -o errexit
	set -o errtrace
	set -o nounset
	set -o pipefail
	#set -o xtrace
	
	source /etc/profile 2>&1 1>/dev/null
	
	function start_installation
	{
	  local pid; pid="\$(pidof 'opkg')"
	  
	  while [ -d "/proc/\${pid}" ]; do
	    sleep 1
	  done
	  
	  /root/install_sssd.sh 2>&1 1>>/var/log/sssd_install.log
	}
	
	start_installation &
EOF_POSTINST_FILE

chmod +x "${SSSD_META_IPK_CONTROL_DIR}/postinst"

build_ipk "${SSSD_META_IPK_BUILD_DIR}" "${SSSD_REPO_FILE_NAME}"

cp "${SSSD_META_IPK_BUILD_DIR}/${SSSD_REPO_FILE_NAME}" "${PTXDIST_PACKAGES_DIR}"

# delete working directory
cleanup
