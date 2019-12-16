## -*-makefile-*-
# $Id: wago-codesys3-045-tscpamiecinterface.make 43384 2019-10-07 12:15:20Z charline.wueste@wago.com $
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
PACKAGES-$(PTXCONF_CDS3_TSCPAMIECINTERFACE) += cds3-tscpamiecinterface

CDS3_TSCPAMIECINTERFACE_VERSION	     := 0.0.1
CDS3_TSCPAMIECINTERFACE_MD5          :=
CDS3_TSCPAMIECINTERFACE              := TscPamIecInterface
CDS3_TSCPAMIECINTERFACE_BUILDCONFIG := Release
CDS3_TSCPAMIECINTERFACE_DIR          := $(BUILDDIR)/$(CDS3_TSCPAMIECINTERFACE)/src
CDS3_TSCPAMIECINTERFACE_BUILDROOT_DIR  := $(BUILDDIR)/$(CDS3_TSCPAMIECINTERFACE)
CDS3_TSCPAMIECINTERFACE_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCPAMIECINTERFACE)
CDS3_TSCPAMIECINTERFACE_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCPAMIECINTERFACE)
CDS3_TSCPAMIECINTERFACE_SRC          := $(CDS3_TSCPAMIECINTERFACE_SRC_DIR)/$(CDS3_TSCPAMIECINTERFACE)
CDS3_TSCPAMIECINTERFACE_BIN          := lib$(CDS3_TSCPAMIECINTERFACE).so.$(CDS3_TSCPAMIECINTERFACE_VERSION)

CDS3_TSCPAMIECINTERFACE_BUILD_DIR      := $(CDS3_TSCPAMIECINTERFACE_BUILDROOT_DIR)/bin/$(CDS3_TSCPAMIECINTERFACE_BUILDCONFIG)
CDS3_TSCPAMIECINTERFACE_LICENSE        := WAGO
CDS3_TSCPAMIECINTERFACE_CONF_TOOL      := NO
CDS3_TSCPAMIECINTERFACE_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(CDS3_TSCPAMIECINTERFACE_BUILDCONFIG) \
BIN_DIR=$(CDS3_TSCPAMIECINTERFACE_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

CDS3_TSCPAMIECINTERFACE_PACKAGE_NAME := cds3-tscpamiecinterface_$(CDS3_TSCPAMIECINTERFACE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCPAMIECINTERFACE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(CDS3_TSCPAMIECINTERFACE_SRC_DIR):
	@echo "Error: $@: directory not found!" >&2; \
	exit 2

$(STATEDIR)/cds3-tscpamiecinterface.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscpamiecinterface.extract:
	@$(call targetinfo)
	@mkdir -p $(CDS3_TSCPAMIECINTERFACE_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(CDS3_TSCPAMIECINTERFACE_DIR) ]; then \
	  ln -s $(CDS3_TSCPAMIECINTERFACE_SRC_DIR) $(CDS3_TSCPAMIECINTERFACE_DIR); \
	fi
endif
	@$(call touch)
	
$(STATEDIR)/cds3-tscpamiecinterface.extract.post:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscpamiecinterface.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, CDS3_TSCPAMIECINTERFACE)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscpamiecinterface.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCPAMIECINTERFACE)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscpamiecinterface.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(CDS3_TSCPAMIECINTERFACE_PKGDIR) && \
	  tar xvzf $(CDS3_TSCPAMIECINTERFACE_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCPAMIECINTERFACE_PACKAGE_NAME).tgz -C $(CDS3_TSCPAMIECINTERFACE_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, CDS3_TSCPAMIECINTERFACE)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(CDS3_TSCPAMIECINTERFACE_PLATFORMCONFIGPACKAGEDIR)
	@cd $(CDS3_TSCPAMIECINTERFACE_PKGDIR) && tar cvzf $(CDS3_TSCPAMIECINTERFACE_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCPAMIECINTERFACE_PACKAGE_NAME).tgz *
endif
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscpamiecinterface.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscpamiecinterface)

	@$(call install_fixup,cds3-tscpamiecinterface,PRIORITY,optional)
	@$(call install_fixup,cds3-tscpamiecinterface,SECTION,base)
	@$(call install_fixup,cds3-tscpamiecinterface,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscpamiecinterface,DESCRIPTION,missing)

	@$(call install_lib, cds3-tscpamiecinterface, 0, 0, 0644, libTscPamIecInterface)
	@$(call install_link, cds3-tscpamiecinterface, ../$(CDS3_TSCPAMIECINTERFACE_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCPAMIECINTERFACE).so);


	@$(call install_finish,cds3-tscpamiecinterface)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscpamiecinterface.clean:
	@$(call targetinfo)
	@if [ -d $(CDS3_TSCPAMIECINTERFACE_DIR) ]; then \
		$(CDS3_TSCPAMIECINTERFACE_MAKE_ENV) $(CDS3_TSCPAMIECINTERFACE_PATH) $(MAKE) $(MFLAGS) -C $(CDS3_TSCPAMIECINTERFACE_DIR) clean; \
	fi
	@$(call clean_pkg, CDS3_TSCPAMIECINTERFACE)
	@rm -rf $(CDS3_TSCPAMIECINTERFACE_BUILDROOT_DIR)
	
