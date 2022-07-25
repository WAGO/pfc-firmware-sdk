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
PACKAGES-$(PTXCONF_SFP_TOOL) += sfp-tool

#
# Paths and names
#
SFP_TOOL_VERSION        := 1.0.0
SFP_TOOL_MD5            :=
SFP_TOOL                := sfp-tool
SFP_TOOL_BUILDCONFIG    := Release
SFP_TOOL_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/utils/libsfp
SFP_TOOL_BUILDROOT_DIR  := $(BUILDDIR)/$(SFP_TOOL)
SFP_TOOL_DIR            := $(SFP_TOOL_BUILDROOT_DIR)/src
SFP_TOOL_BUILD_DIR      := $(SFP_TOOL_BUILDROOT_DIR)/bin/$(SFP_TOOL_BUILDCONFIG)
SFP_TOOL_LICENSE        := WAGO
SFP_TOOL_CONF_TOOL      := NO
SFP_TOOL_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(SFP_TOOL_BUILDCONFIG) \
BIN_DIR=$(SFP_TOOL_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

SFP_TOOL_PACKAGE_NAME             := $(SFP_TOOL)_$(SFP_TOOL_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
SFP_TOOL_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/sfp-tool.extract:
	@$(call targetinfo)
	@mkdir -p $(SFP_TOOL_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(SFP_TOOL_DIR) ]; then \
	  ln -s $(SFP_TOOL_SRC_DIR) $(SFP_TOOL_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/sfp-tool.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, SFP_TOOL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/sfp-tool.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, SFP_TOOL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/sfp-tool.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(SFP_TOOL_PKGDIR) && \
	  tar xvzf $(SFP_TOOL_PLATFORMCONFIGPACKAGEDIR)/$(SFP_TOOL_PACKAGE_NAME).tgz -C $(SFP_TOOL_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, SFP_TOOL)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(SFP_TOOL_PLATFORMCONFIGPACKAGEDIR)
	@cd $(SFP_TOOL_PKGDIR) && tar cvzf $(SFP_TOOL_PLATFORMCONFIGPACKAGEDIR)/$(SFP_TOOL_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/sfp-tool.targetinstall:
	@$(call targetinfo)

	@$(call install_init, sfp-tool)
	@$(call install_fixup, sfp-tool,PRIORITY,optional)
	@$(call install_fixup, sfp-tool,SECTION,base)
	@$(call install_fixup, sfp-tool,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, sfp-tool,DESCRIPTION,"WAGO SFP tooling")


ifdef PTXCONF_SFP_TOOL_FLASH
	@$(call install_copy, sfp-tool, 0, 0, 0755, -, /etc/specific/sfp-flash)
	@$(call install_copy, sfp-tool, 0, 0, 0440, -, /etc/specific/sfp.fw)
endif # PTXCONF_SFP_TOOL_FLASH

	@$(call install_finish, sfp-tool)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/sfp-tool.clean:
	@$(call targetinfo)
	@if [ -d $(SFP_TOOL_DIR) ]; then \
		$(SFP_TOOL_MAKE_ENV) $(SFP_TOOL_PATH) $(MAKE) $(MFLAGS) -C $(SFP_TOOL_DIR) clean; \
	fi
	@$(call clean_pkg, SFP_TOOL)
	@rm -rf $(SFP_TOOL_BUILDROOT_DIR)

# vim: syntax=make
