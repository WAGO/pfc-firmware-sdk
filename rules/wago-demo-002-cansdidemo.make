# -*-makefile-*-
# $Id$
#
# Copyright (C) 2008 by 
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CANSDIDEMO) +=cansdidemo

#
# Paths and names
#
CANSDIDEMO_VERSION	:= 0.3
CANSDIDEMO		:= cansdidemo
CANSDIDEMO_URL		:= file://$(SRCDIR)/$(CANSDIDEMO)
CANSDIDEMO_DIR		:= $(BUILDDIR)/$(CANSDIDEMO)

CDUP = ..

CANSDIDEMO_PACKAGE_NAME := $(CANSDIDEMO)_$(CANSDIDEMO_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CANSDIDEMO_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cansdidemo.get:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cansdidemo.extract:
	@$(call targetinfo, $@)
	@mkdir -p $(CANSDIDEMO_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call clean, $(CANSDIDEMO_DIR))
	@cp -rd $(SRCDIR)/$(CANSDIDEMO) $(CANSDIDEMO_DIR)
endif
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

CANSDIDEMO_PATH	:= PATH=$(CROSS_PATH)
CANSDIDEMO_ENV 	:= $(CROSS_ENV)

$(STATEDIR)/cansdidemo.prepare:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/cansdidemo.compile:
	@$(call targetinfo, $@)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@cd $(CANSDIDEMO_DIR)/master && \
		$(CANSDIDEMO_ENV) $(CANSDIDEMO_PATH) DIST_DIR=$(PTXDIST_PLATFORMDIR) \
		env \
		DIST_DIR=$(PTXDIST_PLATFORMDIR) CROSS_COMPILE=$(COMPILER_PREFIX) \
		$(MAKE)			
	@cd $(CANSDIDEMO_DIR)/slave && \
		$(CANSDIDEMO_ENV) $(CANSDIDEMO_PATH) DIST_DIR=$(PTXDIST_PLATFORMDIR) \
		env \
		DIST_DIR=$(PTXDIST_PLATFORMDIR) CROSS_COMPILE=$(COMPILER_PREFIX) \
		$(MAKE)			
	@cd $(CANSDIDEMO_DIR)/layer2 && \
		$(CANSDIDEMO_ENV) $(CANSDIDEMO_PATH) DIST_DIR=$(PTXDIST_PLATFORMDIR) \
		env \
		DIST_DIR=$(PTXDIST_PLATFORMDIR) CROSS_COMPILE=$(COMPILER_PREFIX) \
		$(MAKE)

endif        

	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cansdidemo.install:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cansdidemo.targetinstall:
	@$(call targetinfo, $@)

	@$(call install_init, cansdidemo)

	@$(call install_fixup,cansdidemo,PRIORITY,optional)
	@$(call install_fixup,cansdidemo,VERSION,$(CANSDIDEMO_VERSION))	
	@$(call install_fixup,cansdidemo,SECTION,base)
	@$(call install_fixup,cansdidemo,AUTHOR,"Axel Wueste")
	@$(call install_fixup,cansdidemo,DESCRIPTION,missing)
	
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
    # Extract precompiled binaries from archive

	rm -rf $(CANSDIDEMO_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(CANSDIDEMO_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CANSDIDEMO_PLATFORMCONFIGPACKAGEDIR)/$(CANSDIDEMO_PACKAGE_NAME).ipk  
	@$(call install_archive, cansdidemo, 0, 0, $(CANSDIDEMO_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
    # WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish
#
	@$(call install_copy, cansdidemo, 0, 0, 0755, $(CANSDIDEMO_DIR)/master/candemo_sdi_master, /usr/sbin/candemo_sdi_master)
	@$(call install_copy, cansdidemo, 0, 0, 0755, $(CANSDIDEMO_DIR)/slave/candemo_sdi_slave, /usr/sbin/candemo_sdi_slave)
	@$(call install_copy, cansdidemo, 0, 0, 0755, $(CANSDIDEMO_DIR)/layer2/candemo_sdi_layer2, /usr/sbin/candemo_sdi_layer2)

endif

	@$(call install_finish,cansdidemo)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CANSDIDEMO_PACKAGE_NAME).ipk $(CANSDIDEMO_PLATFORMCONFIGPACKAGEDIR)/
endif

	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

cansdidemo_clean:
	rm -rf $(STATEDIR)/cansdidemo.*
	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/sbin/candemo_sdi*.*.*
	rm -rf $(CANSDIDEMO_DIR)


	
# vim: syntax=make
