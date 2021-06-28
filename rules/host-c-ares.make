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
# Host, image and cross packages don’t need to install anything in the target
# file system. Therefore, PTXdist only respects the targetinstall and
# targetinstall.post stages for packages whose name doesn’t start with host-,
# image-, or cross-.

# vim: syntax=make
