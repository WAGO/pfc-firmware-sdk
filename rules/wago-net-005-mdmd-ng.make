# -*-makefile-*-
#
# Copyright (C) 2019 by Wago Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_MDMD_NG) += mdmd-ng 

#
# Paths and names
#
MDMD_NG_VERSION        := 0.4.0
MDMD_NG_MD5            := 
MDMD_NG                := mdmd-ng
MDMD_NG_URL            := file://$(PTXDIST_WORKSPACE)/wago_intern/mdmd-ng
MDMD_NG_BUILDCONFIG    := Release
MDMD_NG_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/mdmd-ng
MDMD_NG_BUILDROOT_DIR  := $(BUILDDIR)/mdmd-ng
MDMD_NG_DIR            := $(MDMD_NG_BUILDROOT_DIR)/src
MDMD_NG_BUILD_DIR      := $(MDMD_NG_BUILDROOT_DIR)/bin/$(MDMD_NG_BUILDCONFIG)
MDMD_NG_BIN            :=
MDMD_NG_LICENSE        := unknown

MDMD_NG_CONF_TOOL      := NO
MDMD_NG_MAKE_ENV       := $(CROSS_ENV) \
 BUILDCONFIG=$(MDMD_NG_BUILDCONFIG) \
 BIN_DIR=$(MDMD_NG_BUILD_DIR) \
 SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

MDMD_NG_PACKAGE_NAME   := $(MDMD_NG)_$(MDMD_NG_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
MDMD_NG_PLATFORMCONFIGPACKAGEDIR   := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(STATEDIR)/mdmd-ng.get:
#   @$(call targetinfo)
#   @$(call get, MDMD_NG)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------
$(STATEDIR)/mdmd-ng.extract:
	@$(call targetinfo)

	@mkdir -p $(MDMD_NG_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(MDMD_NG_DIR) ]; then \
	  ln -s $(MDMD_NG_SRC_DIR) $(MDMD_NG_DIR);\
	fi
endif  
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract.post
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/mdmd-ng.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------
$(STATEDIR)/mdmd-ng.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/prepare, MDMD_NG)
	@$(call xslt_compile, $(MDMD_NG_DIR)/src/specific_pfc/xml/mdmdng_diag.xml)
	@echo "Create mdmd-ng diagnostic header file in sysroot-target/usr/include/diagnostic directory by using of XSLT-script"
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
$(STATEDIR)/mdmd-ng.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/compile, MDMD_NG)
endif
	@$(call touch)
	

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------
$(STATEDIR)/mdmd-ng.install:
	@$(call targetinfo)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#   BSP mode: install by extracting tgz file
	@mkdir -p $(MDMD_NG_PKGDIR) && \
  	tar xvzf $(MDMD_NG_PLATFORMCONFIGPACKAGEDIR)/$(MDMD_NG_PACKAGE_NAME).tgz -C $(MDMD_NG_PKGDIR)
else
#   "normal" mode: call "make install"    
	@$(call world/install, MDMD_NG)
	
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#   # save install directory contents for BSP
	@mkdir -p $(MDMD_NG_PLATFORMCONFIGPACKAGEDIR)
	@cd $(MDMD_NG_PKGDIR) && tar cvzf $(MDMD_NG_PLATFORMCONFIGPACKAGEDIR)/$(MDMD_NG_PACKAGE_NAME).tgz *
endif
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------
$(STATEDIR)/mdmd-ng.targetinstall:
	@$(call targetinfo)

	@$(call install_init, mdmd-ng)
	@$(call install_fixup, mdmd-ng,PRIORITY,optional)
	@$(call install_fixup, mdmd-ng,SECTION,base)
	@$(call install_fixup, mdmd-ng,AUTHOR,"Team PFC")
	@$(call install_fixup, mdmd-ng,DESCRIPTION,missing)
#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish
#
	@$(call install_lib, mdmd-ng, 0, 0, 0644, libfr)
	@$(call install_lib, mdmd-ng, 0, 0, 0644, libmodem)
	@$(call install_lib, mdmd-ng, 0, 0, 0644, libmdmd-ng)
	@$(call install_copy, mdmd-ng, 0, 0, 0750, /etc/specific/features/)
	@$(call install_copy, mdmd-ng, 0, 0, 0750, -, /usr/bin/mdmd-ng)
	@$(call install_copy, mdmd-ng, 0, 0, 0750, -, /etc/config-tools/config_mdmd-ng)
	@$(call install_copy, mdmd-ng, 0, 0, 0750, -, /etc/config-tools/backup-restore/mdmd-ng_backup_restore)
	@$(call install_copy, mdmd-ng, 0, 0, 0640, -, /etc/mdmd-ng/settings.json)
	@$(call install_copy, mdmd-ng, 0, 0, 0640, -, /etc/mdmd-ng/libmodem.rc)
	@$(call install_copy, mdmd-ng, 0, 0, 0640, -, /etc/mdmd-ng/drvconf/ec25.conf)
	@$(call install_copy, mdmd-ng, 0, 0, 0640, -, /etc/mdmd-ng/drvconf/ec25.mdi)
	@$(call install_copy, mdmd-ng, 0, 0, 0640, -, /etc/mdmd-ng/drvconf/ec25.func)
	@$(call install_copy, mdmd-ng, 0, 0, 0644, -, /usr/lib/mdmd-ng/ec25.md)
	@$(call install_copy, mdmd-ng, 0, 0, 0644, -, /usr/lib/mdmd-ng/qmi.md)
	@$(call install_copy, mdmd-ng, 0, 0, 0750, -, /etc/init.d/mdmd-ng)
	@$(call install_copy, mdmd-ng, 0, 0, 0640, -, /etc/specific/features/wireless-mobile-EC25)
	@$(call install_link, mdmd-ng, /etc/init.d/mdmd-ng, /etc/rc.d/S60_mdmd-ng)
	
	@$(call install_finish, mdmd-ng)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------
$(STATEDIR)/mdmd-ng.clean:
	@$(call targetinfo)
	@if [ -d $(MDMD_NG_DIR) ]; then \
	$(MDMD_NG_MAKE_ENV) $(MDMD_NG_PATH) $(MAKE) $(MFLAGS) -C $(MDMD_NG_DIR) clean; \
	fi
	@$(call clean_pkg, MDMD_NG)
	@rm -rf $(MDMD_NG_BUILDROOT_DIR)

# vim: syntax=make
