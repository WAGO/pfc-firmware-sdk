# -*-makefile-*-
#
# Copyright (C) 2007 by Sascha Hauer
#               2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_ETHTOOL) += ethtool

#
# Paths and names
#
ETHTOOL_VERSION	:= 5.7
ETHTOOL_MD5	:= 3293642e58a205a834cbe5754df9c832
ETHTOOL_SUFFIX	:= tar.xz
ETHTOOL		:= ethtool-$(ETHTOOL_VERSION)
ETHTOOL_URL	:= $(call ptx/mirror, KERNEL, ../software/network/ethtool/$(ETHTOOL).$(ETHTOOL_SUFFIX))
ETHTOOL_SOURCE	:= $(SRCDIR)/$(ETHTOOL).$(ETHTOOL_SUFFIX)
ETHTOOL_DIR	:= $(BUILDDIR)/$(ETHTOOL)
ETHTOOL_LICENSE := GPL-2.0-only
ETHTOOL_LICENSE_FILES	:= \
	file://COPYING;md5=b234ee4d69f5fce4486a80fdaf4a4263

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

ETHTOOL_CONF_TOOL := autoconf
ETHTOOL_CONF_OPT  := \
	$(CROSS_AUTOCONF_USR) \
	--enable-pretty-dump

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ethtool.targetinstall:
	@$(call targetinfo)

	@$(call install_init, ethtool)
	@$(call install_fixup, ethtool,PRIORITY,optional)
	@$(call install_fixup, ethtool,SECTION,base)
	@$(call install_fixup, ethtool,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, ethtool,DESCRIPTION,missing)

	@$(call install_copy, ethtool, 0, 0, 0755, -, /usr/sbin/ethtool)

	@$(call install_finish, ethtool)

	@$(call touch)

# vim: syntax=make
