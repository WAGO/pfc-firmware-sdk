# -*-makefile-*-
#
# Copyright (C) 2016 by Falk Werner/Jobst Wellensiek
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_TSCSECURE) += cds3-tscsecure

#
# Paths and names
#

CDS3_TSCSECURE_VERSION        := 2.1.2
CDS3_TSCSECURE_MD5            :=
CDS3_TSCSECURE                := cds3-tscsecure
CDS3_TSCSECURE_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/cds3-tscsecure
CDS3_TSCSECURE_URL            := file://$(CDS3_TSCSECURE_SRC_DIR)
CDS3_TSCSECURE_BUILDCONFIG    := Release
CDS3_TSCSECURE_BUILDROOT_DIR  := $(BUILDDIR)/$(CDS3_TSCSECURE)-$(CDS3_TSCSECURE_VERSION)
CDS3_TSCSECURE_DIR            := $(CDS3_TSCSECURE_BUILDROOT_DIR)/src
CDS3_TSCSECURE_BUILD_DIR      := $(CDS3_TSCSECURE_BUILDROOT_DIR)/bin/$(CDS3_TSCSECURE_BUILDCONFIG)
CDS3_TSCSECURE_LICENSE        := unknown
CDS3_TSCSECURE_BIN            := libTscSecure.so.$(CDS3_TSCSECURE_VERSION)
CDS3_TSCSECURE_PATH           := PATH=$(CROSS_PATH)
CDS3_TSCSECURE_CONF_TOOL      := NO
CDS3_TSCSECURE_MAKE_ENV       := $(CROSS_ENV) \
PTXCONF_CDS3_RTS_TESTS=$(PTXCONF_CDS3_RTS_TESTS) \
BUILDCONFIG=$(CDS3_TSCSECURE_BUILDCONFIG) \
BIN_DIR=$(CDS3_TSCSECURE_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

CDS3_TSCSECURE_PACKAGE_NAME := $(CDS3_TSCSECURE)_$(CDS3_TSCSECURE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCSECURE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsecure.extract:
	@$(call targetinfo)
	@mkdir -p $(CDS3_TSCSECURE_BUILDROOT_DIR)	
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	@if [ ! -L $(CDS3_TSCSECURE_DIR) ]; then \
		ln -s $(CDS3_TSCSECURE_SRC_DIR) $(CDS3_TSCSECURE_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract.post
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/cds3-tscsecure.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsecure.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	@$(call world/prepare, CDS3_TSCSECURE)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsecure.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, CDS3_TSCSECURE)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsecure.install:
	@$(call targetinfo)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#   BSP mode: install by extracting tgz file
	@mkdir -p $(CDS3_TSCSECURE_PKGDIR) && \
  tar xvzf $(CDS3_TSCSECURE_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCSECURE_PACKAGE_NAME).tgz -C $(CDS3_TSCSECURE_PKGDIR)
else	
# 	normal mode, call "make install"	
	
	@$(call world/install, CDS3_TSCSECURE)
	
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#   # save install directory to tgz for BSP mode
	@mkdir -p $(CDS3_TSCSECURE_PLATFORMCONFIGPACKAGEDIR)
	@cd $(CDS3_TSCSECURE_PKGDIR) && tar cvzf $(CDS3_TSCSECURE_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCSECURE_PACKAGE_NAME).tgz *
endif
endif
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsecure.targetinstall:
	@$(call targetinfo)

	@$(call install_init, cds3-tscsecure)
	@$(call install_fixup, cds3-tscsecure,PRIORITY,optional)
	@$(call install_fixup, cds3-tscsecure,SECTION,base)
	@$(call install_fixup, cds3-tscsecure,AUTHOR,"Jobst Wellensiek")
	@$(call install_fixup, cds3-tscsecure,DESCRIPTION,missing)

#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish
#
	@$(call install_lib, cds3-tscsecure, 0, 0, 0644, libTscSecure)
	@$(call install_link, cds3-tscsecure, ../$(CDS3_TSCSECURE_BIN), /usr/lib/cds3-custom-components/libTscSecure.so)

	@$(call install_finish, cds3-tscsecure)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsecure.clean:
	@$(call targetinfo)
	@if [ -d $(CDS3_TSCSECURE_DIR) ]; then \
	  $(CDS3_TSCSECURE_MAKE_ENV) $(CDS3_TSCSECURE_PATH) $(MAKE) $(MFLAGS) -C $(CDS3_TSCSECURE_DIR) clean; \
	fi
	@$(call clean_pkg, CDS3_TSCSECURE)
	@rm -rf $(CDS3_TSCSECURE_BUILDROOT_DIR)

# vim: syntax=make
