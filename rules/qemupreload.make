# -*-makefile-*-
#
# Copyright (C) 2018 by J. Breitenstein
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_QEMUPRELOAD) += qemupreload

#
# Paths and names
#
QEMUPRELOAD_VERSION        := 0.0.1
QEMUPRELOAD_MD5            :=
QEMUPRELOAD                := qemupreload
QEMUPRELOAD_BUILDCONFIG    := Debug
QEMUPRELOAD_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(QEMUPRELOAD)
QEMUPRELOAD_BUILDROOT_DIR  := $(BUILDDIR)/$(QEMUPRELOAD)/lib
QEMUPRELOAD_DIR            := $(QEMUPRELOAD_BUILDROOT_DIR)/src
QEMUPRELOAD_BUILD_DIR      := $(QEMUPRELOAD_BUILDROOT_DIR)/bin/$(QEMUPRELOAD_BUILDCONFIG)
QEMUPRELOAD_BIN            := $(QEMUPRELOAD).so.$(QEMUPRELOAD_VERSION)
QEMUPRELOAD_CONF_TOOL      := NO
QEMUPRELOAD_LICENSE        := unknown
QEMUPRELOAD_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(QEMUPRELOAD_BUILDCONFIG) \
BIN_DIR=$(QEMUPRELOAD_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build



# FIXME: dirty counter dependency
$(STATEDIR)/host-ct-build.install.post: $(STATEDIR)/qemupreload.install



# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(QEMUPRELOAD_SOURCE):
#	@$(call targetinfo)
#	@$(call get, QEMUPRELOAD)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/qemupreload.extract: $(STATEDIR)/host-ct-build.install
	@$(call targetinfo)
	@mkdir -p $(QEMUPRELOAD_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES  
	@if [ ! -L $(QEMUPRELOAD_DIR) ]; then \
	  ln -s $(QEMUPRELOAD_SRC_DIR) $(QEMUPRELOAD_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


#$(STATEDIR)/qemupreload.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, QEMUPRELOAD)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/qemupreload.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES  
	@$(call world/compile, QEMUPRELOAD)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/qemupreload.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES  
	@$(call world/install, QEMUPRELOAD)
endif
	@$(call touch)

$(STATEDIR)/host-ct-build.install.post: $(STATEDIR)/qemupreload.install.post

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

# Dont't install into target
$(STATEDIR)/qemupreload.targetinstall:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/qemupreload.clean:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES  
	@if [ -d $(QEMUPRELOAD_DIR) ]; then \
		$(QEMUPRELOAD_MAKE_ENV) $(QEMUPRELOAD_PATH) $(MAKE) $(MFLAGS) -C $(QEMUPRELOAD_DIR) clean; \
	fi
	@$(call clean_pkg, QEMUPRELOAD)
	@rm -rf $(QEMUPRELOAD_BUILDROOT_DIR)
endif

# vim: syntax=make
