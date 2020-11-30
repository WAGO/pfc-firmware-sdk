# -*-makefile-*-
#
# Copyright (C) 2014 Dr. Neuhaus Telekommunikation GmbH, Hamburg Germany, Oliver Graute <oliver.graute@neuhaus.de>
# Copyright (C) 2017 Marvin Schmidt <Marvin.Schmidt@who-ing.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_C_ARES) += host-c-ares

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# cmake
#
HOST_C_ARES_CONF_TOOL      := cmake

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-c-ares.targetinstall:
	@$(call targetinfo)

	@$(call install_init, c-ares)
	@$(call install_fixup, c-ares,PRIORITY,optional)
	@$(call install_fixup, c-ares,SECTION,base)
	@$(call install_fixup, c-ares,AUTHOR,"<oliver.graute@neuhaus.de>")
	@$(call install_fixup, c-ares,DESCRIPTION,missing)

	@$(call install_lib, c-ares, 0, 0, 0644, libcares)

	@$(call install_finish, c-ares)

	@$(call touch)

# vim: syntax=make
