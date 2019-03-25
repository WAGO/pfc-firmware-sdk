# -*-makefile-*-
#
# Copyright (C) 2010 by Jon Ringle
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WATCHDOG) += watchdog

#
# Paths and names
#
WATCHDOG_VERSION   := 5.15
WATCHDOG_MD5       := 678c32f6f35a0492c9c1b76b4aa88828
WATCHDOG           := watchdog-$(WATCHDOG_VERSION)
WATCHDOG_SUFFIX    := tar.gz
WATCHDOG_URL       := http://downloads.sourceforge.net/project/watchdog/watchdog/5.15/watchdog-5.15.tar.gz
#WATCHDOG_URL       := $(call ptx/mirror, SF, watchdog/$(WATCHDOG).$(WATCHDOG_SUFFIX))
WATCHDOG_SOURCE    := $(SRCDIR)/$(WATCHDOG).$(WATCHDOG_SUFFIX)
WATCHDOG_DIR       := $(BUILDDIR)/$(WATCHDOG)
WATCHDOG_LICENSE   := GPLv2

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

WATCHDOG_CONF_TOOL := autoconf

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/watchdog.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  watchdog)
	@$(call install_fixup, watchdog,PRIORITY,optional)
	@$(call install_fixup, watchdog,SECTION,base)
	@$(call install_fixup, watchdog,AUTHOR,"Jon Ringle")
	@$(call install_fixup, watchdog,DESCRIPTION,missing)

	@$(call install_alternative, watchdog, 0, 0, 0644, /etc/watchdog.conf)
	@$(call install_copy, watchdog, 0, 0, 0755, -, /usr/sbin/watchdog)
	@$(call install_copy, watchdog, 0, 0, 0755, /etc/init.d)
	@$(call install_alternative, watchdog, 0, 0, 0755, /etc/init.d/watchdog)
	@$(call install_copy, watchdog, 0, 0, 0755, /etc/rc.d/disabled)
	@$(call install_link, watchdog, ../init.d/watchdog, /etc/rc.d/disabled/S99_watchdog)

	@$(call install_finish, watchdog)

	@$(call touch)

# vim: syntax=make
