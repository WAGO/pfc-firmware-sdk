# -*-makefile-*-
#
# Copyright (C) 2019 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_NETWORK_CONFIG) += network_config

#
# Paths and names
#
NETWORK_CONFIG_VERSION        := 1.1.0
NETWORK_CONFIG_MD5            :=
NETWORK_CONFIG                := network_config
NETWORK_CONFIG_BUILDCONFIG    := Release
NETWORK_CONFIG_SRC_DIR        := $(PTXDIST_WORKSPACE)/local_src/config-tools/$(NETWORK_CONFIG)
NETWORK_CONFIG_BUILDROOT_DIR  := $(BUILDDIR)/$(NETWORK_CONFIG)
NETWORK_CONFIG_DIR            := $(NETWORK_CONFIG_BUILDROOT_DIR)/src
NETWORK_CONFIG_BUILD_DIR      := $(NETWORK_CONFIG_BUILDROOT_DIR)/bin/$(NETWORK_CONFIG_BUILDCONFIG)
NETWORK_CONFIG_LICENSE        := WAGO
NETWORK_CONFIG_CONF_TOOL      := NO
NETWORK_CONFIG_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(NETWORK_CONFIG_BUILDCONFIG) \
BIN_DIR=$(NETWORK_CONFIG_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(NETWORK_CONFIG_SRC_DIR):
	@echo "Error: $@: directory not found!" >&2; exit 2

$(STATEDIR)/network_config.extract:  | $(NETWORK_CONFIG_SRC_DIR) 
	@$(call targetinfo)
	@mkdir -p $(NETWORK_CONFIG_BUILDROOT_DIR)
	@if [ ! -L $(NETWORK_CONFIG_DIR) ]; then \
	  ln -s $(NETWORK_CONFIG_SRC_DIR) $(NETWORK_CONFIG_DIR); \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/network_config.prepare:
	@$(call targetinfo)
	@$(call world/prepare, NETWORK_CONFIG)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/network_config.compile:
	@$(call targetinfo)
	@$(call world/compile, NETWORK_CONFIG)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/network_config.install:
	@$(call targetinfo)

	@$(call world/install, NETWORK_CONFIG)


	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/network_config.targetinstall:
	@$(call targetinfo)

	@$(call install_init, network_config)
	@$(call install_fixup, network_config,PRIORITY,optional)
	@$(call install_fixup, network_config,SECTION,base)
	@$(call install_fixup, network_config,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, network_config,DESCRIPTION,missing)

	@$(call install_copy, network_config, 0, 0, 0755,$(NETWORK_CONFIG_BUILD_DIR)/network_config.elf, /etc/config-tools/network_config)

	@$(call install_finish, network_config)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/network_config.clean:
	@$(call targetinfo)
	@if [ -d $(NETWORK_CONFIG_DIR) ]; then \
		$(NETWORK_CONFIG_MAKE_ENV) $(NETWORK_CONFIG_PATH) $(MAKE) $(MFLAGS) -C $(NETWORK_CONFIG_DIR) clean; \
	fi
	@$(call clean_pkg, NETWORK_CONFIG)
	@rm -rf $(NETWORK_CONFIG_BUILDROOT_DIR)

# vim: syntax=make
