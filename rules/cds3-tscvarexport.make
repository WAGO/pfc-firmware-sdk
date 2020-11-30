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
PACKAGES-$(PTXCONF_CDS3_TSCVAREXPORT) += cds3-tscvarexport

#
# Paths and names
#
CDS3_TSCVAREXPORT_VERSION        := 0.6.0
CDS3_TSCVAREXPORT_MD5            :=
CDS3_TSCVAREXPORT                := cds3-tscvarexport
CDS3_TSCVAREXPORT_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/cds3-tscvarexport
CDS3_TSCVAREXPORT_URL            := file://$(CDS3_TSCVAREXPORT_SRC_DIR)
CDS3_TSCVAREXPORT_BUILDCONFIG    := Release
CDS3_TSCVAREXPORT_BUILDROOT_DIR  := $(BUILDDIR)/$(CDS3_TSCVAREXPORT)-$(CDS3_TSCVAREXPORT_VERSION)
CDS3_TSCVAREXPORT_DIR            := $(CDS3_TSCVAREXPORT_BUILDROOT_DIR)/src
CDS3_TSCVAREXPORT_BUILD_DIR      := $(CDS3_TSCVAREXPORT_BUILDROOT_DIR)/bin/$(CDS3_TSCVAREXPORT_BUILDCONFIG)
CDS3_TSCVAREXPORT_LICENSE        := unknown
CDS3_TSCVAREXPORT_BIN            := libTscVarExport.so.$(CDS3_TSCVAREXPORT_VERSION)
CDS3_TSCVAREXPORT_PATH           := PATH=$(CROSS_PATH)
CDS3_TSCVAREXPORT_CONF_TOOL      := NO
CDS3_TSCVAREXPORT_MAKE_ENV       := $(CROSS_ENV) \
PTXCONF_CDS3_RTS_TESTS=$(PTXCONF_CDS3_RTS_TESTS) \
BUILDCONFIG=$(CDS3_TSCVAREXPORT_BUILDCONFIG) \
BIN_DIR=$(CDS3_TSCVAREXPORT_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

CDS3_TSCVAREXPORT_PACKAGE_NAME := $(CDS3_TSCVAREXPORT)_$(CDS3_TSCVAREXPORT_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCVAREXPORT_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscvarexport.extract:
	@$(call targetinfo)
	@mkdir -p $(CDS3_TSCVAREXPORT_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(CDS3_TSCVAREXPORT_DIR) ]; then \
	ln -s $(CDS3_TSCVAREXPORT_SRC_DIR) $(CDS3_TSCVAREXPORT_DIR); \
	fi
endif
	@$(call touch)
# ----------------------------------------------------------------------------
# Extract.post
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/cds3-tscvarexport.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif
# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscvarexport.prepare:
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	@$(call targetinfo)
	@$(call world/prepare, CDS3_TSCVAREXPORT)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscvarexport.compile:
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call targetinfo)
	@$(call world/compile, CDS3_TSCVAREXPORT)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscvarexport.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#   BSP mode: install by extracting tgz file
	@mkdir -p $(CDS3_TSCVAREXPORT_PKGDIR) && \
	tar xvzf $(CDS3_TSCVAREXPORT_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCVAREXPORT_PACKAGE_NAME).tgz -C $(CDS3_TSCVAREXPORT_PKGDIR)
else
	@$(call world/install, CDS3_TSCVAREXPORT)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(CDS3_TSCVAREXPORT_PLATFORMCONFIGPACKAGEDIR)
	@cd $(CDS3_TSCVAREXPORT_PKGDIR) && tar cvzf $(CDS3_TSCVAREXPORT_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCVAREXPORT_PACKAGE_NAME).tgz *
endif
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscvarexport.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, cds3-tscvarexport)
	@$(call install_fixup, cds3-tscvarexport,PRIORITY,optional)
	@$(call install_fixup, cds3-tscvarexport,SECTION,base)
	@$(call install_fixup, cds3-tscvarexport,AUTHOR,"Falk Werner/Jobst Wellensiek")
	@$(call install_fixup, cds3-tscvarexport,DESCRIPTION,missing)

#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish
#
	@$(call install_lib, cds3-tscvarexport, 0, 0, 0644, libTscVarExport)
	@$(call install_link, cds3-tscvarexport, ../$(CDS3_TSCVAREXPORT_BIN), /usr/lib/cds3-custom-components/libTscVarExport.so)

	@$(call install_lib, cds3-tscvarexport, 0, 0, 0644, libvarexport)

	@$(call install_finish, cds3-tscvarexport)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscvarexport.clean:
	@$(call targetinfo)
	@if [ -d $(CDS3_TSCVAREXPORT_DIR) ]; then \
	  $(CDS3_TSCVAREXPORT_MAKE_ENV) $(CDS3_TSCVAREXPORT_PATH) $(MAKE) $(MFLAGS) -C $(CDS3_TSCVAREXPORT_DIR) clean; \
	fi
	@$(call clean_pkg, CDS3_TSCVAREXPORT)
	@rm -rf $(CDS3_TSCVAREXPORT_BUILDROOT_DIR)

# vim: syntax=make
