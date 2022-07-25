# -*-makefile-*-
#
# Copyright (C) 2018 by WAGO GmbH
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBWAGODRM) += libwagodrm

#
# Paths and names
#
LIBWAGODRM_VERSION        := 1.0.1
LIBWAGODRM_MD5            :=
LIBWAGODRM                := libwagodrm
LIBWAGODRM_URL            := file://$(PTXDIST_WORKSPACE)/wago_intern/$(LIBWAGODRM)
LIBWAGODRM_BUILDCONFIG    := Release
LIBWAGODRM_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(LIBWAGODRM)
LIBWAGODRM_BUILDROOT_DIR  := $(BUILDDIR)/$(LIBWAGODRM)
LIBWAGODRM_DIR            := $(LIBWAGODRM_BUILDROOT_DIR)/src
LIBWAGODRM_BUILD_DIR      := $(LIBWAGODRM_BUILDROOT_DIR)/bin/$(LIBWAGODRM_BUILDCONFIG)
LIBWAGODRM_LICENSE        := WAGO
LIBWAGODRM_BIN            := $(LIBWAGODRM).so.$(LIBWAGODRM_VERSION)
LIBWAGODRM_CONF_TOOL      := NO
LIBWAGODRM_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(LIBWAGODRM_BUILDCONFIG) \
BIN_DIR=$(LIBWAGODRM_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

LIBWAGODRM_PACKAGE_NAME         := $(LIBWAGODRM)_$(LIBWAGODRM_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBWAGODRM_PLATFORMCONFIGPACKAGEDIR    := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libwagodrm.extract:
	@$(call targetinfo)
	@mkdir -p $(LIBWAGODRM_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(LIBWAGODRM_DIR) ]; then \
	  ln -s $(LIBWAGODRM_SRC_DIR) $(LIBWAGODRM_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract.post
# ----------------------------------------------------------------------------

$(STATEDIR)/libwagodrm.extract.post:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libwagodrm.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, LIBWAGODRM)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/libwagodrm.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, LIBWAGODRM)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libwagodrm.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#   BSP mode: install by extracting tgz file
	@mkdir -p $(LIBWAGODRM_PKGDIR) && \
  	tar xvzf $(LIBWAGODRM_PLATFORMCONFIGPACKAGEDIR)/$(LIBWAGODRM_PACKAGE_NAME).tgz -C $(LIBWAGODRM_PKGDIR)
else
# 	normal mode, call "make install"

	@$(call world/install, LIBWAGODRM)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#   # save install directory to tgz for BSP mode
	@mkdir -p $(LIBWAGODRM_PLATFORMCONFIGPACKAGEDIR)
	@cd $(LIBWAGODRM_PKGDIR) && tar cvzf $(LIBWAGODRM_PLATFORMCONFIGPACKAGEDIR)/$(LIBWAGODRM_PACKAGE_NAME).tgz *
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_TRUNK
	cp $(LIBWAGODRM_SRC_DIR)/inc/libwagodrm.h $(PTXCONF_SYSROOT_TARGET)/usr/include/libwagodrm.h
endif
	@$(call touch)
# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libwagodrm.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libwagodrm)
	@$(call install_fixup, libwagodrm,PRIORITY,optional)
	@$(call install_fixup, libwagodrm,SECTION,base)
	@$(call install_fixup, libwagodrm,AUTHOR,"WAGO GmbH")
	@$(call install_fixup, libwagodrm,DESCRIPTION,missing)

	@$(call install_lib, libwagodrm, 0, 0, 0644, libwagodrm)
#	@$(call install_copy, libwagodrm, 0, 0, 0644, $(LIBWAGODRM_BUILD_DIR)/libwagodrm.so, /usr/lib/$(LIBWAGODRM_BIN))
#	@$(call install_link, libwagodrm, $(LIBWAGODRM_BIN), /usr/lib/$(LIBWAGODRM).so.0)
#	@$(call install_link, libwagodrm, $(LIBWAGODRM_BIN), /usr/lib/$(LIBWAGODRM).so)

	@$(call install_finish, libwagodrm)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libwagodrm.clean:
	@$(call targetinfo)
	@if [ -d $(LIBWAGODRM_DIR) ]; then \
		$(LIBWAGODRM_MAKE_ENV) $(LIBWAGODRM_PATH) $(MAKE) $(MFLAGS) -C $(LIBWAGODRM_DIR) clean; \
	fi
	@$(call clean_pkg, LIBWAGODRM)
	@rm -rf $(LIBWAGODRM_BUILDROOT_DIR)

# vim: syntax=make

