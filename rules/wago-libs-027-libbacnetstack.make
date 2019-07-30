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
PACKAGES-$(PTXCONF_LIBBACNETSTACK) += libbacnetstack

#
#--- paths and names --------------------------------------------------------- 
#
LIBBACNETSTACK_VERSION           := 0.0.1
LIBBACNETSTACK_MD5               :=
LIBBACNETSTACK                   := libbacnetstack
LIBBACNETSTACK_SRC_DIR           := $(PTXDIST_WORKSPACE)/wago_intern/device/bacnet/$(LIBBACNETSTACK)
LIBBACNETSTACK_URL               := file://$(LIBBACNETSTACK_SRC_DIR)
LIBBACNETSTACK_BUILDCONFIG       := Release
LIBBACNETSTACK_BUILDROOT_DIR     := $(BUILDDIR)/$(LIBBACNETSTACK)
LIBBACNETSTACK_DIR               := $(LIBBACNETSTACK_BUILDROOT_DIR)/$(LIBBACNETSTACK)
LIBBACNETSTACK_BUILD_DIR         := $(LIBBACNETSTACK_BUILDROOT_DIR)/bin/$(LIBBACNETSTACK_BUILDCONFIG)
LIBBACNETSTACK_LICENSE           := unknown
LIBBACNETSTACK_BIN               := $(LIBBACNETSTACK).a
LIBBACNETSTACK_CONF_TOOL         := NO
LIBBACNETSTACK_MAKE_ENV          := $(CROSS_ENV) \
              BUILDCONFIG=$(LIBBACNETSTACK_BUILDCONFIG) \
              BIN_DIR=$(LIBBACNETSTACK_BUILD_DIR) \
              TARGET_ARCH=$(PTXCONF_ARCH_STRING) \
              ARM_ARCH_VERSION=7 \
              SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

LIBBACNETSTACK_PATH              := PATH=$(CROSS_PATH)
LIBBACNETSTACK_PACKAGE_NAME      := $(LIBBACNETSTACK)_$(LIBBACNETSTACK_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBBACNETSTACK_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libbacnetstack.extract:
	@$(call targetinfo)
	@mkdir -p $(LIBBACNETSTACK_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(LIBBACNETSTACK_DIR) ]; then \
		ln -s $(LIBBACNETSTACK_SRC_DIR) $(LIBBACNETSTACK_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract.post
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/libbacnetstack.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libbacnetstack.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, LIBBACNETSTACK)
	@$(call xslt_compile, $(LIBBACNETSTACK_DIR)/config/bacnetdiag.xml)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/libbacnetstack.compile:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, LIBBACNETSTACK)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libbacnetstack.install:
	@$(call targetinfo)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#   BSP mode: install by extracting tgz file
	@mkdir -p $(LIBBACNETSTACK_PKGDIR) && \
  	tar xvzf $(LIBBACNETSTACK_PLATFORMCONFIGPACKAGEDIR)/$(LIBBACNETSTACK_PACKAGE_NAME).tgz -C $(LIBBACNETSTACK_PKGDIR)
else	
# 	normal mode, call "make install"	
	
	@$(call world/install, LIBBACNETSTACK)
	
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#   # save install directory to tgz for BSP mode
	@mkdir -p $(LIBBACNETSTACK_PLATFORMCONFIGPACKAGEDIR)
	@cd $(LIBBACNETSTACK_PKGDIR) && tar cvzf $(LIBBACNETSTACK_PLATFORMCONFIGPACKAGEDIR)/$(LIBBACNETSTACK_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libbacnetstack.targetinstall:
	@$(call targetinfo)
	@$(call install_init, libbacnetstack)	

	@$(call install_fixup, libbacnetstack,PRIORITY,optional)
	@$(call install_fixup, libbacnetstack,SECTION,base)
	@$(call install_fixup, libbacnetstack,AUTHOR,"Andreas Hiebert <andreas.hiebert@wago.com>")
	@$(call install_fixup, libbacnetstack,DESCRIPTION,"BACnet stack for the PFC controller" )

	@$(call install_copy, libbacnetstack, 0, 0, 0755, /home/user/bacnet)

	@$(call install_finish, libbacnetstack)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

PTX_ROOT=$(PTXCONF_SYSROOT_TARGET)/../root
PTX_DAL=$(PTXCONF_SYSROOT_TARGET)/usr/lib/dal

$(STATEDIR)/libbacnetstack.clean:
	@$(call targetinfo)
	@if [ -d $(LIBBACNETSTACK_DIR) ]; then \
	  $(LIBBACNETSTACK_MAKE_ENV) $(LIBBACNETSTACK_PATH) $(MAKE) $(MFLAGS) -C $(LIBBACNETSTACK_DIR) clean; \
	fi
	@$(call clean_pkg, LIBBACNETSTACK)
	@rm -rf $(LIBBACNETSTACK_BUILDROOT_DIR)
	@echo Deleting default bacnet folder:	
	@echo $(PTXDIST_PLATFORMDIR)/root/home/user/bacnet
	@rm -rf $(PTXDIST_PLATFORMDIR)/root/home/user/bacnet
	@echo $(PTXCONF_SYSROOT_TARGET)/home/user/bacnet
	@rm -rf $(PTXCONF_SYSROOT_TARGET)/home/user/bacnet
	
# vim: syntax=make

