## -*-makefile-*-
# $Id: wago-codesys3-044-tscdrmiecinterface.make 65677 2022-03-11 11:28:13Z falk.werner@wago.com $
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
PACKAGES-$(PTXCONF_CDS3_TSCDRMIECINTERFACE) += cds3-tscdrmiecinterface

CDS3_TSCDRMIECINTERFACE_VERSION	     := 0.1.0
CDS3_TSCDRMIECINTERFACE_MD5          :=
CDS3_TSCDRMIECINTERFACE              := TscDrmIecInterface
CDS3_TSCDRMIECINTERFACE_BUILDCONFIG := Release
CDS3_TSCDRMIECINTERFACE_DIR          := $(BUILDDIR)/$(CDS3_TSCDRMIECINTERFACE)/src
CDS3_TSCDRMIECINTERFACE_BUILDROOT_DIR  := $(BUILDDIR)/$(CDS3_TSCDRMIECINTERFACE)
CDS3_TSCDRMIECINTERFACE_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCDRMIECINTERFACE)
CDS3_TSCDRMIECINTERFACE_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCDRMIECINTERFACE)
CDS3_TSCDRMIECINTERFACE_SRC          := $(CDS3_TSCDRMIECINTERFACE_SRC_DIR)/$(CDS3_TSCDRMIECINTERFACE)
CDS3_TSCDRMIECINTERFACE_BIN          := lib$(CDS3_TSCDRMIECINTERFACE).so.$(CDS3_TSCDRMIECINTERFACE_VERSION)

CDS3_TSCDRMIECINTERFACE_BUILD_DIR      := $(CDS3_TSCDRMIECINTERFACE_BUILDROOT_DIR)/bin/$(CDS3_TSCDRMIECINTERFACE_BUILDCONFIG)
CDS3_TSCDRMIECINTERFACE_LICENSE        := WAGO
CDS3_TSCDRMIECINTERFACE_CONF_TOOL      := NO
CDS3_TSCDRMIECINTERFACE_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(CDS3_TSCDRMIECINTERFACE_BUILDCONFIG) \
BIN_DIR=$(CDS3_TSCDRMIECINTERFACE_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

CDS3_TSCDRMIECINTERFACE_PACKAGE_NAME := cds3-tscdrmiecinterface_$(CDS3_TSCDRMIECINTERFACE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCDRMIECINTERFACE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(CDS3_TSCDRMIECINTERFACE_SRC_DIR):
	@echo "Error: $@: directory not found!" >&2; \
	exit 2

$(STATEDIR)/cds3-tscdrmiecinterface.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscdrmiecinterface.extract:
	@$(call targetinfo)
	@mkdir -p $(CDS3_TSCDRMIECINTERFACE_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(CDS3_TSCDRMIECINTERFACE_DIR) ]; then \
	  ln -s $(CDS3_TSCDRMIECINTERFACE_SRC_DIR) $(CDS3_TSCDRMIECINTERFACE_DIR); \
	fi
endif
	@$(call touch)
	
$(STATEDIR)/cds3-tscdrmiecinterface.extract.post:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscdrmiecinterface.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, CDS3_TSCDRMIECINTERFACE)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscdrmiecinterface.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCDRMIECINTERFACE)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscdrmiecinterface.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(CDS3_TSCDRMIECINTERFACE_PKGDIR) && \
	  tar xvzf $(CDS3_TSCDRMIECINTERFACE_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCDRMIECINTERFACE_PACKAGE_NAME).tgz -C $(CDS3_TSCDRMIECINTERFACE_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, CDS3_TSCDRMIECINTERFACE)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(CDS3_TSCDRMIECINTERFACE_PLATFORMCONFIGPACKAGEDIR)
	@cd $(CDS3_TSCDRMIECINTERFACE_PKGDIR) && tar cvzf $(CDS3_TSCDRMIECINTERFACE_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCDRMIECINTERFACE_PACKAGE_NAME).tgz *
endif
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscdrmiecinterface.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscdrmiecinterface)

	@$(call install_fixup,cds3-tscdrmiecinterface,PRIORITY,optional)
	@$(call install_fixup,cds3-tscdrmiecinterface,SECTION,base)
	@$(call install_fixup,cds3-tscdrmiecinterface,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscdrmiecinterface,DESCRIPTION,missing)

	@$(call install_lib, cds3-tscdrmiecinterface, 0, 0, 0644, libTscDrmIecInterface)
	@$(call install_link, cds3-tscdrmiecinterface, ../$(CDS3_TSCDRMIECINTERFACE_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCDRMIECINTERFACE).so);


	@$(call install_finish,cds3-tscdrmiecinterface)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscdrmiecinterface.clean:
	@$(call targetinfo)
	@if [ -d $(CDS3_TSCDRMIECINTERFACE_DIR) ]; then \
		$(CDS3_TSCDRMIECINTERFACE_MAKE_ENV) $(CDS3_TSCDRMIECINTERFACE_PATH) $(MAKE) $(MFLAGS) -C $(CDS3_TSCDRMIECINTERFACE_DIR) clean; \
	fi
	@$(call clean_pkg, CDS3_TSCDRMIECINTERFACE)
	@rm -rf $(CDS3_TSCDRMIECINTERFACE_BUILDROOT_DIR)
	
