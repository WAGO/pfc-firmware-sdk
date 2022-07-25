# -*-makefile-*-
#
# Copyright (C) 2022 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_SNMP_CONFIG_LIB) += snmp-config-lib

#
# Paths and names
#
SNMP_CONFIG_LIB_VERSION        := 1.0.0
SNMP_CONFIG_LIB_MD5            :=
SNMP_CONFIG_LIB                := snmp-config-lib
SNMP_CONFIG_LIB_BUILDCONFIG    := Release
SNMP_CONFIG_LIB_SRC_DIR        := $(PTXDIST_WORKSPACE)/local_src/$(SNMP_CONFIG_LIB)
SNMP_CONFIG_LIB_BUILDROOT_DIR  := $(BUILDDIR)/$(SNMP_CONFIG_LIB)
SNMP_CONFIG_LIB_DIR            := $(SNMP_CONFIG_LIB_BUILDROOT_DIR)/src
SNMP_CONFIG_LIB_BUILD_DIR      := $(SNMP_CONFIG_LIB_BUILDROOT_DIR)/bin/$(SNMP_CONFIG_LIB_BUILDCONFIG)
SNMP_CONFIG_LIB_LICENSE        := WAGO
SNMP_CONFIG_LIB_CONF_TOOL      := NO
SNMP_CONFIG_LIB_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(SNMP_CONFIG_LIB_BUILDCONFIG) \
BIN_DIR=$(SNMP_CONFIG_LIB_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

SNMP_CONFIG_LIB_PACKAGE_NAME             := $(SNMP_CONFIG_LIB)_$(SNMP_CONFIG_LIB_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
SNMP_CONFIG_LIB_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages


# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(SNMP_CONFIG_LIB_SRC_DIR):
	@echo "Error: $@: directory not found!" >&2; \
	exit 2


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/snmp-config-lib.extract: | $(SNMP_CONFIG_LIB_SRC_DIR)
	@$(call targetinfo)
	@echo "create link ($(SNMP_CONFIG_LIB_DIR) -> $(SNMP_CONFIG_LIB_SRC_DIR))"
	@mkdir -p $(SNMP_CONFIG_LIB_BUILDROOT_DIR)
	@if [ ! -L $(SNMP_CONFIG_LIB_DIR) ]; then \
	  ln -s $(SNMP_CONFIG_LIB_SRC_DIR) $(SNMP_CONFIG_LIB_DIR); \
	fi
	@$(call touch)


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#$(STATEDIR)/snmp-config-lib.prepare:
#	@$(call targetinfo)
#ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#	@$(call world/prepare, SNMP_CONFIG_LIB)
#endif
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/snmp-config-lib.compile:
#	@$(call targetinfo)
#ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#	@$(call world/compile, SNMP_CONFIG_LIB)
#endif
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/snmp-config-lib.install:
#	@$(call targetinfo)
#ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
## BSP mode: install by extracting tgz file
#	@mkdir -p $(SNMP_CONFIG_LIB_PKGDIR) && \
#	  tar xvzf $(SNMP_CONFIG_LIB_PLATFORMCONFIGPACKAGEDIR)/$(SNMP_CONFIG_LIB_PACKAGE_NAME).tgz -C $(SNMP_CONFIG_LIB_PKGDIR)
#else
## normal mode, call "make install"
#
#	@$(call world/install, SNMP_CONFIG_LIB)
#
#ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
## save install directory to tgz for BSP mode
#	@mkdir -p $(SNMP_CONFIG_LIB_PLATFORMCONFIGPACKAGEDIR)
#	@cd $(SNMP_CONFIG_LIB_PKGDIR) && tar cvzf $(SNMP_CONFIG_LIB_PLATFORMCONFIGPACKAGEDIR)/$(SNMP_CONFIG_LIB_PACKAGE_NAME).tgz *
#endif
#endif

#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/snmp-config-lib.targetinstall:
	@$(call targetinfo)

	@$(call install_init, snmp-config-lib)
	@$(call install_fixup, snmp-config-lib,PRIORITY,optional)
	@$(call install_fixup, snmp-config-lib,SECTION,base)
	@$(call install_fixup, snmp-config-lib,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, snmp-config-lib,DESCRIPTION,missing)

	@$(call install_lib, snmp-config-lib, 0, 0, 0644, libsnmpconfig)

	@$(call install_finish, snmp-config-lib)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/snmp-config-lib.clean:
	@$(call targetinfo)
	@if [ -d $(SNMP_CONFIG_LIB_DIR) ]; then \
		$(SNMP_CONFIG_LIB_MAKE_ENV) $(SNMP_CONFIG_LIB_PATH) $(MAKE) $(MFLAGS) -C $(SNMP_CONFIG_LIB_DIR) clean; \
	fi
	@$(call clean_pkg, SNMP_CONFIG_LIB)
	@rm -rf $(SNMP_CONFIG_LIB_BUILDROOT_DIR)

# vim: syntax=make
