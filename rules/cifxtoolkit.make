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
# We provide this package
#
PACKAGES-$(PTXCONF_CIFXTOOLKIT) += cifxtoolkit

#
# Paths and names
#
CIFXTOOLKIT_VERSION	:= 1.2.0.0
CIFXTOOLKIT_SHORT_VERSION := 1
CIFXTOOLKIT			:= cifxtoolkit
CIFXTOOLKIT_SRC_DIR	:= $(PTXDIST_WORKSPACE)/wago_intern/device/profibus/master/$(CIFXTOOLKIT)-$(CIFXTOOLKIT_VERSION)
CIFXTOOLKIT_DIR		:= $(BUILDDIR)/$(CIFXTOOLKIT)-$(CIFXTOOLKIT_VERSION)
CIFXTOOLKIT_LICENSE	:= Hilscher Open Source License
CIFXTOOLKIT_ENV     := $(CROSS_ENV) CIFXTOOLKIT_VERSION=$(CIFXTOOLKIT_VERSION)

CIFXTOOLKIT_BIN     := libcifXToolkit.so.$(CIFXTOOLKIT_VERSION)
CIFXTOOLKIT_PACKAGE_NAME := $(CIFXTOOLKIT)_$(CIFXTOOLKIT_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CIFXTOOLKIT_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cifxtoolkit.extract:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@mkdir -p $(CIFXTOOLKIT_DIR)
	@rsync -a --exclude=".*" --exclude=objs/ --exclude=.lib/ --exclude="*.d" --exclude="*.o"  $(CIFXTOOLKIT_SRC_DIR) $(BUILDDIR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

CIFXTOOLKIT_PATH	:= PATH=$(CROSS_PATH)

$(STATEDIR)/cifxtoolkit.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/cifxtoolkit.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	cd $(CIFXTOOLKIT_DIR) && $(CIFXTOOLKIT_ENV) $(MAKE) $(PARALLELMFLAGS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------
CIFX_INCLUDES:= $(CIFXTOOLKIT_SRC_DIR)/Source
CIFX_LIBS:= $(CIFXTOOLKIT_DIR)/lib
SYSROOT_CIFX_INCLUDES:= $(PTXCONF_SYSROOT_TARGET)/usr/include/cifx
SYSROOT_CIFX_LIBS:= $(PTXCONF_SYSROOT_TARGET)/usr/lib

$(STATEDIR)/cifxtoolkit.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	#   BSP mode: install by extracting tgz file
	@mkdir -p $(CIFXTOOLKIT_PKGDIR) && \
	tar xvzf $(CIFXTOOLKIT_PLATFORMCONFIGPACKAGEDIR)/$(CIFXTOOLKIT_PACKAGE_NAME).tgz -C $(CIFXTOOLKIT_PKGDIR)
else	
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	
	cp $(CIFX_LIBS)/* $(SYSROOT_CIFX_LIBS)
	cd $(SYSROOT_CIFX_LIBS) && ln -sf libcifXToolkit.so.$(CIFXTOOLKIT_VERSION) libcifXToolkit.so.$(CIFXTOOLKIT_SHORT_VERSION)
	cd $(SYSROOT_CIFX_LIBS) && ln -sf libcifXToolkit.so.$(CIFXTOOLKIT_VERSION) libcifXToolkit.so

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# save install files to tgz for BSP mode
	mkdir -p $(CIFXTOOLKIT_PLATFORMCONFIGPACKAGEDIR)
	mkdir -p $(CIFXTOOLKIT_PKGDIR)
	mkdir -p $(CIFXTOOLKIT_PKGDIR)/usr
	mkdir -p $(CIFXTOOLKIT_PKGDIR)/usr/lib
	cd $(CIFXTOOLKIT_PKGDIR) && cp $(CIFX_LIBS)/* $(CIFXTOOLKIT_PKGDIR)/usr/lib
	cd $(CIFXTOOLKIT_PKGDIR) && tar cvzf $(CIFXTOOLKIT_PLATFORMCONFIGPACKAGEDIR)/$(CIFXTOOLKIT_PACKAGE_NAME).tgz *
endif
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cifxtoolkit.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cifxtoolkit)
	@$(call install_fixup, cifxtoolkit,PRIORITY,optional)
	@$(call install_fixup, cifxtoolkit,SECTION,base)
	@$(call install_fixup, cifxtoolkit,AUTHOR,"<WAGO GmbH \& Co. KG>")
	@$(call install_fixup, cifxtoolkit,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call install_lib,  cifxtoolkit, 0, 0, 0775, libcifXToolkit)
	@$(call install_link, cifxtoolkit, $(CIFXTOOLKIT_BIN), /usr/lib/libcifXToolkit.so)
	@$(call install_link, cifxtoolkit, $(CIFXTOOLKIT_BIN), /usr/lib/libcifXToolkit.so.$(CIFXTOOLKIT_SHORT_VERSION))
else	
	@$(call install_copy, cifxtoolkit, 0, 0, 0755, $(CIFX_LIBS)/libcifXToolkit.so.$(CIFXTOOLKIT_VERSION), /usr/lib/libcifXToolkit.so.$(CIFXTOOLKIT_VERSION))
	@$(call install_link, cifxtoolkit, libcifXToolkit.so.$(CIFXTOOLKIT_VERSION), /usr/lib/libcifXToolkit.so)
	@$(call install_link, cifxtoolkit, libcifXToolkit.so.$(CIFXTOOLKIT_VERSION), /usr/lib/libcifXToolkit.so.$(CIFXTOOLKIT_SHORT_VERSION))
endif

	@$(call install_finish, cifxtoolkit)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cifxtoolkit.clean:
	rm -rf $(SYSROOT_CIFX_INCLUDES)
	rm -f $(STATEDIR)/cifxtoolkit.*
	rm -rf $(PKGDIR)/cifxtoolkit*
	rm -f $(SYSROOT_CIFX_LIBS)/libcifXToolkit*
	rm -rf $(CIFXTOOLKIT_DIR)

# vim: syntax=make
