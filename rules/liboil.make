# -*-makefile-*-
#
# Copyright (C) 2006 by Robert Schwebel <r.schwebel@pengutronix.de>
#                       Pengutronix <info@pengutronix.de>, Germany
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBOIL) += liboil

#
# Paths and names
#
LIBOIL_VERSION	:= 0.3.17
LIBOIL_MD5	:= 47dc734f82faeb2964d97771cfd2e701
LIBOIL		:= liboil-$(LIBOIL_VERSION)
LIBOIL_SUFFIX	:= tar.gz
LIBOIL_URL	:= http://liboil.freedesktop.org/download/$(LIBOIL).$(LIBOIL_SUFFIX)
LIBOIL_SOURCE	:= $(SRCDIR)/$(LIBOIL).$(LIBOIL_SUFFIX)
LIBOIL_DIR	:= $(BUILDDIR)/$(LIBOIL)
LIBOIL_LICENSE	:= BSD-2-Clause

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBOIL_CONF_ENV	:= $(CROSS_ENV)
ifdef PTXCONF_ARCH_ARM64
LIBOIL_CONF_ENV += as_cv_unaligned_access=no
endif

#
# autoconf
#
LIBOIL_CONF_TOOL := autoconf

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/liboil.targetinstall:
	@$(call targetinfo)

	@$(call install_init, liboil)
	@$(call install_fixup, liboil,PRIORITY,optional)
	@$(call install_fixup, liboil,SECTION,base)
	@$(call install_fixup, liboil,AUTHOR,"Guillaume GOURAT <guillaume.gourat@nexvision.fr>")
	@$(call install_fixup, liboil,DESCRIPTION,missing)

	@$(call install_lib, liboil, 0, 0, 0644, liboil-0.3)

	@$(call install_finish, liboil)

	@$(call touch)

# vim: syntax=make
