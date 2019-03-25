# -*-makefile-*-
#
# Copyright (C) 2019 by <AGa>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WAGO_SAMPLE_OPENVPN_CONFIG) += wago-sample-openvpn-config

WAGO_SAMPLE_OPENVPN_CONFIG_VERSION	:= 1.0.0

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-sample-openvpn-config.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, wago-sample-openvpn-config)
	@$(call install_fixup,wago-sample-openvpn-config,PRIORITY,optional)
	@$(call install_fixup,wago-sample-openvpn-config,SECTION,base)
	@$(call install_fixup,wago-sample-openvpn-config,AUTHOR,"<AGa>")
	@$(call install_fixup,wago-sample-openvpn-config,DESCRIPTION,missing)

	@$(call install_alternative, wago-sample-openvpn-config, 0, 0, 0640, /etc/openvpn/openvpn.conf, n)

	@$(call install_finish,wago-sample-openvpn-config)
	@$(call touch)

# vim: syntax=make
