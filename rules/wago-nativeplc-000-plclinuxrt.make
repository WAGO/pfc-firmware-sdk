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
PACKAGES-$(PTXCONF_DALSHELL) += dalshell

DALSHELL_VERSION    := 1.0
DALSHELL            := dalshell
DALSHELL_DIR        := $(BUILDDIR)/$(DALSHELL)
DALSHELL_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/plc/$(DALSHELL)
DALSHELL_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/plc/c/$(DALSHELL)

DALSHELL_PACKAGE_NAME := dalshell_$(DALSHELL_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
DALSHELL_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/dalshell.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/dalshell.extract:
	@$(call targetinfo)
	@mkdir -p $(DALSHELL_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	mkdir -p $(DALSHELL_DIR)
	rsync -a --exclude=".*" --exclude=objs/ --exclude="*.d" --exclude="*.o" --exclude="plclinux_rt" $(DALSHELL_SRC_DIR) $(BUILDDIR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/dalshell.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/dalshell.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	cd $(DALSHELL_DIR) && \
		SYSROOT_TARGET=$(PTXDIST_PLATFORMDIR)/sysroot-target CROSS_COMPILE=$(COMPILER_PREFIX) $(MAKE) $(PARALLELMFLAGS) all
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/dalshell.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/dalshell.targetinstall:
	@$(call targetinfo)
	@$(call install_init, dalshell)
	@$(call install_fixup,dalshell,PRIORITY,optional)
	@$(call install_fixup,dalshell,SECTION,base)
	@$(call install_fixup,dalshell,AUTHOR,"WAGO")
	@$(call install_fixup,dalshell,DESCRIPTION,missing)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(DALSHELL_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(DALSHELL_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(DALSHELL_PLATFORMCONFIGPACKAGEDIR)/$(DALSHELL_PACKAGE_NAME).ipk  
	@$(call install_archive, dalshell, 0, 0, $(DALSHELL_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_copy, dalshell, 0, 0, 0755, $(DALSHELL_DIR)/project/output/dalshell, /usr/sbin/dalshell);
	@$(call install_copy, dalshell, 0, 0, 0644, $(DALSHELL_DIR)/project/data/pfc_batch_process.conf.0x0008, /etc/specific/pfc_batch_process.conf.0x0008);
	@$(call install_copy, dalshell, 0, 0, 0644, $(DALSHELL_DIR)/project/data/pfc_batch_process.conf.0x000A, /etc/specific/pfc_batch_process.conf.0x000A);
	@$(call install_copy, dalshell, 0, 0, 0644, $(DALSHELL_DIR)/project/data/pfc_batch_process.conf.0x000C, /etc/specific/pfc_batch_process.conf.0x000C);
	@$(call install_copy, dalshell, 0, 0, 0644, $(DALSHELL_DIR)/project/data/pfc_batch_process.conf.0x000E, /etc/specific/pfc_batch_process.conf.0x000E);
	@$(call install_copy, dalshell, 0, 0, 0644, $(DALSHELL_DIR)/project/data/pfc_batch_process.conf.0x000F, /etc/specific/pfc_batch_process.conf.0x000F);
	@$(call install_copy, dalshell, 0, 0, 0644, $(DALSHELL_DIR)/project/data/pfc_batch_process.conf.0x004E, /etc/specific/pfc_batch_process.conf.0x004E);
	#@$(call install_link, dalshell, ../../etc/specific/pfc_batch_process.conf.default, /usr/share/pfc_batch_process.conf)	
endif
	@$(call install_finish,dalshell)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(DALSHELL_PACKAGE_NAME).ipk $(DALSHELL_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/dalshell.clean:
	rm -rf $(STATEDIR)/dalshell.*
	rm -rf $(PKGDIR)/dalshell*
	rm -rf $(DALSHELL_DIR)
