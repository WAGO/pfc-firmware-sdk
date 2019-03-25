# -*-makefile-*-
#
# Copyright (C) 2018 by LHe
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_RETAIN_DATA) += retain_data

#
# Paths and names
#
RETAIN_DATA_VERSION        := 0.0.1
RETAIN_DATA_MD5            :=
RETAIN_DATA                := retain_data
RETAIN_DATA_URL            := file://wago_intern/$(RETAIN_DATA)
RETAIN_DATA_BUILDCONFIG    := Release
RETAIN_DATA_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(RETAIN_DATA)
RETAIN_DATA_BUILDROOT_DIR  := $(BUILDDIR)/$(RETAIN_DATA)
RETAIN_DATA_DIR            := $(RETAIN_DATA_BUILDROOT_DIR)/src
RETAIN_DATA_BUILD_DIR      := $(RETAIN_DATA_BUILDROOT_DIR)/bin/$(RETAIN_DATA_BUILDCONFIG)
RETAIN_DATA_LICENSE        := WAGO
RETAIN_DATA_PATH           := PATH=$(CROSS_PATH)
RETAIN_DATA_BIN            :=
RETAIN_DATA_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(RETAIN_DATA_BUILDCONFIG) \
BIN_DIR=$(RETAIN_DATA_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

RETAIN_DATA_PACKAGE_NAME         := $(RETAIN_DATA)_$(RETAIN_DATA_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
RETAIN_DATA_PLATFORMCONFIGPACKAGEDIR    := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#$(STATEDIR)/retain_data.extract:

$(STATEDIR)/retain_data.extract:
	@$(call targetinfo)
	@mkdir -p $(RETAIN_DATA_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(RETAIN_DATA_DIR) ]; then \
	  ln -s $(RETAIN_DATA_SRC_DIR) $(RETAIN_DATA_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract.post
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/retain_data.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

RETAIN_DATA_CONF_TOOL	:= NO

#$(STATEDIR)/retain_data.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, RETAIN_DATA)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/retain_data.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, RETAIN_DATA)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/retain_data.install:
	@$(call targetinfo)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#   BSP mode: install by extracting tgz file
	@mkdir -p $(RETAIN_DATA_PKGDIR) && \
  	tar xvzf $(RETAIN_DATA_PLATFORMCONFIGPACKAGEDIR)/$(RETAIN_DATA_PACKAGE_NAME).tgz -C $(RETAIN_DATA_PKGDIR)
else
# 	normal mode, call "make install"

	@$(call world/install, RETAIN_DATA)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#   # save install directory to tgz for BSP mode
	@mkdir -p $(RETAIN_DATA_PLATFORMCONFIGPACKAGEDIR)
	@cd $(RETAIN_DATA_PKGDIR) && tar cvzf $(RETAIN_DATA_PLATFORMCONFIGPACKAGEDIR)/$(RETAIN_DATA_PACKAGE_NAME).tgz *
endif
endif
#ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_TRUNK
#	cp $(RETAIN_DATA_SRC_DIR)/inc/libwagodrm.h $(PTXCONF_SYSROOT_TARGET)/usr/include/libwagodrm.h
#endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/retain_data.targetinstall:
	@$(call targetinfo)

	@$(call install_init, retain_data)
	@$(call install_fixup, retain_data,PRIORITY,optional)
	@$(call install_fixup, retain_data,SECTION,base)
	@$(call install_fixup, retain_data,AUTHOR,"LHe")
	@$(call install_fixup, retain_data,DESCRIPTION,missing)

	@$(call install_copy, retain_data, 0, 0, 0755, -, /usr/sbin/retain_data)

	@$(call install_finish, retain_data)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/retain_data.clean:
	@$(call targetinfo)
	@if [ -d $(RETAIN_DATA_DIR) ]; then \
		$(RETAIN_DATA_MAKE_ENV) $(RETAIN_DATA_PATH) $(MAKE) $(MFLAGS) -C $(RETAIN_DATA_DIR) clean; \
	fi
	@$(call clean_pkg, RETAIN_DATA)
	@rm -rf $(RETAIN_DATA_BUILDROOT_DIR)

# vim: syntax=make
