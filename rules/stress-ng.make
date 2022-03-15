# -*-makefile-*-
#
# Copyright (C) 2018 by Jan Luebbe <jlu@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
ifndef PTXCONF_STRESS_NG_SKIP_TARGETINSTALL
PACKAGES-$(PTXCONF_STRESS_NG) += stress-ng
else
LAZY_PACKAGES-$(PTXCONF_STRESS_NG) += stress-ng
endif

#
# Paths and names
#
STRESS_NG_VERSION	:= 0.11.15
STRESS_NG_MD5		:= 67c77e0be99ca80ef8d40d1942e42453
STRESS_NG		:= stress-ng-$(STRESS_NG_VERSION)
STRESS_NG_SUFFIX	:= tar.gz
STRESS_NG_URL		:= https://github.com/ColinIanKing/stress-ng/tarball/V$(STRESS_NG_VERSION)
STRESS_NG_SOURCE	:= $(SRCDIR)/$(STRESS_NG).$(STRESS_NG_SUFFIX)
STRESS_NG_DIR		:= $(BUILDDIR)/$(STRESS_NG)
STRESS_NG_LICENSE	:= GPL-2.0-or-later

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

STRESS_NG_CONF_TOOL	:= NO

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

STRESS_NG_MAKE_ENV	:= $(CROSS_ENV)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/stress-ng.targetinstall:
	@$(call targetinfo)

	@$(call install_init, stress-ng)
	@$(call install_fixup, stress-ng,PRIORITY,optional)
	@$(call install_fixup, stress-ng,SECTION,base)
	@$(call install_fixup, stress-ng,AUTHOR,"Jan Luebbe <jlu@pengutronix.de>")
	@$(call install_fixup, stress-ng,DESCRIPTION,missing)

	@$(call install_copy_toolchain_lib, stress-ng, libatomic.so)
	@$(call install_copy, stress-ng, 0, 0, 0755, -, /usr/bin/stress-ng)

	@$(call install_finish, stress-ng)

	@$(call touch)

# vim: syntax=make
