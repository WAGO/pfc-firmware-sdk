# -*-makefile-*-
#
# Copyright (C) 2017 by Patrick Enns <patrick.enns@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WAGO_MODEMMANAGER) += wago-modemmanager

WAGO_MODEMMANAGER_VERSION := 2.0.0

#
# Paths and names
#
WAGO_MODEMMANAGER                := wago-modemmanager
WAGO_MODEMMANAGER_BUILDCONFIG    := Release
WAGO_MODEMMANAGER_URL            := file://$(PTXDIST_WORKSPACE)/local_src/modemmanager
WAGO_MODEMMANAGER_BUILDROOT_DIR  := $(BUILDDIR)/$(WAGO_MODEMMANAGER)
WAGO_MODEMMANAGER_DIR            := $(BUILDDIR)/$(WAGO_MODEMMANAGER)
WAGO_MODEMMANAGER_BUILD_DIR      := $(WAGO_MODEMMANAGER_BUILDROOT_DIR)/bin/$(WAGO_MODEMMANAGER_BUILDCONFIG)
WAGO_MODEMMANAGER_SRC            := $(PTXDIST_WORKSPACE)/local_src/$(WAGO_MODEMMANAGER)/src
#WAGO_MODEMMANAGER_PATH          := PATH=$(CROSS_PATH)
#WAGO_MODEMMANAGER_CONF_TOOL     := NO
WAGO_MODEMMANAGER_MAKE_ENV       := $(CROSS_ENV) \
 BUILDCONFIG=$(WAGO_MODEMMANAGER_BUILDCONFIG) \
 BIN_DIR=$(WAGO_MODEMMANAGER_BUILD_DIR) \
 SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build \
# PKG_CONFIG_PATH=$(PTXDIST_SYSROOT_TARGET)/usr/lib/pkg-config


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#$(STATEDIR)/wago-modemmanager.extract:
#	@$(call targetinfo, $@)
#	@$(call clean, $(WAGO_MODEMMANAGER_DIR))
#	cp -rd $(WAGO_MODEMMANAGER_SOURCE) $(WAGO_MODEMMANAGER_DIR)
#	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#$(STATEDIR)/wago-modemmanager.prepare:
#	@$(call targetinfo)
#	@echo "Create modemmanager diagnostic header file in sysroot-target/usr/include/diagnostic directory by using of XSLT-script"
#	$(NOPRINT)$(call xslt_compile, $(WAGO_MODEMMANAGER_DIR)/modemmanager_diag.xml)
#	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/wago-modemmanager.compile:
#	@$(call targetinfo, $@)
#	@cd $(WAGO_MODEMMANAGER_DIR) && \
#		$(WAGO_MODEMMANAGER_MAKE_ENV) \
#		$(MAKE)	CROSS_COMPILE=$(COMPILER_PREFIX) SYSROOT=$(PTXDIST_SYSROOT_TARGET)
#	@$(call touch, $@)
	

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/wago-modemmanager.install:
#	@$(call targetinfo, $@)
#	@cd $(WAGO_MODEMMANAGER_DIR) && \
#		$(WAGO_MODEMMANAGER_MAKE_ENV) \
#		$(MAKE)	CROSS_COMPILE=$(COMPILER_PREFIX) SYSROOT=$(PTXDIST_SYSROOT_TARGET)
#	@$(call touch, $@)
#	@$(call targetinfo)
#	@$(call world/install, wago-modemmanager)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-modemmanager.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wago-modemmanager)
	@$(call install_fixup, wago-modemmanager,PRIORITY,optional)
	@$(call install_fixup, wago-modemmanager,SECTION,base)
	@$(call install_fixup, wago-modemmanager,AUTHOR,"Patrick Enns)
	@$(call install_fixup, wago-modemmanager,DESCRIPTION,missing)

#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish
#
	@$(call install_copy, wago-modemmanager, 0, 0, 0755, -, /usr/bin/mdmd)
	@$(call install_copy, wago-modemmanager, 0, 0, 0755, -, /usr/bin/mdmcli)

	@$(call install_finish, wago-modemmanager)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-modemmanager.clean:
	@$(call targetinfo)
	@if [ -d $(WAGO_MODEMMANAGER_DIR) ]; then \
		$(WAGO_MODEMMANAGER_MAKE_ENV) $(WAGO_MODEMMANAGER_PATH) $(MAKE) $(MFLAGS) -C $(WAGO_MODEMMANAGER_DIR) clean; \
	fi
	@$(call clean_pkg, WAGO_MODEMMANAGER)
	@rm -rf $(WAGO_MODEMMANAGER_BUILDROOT_DIR)

# vim: syntax=make
