# -*-makefile-*-
#
# Copyright (C) 2007 by Robert Schwebel
#               2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_HTOP) += htop

#
# Paths and names
#
HTOP_VERSION	:= 2.2.0
HTOP_MD5	:= 0d816b6beed31edc75babcfbf863ffa8
HTOP		:= htop-$(HTOP_VERSION)
HTOP_SUFFIX	:= tar.gz
HTOP_URL	:= http://hisham.hm/htop/releases/$(HTOP_VERSION)/$(HTOP).$(HTOP_SUFFIX)
HTOP_SOURCE	:= $(SRCDIR)/$(HTOP).$(HTOP_SUFFIX)
HTOP_DIR	:= $(BUILDDIR)/$(HTOP)
HTOP_LICENSE	:= GPL-2.0-only

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HTOP_CONF_ENV	:= \
	$(CROSS_ENV) \
	HTOP_NCURSES_CONFIG_SCRIPT=ncurses6-config
#
# autoconf
#
HTOP_CONF_TOOL	:= autoconf
HTOP_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--enable-proc \
	--disable-openvz \
	--enable-cgroup \
	--disable-vserver \
	--disable-ancient-vserver \
	--enable-taskstats \
	--disable-unicode \
	--enable-linux-affinity \
	--disable-hwloc \
	--disable-setuid \
	--disable-delayacct

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/htop.targetinstall:
	@$(call targetinfo)

	@$(call install_init, htop)
	@$(call install_fixup, htop,PRIORITY,optional)
	@$(call install_fixup, htop,SECTION,base)
	@$(call install_fixup, htop,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, htop,DESCRIPTION,missing)

	@$(call install_copy, htop, 0, 0, 0755, -, /usr/bin/htop)

	@$(call install_finish, htop)

	@$(call touch)

# vim: syntax=make
