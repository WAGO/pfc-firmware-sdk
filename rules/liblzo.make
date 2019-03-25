# -*-makefile-*-
#
# Copyright (C) 2007 by Carsten Schlote <c.schlote@konzeptpark.de>
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
PACKAGES-$(PTXCONF_LIBLZO) += liblzo

#
# Paths and names
#
LIBLZO_VERSION	:= 2.09
LIBLZO_MD5	:= c7ffc9a103afe2d1bba0b015e7aa887f
LIBLZO		:= lzo-$(LIBLZO_VERSION)
LIBLZO_SUFFIX	:= tar.gz
LIBLZO_URL	:= http://www.oberhumer.com/opensource/lzo/download/$(LIBLZO).$(LIBLZO_SUFFIX)
LIBLZO_SOURCE	:= $(SRCDIR)/$(LIBLZO).$(LIBLZO_SUFFIX)
LIBLZO_DIR	:= $(BUILDDIR)/$(LIBLZO)
LIBLZO_LICENSE	:= GPLv2

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBLZO_PATH	:= PATH=$(CROSS_PATH)
LIBLZO_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
LIBLZO_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--$(call ptx/endis, PTXCONF_LIBLZO_SHARED)-shared \
	--$(call ptx/endis, PTXCONF_LIBLZO_STATIC)-static

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/liblzo.targetinstall:
	@$(call targetinfo)

	@$(call install_init, liblzo)
	@$(call install_fixup, liblzo,PRIORITY,optional)
	@$(call install_fixup, liblzo,SECTION,base)
	@$(call install_fixup, liblzo,AUTHOR,"Carsten Schlote <c.schlote@konzeptpark.de>")
	@$(call install_fixup, liblzo,DESCRIPTION,missing)

ifdef PTXCONF_LIBLZO_SHARED
	@$(call install_lib, liblzo, 0, 0, 0644, liblzo2)
endif
	@$(call install_finish, liblzo)

	@$(call touch)

# vim: syntax=make
