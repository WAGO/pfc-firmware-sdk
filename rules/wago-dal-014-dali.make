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
PACKAGES-$(PTXCONF_DALI) += dali

#
#--- paths and names --------------------------------------------------------- 
#
DALI_VERSION           := 0.0.1
DALI_MD5               := 
DALI                   := dali
DALI_URL               := file://$(PTXDIST_WORKSPACE)/wago_intern/device/src/dali/libdali
DALI_BUILDCONFIG       := Release
DALI_SRC_DIR           := $(PTXDIST_WORKSPACE)/wago_intern/device/src/$(DALI)/libdali
DALI_BUILDROOT_DIR     := $(BUILDDIR)/$(DALI)
DALI_DIR               := $(DALI_BUILDROOT_DIR)/src
DALI_BUILD_DIR         := $(DALI_BUILDROOT_DIR)/bin/$(DALI_BUILDCONFIG)
DALI_LICENSE           := unknown
DALI_BIN               := lib$(DALI).so.$(DALI_VERSION)
DALI_CONF_TOOL         := NO
DALI_MAKE_ENV          := $(CROSS_ENV) \
BUILDCONFIG=$(DALI_BUILDCONFIG) \
BIN_DIR=$(DALI_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

DALI_PATH              := PATH=$(CROSS_PATH)
DALI_PACKAGE_NAME      := $(DALI)_$(DALI_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
DALI_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------
#$(STATEDIR)/dali.get:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/dali.extract:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	mkdir -p $(DALI_BUILDROOT_DIR)
	@if [ ! -L $(DALI_DIR) ]; then \
	  ln -s $(DALI_SRC_DIR) $(DALI_DIR); \
	fi
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------
$(STATEDIR)/dali.prepare:
	@$(call targetinfo)
	@$(call touch)DALI_PACKAGE_NAME


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/dali.compile:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(DALI_DIR) && \
		$(DALI_MAKE_ENV) $(DALI_PATH) DIST_DIR=$(PTXDIST_PLATFORMDIR) \
		env \
		DIST_DIR=$(PTXDIST_PLATFORMDIR) CROSS_COMPILE=$(COMPILER_PREFIX) \
		$(MAKE)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/dali.install:
	@$(call targetinfo)
	@$(call world/install, DALI)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#Backup header files as archive for later use in configs/@platform@/packages
	cd $(PTXCONF_SYSROOT_TARGET) && \
	tar -czvf $(DALI_PLATFORMCONFIGPACKAGEDIR)/$(DALI_PACKAGE_NAME).tgz \
		usr/lib/libdali.so.$(DALI_VERSION) \
		usr/include/src/dali_dal.h && \
	mv $(DALI_PACKAGE_NAME).tgz $(DALI_PLATFORMCONFIGPACKAGEDIR)
endif
endif

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	tar -xzvf $(DALI_PLATFORMCONFIGPACKAGEDIR)/$(DALI_PACKAGE_NAME).tgz -C $(PTXCONF_SYSROOT_TARGET)
endif	
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/dali.targetinstall:
	@$(call targetinfo)
	@$(call install_init, dali)

	@$(call install_fixup, dali,PRIORITY,optional)
	@$(call install_fixup, dali,SECTION,base)
	@$(call install_fixup, dali,AUTHOR,"Rolf Schulenburg <rolf.schulenburg@wago.com>")
	@$(call install_fixup, dali,DESCRIPTION,"DAL devices for the single room controller" )

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# Extract precompiled binaries from archive
	rm -rf $(DALI_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(DALI_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(DALI_PLATFORMCONFIGPACKAGEDIR)/$(DALI_PACKAGE_NAME).ipk
	@$(call install_archive, dali, 0, 0, $(DALI_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_lib, dali, 0, 0, 0644, libdali)
	@$(call install_link, dali, ../libdali.so.$(DALI_VERSION), /usr/lib/dal/libdali.so)
endif
	@$(call install_finish, dali)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(DALI_PACKAGE_NAME).ipk $(DALI_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

PTX_ROOT=$(PTXCONF_SYSROOT_TARGET)/../root
PTX_ROOT_DEBUG=$(PTXCONF_SYSROOT_TARGET)/../root-debug
PTX_DAL=$(PTXCONF_SYSROOT_TARGET)/usr/lib/dal

$(STATEDIR)/dali.clean:
	@$(call targetinfo)
	@if [ -d $(DALI_DIR) ]; then \
		$(DALI_MAKE_ENV) $(DALI_PATH) $(MAKE) $(MFLAGS) -C $(DALI_DIR) clean; \
	fi
	@$(call clean_pkg, DALI)
	@rm -rf $(DALI_BUILDROOT_DIR)

# vim: syntax=make

