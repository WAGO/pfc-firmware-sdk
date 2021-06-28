# -*-makefile-*-
#
# Copyright (C) 2020 by WAGO Kontakttechnik GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_TSCMODEMIECINTERFACE) += cds3-tscmodemiecinterface

#
# Paths and names
#
CDS3_TSCMODEMIECINTERFACE_VERSION        := 1.1.2
CDS3_TSCMODEMIECINTERFACE_MD5            :=
CDS3_TSCMODEMIECINTERFACE                := cds3-tscmodemiecinterface
CDS3_TSCMODEMIECINTERFACE_BUILDCONFIG    := Debug
CDS3_TSCMODEMIECINTERFACE_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCMODEMIECINTERFACE)
CDS3_TSCMODEMIECINTERFACE_BUILDROOT_DIR  := $(BUILDDIR)/$(CDS3_TSCMODEMIECINTERFACE)
CDS3_TSCMODEMIECINTERFACE_DIR            := $(CDS3_TSCMODEMIECINTERFACE_BUILDROOT_DIR)/src
CDS3_TSCMODEMIECINTERFACE_BUILD_DIR      := $(CDS3_TSCMODEMIECINTERFACE_BUILDROOT_DIR)/bin/$(CDS3_TSCMODEMIECINTERFACE_BUILDCONFIG)
CDS3_TSCMODEMIECINTERFACE_BIN            := libTscModemIecInterface.so.$(CDS3_TSCMODEMIECINTERFACE_VERSION)
CDS3_TSCMODEMIECINTERFACE_LICENSE        := WAGO
CDS3_TSCMODEMIECINTERFACE_CONF_TOOL      := NO
CDS3_TSCMODEMIECINTERFACE_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(CDS3_TSCMODEMIECINTERFACE_BUILDCONFIG) \
BIN_DIR=$(CDS3_TSCMODEMIECINTERFACE_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

CDS3_TSCMODEMIECINTERFACE_PACKAGE_NAME             := $(CDS3_TSCMODEMIECINTERFACE)_$(CDS3_TSCMODEMIECINTERFACE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCMODEMIECINTERFACE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscmodemiecinterface.extract: 
	@$(call targetinfo)
	@mkdir -p $(CDS3_TSCMODEMIECINTERFACE_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(CDS3_TSCMODEMIECINTERFACE_DIR) ]; then \
	  ln -s $(CDS3_TSCMODEMIECINTERFACE_SRC_DIR) $(CDS3_TSCMODEMIECINTERFACE_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract.post
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/cds3-tscmodemiecinterface.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscmodemiecinterface.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, CDS3_TSCMODEMIECINTERFACE)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscmodemiecinterface.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, CDS3_TSCMODEMIECINTERFACE)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscmodemiecinterface.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(CDS3_TSCMODEMIECINTERFACE_PKGDIR) && \
	  tar xvzf $(CDS3_TSCMODEMIECINTERFACE_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCMODEMIECINTERFACE_PACKAGE_NAME).tgz -C $(CDS3_TSCMODEMIECINTERFACE_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, CDS3_TSCMODEMIECINTERFACE)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(CDS3_TSCMODEMIECINTERFACE_PLATFORMCONFIGPACKAGEDIR)
	@cd $(CDS3_TSCMODEMIECINTERFACE_PKGDIR) && tar cvzf $(CDS3_TSCMODEMIECINTERFACE_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCMODEMIECINTERFACE_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscmodemiecinterface.targetinstall:
	@$(call targetinfo)

	@$(call install_init, cds3-tscmodemiecinterface)
	@$(call install_fixup, cds3-tscmodemiecinterface,PRIORITY,optional)
	@$(call install_fixup, cds3-tscmodemiecinterface,SECTION,base)
	@$(call install_fixup, cds3-tscmodemiecinterface,AUTHOR,"WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, cds3-tscmodemiecinterface,DESCRIPTION,missing)

	@$(call install_lib, cds3-tscmodemiecinterface, 0, 0, 0640, libTscModemIecInterface)
	@$(call install_link, cds3-tscmodemiecinterface, ../$(CDS3_TSCMODEMIECINTERFACE_BIN), /usr/lib/cds3-custom-components/libTscModemIecInterface.so)

	@$(call install_finish, cds3-tscmodemiecinterface)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscmodemiecinterface.clean:
	@$(call targetinfo)
	@if [ -d $(CDS3_TSCMODEMIECINTERFACE_DIR) ]; then \
		$(CDS3_TSCMODEMIECINTERFACE_MAKE_ENV) $(CDS3_TSCMODEMIECINTERFACE_PATH) $(MAKE) $(MFLAGS) -C $(CDS3_TSCMODEMIECINTERFACE_DIR) clean; \
	fi
	@$(call clean_pkg, CDS3_TSCMODEMIECINTERFACE)
	@rm -rf $(CDS3_TSCMODEMIECINTERFACE_BUILDROOT_DIR)

# vim: syntax=make
