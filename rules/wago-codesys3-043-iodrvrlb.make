# -*-makefile-*-
# $Id$
#
# Copyright (C) 2013 by Wago Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_IODRVRLB) += cds3-iodrvrlb
 
CDS3_IODRVRLB_VERSION       := 0.0.1
CDS3_IODRVRLB               := IoDrvRlb
CDS3_IODRVRLB_URL           := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVRLB)
CDS3_IODRVRLB_SRC_DIR       := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVRLB)
CDS3_IODRVRLB_SRC           := $(CDS3_IODRVRLB_SRC_DIR)/$(CDS3_IODRVRLB)
CDS3_IODRVRLB_SO_NAME       := lib$(CDS3_IODRVRLB).so
CDS3_IODRVRLB_BIN           := $(CDS3_IODRVRLB_SO_NAME).$(CDS3_IODRVRLB_VERSION)
CDS3_IODRVRLB_BUILDCONFIG   := Debug
CDS3_IODRVRLB_BUILDROOT_DIR := $(BUILDDIR)/$(CDS3_IODRVRLB)
CDS3_IODRVRLB_DIR           := $(CDS3_IODRVRLB_BUILDROOT_DIR)
CDS3_IODRVRLB_BUILD_DIR     := $(CDS3_IODRVRLB_BUILDROOT_DIR)/bin/$(CDS3_IODRVRLB_BUILDCONFIG)
CDS3_IODRVRLB_PACKAGE_NAME  := $(CDS3_IODRVRLB)_$(CDS3_IODRVRLB_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)

CDS3_IODRVRLB_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

CDS3_IODRVRLB_PATH      := PATH=$(CROSS_PATH)
CDS3_IODRVRLB_MAKE_ENV  := $(CROSS_ENV) \
    BUILDCONFIG=$(CDS3_IODRVRLB_BUILDCONFIG) \
    BIN_DIR=$(CDS3_IODRVRLB_BUILD_DIR) \
    SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

CDS3_IODRVRLB_MAKE_OPT  := CC=$(CROSS_CC)



ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvrlb.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvrlb.extract:
	@$(call targetinfo)
	@if [ ! -L $(CDS3_IODRVRLB_DIR) ]; then \
		ln -s $(CDS3_IODRVRLB_SRC_DIR) $(CDS3_IODRVRLB_DIR); \
	fi
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
$(STATEDIR)/cds3-iodrvrlb.compile:
	@$(call targetinfo)
	$(call world/compile, CDS3_IODRVRLB)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------
$(STATEDIR)/cds3-iodrvrlb.install:
	@$(call targetinfo)
	@$(call world/install, CDS3_IODRVRLB)
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvrlb.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-iodrvrlb)

	@$(call install_fixup, cds3-iodrvrlb, PRIORITY, optional)
	@$(call install_fixup, cds3-iodrvrlb, SECTION, base)
	@$(call install_fixup, cds3-iodrvrlb, AUTHOR, "WAGO")
	@$(call install_fixup, cds3-iodrvrlb, DESCRIPTION, "Codesys3 IO-Driver for rocket local bus")

	@$(call install_lib, cds3-iodrvrlb, 0, 0, 0644, libIoDrvRlb)
	@$(call install_link, cds3-iodrvrlb, ../$(CDS3_IODRVRLB_BIN), /usr/lib/cds3-custom-components/$(CDS3_IODRVRLB_SO_NAME))
  
	@$(call install_finish, cds3-iodrvrlb)

	@$(call touch)



else
# Putting them into one rule creates circular dependencies...
$(STATEDIR)/cds3-iodrvrlb.%:
	@$(call targetinfo)
	@echo "Skipping ($*) for PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES"
	@$(call touch)
$(STATEDIR)/cds3-iodrvrlb.%.pack:
	@$(call targetinfo)
	@echo "Skipping ($*) for PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES"
	@$(call touch)
$(STATEDIR)/cds3-iodrvrlb.%.post:
	@$(call targetinfo)
	@echo "Skipping ($*) for PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES"
	@$(call touch)
endif



# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvrlb.clean:
	rm -rf $(STATEDIR)/cds3-iodrvrlb.*
	rm -rf $(PKGDIR)/cds3-iodrvrlb_*
	rm -rf $(CDS3_IODRVRLB_DIR)
