# -*-makefile-*-
#
# Copyright (C) 2009 by Wolfram Sang
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PV) += pv

#
# Paths and names
#
PV_VERSION	:= 1.6.6
PV_MD5		:= ff3564fddcc2b9bd4a9c1d143aba4b4c
PV		:= pv-$(PV_VERSION)
PV_SUFFIX	:= tar.bz2
PV_URL		:= http://www.ivarch.com/programs/sources/$(PV).$(PV_SUFFIX)
PV_SOURCE	:= $(SRCDIR)/$(PV).$(PV_SUFFIX)
PV_DIR		:= $(BUILDDIR)/$(PV)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

PV_PATH	:= PATH=$(CROSS_PATH)
PV_ENV 	:= $(CROSS_ENV)
PV_MAKEVARS := $(CROSS_ENV_LD)

#
# autoconf
#
PV_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--disable-nls \
	--disable-splice \
	--disable-ipc \
	--$(call ptx/endis, PTXCONF_GLOBAL_LARGE_FILE)-lfs \
	--enable-debugging

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pv.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pv)
	@$(call install_fixup, pv,PRIORITY,optional)
	@$(call install_fixup, pv,SECTION,base)
	@$(call install_fixup, pv,AUTHOR,"Wolfram Sang <w.sang@pengutronix.de>")
	@$(call install_fixup, pv,DESCRIPTION,missing)

	@$(call install_copy, pv, 0, 0, 0755, -, /usr/bin/pv)

	@$(call install_finish, pv)

	@$(call touch)

# vim: syntax=make
