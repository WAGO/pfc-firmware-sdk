# -*-makefile-*-
# $Id$
#
# Copyright (C) 2013 by WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_TSCSYSPN) += cds3-tscsyspn

CDS3_TSCSYSPN_VERSION      := 0.0.1
CDS3_TSCSYSPN_MD5          :=
CDS3_TSCSYSPN              := TscSysPn
CDS3_TSCSYSPN_SUFFIX       :=
CDS3_TSCSYSPN_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCSYSPN)
CDS3_TSCSYSPN_BUILDCONFIG  := Release
CDS3_TSCSYSPN_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCSYSPN)
CDS3_TSCSYSPN_BUILDROOT_DIR  := $(BUILDDIR)/$(CDS3_TSCSYSPN)
CDS3_TSCSYSPN_DIR          := $(CDS3_TSCSYSPN_BUILDROOT_DIR)/src
CDS3_TSCSYSPN_BUILD_DIR    := $(CDS3_TSCSYSPN_BUILDROOT_DIR)/bin/$(CDS3_TSCSYSPN_BUILDCONFIG)
CDS3_TSCSYSPN_LICENSE    := unknown
CDS3_TSCSYSPN_BIN          := lib$(CDS3_TSCSYSPN).so.$(CDS3_TSCSYSPN_VERSION)

CDS3_TSCSYSPN_PACKAGE_NAME := cds3-tscsyspn_$(CDS3_TSCSYSPN_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCSYSPN_PLATFORMCONFIGPACKAGEDIR   := $(PTXDIST_PLATFORMCONFIGDIR)/packages



# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsyspn.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCSYSPN_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(CDS3_TSCSYSPN_DIR) ]; then \
	ln -s $(CDS3_TSCSYSPN_SRC_DIR) $(CDS3_TSCSYSPN_DIR); \
	fi
endif
	@$(call touch)

$(STATEDIR)/cds3-tscsyspn.extract.post:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

CDS3_TSCSYSPN_CONF_TOOL    := NO



# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCSYSPN_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCSYSPN_MAKE_ENV  := $(CROSS_ENV) \
BUILDCONFIG=$(CDS3_TSCSYSPN_BUILDCONFIG) \
BIN_DIR=$(CDS3_TSCSYSPN_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

$(STATEDIR)/cds3-tscsyspn.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
  # WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	$(call world/compile, CDS3_TSCSYSPN)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsyspn.install:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/install, CDS3_TSCSYSPN)
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsyspn.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, cds3-tscsyspn)
	@$(call install_fixup, cds3-tscsyspn,PRIORITY,optional)
	@$(call install_fixup, cds3-tscsyspn,SECTION,base)
	@$(call install_fixup, cds3-tscsyspn,AUTHOR,"Steffen Borcherding")
	@$(call install_fixup, cds3-tscsyspn,DESCRIPTION,missing)
  
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCSYSPN_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCSYSPN_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCSYSPN_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCSYSPN_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscsyspn, 0, 0, $(CDS3_TSCSYSPN_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_lib, cds3-tscsyspn, 0, 0, 0644, libTscSysPn)
	@$(call install_link, cds3-tscsyspn, ../$(CDS3_TSCSYSPN_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCSYSPN).so);


endif

	@$(call install_finish,cds3-tscsyspn)
	
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCSYSPN_PACKAGE_NAME).ipk $(CDS3_TSCSYSPN_PLATFORMCONFIGPACKAGEDIR)/
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsyspn.clean:
	@$(call targetinfo)
	@-if [ -d $(CDS3_TSCSYSPN_DIR) ]; then \
	$(CDS3_TSCSYSPN_MAKE_ENV) $(CDS3_TSCSYSPN_PATH) $(MAKE) $(MFLAGS) -C $(CDS3_TSCSYSPN_DIR) clean; \
	fi
	@$(call clean_pkg, CDS3_TSCSYSPN)
	@rm -rf $(CDS3_TSCSYSPN_BUILDROOT_DIR)
  
