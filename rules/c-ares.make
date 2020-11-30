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
PACKAGES-$(PTXCONF_C_ARES) += c-ares

#
# Paths and names
#
C_ARES_VERSION  := 1.16.1
C_ARES_MD5  := 62dece8675590445d739b23111d93692
C_ARES		:= c-ares-$(C_ARES_VERSION)
C_ARES_SUFFIX	:= tar.gz
# NOTE: The haxx.se tarball is missing the cmake files that we need
C_ARES_URL	:= https://github.com/c-ares/c-ares/archive/cares-1_16_1.$(C_ARES_SUFFIX)
#C_ARES_URL	:= http://c-ares.haxx.se/download/$(C_ARES).$(C_ARES_SUFFIX)
C_ARES_SOURCE	:= $(SRCDIR)/$(C_ARES).$(C_ARES_SUFFIX)
C_ARES_DIR	:= $(BUILDDIR)/$(C_ARES)
C_ARES_LICENSE	:= MIT
C_ARES_LICENSE_FILE := LICENSE.md


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# cmake
#
C_ARES_CONF_TOOL      := cmake

C_ARES_CONF_OPT       := $(CROSS_CMAKE_USR)
C_ARES_CONF_OPT       += -DCARES_STATIC:BOOL=ON
C_ARES_CONF_OPT       += -DCARES_INSTALL:BOOL=ON

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/c-ares.targetinstall:
	@$(call targetinfo)

	@$(call install_init, c-ares)
	@$(call install_fixup, c-ares,PRIORITY,optional)
	@$(call install_fixup, c-ares,SECTION,base)
	@$(call install_fixup, c-ares,AUTHOR,"<oliver.graute@neuhaus.de>")
	@$(call install_fixup, c-ares,DESCRIPTION,missing)

	@$(call install_lib, c-ares, 0, 0, 0644, libcares)
	
	@$(call install_copy, c-ares, 0, 0, 0644, $(C_ARES_DIR)/$(C_ARES_LICENSE_FILE), /usr/share/licenses/oss/license.c-ares_$(C_ARES_VERSION).txt)

	@$(call install_finish, c-ares)

	@$(call touch)

# vim: syntax=make
