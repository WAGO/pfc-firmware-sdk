# -*-makefile-*-
#
# Copyright (C) 2020 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_TSCPROCESS) += cds3-tscprocess

#
# Paths and names
#
CDS3_TSCPROCESS_VERSION        := 0.1.0
CDS3_TSCPROCESS_MD5            :=
CDS3_TSCPROCESS                := cds3-tscprocess
CDS3_TSCPROCESS_BUILDCONFIG    := Release
CDS3_TSCPROCESS_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCPROCESS)
CDS3_TSCPROCESS_BUILDROOT_DIR  := $(BUILDDIR)/$(CDS3_TSCPROCESS)
CDS3_TSCPROCESS_DIR            := $(CDS3_TSCPROCESS_BUILDROOT_DIR)/src
CDS3_TSCPROCESS_BUILD_DIR      := $(CDS3_TSCPROCESS_BUILDROOT_DIR)/bin/$(CDS3_TSCPROCESS_BUILDCONFIG)
CDS3_TSCPROCESS_BIN            := libcds3-tscprocess.so.$(CDS3_TSCPROCESS_VERSION)
CDS3_TSCPROCESS_LICENSE        := WAGO
CDS3_TSCPROCESS_CONF_TOOL      := NO
CDS3_TSCPROCESS_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(CDS3_TSCPROCESS_BUILDCONFIG) \
BIN_DIR=$(CDS3_TSCPROCESS_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

CDS3_TSCPROCESS_PACKAGE_NAME             := $(CDS3_TSCPROCESS)_$(CDS3_TSCPROCESS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCPROCESS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscprocess.extract: 
	@$(call targetinfo)
	@mkdir -p $(CDS3_TSCPROCESS_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(CDS3_TSCPROCESS_DIR) ]; then \
	  ln -s $(CDS3_TSCPROCESS_SRC_DIR) $(CDS3_TSCPROCESS_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscprocess.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, CDS3_TSCPROCESS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscprocess.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, CDS3_TSCPROCESS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscprocess.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(CDS3_TSCPROCESS_PKGDIR) && \
	  tar xvzf $(CDS3_TSCPROCESS_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCPROCESS_PACKAGE_NAME).tgz -C $(CDS3_TSCPROCESS_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, CDS3_TSCPROCESS)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(CDS3_TSCPROCESS_PLATFORMCONFIGPACKAGEDIR)
	@cd $(CDS3_TSCPROCESS_PKGDIR) && tar cvzf $(CDS3_TSCPROCESS_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCPROCESS_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscprocess.targetinstall:
	@$(call targetinfo)

	@$(call install_init, cds3-tscprocess)
	@$(call install_fixup, cds3-tscprocess,PRIORITY,optional)
	@$(call install_fixup, cds3-tscprocess,SECTION,base)
	@$(call install_fixup, cds3-tscprocess,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, cds3-tscprocess,DESCRIPTION,missing)

	@$(call install_lib, cds3-tscprocess, 0, 0, 0644, libcds3-tscprocess)
	@$(call install_link, cds3-tscprocess, ../$(CDS3_TSCPROCESS_BIN), /usr/lib/cds3-custom-components/libcds3-tscprocess.so)


	@$(call install_finish, cds3-tscprocess)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscprocess.clean:
	@$(call targetinfo)
	@if [ -d $(CDS3_TSCPROCESS_DIR) ]; then \
		$(CDS3_TSCPROCESS_MAKE_ENV) $(CDS3_TSCPROCESS_PATH) $(MAKE) $(MFLAGS) -C $(CDS3_TSCPROCESS_DIR) clean; \
	fi
	@$(call clean_pkg, CDS3_TSCPROCESS)
	@rm -rf $(CDS3_TSCPROCESS_BUILDROOT_DIR)

# vim: syntax=make
