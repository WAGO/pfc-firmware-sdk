# -*-makefile-*-
#
# Copyright (C) 2003-2008 by Robert Schwebel <r.schwebel@pengutronix.de>
#                            Pengutronix <info@pengutronix.de>, Germany
#               2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_GETTEXT) += gettext

#
# Paths and names
#
GETTEXT_VERSION	:= 0.19.8.1
GETTEXT_MD5	:= df3f5690eaa30fd228537b00cb7b7590
GETTEXT		:= gettext-$(GETTEXT_VERSION)
GETTEXT_SUFFIX	:= tar.xz
GETTEXT_URL	:= $(call ptx/mirror, GNU, gettext/$(GETTEXT).$(GETTEXT_SUFFIX))
GETTEXT_SOURCE	:= $(SRCDIR)/$(GETTEXT).$(GETTEXT_SUFFIX)
GETTEXT_DIR	:= $(BUILDDIR)/$(GETTEXT)
GETTEXT_LICENSE	:= GPL-3.0-or-later AND LGPL-2.1-or-later
GETTEXT_LICENSE_FILES := \
	file://COPYING;md5=d32239bcb673463ab874e80d47fae504 \
	file://gettext-runtime/intl/COPYING.LIB;md5=a4b192f7208753fc0fc8d88c733e6106 \

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

GETTEXT_PATH	:= PATH=$(CROSS_PATH)
GETTEXT_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
GETTEXT_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--disable-java \
	--disable-native-java \
	--disable-csharp \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--enable-threads=posix \
	--enable-static \
	--disable-nls \
	--disable-rpath \
	--enable-c++ \
	--disable-relocatable \
	--enable-libasprintf \
	--disable-acl \
	--disable-openmp \
	--disable-curses \
	--without-emacs \
	--without-bzip2 \
	--without-xz

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/gettext.targetinstall:
	@$(call targetinfo)

	@$(call install_init, gettext)
	@$(call install_fixup, gettext,PRIORITY,optional)
	@$(call install_fixup, gettext,SECTION,base)
	@$(call install_fixup, gettext,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, gettext,DESCRIPTION,missing)

	@$(call install_copy, gettext, 0, 0, 0755, -, /usr/bin/xgettext)
	@$(call install_copy, gettext, 0, 0, 0755, -, /usr/bin/gettext)

	@$(call install_lib, gettext, 0, 0, 0644, libgettextlib-$(GETTEXT_VERSION))
	@$(call install_lib, gettext, 0, 0, 0644, libasprintf)
	@$(call install_lib, gettext, 0, 0, 0644, libgettextpo)
	@$(call install_lib, gettext, 0, 0, 0644,libgettextsrc-$(GETTEXT_VERSION))

	@$(call install_finish, gettext)

	@$(call touch)

# vim: syntax=make
