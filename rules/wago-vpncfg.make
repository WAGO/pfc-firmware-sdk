# -*-makefile-*-
#
# Copyright (C) 2014 by Mariusz Podlesny <mariusz.podlesny@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WAGO_VPNCFG) += wago-vpncfg

WAGO_VPNCFG_VERSION	:= 1.0.0

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(STATEDIR)/wago-vpncfg.get:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#$(STATEDIR)/wago-vpncfg.extract:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#$(STATEDIR)/wago-vpncfg.prepare:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/wago-vpncfg.compile:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-vpncfg.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-vpncfg.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  wago-vpncfg)
	@$(call install_fixup, wago-vpncfg, PRIORITY, optional)
	@$(call install_fixup, wago-vpncfg, SECTION, base)
	@$(call install_fixup, wago-vpncfg, AUTHOR, "Mariusz Podlesny <mariusz.podlesny@wago.com>")
	@$(call install_fixup, wago-vpncfg, DESCRIPTION, missing)

	@$(call install_copy, wago-vpncfg, 0, 0, 0755, /etc/certificates)
	@$(call install_copy, wago-vpncfg, 0, 0, 0700, /etc/certificates/keys)

	@$(call install_alternative, wago-vpncfg, 0, 0, 0644, /lib/lsb/init-functions)

ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, wago-vpncfg, 0, 0, 0700, /etc/init.d/ipsec)
	@$(call install_alternative, wago-vpncfg, 0, 0, 0700, /etc/init.d/openvpn)

ifneq ($(PTXCONF_WAGO_VPNCFG_IPSEC_RC_LINK),"")
	@$(call install_link, wago-vpncfg, ../init.d/ipsec, /etc/rc.d/$(PTXCONF_WAGO_VPNCFG_IPSEC_RC_LINK))
endif

ifneq ($(PTXCONF_WAGO_VPNCFG_OVPN_RC_LINK),"")
	@$(call install_link, wago-vpncfg, ../init.d/openvpn, /etc/rc.d/$(PTXCONF_WAGO_VPNCFG_OVPN_RC_LINK))
endif


else
	$(error "Please supply an init file for your init system!")
endif

	@$(call install_finish,wago-vpncfg)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/wago-vpncfg.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, WAGO_VPNCFG)

# vim: syntax=make
