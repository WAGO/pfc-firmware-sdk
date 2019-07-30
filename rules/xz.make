# -*-makefile-*-
#
# Copyright (C) 2012 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_XZ) += xz

#
# Paths and names
#
XZ_VERSION	:= 5.2.4
XZ_MD5		:= b3264b15ab1db04c8c428dc81838d4eb
XZ		:= xz-$(XZ_VERSION)
XZ_SUFFIX	:= tar.bz2
XZ_URL		:= http://tukaani.org/xz/$(XZ).$(XZ_SUFFIX)
XZ_SOURCE	:= $(SRCDIR)/$(XZ).$(XZ_SUFFIX)
XZ_DIR		:= $(BUILDDIR)/$(XZ)
XZ_LICENSE	:= public_domain AND LGPL-2.1-or-later AND GPL-2.0-or-later AND GPL-3.0-or-later
XZ_LICENSE_FILES := \
	file://COPYING;md5=97d554a32881fee0aa283d96e47cb24a \
	file://COPYING.GPLv2;md5=b234ee4d69f5fce4486a80fdaf4a4263 \
	file://COPYING.GPLv3;md5=d32239bcb673463ab874e80d47fae504 \
	file://COPYING.LGPLv2.1;md5=4fbd65380cdd255951079008b364516c

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
XZ_CONF_TOOL	:= autoconf
XZ_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-debug \
	--disable-external-sha256 \
	--enable-assembler \
	--disable-small \
	--enable-threads \
	--$(call ptx/endis,PTXCONF_XZ_TOOLS)-xz \
	--$(call ptx/endis,PTXCONF_XZ_TOOLS)-xzdec \
	--disable-lzmadec \
	--disable-lzmainfo \
	--disable-lzma-links \
	--$(call ptx/endis,PTXCONF_XZ_TOOLS)-scripts \
	--enable-shared \
	--disable-doc \
	--enable-symbol-versions \
	--disable-sandbox \
	--disable-static \
	--disable-nls \
	--disable-rpath \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--enable-unaligned-access=auto \
	--disable-werror

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/xz.targetinstall:
	@$(call targetinfo)

	@$(call install_init, xz)
	@$(call install_fixup, xz,PRIORITY,optional)
	@$(call install_fixup, xz,SECTION,base)
	@$(call install_fixup, xz,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, xz,DESCRIPTION,missing)

	@$(call install_lib, xz, 0, 0, 0644, liblzma)
ifdef PTXCONF_XZ_TOOLS
	@$(call install_copy, xz, 0, 0, 0755, -, /usr/bin/xz)
	@$(call install_link, xz, xz, /usr/bin/unxz)
	@$(call install_link, xz, xz, /usr/bin/xzcat)

	@$(call install_copy, xz, 0, 0, 0755, -, /usr/bin/xzdec)

	@$(call install_copy, xz, 0, 0, 0755, -, /usr/bin/xzdiff)
	@$(call install_link, xz, xzdiff, /usr/bin/xzcmp)

	@$(call install_copy, xz, 0, 0, 0755, -, /usr/bin/xzgrep)
	@$(call install_link, xz, xzgrep, /usr/bin/xzegrep)
	@$(call install_link, xz, xzgrep, /usr/bin/xzfgrep)

	@$(call install_copy, xz, 0, 0, 0755, -, /usr/bin/xzless)
	@$(call install_copy, xz, 0, 0, 0755, -, /usr/bin/xzmore)
endif

	@$(call install_finish, xz)

	@$(call touch)

# vim: syntax=make
