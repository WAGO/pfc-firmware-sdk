# -*-makefile-*-
#
# Copyright (C) 2012 by <WAGO Kontakttechnik GmbH & Co. KG>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
#--- provided package --------------------------------------------------------
#
PACKAGES-$(PTXCONF_LIBBACNETCONFIG) += libbacnetconfig
#
#--- paths and names --------------------------------------------------------- 
#
LIBBACNETCONFIG_VERSION           := 0.0.1
LIBBACNETCONFIG_MD5               :=
LIBBACNETCONFIG                   := libbacnetconfig
LIBBACNETCONFIG_URL               := file://$(PTXDIST_WORKSPACE)/wago_intern/device/bacnet/$(LIBBACNETCONFIG)
LIBBACNETCONFIG_BUILDCONFIG       := Release
LIBBACNETCONFIG_SRC_DIR           := $(PTXDIST_WORKSPACE)/wago_intern/device/bacnet/$(LIBBACNETCONFIG)
LIBBACNETCONFIG_BUILDROOT_DIR     := $(BUILDDIR)/$(LIBBACNETCONFIG)
LIBBACNETCONFIG_DIR               := $(LIBBACNETCONFIG_BUILDROOT_DIR)/$(LIBBACNETCONFIG)
LIBBACNETCONFIG_BUILD_DIR         := $(LIBBACNETCONFIG_BUILDROOT_DIR)/bin/$(LIBBACNETCONFIG_BUILDCONFIG)
LIBBACNETCONFIG_LICENSE           := unknown
LIBBACNETCONFIG_BIN               := $(LIBBACNETCONFIG).so.$(LIBBACNETCONFIG_VERSION)
LIBBACNETCONFIG_SO_NAME           := $(LIBBACNETCONFIG).so
LIBBACNETCONFIG_CONF_TOOL         := NO
LIBBACNETCONFIG_MAKE_ENV          := $(CROSS_ENV) \
BUILDCONFIG=$(LIBBACNETCONFIG_BUILDCONFIG) \
              BIN_DIR=$(LIBBACNETCONFIG_BUILD_DIR) \
              TARGET_ARCH=$(PTXCONF_ARCH_STRING) \
              ARM_ARCH_VERSION=7 \
              SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

LIBBACNETCONFIG_PATH              := PATH=$(CROSS_PATH)
LIBBACNETCONFIG_PACKAGE_NAME      := $(LIBBACNETCONFIG)_$(LIBBACNETCONFIG_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBBACNETCONFIG_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libbacnetconfig.extract:
	@$(call targetinfo)
	@mkdir -p $(LIBBACNETCONFIG_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(LIBBACNETCONFIG_DIR) ]; then \
		ln -s $(LIBBACNETCONFIG_SRC_DIR) $(LIBBACNETCONFIG_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract.post
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/libbacnetconfig.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libbacnetconfig.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	@$(call world/prepare, LIBBACNETCONFIG)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/libbacnetconfig.compile:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, LIBBACNETCONFIG)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libbacnetconfig.install:
	@$(call targetinfo)
	
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#   BSP mode: install by extracting tgz file
	@mkdir -p $(LIBBACNETCONFIG_PKGDIR) && \
  	tar xvzf $(LIBBACNETCONFIG_PLATFORMCONFIGPACKAGEDIR)/$(LIBBACNETCONFIG_PACKAGE_NAME).tgz -C $(LIBBACNETCONFIG_PKGDIR)
else	
# 	normal mode, call "make install"

	@$(call world/install, LIBBACNETCONFIG)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#   # save install directory to tgz for BSP mode
	@mkdir -p $(LIBBACNETCONFIG_PLATFORMCONFIGPACKAGEDIR)
	@cd $(LIBBACNETCONFIG_PKGDIR) && tar cvzf $(LIBBACNETCONFIG_PLATFORMCONFIGPACKAGEDIR)/$(LIBBACNETCONFIG_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libbacnetconfig.targetinstall:
	@$(call targetinfo)
	@$(call install_init, libbacnetconfig)


	@$(call install_fixup, libbacnetconfig, PRIORITY, optional)
	@$(call install_fixup, libbacnetconfig, SECTION, base)
	@$(call install_fixup, libbacnetconfig, AUTHOR, "Nico Baade <nico.baade@wago.com>")
	@$(call install_fixup, libbacnetconfig, DESCRIPTION, "BACnet configuration library for the PFC controller" )

	@$(call install_lib, libbacnetconfig, 0, 0, 0644, libbacnetconfig)
	@$(call install_link, libbacnetconfig, $(LIBBACNETCONFIG_BIN), /usr/lib/$(LIBBACNETCONFIG_SO_NAME))

	@$(call install_finish, libbacnetconfig)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

PTX_ROOT=$(PTXCONF_SYSROOT_TARGET)/../root
PTX_BACNETCONFIG=$(PTXCONF_SYSROOT_TARGET)/usr/lib/

$(STATEDIR)/libbacnetconfig.clean:
	@$(call targetinfo)
	@if [ -d $(LIBBACNETCONFIG_DIR) ]; then \
		$(LIBBACNETCONFIG_MAKE_ENV) $(LIBBACNETCONFIG_PATH) $(MAKE) $(MFLAGS) -C $(LIBBACNETCONFIG_DIR) clean; \
	fi
	@$(call clean_pkg, LIBBACNETCONFIG)
	@rm -rf $(LIBBACNETCONFIG_BUILDROOT_DIR)
	
# vim: syntax=make
