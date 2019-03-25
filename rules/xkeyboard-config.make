# -*-makefile-*-
#
# Copyright (C) 2009 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_XKEYBOARD_CONFIG) += xkeyboard-config

#
# Paths and names
#
XKEYBOARD_CONFIG_VERSION	:= 2.0
XKEYBOARD_CONFIG_MD5		:= bb8a98ee61cdc4bd835fdfd2b5cee3e6 06a8c600d086988cc98ea186d39d552e
XKEYBOARD_CONFIG		:= xkeyboard-config-$(XKEYBOARD_CONFIG_VERSION)
XKEYBOARD_CONFIG_SUFFIX		:= tar.bz2
XKEYBOARD_CONFIG_URL		:= $(call ptx/mirror, XORG, individual/data/xkeyboard-config/$(XKEYBOARD_CONFIG).$(XKEYBOARD_CONFIG_SUFFIX))
XKEYBOARD_CONFIG_SOURCE		:= $(SRCDIR)/$(XKEYBOARD_CONFIG).$(XKEYBOARD_CONFIG_SUFFIX)
XKEYBOARD_CONFIG_DIR		:= $(BUILDDIR)/$(XKEYBOARD_CONFIG)
XKEYBOARD_CONFIG_LICENSE	:= MIT

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

XKEYBOARD_CONFIG_PATH	:= PATH=$(CROSS_PATH)
XKEYBOARD_CONFIG_ENV	:= $(CROSS_ENV)

#
# autoconf
#
XKEYBOARD_CONFIG_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--datadir=$(XORG_DATADIR)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/xkeyboard-config.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  xkeyboard-config)
	@$(call install_fixup, xkeyboard-config,PRIORITY,optional)
	@$(call install_fixup, xkeyboard-config,SECTION,base)
	@$(call install_fixup, xkeyboard-config,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, xkeyboard-config,DESCRIPTION,missing)

	@cd $(XKEYBOARD_CONFIG_PKGDIR) &&					\
	for dir in `find .$(XORG_DATADIR)/X11/xkb -type d`; do	\
		$(call install_copy, xkeyboard-config, 0, 0, 0755, /$$dir);	\
	done
	@cd $(XKEYBOARD_CONFIG_PKGDIR) &&					\
	for file in `find .$(XORG_DATADIR)/X11/xkb -type f`; do\
		$(call install_copy, xkeyboard-config, 0, 0, 0644,		\
			$(XKEYBOARD_CONFIG_PKGDIR)/$$file, /$$file);		\
	done
	@$(call install_alternative, xkeyboard-config, 0, 0, 0755, /usr/share/X11/xkb/symbols/level3)

	@$(call install_finish, xkeyboard-config)

	@$(call touch)

# vim: syntax=make
