## -*-makefile-*-
#
# Copyright (C) 2018 by Wago Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_IODRVPN) += cds3-iodrvpn

CDS3_IODRVPN_VERSION    	:= 1.1.0
CDS3_IODRVPN_MD5		    :=
CDS3_IODRVPN            	:= IoDrvPn
CDS3_IODRVPN_SUFFIX		    :=
CDS3_IODRVPN_URL        	:= file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVPN)
CDS3_IODRVPN_BUILDCONFIG	:= Release
CDS3_IODRVPN_SRC_DIR    	:= $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVPN)
CDS3_IODRVPN_BUILDROOT_DIR	:= $(BUILDDIR)/$(CDS3_IODRVPN)
CDS3_IODRVPN_DIR		    := $(CDS3_IODRVPN_BUILDROOT_DIR)/src
CDS3_IODRVPN_BUILD_DIR		:= $(CDS3_IODRVPN_BUILDROOT_DIR)/bin/$(CDS3_IODRVPN_BUILDCONFIG)
CDS3_IODRVPN_LICENSE		:= unknown
CDS3_IODRVPN_BIN        	:= lib$(CDS3_IODRVPN).so.$(CDS3_IODRVPN_VERSION)

CDS3_IODRVPN_PACKAGE_NAME               := cds3-iodrvpn_$(CDS3_IODRVPN_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_IODRVPN_PLATFORMCONFIGPACKAGEDIR   := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvpn.extract: 
	@$(call targetinfo)
	@mkdir -p $(CDS3_IODRVPN_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(CDS3_IODRVPN_DIR) ]; then \
		ln -s $(CDS3_IODRVPN_SRC_DIR) $(CDS3_IODRVPN_DIR); \
	fi
endif
	@$(call touch)

$(STATEDIR)/cds3-iodrvpn.extract.post:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

CDS3_IODRVPN_CONF_TOOL		:= NO

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

CDS3_IODRVPN_PATH := PATH=$(CROSS_PATH)
CDS3_IODRVPN_MAKE_ENV		:= $(CROSS_ENV) \
BUILDCONFIG=$(CDS3_IODRVPN_BUILDCONFIG) \
BIN_DIR=$(CDS3_IODRVPN_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

$(STATEDIR)/cds3-iodrvpn.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/compile, CDS3_IODRVPN)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvpn.install:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/install, CDS3_IODRVPN)
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvpn.targetinstall:
	@$(call targetinfo)

	@$(call install_init, cds3-iodrvpn)
	@$(call install_fixup, cds3-iodrvpn,PRIORITY,optional)
	@$(call install_fixup, cds3-iodrvpn,SECTION,base)
	@$(call install_fixup, cds3-iodrvpn,AUTHOR,"Steffen Borcherding")
	@$(call install_fixup, cds3-iodrvpn,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	@rm -rf $(CDS3_IODRVPN_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	@cd $(CDS3_IODRVPN_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_IODRVPN_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVPN_PACKAGE_NAME).ipk 
	@$(call install_archive, cds3-iodrvpn, 0, 0, $(CDS3_IODRVPN_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE	
	@$(call install_lib, cds3-iodrvpn, 0, 0, 0644, libIoDrvPn)
	@$(call install_link, cds3-iodrvpn, ../$(CDS3_IODRVPN_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_IODRVPN).so);
endif

	@$(call install_finish, cds3-iodrvpn)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	@cp $(PKGDIR)/$(CDS3_IODRVPN_PACKAGE_NAME).ipk $(CDS3_IODRVPN_PLATFORMCONFIGPACKAGEDIR)/
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvpn.clean:
	@$(call targetinfo)
	@-if [ -d $(CDS3_IODRVPN_DIR) ]; then \
	$(CDS3_IODRVPN_MAKE_ENV) $(CDS3_IODRVPN_PATH) $(MAKE) $(MFLAGS) -C $(CDS3_IODRVPN_DIR) clean; \
	fi
	@$(call clean_pkg, CDS3_IODRVPN)
	@rm -rf $(CDS3_IODRVPN_BUILDROOT_DIR)

# vim: syntax=make
