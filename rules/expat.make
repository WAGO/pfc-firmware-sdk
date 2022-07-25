# -*-makefile-*-
#
# Copyright (C) 2003 by Robert Schwebel <r.schwebel@pengutronix.de>
#                       Pengutronix <info@pengutronix.de>, Germany
#               2007, 2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_EXPAT) += expat

#
# Paths and names
#
EXPAT_VERSION	:= 2.4.7
EXPAT_MD5	:= cd3d5825dbd57519232cfcadd14a5725
EXPAT		:= expat-$(EXPAT_VERSION)
EXPAT_SUFFIX	:= tar.bz2
EXPAT_URL	:= $(call ptx/mirror, SF, expat/$(EXPAT).$(EXPAT_SUFFIX))
EXPAT_SOURCE	:= $(SRCDIR)/$(EXPAT).$(EXPAT_SUFFIX)
EXPAT_DIR	:= $(BUILDDIR)/$(EXPAT)
EXPAT_LICENSE	:= MIT

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
EXPAT_CONF_TOOL	:= autoconf
EXPAT_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-static \
	--disable-xml-attr-info \
	--enable-xml-context \
	--without-xmlwf \
	--without-examples \
	--without-tests \
	--without-libbsd \
	--without-docbook

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/expat.targetinstall:
	@$(call targetinfo)

	@$(call install_init, expat)
	@$(call install_fixup, expat,PRIORITY,optional)
	@$(call install_fixup, expat,SECTION,base)
	@$(call install_fixup, expat,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, expat,DESCRIPTION,missing)

	@$(call install_lib, expat, 0, 0, 0644, libexpat)

	@$(call install_finish, expat)

	@$(call touch)

# vim: syntax=make