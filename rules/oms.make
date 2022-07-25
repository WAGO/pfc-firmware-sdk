# -*-makefile-*-
#
# Copyright (C) 2022 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_OMS) += oms

#
# Paths and names
#
OMS_VERSION        := 0.2.0
OMS_MD5            :=
OMS                := oms
OMS_BUILDCONFIG    := Release
OMS_SRC_DIR        := $(PTXDIST_WORKSPACE)/src/$(OMS)
OMS_BUILDROOT_DIR  := $(BUILDDIR)/$(OMS)
OMS_DIR            := $(OMS_BUILDROOT_DIR)/src
OMS_BUILD_DIR      := $(OMS_BUILDROOT_DIR)/bin/$(OMS_BUILDCONFIG)
OMS_LICENSE        := WAGO
OMS_CONF_TOOL      := NO
OMS_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(OMS_BUILDCONFIG) \
BIN_DIR=$(OMS_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

OMS_PACKAGE_NAME             := $(OMS)_$(OMS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
OMS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/oms.extract: 
	@$(call targetinfo)
	@mkdir -p $(OMS_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(OMS_DIR) ]; then \
	  ln -s $(OMS_SRC_DIR) $(OMS_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/oms.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, OMS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/oms.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, OMS)
endif
	@$(call touch)
# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/oms.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(OMS_PKGDIR) && \
	  tar xvzf $(OMS_PLATFORMCONFIGPACKAGEDIR)/$(OMS_PACKAGE_NAME).tgz -C $(OMS_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, OMS)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(OMS_PLATFORMCONFIGPACKAGEDIR)
	@cd $(OMS_PKGDIR) && tar cvzf $(OMS_PLATFORMCONFIGPACKAGEDIR)/$(OMS_PACKAGE_NAME).tgz *
endif
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/oms.targetinstall:
	@$(call targetinfo)

	@$(call install_init, oms)
	@$(call install_fixup, oms,PRIORITY,optional)
	@$(call install_fixup, oms,SECTION,base)
	@$(call install_fixup, oms,AUTHOR, "<Hans Florian Scholz>")
	@$(call install_fixup, oms,DESCRIPTION,missing)

	@$(call install_lib, oms, 0, 0, 0644, liboms)
	
ifdef PTXCONF_OMS_OMSD
	@for i in $(shell cd $(OMS_PKGDIR) && find bin sbin usr/bin usr/sbin -type f); do \
		$(call install_copy, oms, 0, 0, 0755, -, /$$i); \
	done
	
	@$(call install_alternative, oms, 0, 0, 0640, /etc/oms.d/omsd.conf,n)
	@$(call install_alternative, oms, 0, 0, 0770, /etc/oms.d/power_on_reset.sh,n)
	@$(call install_copy, oms, 0, 0, 0755, -, /etc/init.d/omsdaemon)
	
ifdef PTXCONF_OMS_FIX_IP
	@$(call install_alternative, oms, 0, 0, 0640, /etc/oms.d/pfc/fix_ip.conf,n)
	@$(call install_alternative, oms, 0, 0, 0770, /etc/oms.d/fix_ip.sh,n)
endif
ifdef PTXCONF_OMS_FACTORY_DEFUALTS
	@$(call install_alternative, oms, 0, 0, 0640, /etc/oms.d/pfc/factory_defaults.conf,n)
	@$(call install_alternative, oms, 0, 0, 0770, /etc/oms.d/set_factory_defaults.sh,n)
endif

endif

	@$(call install_finish, oms)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/oms.clean:
	@$(call targetinfo)
	@if [ -d $(OMS_DIR) ]; then \
		$(OMS_MAKE_ENV) $(OMS_PATH) $(MAKE) $(MFLAGS) -C $(OMS_DIR) clean; \
	fi
	@$(call clean_pkg, OMS)
	@rm -rf $(OMS_BUILDROOT_DIR)

# vim: syntax=make
