# -*-makefile-*-
#
# Copyright (C) 2005 by Robert Schwebel
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
PACKAGES-$(PTXCONF_XMLSTARLET) += xmlstarlet

#
# Paths and names
#
XMLSTARLET_VERSION	:= 1.6.1
XMLSTARLET		:= xmlstarlet-$(XMLSTARLET_VERSION)
XMLSTARLET_SUFFIX	:= tar.gz
XMLSTARLET_MD5      := f3c5dfa3b1a2ee06cd57c255cc8b70a0
XMLSTARLET_URL		:= $(PTXCONF_SETUP_SFMIRROR)/xmlstar/$(XMLSTARLET).$(XMLSTARLET_SUFFIX)
XMLSTARLET_SOURCE	:= $(SRCDIR)/$(XMLSTARLET).$(XMLSTARLET_SUFFIX)
XMLSTARLET_DIR		:= $(BUILDDIR)/$(XMLSTARLET)
XMLSTARLET_LICENSE	:= Custom License

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(XMLSTARLET_SOURCE):
	@$(call targetinfo)
	@$(call get, XMLSTARLET)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

XMLSTARLET_PATH := PATH=$(CROSS_PATH)
XMLSTARLET_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
# Don't set --with-libiconv-prefix (not necessary for libc-iconv)
#
XMLSTARLET_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--with-libxml-prefix=$(SYSROOT)/usr \
	--with-libxslt-prefix=$(SYSROOT)/usr \
	--with-libiconv-prefix=$(SYSROOT)/usr

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/xmlstarlet.targetinstall:
	@$(call targetinfo)

	@$(call install_init, xmlstarlet)
	@$(call install_fixup, xmlstarlet,PRIORITY,optional)
	@$(call install_fixup, xmlstarlet,SECTION,base)
	@$(call install_fixup, xmlstarlet,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, xmlstarlet,DESCRIPTION,missing)

	@$(call install_copy, xmlstarlet, 0, 0, 0755, \
		$(XMLSTARLET_PKGDIR)/usr/bin/xml, /usr/bin/xmlstarlet)

	@$(call install_finish, xmlstarlet)

	@$(call touch)

# vim: syntax=make
