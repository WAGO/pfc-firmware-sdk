# -*-makefile-*-
#
# Copyright (C) 2012 by <WAGO GmbH & Co. KG>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
#--- provided package --------------------------------------------------------
#
PACKAGES-$(PTXCONF_EXTBUS) += extbus

#
#--- paths and names --------------------------------------------------------- 
#
EXTBUS_VERSION         := 0.0.1
EXTBUS_MD5             := 
EXTBUS                 := extbus
EXTBUS_URL             := file://$(PTXDIST_WORKSPACE)/wago_intern/device/src/$(EXTBUS)
EXTBUS_BUILDCONFIG     := Release
EXTBUS_SRC_DIR         := $(PTXDIST_WORKSPACE)/wago_intern/device/src/$(EXTBUS)
EXTBUS_BUILDROOT_DIR   := $(BUILDDIR)/$(EXTBUS)
EXTBUS_DIR             := $(EXTBUS_BUILDROOT_DIR)/src
EXTBUS_BUILD_DIR       := $(EXTBUS_BUILDROOT_DIR)/bin/$(EXTBUS_BUILDCONFIG)
EXTBUS_LICENSE         := unknown
EXTBUS_BIN             := lib$(EXTBUS).so.$(EXTBUS_VERSION)
EXTBUS_CONF_TOOL       := NO
EXTBUS_MAKE_ENV        := $(CROSS_ENV) \
BUILDCONFIG=$(EXTBUS_BUILDCONFIG) \
BIN_DIR=$(EXTBUS_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

EXTBUS_PATH            := PATH=$(CROSS_PATH)
EXTBUS_PACKAGE_NAME    := $(EXTBUS)_$(EXTBUS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
EXTBUS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/extbus.get:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/extbus.extract:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	mkdir -p $(EXTBUS_BUILDROOT_DIR)
	@if [ ! -L $(EXTBUS_DIR) ]; then \
	  ln -s $(EXTBUS_SRC_DIR) $(EXTBUS_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/extbus.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/extbus.compile:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(EXTBUS_DIR) && \
		$(EXTBUS_MAKE_ENV) $(EXTBUS_PATH) DIST_DIR=$(PTXDIST_PLATFORMDIR) \
		env \
		DIST_DIR=$(PTXDIST_PLATFORMDIR) CROSS_COMPILE=$(COMPILER_PREFIX) \
		$(MAKE)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/extbus.install:
	@$(call targetinfo)
	@$(call world/install, EXTBUS)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#Backup header files as archive for later use in configs/@platform@/packages
	cd $(PTXCONF_SYSROOT_TARGET) && \
	tar -czvf $(EXTBUS_PLATFORMCONFIGPACKAGEDIR)/$(EXTBUS_PACKAGE_NAME).tgz \
		usr/lib/libextbus.so.$(EXTBUS_VERSION)
		usr/include/src/extbus_dal.h && \
	mv $(EXTBUS_PACKAGE_NAME).tgz $(EXTBUS_PLATFORMCONFIGPACKAGEDIR)
endif
endif

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	tar -xzvf $(EXTBUS_PLATFORMCONFIGPACKAGEDIR)/$(EXTBUS_PACKAGE_NAME).tgz -C $(PTXCONF_SYSROOT_TARGET)
endif	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/extbus.targetinstall:
	@$(call targetinfo)
	@$(call install_init, extbus)
	
	@$(call install_fixup, extbus,PRIORITY,optional)
	@$(call install_fixup, extbus,SECTION,base)
	@$(call install_fixup, extbus,AUTHOR,"Markus Schubert <markus.schubert@wago.com>")
	@$(call install_fixup, extbus,DESCRIPTION,"DAL devices for the single room controller")

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# Extract precompiled binaries from archive
	rm -rf $(EXTBUS_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(EXTBUS_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(EXTBUS_PLATFORMCONFIGPACKAGEDIR)/$(EXTBUS_PACKAGE_NAME).ipk
	@$(call install_archive, extbus, 0, 0, $(EXTBUS_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_lib, extbus, 0, 0, 0644, libextbus)
	@$(call install_link, extbus, ../libextbus.so.$(EXTBUS_VERSION), /usr/lib/dal/libextbus.so)
endif
	@$(call install_finish, extbus)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(EXTBUS_PACKAGE_NAME).ipk $(EXTBUS_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

PTX_ROOT=$(PTXCONF_SYSROOT_TARGET)/../root
PTX_ROOT_DEBUG=$(PTXCONF_SYSROOT_TARGET)/../root-debug
PTX_DAL=$(PTXCONF_SYSROOT_TARGET)/usr/lib/dal

$(STATEDIR)/extbus.clean:
	@$(call targetinfo)
	@echo Deleting state files...
	@rm -rf $(STATEDIR)/extbus.*
	@echo Deleting extbus dir: $(EXTBUS_DIR)
	@rm -rf $(EXTBUS_DIR)
	@echo Deleting installed files.....
	@rm -f $(PTX_ROOT)/usr/lib/libextbus*	
	@rm -f $(PTX_ROOT_DEBUG)/usr/lib/libextbus*
	@rm -f $(PTX_DAL)/libextbus*
	@echo Deleting package: $(PKGDIR)/$(EXTBUS)*
	@rm -rf $(PKGDIR)/$(EXTBUS)*
	@$(call clean_pkg, EXTBUS)

