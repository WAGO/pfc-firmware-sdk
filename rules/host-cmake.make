# -*-makefile-*-
#
# Copyright (C) 2006 by Erwin Rol
#               2009, 2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_CMAKE) += host-cmake

#
# Paths and names
#
HOST_CMAKE_VERSION	:= 3.13.5
HOST_CMAKE_MD5		:= 3e8946d34d5519138a70ff562144e076
HOST_CMAKE		:= cmake-$(HOST_CMAKE_VERSION)
HOST_CMAKE_SUFFIX	:= tar.gz
HOST_CMAKE_URL		:= https://cmake.org/files/v$(basename $(HOST_CMAKE_VERSION))/$(HOST_CMAKE).$(HOST_CMAKE_SUFFIX)
HOST_CMAKE_SOURCE	:= $(SRCDIR)/$(HOST_CMAKE).$(HOST_CMAKE_SUFFIX)
HOST_CMAKE_DIR		:= $(HOST_BUILDDIR)/$(HOST_CMAKE)
HOST_CMAKE_LICENSE	:= BSD-3-Clause AND Apache-2.0 AND bzip2-1.0.5 AND (MIT OR public_domain) AND MIT
HOST_CMAKE_LICENSE_FILES := \
	file://Copyright.txt;md5=f61f5f859bc5ddba2b050eb10335e013 \
	file://Utilities/GitSetup/LICENSE;md5=3b83ef96387f14655fc854ddc3c6bd57 \
	file://Utilities/cmbzip2/LICENSE;md5=7023994919680c533b77301b306ea1c9 \
	file://Utilities/cmjsoncpp/LICENSE;md5=fa2a23dd1dc6c139f35105379d76df2b \
	file://Utilities/cmlibuv/LICENSE;md5=a68902a430e32200263d182d44924d47

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_CMAKE_CONF_ENV	:= \
	$(HOST_ENV) \
	MAKEFLAGS="$(PARALLELMFLAGS)"

HOST_CMAKE_BUILD_OOT	:= YES
HOST_CMAKE_CONF_TOOL	:= autoconf
HOST_CMAKE_CONF_OPT	:= \
	$(HOST_AUTOCONF) \
	-- \
	-DBUILD_TESTING=NO \
	-DCMAKE_USE_OPENSSL=YES


$(STATEDIR)/host-cmake.install.post: \
	$(PTXDIST_CMAKE_TOOLCHAIN_TARGET) \
	$(PTXDIST_CMAKE_TOOLCHAIN_HOST)

# vim: syntax=make
