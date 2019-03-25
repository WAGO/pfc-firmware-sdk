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
PACKAGES-$(PTXCONF_ONBOARDIO) += onboardio

#
#--- paths and names --------------------------------------------------------- 
#
ONBOARDIO_VERSION           := 0.0.1
ONBOARDIO_MD5               := 
ONBOARDIO                   := onboardio
ONBOARDIO_URL               := file://$(PTXDIST_WORKSPACE)/wago_intern/device/src/onboardio
ONBOARDIO_BUILDCONFIG       := Release
ONBOARDIO_SRC_DIR           := $(PTXDIST_WORKSPACE)/wago_intern/device/src/$(ONBOARDIO)
ONBOARDIO_BUILDROOT_DIR     := $(BUILDDIR)/$(ONBOARDIO)
ONBOARDIO_DIR               := $(ONBOARDIO_BUILDROOT_DIR)/src
ONBOARDIO_BUILD_DIR         := $(ONBOARDIO_BUILDROOT_DIR)/bin/$(ONBOARDIO_BUILDCONFIG)
ONBOARDIO_LICENSE           := unknown
ONBOARDIO_BIN               := lib$(ONBOARDIO).so.$(ONBOARDIO_VERSION)
ONBOARDIO_CONF_TOOL         := NO
ONBOARDIO_MAKE_ENV          := $(CROSS_ENV) \
BUILDCONFIG=$(ONBOARDIO_BUILDCONFIG) \
BIN_DIR=$(ONBOARDIO_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

ONBOARDIO_PATH              := PATH=$(CROSS_PATH)
ONBOARDIO_PACKAGE_NAME      := $(ONBOARDIO)_$(ONBOARDIO_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
ONBOARDIO_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(STATEDIR)/onboardio.get:
#	@$(call targetinfo)
#	@$(call touch)


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/onboardio.extract:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	mkdir -p $(ONBOARDIO_BUILDROOT_DIR)
	@if [ ! -L $(ONBOARDIO_DIR) ]; then \
	  ln -s $(ONBOARDIO_SRC_DIR) $(ONBOARDIO_DIR); \
	fi
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/onboardio.prepare:
	@$(call targetinfo)
	@$(call touch)ONBOARDIO_PACKAGE_NAME


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/onboardio.compile:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(ONBOARDIO_DIR) && \
		$(ONBOARDIO_MAKE_ENV) $(ONBOARDIO_PATH) DIST_DIR=$(PTXDIST_PLATFORMDIR) \
		env \
		DIST_DIR=$(PTXDIST_PLATFORMDIR) CROSS_COMPILE=$(COMPILER_PREFIX) \
		$(MAKE)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/onboardio.install:
	@$(call targetinfo)
	@$(call world/install, ONBOARDIO)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#Backup header files as archive for later use in configs/@platform@/packages
	cd $(PTXCONF_SYSROOT_TARGET) && \
	tar -czvf $(ONBOARDIO_PLATFORMCONFIGPACKAGEDIR)/$(ONBOARDIO_PACKAGE_NAME).tgz \
		usr/lib/libonboardio.so.$(ONBOARDIO_VERSION) \
		usr/include/src/obi_dal.h && \
	mv $(ONBOARDIO_PACKAGE_NAME).tgz $(ONBOARDIO_PLATFORMCONFIGPACKAGEDIR)
endif
endif

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	tar -xzvf $(ONBOARDIO_PLATFORMCONFIGPACKAGEDIR)/$(ONBOARDIO_PACKAGE_NAME).tgz -C $(PTXCONF_SYSROOT_TARGET)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/onboardio.targetinstall:
	@$(call targetinfo)
	@$(call install_init, onboardio)

	@$(call install_fixup, onboardio,PRIORITY,optional)
	@$(call install_fixup, onboardio,SECTION,base)
	@$(call install_fixup, onboardio,AUTHOR,"Rolf Schulenburg <rolf.schulenburg@wago.com>")
	@$(call install_fixup, onboardio,DESCRIPTION,"DAL devices for the single room controller" )
	
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# Extract precompiled binaries from archive
	rm -rf $(ONBOARDIO_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(ONBOARDIO_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(ONBOARDIO_PLATFORMCONFIGPACKAGEDIR)/$(ONBOARDIO_PACKAGE_NAME).ipk
	@$(call install_archive, onboardio, 0, 0, $(ONBOARDIO_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_lib, onboardio, 0, 0, 0644, libonboardio)
	@$(call install_link, onboardio, ../libonboardio.so.$(ONBOARDIO_VERSION), /usr/lib/dal/libonboardio.so)
endif
	@$(call install_finish, onboardio)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(ONBOARDIO_PACKAGE_NAME).ipk $(ONBOARDIO_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

PTX_ROOT=$(PTXCONF_SYSROOT_TARGET)/../root
PTX_ROOT_DEBUG=$(PTXCONF_SYSROOT_TARGET)/../root-debug
PTX_DAL=$(PTXCONF_SYSROOT_TARGET)/usr/lib/dal

$(STATEDIR)/onboardio.clean:
	@$(call targetinfo)
	@if [ -d $(ONBOARDIO_DIR) ]; then \
		$(ONBOARDIO_MAKE_ENV) $(ONBOARDIO_PATH) $(MAKE) $(MFLAGS) -C $(ONBOARDIO_DIR) clean; \
	fi
	@$(call clean_pkg, ONBOARDIO)
	@rm -rf $(ONBOARDIO_BUILDROOT_DIR)

# vim: syntax=make

