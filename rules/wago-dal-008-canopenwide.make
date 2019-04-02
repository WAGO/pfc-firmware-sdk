# -*-makefile-*-
# $Id: template-make 8509 2008-06-12 12:45:40Z mkl $
#
# Copyright (C) 2012 by WAGO
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CANOPENWIDE) += canopenwide

#
# Paths and names
#
CANOPENWIDE_VERSION	:= 0.2
CANOPENWIDE		:= canopenwide
CANOPENWIDE_URL		:= file://$(PTXDIST_WORKSPACE)/wago_intern/wide/libwidecanopen
CANOPENWIDE_DIR		:= $(BUILDDIR)/$(CANOPENWIDE)

CANOPENWIDE_PACKAGE_NAME := $(CANOPENWIDE)_$(CANOPENWIDE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CANOPENWIDE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/canopenwide.get:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/canopenwide.extract:
	@$(call targetinfo, $@)
	@$(call clean, $(CANOPENWIDE_DIR))
	@mkdir -p $(CANOPENWIDE_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	cp -rd $(PTXDIST_WORKSPACE)/wago_intern/wide/libwidecanopen/* $(CANOPENWIDE_DIR)
endif
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

CANOPENWIDE_PATH	:= PATH=$(CROSS_PATH)
CANOPENWIDE_ENV 	:= $(CROSS_ENV)

$(STATEDIR)/canopenwide.prepare:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/canopenwide.compile:
	@$(call targetinfo, $@)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CANOPENWIDE_DIR) && \
		$(CANOPENWIDE_ENV) $(CANOPENWIDE_PATH) DIST_DIR=$(PTXDIST_PLATFORMDIR) \
		env \
		DIST_DIR=$(PTXDIST_PLATFORMDIR) CROSS_COMPILE=$(COMPILER_PREFIX) \
		$(MAKE)
	@cd $(CANOPENWIDE_DIR)/master && \
		$(CANOPENWIDE_ENV) $(CANOPENWIDE_PATH) DIST_DIR=$(PTXDIST_PLATFORMDIR) \
		env \
		DIST_DIR=$(PTXDIST_PLATFORMDIR) CROSS_COMPILE=$(COMPILER_PREFIX) \
		$(MAKE)
	@cd $(CANOPENWIDE_DIR)/slave && \
		$(CANOPENWIDE_ENV) $(CANOPENWIDE_PATH) DIST_DIR=$(PTXDIST_PLATFORMDIR) \
		env \
		DIST_DIR=$(PTXDIST_PLATFORMDIR) CROSS_COMPILE=$(COMPILER_PREFIX) \
		$(MAKE)
	@cd $(CANOPENWIDE_DIR)/layer2 && \
		$(CANOPENWIDE_ENV) $(CANOPENWIDE_PATH) DIST_DIR=$(PTXDIST_PLATFORMDIR) \
		env \
		DIST_DIR=$(PTXDIST_PLATFORMDIR) CROSS_COMPILE=$(COMPILER_PREFIX) \
		$(MAKE)	
endif
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/canopenwide.install:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/canopenwide.targetinstall:
	@$(call targetinfo, $@)
	@$(call install_init, canopenwide)
	@$(call install_fixup,canopenwide,PRIORITY,optional)
	@$(call install_fixup,canopenwide,VERSION,$(CANOPEN_VERSION))	
	@$(call install_fixup,canopenwide,SECTION,base)
	@$(call install_fixup,canopenwide,AUTHOR,"Axel Wueste")
	@$(call install_fixup,canopenwide,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(CANOPENWIDE_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(CANOPENWIDE_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CANOPENWIDE_PLATFORMCONFIGPACKAGEDIR)/$(CANOPENWIDE_PACKAGE_NAME).ipk  
	@$(call install_archive, canopenwide, 0, 0, $(CANOPENWIDE_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_copy,  canopenwide, 0, 0, 0644, $(CANOPENWIDE_DIR)/libwidecanopen.so, /usr/lib/libwidecanopen.so)
	@$(call install_copy,  canopenwide, 0, 0, 0644, $(CANOPENWIDE_DIR)/layer2/libwidecanlayer2.so, /usr/lib/wide/libwidecanlayer2.so)
	@$(call install_copy,  canopenwide, 0, 0, 0644, $(CANOPENWIDE_DIR)/master/libwidecanopenm.so, /usr/lib/wide/libwidecanopenm.so)
	@$(call install_copy,  canopenwide, 0, 0, 0644, $(CANOPENWIDE_DIR)/slave/libwidecanopens.so, /usr/lib/wide/libwidecanopens.so)	 
endif
	@$(call install_finish,canopenwide)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CANOPENWIDE_PACKAGE_NAME).ipk $(CANOPENWIDE_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

canopenwide_clean:
	rm -rf $(STATEDIR)/canopenwide.*
	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/libwidecanopen.*
	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/wide/libwidecanopenm.*
	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/wide/libwidecanlayer2.*
	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/wide/libwidecanopens.*
	rm -rf $(PKGDIR)/canopenwide*
	rm -rf $(CANOPENWIDE_DIR)

