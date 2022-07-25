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
PACKAGES-$(PTXCONF_LIBBRIDGE) += libbridge

#
# Paths and names
#
LIBBRIDGE_VERSION        := 1.5.0
LIBBRIDGE_MD5            :=
LIBBRIDGE                := libbridge
LIBBRIDGE_BUILDCONFIG    := Release
LIBBRIDGE_SRC_DIR        := $(PTXDIST_WORKSPACE)/local_src/$(LIBBRIDGE)
LIBBRIDGE_BUILDROOT_DIR  := $(BUILDDIR)/$(LIBBRIDGE)
LIBBRIDGE_DIR            := $(LIBBRIDGE_BUILDROOT_DIR)/src
LIBBRIDGE_BUILD_DIR      := $(LIBBRIDGE_BUILDROOT_DIR)/bin/$(LIBBRIDGE_BUILDCONFIG)
LIBBRIDGE_LICENSE        := GPLv2
LIBBRIDGE_LICENSE_FILE   := COPYING
LIBBRIDGE_CONF_TOOL      := NO
LIBBRIDGE_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(LIBBRIDGE_BUILDCONFIG) \
BIN_DIR=$(LIBBRIDGE_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(LIBBRIDGE_SRC_DIR):
	@echo "Error: $@: directory not found!" >&2; exit 2

$(STATEDIR)/libbridge.extract:  | $(LIBBRIDGE_SRC_DIR) 
	@$(call targetinfo)
	@mkdir -p $(LIBBRIDGE_BUILDROOT_DIR)
	@if [ ! -L $(LIBBRIDGE_DIR) ]; then \
	  ln -s $(LIBBRIDGE_SRC_DIR) $(LIBBRIDGE_DIR); \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libbridge.prepare:
	@$(call targetinfo)
	@$(call world/prepare, LIBBRIDGE)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/libbridge.compile:
	@$(call targetinfo)
	@$(call world/compile, LIBBRIDGE)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libbridge.install:
	@$(call targetinfo)
	@$(call world/install, LIBBRIDGE)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libbridge.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libbridge)
	@$(call install_fixup, libbridge,PRIORITY,optional)
	@$(call install_fixup, libbridge,SECTION,base)
	@$(call install_fixup, libbridge,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, libbridge,DESCRIPTION,missing)

	@$(call install_copy, libbridge, 0, 0, 0644, $(LIBBRIDGE_SRC_DIR)/$(LIBBRIDGE_LICENSE_FILE), /usr/share/licenses/oss/license.$(LIBBRIDGE)_$(LIBBRIDGE_VERSION).txt)

	@$(call install_finish, libbridge)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libbridge.clean:
	@$(call targetinfo)
	@if [ -d $(LIBBRIDGE_DIR) ]; then \
		$(LIBBRIDGE_MAKE_ENV) $(LIBBRIDGE_PATH) $(MAKE) $(MFLAGS) -C $(LIBBRIDGE_DIR) clean; \
	fi
	@$(call clean_pkg, LIBBRIDGE)
	@rm -rf $(LIBBRIDGE_BUILDROOT_DIR)

# vim: syntax=make
