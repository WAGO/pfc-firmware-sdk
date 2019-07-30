# -*-makefile-*-
#
# Copyright (C) 2010 by Alexander Stein <alexander.stein@systec-electronic.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBASSUAN) += libassuan

#
# Paths and names
#
LIBASSUAN_VERSION	:= 2.5.3
LIBASSUAN_MD5		:= 226c504ea78a232224bf3b6846b3adb9
LIBASSUAN		:= libassuan-$(LIBASSUAN_VERSION)
LIBASSUAN_SUFFIX	:= tar.bz2
LIBASSUAN_URL		:= https://www.gnupg.org/ftp/gcrypt/libassuan/$(LIBASSUAN).$(LIBASSUAN_SUFFIX)
LIBASSUAN_SOURCE	:= $(SRCDIR)/$(LIBASSUAN).$(LIBASSUAN_SUFFIX)
LIBASSUAN_DIR		:= $(BUILDDIR)/$(LIBASSUAN)
LIBASSUAN_LICENSE	:= LGPL-2.1-or-later
LIBASSUAN_LICENSE_FILES	:= \
	file://COPYING.LIB;md5=2d5025d4aa3495befef8f17206a5b0a1

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
LIBASSUAN_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--enable-build-timestamp="$(PTXDIST_BUILD_TIMESTAMP)" \
	--disable-doc

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libassuan.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libassuan)
	@$(call install_fixup, libassuan,PRIORITY,optional)
	@$(call install_fixup, libassuan,SECTION,base)
	@$(call install_fixup, libassuan,AUTHOR,"Alexander Stein")
	@$(call install_fixup, libassuan,DESCRIPTION,missing)

	@$(call install_lib, libassuan, 0, 0, 0644, libassuan)

	@$(call install_finish, libassuan)

	@$(call touch)

# vim: syntax=make
