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
PACKAGES-$(PTXCONF_TSCPARAMSERV) += tscparamserv

#
# Paths and names
#
TSCPARAMSERV_VERSION        := 0.1.0
TSCPARAMSERV_MD5            :=
TSCPARAMSERV                := TscParamServ
TSCPARAMSERV_BUILDCONFIG    := Release
TSCPARAMSERV_URL            := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(TSCPARAMSERV)
TSCPARAMSERV_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(TSCPARAMSERV)
TSCPARAMSERV_BUILDROOT_DIR  := $(BUILDDIR)/$(TSCPARAMSERV)
TSCPARAMSERV_DIR            := $(TSCPARAMSERV_BUILDROOT_DIR)
TSCPARAMSERV_BUILD_DIR      := $(TSCPARAMSERV_BUILDROOT_DIR)/bin/$(TSCPARAMSERV_BUILDCONFIG)
TSCPARAMSERV_LICENSE        := WAGO
TSCPARAMSERV_CONF_TOOL      := NO
TSCPARAMSERV_SO_NAME        := lib$(TSCPARAMSERV).so
TSCPARAMSERV_BIN            := $(TSCPARAMSERV_SO_NAME).$(TSCPARAMSERV_VERSION)
TSCPARAMSERV_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(TSCPARAMSERV_BUILDCONFIG) \
BIN_DIR=$(TSCPARAMSERV_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

TSCPARAMSERV_PACKAGE_NAME             := $(TSCPARAMSERV)_$(TSCPARAMSERV_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
TSCPARAMSERV_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/tscparamserv.extract: 
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	if [ ! -L $(TSCPARAMSERV_DIR) ]; then \
	  ln -s $(TSCPARAMSERV_SRC_DIR) $(TSCPARAMSERV_DIR); \
	fi
else
	@mkdir -p $(TSCPARAMSERV_BUILDROOT_DIR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/tscparamserv.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/prepare, TSCPARAMSERV)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/tscparamserv.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, TSCPARAMSERV)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/tscparamserv.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(TSCPARAMSERV_PKGDIR) && \
	  tar xvzf $(TSCPARAMSERV_PLATFORMCONFIGPACKAGEDIR)/$(TSCPARAMSERV_PACKAGE_NAME).tgz -C $(TSCPARAMSERV_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, TSCPARAMSERV)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(TSCPARAMSERV_PLATFORMCONFIGPACKAGEDIR)
	@cd $(TSCPARAMSERV_PKGDIR) && tar cvzf $(TSCPARAMSERV_PLATFORMCONFIGPACKAGEDIR)/$(TSCPARAMSERV_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/tscparamserv.targetinstall:
	@$(call targetinfo)

	@$(call install_init, tscparamserv)
	@$(call install_fixup, tscparamserv,PRIORITY,optional)
	@$(call install_fixup, tscparamserv,SECTION,base)
	@$(call install_fixup, tscparamserv,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, tscparamserv,DESCRIPTION,missing)

	@$(call install_lib, tscparamserv, 0, 0, 0644, libTscParamServ)
	@$(call install_link, tscparamserv, ../$(TSCPARAMSERV_BIN), /usr/lib/cds3-custom-components/$(TSCPARAMSERV_SO_NAME))

	@$(call install_finish, tscparamserv)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/tscparamserv.clean:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ -d $(TSCPARAMSERV_DIR) ]; then \
		$(TSCPARAMSERV_MAKE_ENV) $(TSCPARAMSERV_PATH) $(MAKE) $(MFLAGS) -C $(TSCPARAMSERV_DIR) clean; \
	fi
endif
	@$(call clean_pkg, TSCPARAMSERV)
	@rm -rf $(TSCPARAMSERV_BUILDROOT_DIR)

# vim: syntax=make
