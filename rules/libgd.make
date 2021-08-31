# -*-makefile-*-
#
# Copyright (C) 2003 by Benedikt Spranger <b.spranger@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBGD) += libgd

#
# Paths and names
#
LIBGD_VERSION	:= 2.2.5
LIBGD_MD5	:= ab2bd17470b51387eadfd5289b5c0dfb
LIBGD		:= libgd-$(LIBGD_VERSION)
#LIBGD_VERSION	:= 2.0.36RC1
#LIBGD_MD5	:= 39ac48e6d5e0012a3bd2248a0102f209
#LIBGD		:= gd-$(LIBGD_VERSION)
LIBGD_SUFFIX	:= tar.gz
LIBGD_URL	:= http://www.pengutronix.de/software/ptxdist/temporary-src/$(LIBGD).$(LIBGD_SUFFIX)
LIBGD_SOURCE	:= $(SRCDIR)/$(LIBGD).$(LIBGD_SUFFIX)
LIBGD_DIR	:= $(BUILDDIR)/$(LIBGD)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBGD_PATH	:= PATH=$(CROSS_PATH)
LIBGD_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
LIBGD_AUTOCONF  := $(CROSS_AUTOCONF_USR)

ifdef PTXCONF_LIBGD_X
LIBGD_AUTOCONF += --with-x
else
LIBGD_AUTOCONF += --without-x
endif

ifdef PTXCONF_LIBGD_JPEG
LIBGD_AUTOCONF += --with-jpeg=$(SYSROOT)/usr
else
LIBGD_AUTOCONF += --without-jpeg
endif

ifdef PTXCONF_LIBGD_PNG
LIBGD_AUTOCONF += --with-png=$(SYSROOT)/usr
else
LIBGD_AUTOCONF += --without-png
endif

ifdef PTXCONF_LIBGD_XPM
LIBGD_AUTOCONF += --with-xpm=$(SYSROOT)/usr --with-x
else
LIBGD_AUTOCONF += --without-xpm --without-x
endif

ifdef PTXCONF_LIBGD_FREETYPE
LIBGD_AUTOCONF += --with-freetype=$(SYSROOT)/usr
else
LIBGD_AUTOCONF += --without-freetype
endif

ifdef PTXCONF_LIBGD_FONTCONFIG
LIBGD_AUTOCONF += --with-fontconfig=$(SYSROOT)/usr
else
LIBGD_AUTOCONF += --without-fontconfig
endif

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libgd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libgd)
	@$(call install_fixup, libgd,PRIORITY,optional)
	@$(call install_fixup, libgd,SECTION,base)
	@$(call install_fixup, libgd,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libgd,DESCRIPTION,missing)

	@$(call install_lib, libgd, 0, 0, 0644, libgd)

	@$(call install_finish, libgd)

	@$(call touch)

# vim: syntax=make
