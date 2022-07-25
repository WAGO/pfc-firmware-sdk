# -*-makefile-*-
#
# Copyright (C) 2020 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CONFIG_DNSMASQ) += config_dnsmasq

#
# Paths and names
#
CONFIG_DNSMASQ_VERSION        := 1.0.2
CONFIG_DNSMASQ_MD5            :=
CONFIG_DNSMASQ                := config_dnsmasq
CONFIG_DNSMASQ_BUILDCONFIG    := Release
CONFIG_DNSMASQ_SRC_DIR        := $(PTXDIST_WORKSPACE)/local_src/config-tools/$(CONFIG_DNSMASQ)
CONFIG_DNSMASQ_BUILDROOT_DIR  := $(BUILDDIR)/$(CONFIG_DNSMASQ)
CONFIG_DNSMASQ_DIR            := $(CONFIG_DNSMASQ_BUILDROOT_DIR)/src
CONFIG_DNSMASQ_BUILD_DIR      := $(CONFIG_DNSMASQ_BUILDROOT_DIR)/bin/$(CONFIG_DNSMASQ_BUILDCONFIG)
CONFIG_DNSMASQ_LICENSE        := WAGO
CONFIG_DNSMASQ_CONF_TOOL      := NO
CONFIG_DNSMASQ_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(CONFIG_DNSMASQ_BUILDCONFIG) \
BIN_DIR=$(CONFIG_DNSMASQ_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(CONFIG_DNSMASQ_SRC_DIR):
	@echo "Error: $@: directory not found!" >&2; exit 2

$(STATEDIR)/config_dnsmasq.extract:  | $(CONFIG_DNSMASQ_SRC_DIR) 
	@$(call targetinfo)
	@mkdir -p $(CONFIG_DNSMASQ_BUILDROOT_DIR)
	@if [ ! -L $(CONFIG_DNSMASQ_DIR) ]; then \
	  ln -s $(CONFIG_DNSMASQ_SRC_DIR) $(CONFIG_DNSMASQ_DIR); \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/config_dnsmasq.prepare:
	@$(call targetinfo)
	@$(call world/prepare, CONFIG_DNSMASQ)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/config_dnsmasq.compile:
	@$(call targetinfo)
	@$(call world/compile, CONFIG_DNSMASQ)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/config_dnsmasq.install:
	@$(call targetinfo)

	@$(call world/install, CONFIG_DNSMASQ)


	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/config_dnsmasq.targetinstall:
	@$(call targetinfo)

	@$(call install_init, config_dnsmasq)
	@$(call install_fixup, config_dnsmasq,PRIORITY,optional)
	@$(call install_fixup, config_dnsmasq,SECTION,base)
	@$(call install_fixup, config_dnsmasq,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, config_dnsmasq,DESCRIPTION,missing)

	@$(call install_copy, config_dnsmasq, 0, 0, 0755, $(CONFIG_DNSMASQ_BUILD_DIR)/config_dnsmasq_c.elf,/etc/config-tools/config_dnsmasq_c) 

	# Does not install /etc/dnsmasq.conf because dnsmasq packet has already installed or will install our config file.
	@$(call install_copy,        config_dnsmasq, 0, 0, 0750, /etc/dnsmasq.d)
	@$(call install_alternative, config_dnsmasq, 0, 0, 0644, /etc/dnsmasq.d/dnsmasq_default.conf)
	@$(call install_alternative, config_dnsmasq, 0, 0, 0750, /etc/config-tools/post_netconfig.d/30_dnsmasq)
	@$(call install_alternative, config_dnsmasq, 0, 0, 0644, /etc/specific/network-services.xml)
	@$(call install_alternative, config_dnsmasq, 0, 0, 0700, /etc/config-tools/events/dns/addLocalhostToResolvConf)

	@$(call install_finish, config_dnsmasq)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/config_dnsmasq.clean:
	@$(call targetinfo)
	@if [ -d $(CONFIG_DNSMASQ_DIR) ]; then \
		$(CONFIG_DNSMASQ_MAKE_ENV) $(CONFIG_DNSMASQ_PATH) $(MAKE) $(MFLAGS) -C $(CONFIG_DNSMASQ_DIR) clean; \
	fi
	@$(call clean_pkg, CONFIG_DNSMASQ)
	@rm -rf $(CONFIG_DNSMASQ_BUILDROOT_DIR)

# vim: syntax=make
