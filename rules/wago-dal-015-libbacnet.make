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
PACKAGES-$(PTXCONF_LIBBACNET) += libbacnet
#
#--- paths and names --------------------------------------------------------- 
#
LIBBACNET_VERSION           := 0.0.1
LIBBACNET_MD5               :=
LIBBACNET                   := libbacnet
LIBBACNET_URL               := file://$(PTXDIST_WORKSPACE)/wago_intern/device/bacnet/$(LIBBACNET)
LIBBACNET_BUILDCONFIG       := Release
LIBBACNET_SRC_DIR           := $(PTXDIST_WORKSPACE)/wago_intern/device/bacnet/$(LIBBACNET)
LIBBACNET_BUILDROOT_DIR     := $(BUILDDIR)/$(LIBBACNET)
LIBBACNET_DIR               := $(LIBBACNET_BUILDROOT_DIR)/$(LIBBACNET)
LIBBACNET_BUILD_DIR         := $(LIBBACNET_BUILDROOT_DIR)/bin/$(LIBBACNET_BUILDCONFIG)
LIBBACNET_LICENSE           := unknown
LIBBACNET_BIN               := $(LIBBACNET).so.$(LIBBACNET_VERSION)
LIBBACNET_SO_NAME           := $(LIBBACNET).so
LIBBACNET_CONF_TOOL         := NO
LIBBACNET_MAKE_ENV          := $(CROSS_ENV) \
BUILDCONFIG=$(LIBBACNETSTACK_BUILDCONFIG) \
              BIN_DIR=$(LIBBACNETSTACK_BUILD_DIR) \
              TARGET_ARCH=$(PTXCONF_ARCH_STRING) \
              ARM_ARCH_VERSION=7 \
              SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

LIBBACNET_PATH              := PATH=$(CROSS_PATH)
LIBBACNET_PACKAGE_NAME      := $(LIBBACNET)_$(LIBBACNET_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBBACNET_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libbacnet.extract:
	@$(call targetinfo)
	@mkdir -p $(LIBBACNET_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(LIBBACNET_DIR) ]; then \
	  	ln -s $(LIBBACNET_SRC_DIR) $(LIBBACNET_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract.post
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/libbacnet.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------
$(STATEDIR)/libbacnet.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, LIBBACNET)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

ifdef PTXCONF_LIBBACNETSTACK_SOURCE_LEGACY
LIBBACNET_MAKE_OPT += "LIBBACNETSTACK_REVISION=14"
else
LIBBACNET_MAKE_OPT += "LIBBACNETSTACK_REVISION=22"
endif

$(STATEDIR)/libbacnet.compile:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, LIBBACNET)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libbacnet.install:
	@$(call targetinfo)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#   BSP mode: install by extracting tgz file
	@mkdir -p $(LIBBACNET_PKGDIR) && \
  	tar xvzf $(LIBBACNET_PLATFORMCONFIGPACKAGEDIR)/$(LIBBACNET_PACKAGE_NAME).tgz -C $(LIBBACNET_PKGDIR)
else	
# 	normal mode, call "make install"	
	
	@$(call world/install, LIBBACNET)
	
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#   # save install directory to tgz for BSP mode
	@mkdir -p $(LIBBACNET_PLATFORMCONFIGPACKAGEDIR)
	@cd $(LIBBACNET_PKGDIR) && tar cvzf $(LIBBACNET_PLATFORMCONFIGPACKAGEDIR)/$(LIBBACNET_PACKAGE_NAME).tgz *
endif
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libbacnet.targetinstall:
	@$(call targetinfo)
	@$(call install_init, libbacnet)

	@$(call install_fixup, libbacnet,PRIORITY,optional)
	@$(call install_fixup, libbacnet,SECTION,base)
	@$(call install_fixup, libbacnet,AUTHOR,"Rolf Schulenburg <rolf.schulenburg@wago.com>")
	@$(call install_fixup, libbacnet,DESCRIPTION,"BACnet device for the PFC controller" )
	
	@$(call install_lib, libbacnet, 0, 0, 0644, libbacnet)
	@$(call install_link, libbacnet, ../$(LIBBACNET_BIN), /usr/lib/dal/$(LIBBACNET_SO_NAME))
	
	@$(call install_finish, libbacnet)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

PTX_ROOT=$(PTXCONF_SYSROOT_TARGET)/../root
PTX_DAL=$(PTXCONF_SYSROOT_TARGET)/usr/lib/dal

$(STATEDIR)/libbacnet.clean:
	@$(call targetinfo)
	@if [ -d $(LIBBACNET_DIR) ]; then \
		$(LIBBACNET_MAKE_ENV) $(LIBBACNET_PATH) $(MAKE) $(MFLAGS) -C $(LIBBACNET_DIR) clean; \
	fi
	@$(call clean_pkg, LIBBACNET)
	@rm -rf $(LIBBACNET_BUILDROOT_DIR)

# vim: syntax=make

