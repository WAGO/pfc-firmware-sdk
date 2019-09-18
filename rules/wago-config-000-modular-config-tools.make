# -*-makefile-*-
#
# Copyright (C) 2017 by <PEn>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_MODULAR_CONFIG_TOOLS) += modular-config-tools

#
# Paths and names
#
MODULAR_CONFIG_TOOLS_VERSION        := 0.2.1
MODULAR_CONFIG_TOOLS_MD5            :=
MODULAR_CONFIG_TOOLS                := modular-config-tools
MODULAR_CONFIG_TOOLS_URL            := file://shared_public/$(MODULAR_CONFIG_TOOLS)
MODULAR_CONFIG_TOOLS_BUILDCONFIG    := Release
MODULAR_CONFIG_TOOLS_SRC_DIR        := $(PTXDIST_WORKSPACE)/shared_public/$(MODULAR_CONFIG_TOOLS)
MODULAR_CONFIG_TOOLS_BUILDROOT_DIR  := $(BUILDDIR)/$(MODULAR_CONFIG_TOOLS)
MODULAR_CONFIG_TOOLS_DIR            := $(MODULAR_CONFIG_TOOLS_BUILDROOT_DIR)/src
MODULAR_CONFIG_TOOLS_BUILD_DIR      := $(MODULAR_CONFIG_TOOLS_BUILDROOT_DIR)/bin/$(MODULAR_CONFIG_TOOLS_BUILDCONFIG)
MODULAR_CONFIG_TOOLS_LICENSE        := MPL-2.0, MIT
MODULAR_CONFIG_TOOLS_PATH           := PATH=$(CROSS_PATH)
MODULAR_CONFIG_TOOLS_BIN            := 
MODULAR_CONFIG_TOOLS_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(MODULAR_CONFIG_TOOLS_BUILDCONFIG) \
BIN_DIR=$(MODULAR_CONFIG_TOOLS_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/modular-config-tools.extract:
	@$(call targetinfo)
	@mkdir -p $(MODULAR_CONFIG_TOOLS_BUILDROOT_DIR)
	@if [ ! -L $(MODULAR_CONFIG_TOOLS_DIR) ]; then \
	  ln -s $(MODULAR_CONFIG_TOOLS_SRC_DIR) $(MODULAR_CONFIG_TOOLS_DIR); \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

MODULAR_CONFIG_TOOLS_CONF_TOOL	:= NO

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/modular-config-tools.compile:
	@$(call targetinfo)

ifdef PTXCONF_CT_CONFIG_TOOL_UTILS_LIB_C
	@$(call world/compile, MODULAR_CONFIG_TOOLS)
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/modular-config-tools.install:
	@$(call targetinfo)

ifdef PTXCONF_CT_CONFIG_TOOL_UTILS_LIB_C
	@mkdir -p $(PTXCONF_SYSROOT_TARGET)/usr/include/ctutil/
	@install -m 0644 $(MODULAR_CONFIG_TOOLS_SRC_DIR)/inc/ctutil/*.h \
		$(PTXCONF_SYSROOT_TARGET)/usr/include/ctutil
	@mkdir -p $(PTXCONF_SYSROOT_TARGET)/usr/include/ctparts/
	@install -m 0644 $(MODULAR_CONFIG_TOOLS_SRC_DIR)/inc/ctparts/*.h \
		$(PTXCONF_SYSROOT_TARGET)/usr/include/ctparts
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/modular-config-tools.targetinstall:
	@$(call targetinfo)

	@$(call install_init, modular-config-tools)
	@$(call install_fixup, modular-config-tools,PRIORITY,optional)
	@$(call install_fixup, modular-config-tools,SECTION,base)
	@$(call install_fixup, modular-config-tools,AUTHOR,"<PEn>")
	@$(call install_fixup, modular-config-tools,DESCRIPTION,missing)

	@$(call install_copy, modular-config-tools, 0, 0, 0755, /etc/config-tools);

ifdef PTXCONF_CT_CONFIG_LINUX_USER
	@$(call install_copy, modular-config-tools, 0, 0, 0750, $(MODULAR_CONFIG_TOOLS_BUILD_DIR)/config_linux_user.elf, /etc/config-tools/config_linux_user)
endif

ifdef PTXCONF_CT_RESERVED_AREA
	@$(call install_copy, modular-config-tools, 0, 0, 0750, $(MODULAR_CONFIG_TOOLS_BUILD_DIR)/reserved_area.elf, /etc/config-tools/reserved_area)
endif

	@$(call install_finish, modular-config-tools)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/modular-config-tools.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/ctutil/
	@rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/ctparts/
	@if [ -d $(MODULAR_CONFIG_TOOLS_DIR) ]; then \
	  $(MODULAR_CONFIG_TOOLS_MAKE_ENV) $(MODULAR_CONFIG_TOOLS_PATH) $(MAKE) $(MFLAGS) -C $(MODULAR_CONFIG_TOOLS_DIR) clean; \
	fi
	@$(call clean_pkg, MODULAR_CONFIG_TOOLS)
	@rm -rf $(MODULAR_CONFIG_TOOLS_BUILDROOT_DIR)

# vim: syntax=make

