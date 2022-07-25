#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of project fwupdate (PTXdist package wago-fwupdate).
#
# Copyright (c) 2018 WAGO GmbH & Co. KG
#
# Contributors:
#   PEn: WAGO GmbH & Co. KG
#######################################################################################################################
# Makefile for PTXdist package wago-fwupdate.
#
# For further information about the PTXdist project and license conditions
# see the PTXdist README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WAGO_FWUPDATE) += wago-fwupdate

#
# Paths and names
#
WAGO_FWUPDATE_VERSION        := 0.1.0
WAGO_FWUPDATE_MD5            :=
WAGO_FWUPDATE_WAGONAME       := fwupdate
WAGO_FWUPDATE                := wago-$(WAGO_FWUPDATE_WAGONAME)
WAGO_FWUPDATE_URL            := file://shared_public/$(WAGO_FWUPDATE_WAGONAME)
WAGO_FWUPDATE_BUILDCONFIG    := Release
WAGO_FWUPDATE_SRC_DIR        := $(PTXDIST_WORKSPACE)/shared_public/$(WAGO_FWUPDATE_WAGONAME)
WAGO_FWUPDATE_BUILDROOT_DIR  := $(BUILDDIR)/$(WAGO_FWUPDATE)
WAGO_FWUPDATE_DIR            := $(WAGO_FWUPDATE_BUILDROOT_DIR)/src
WAGO_FWUPDATE_BUILD_DIR      := $(WAGO_FWUPDATE_BUILDROOT_DIR)/bin/$(WAGO_FWUPDATE_BUILDCONFIG)
WAGO_FWUPDATE_LICENSE        := MPL-2.0
WAGO_FWUPDATE_PATH           := PATH=$(CROSS_PATH)
WAGO_FWUPDATE_BIN            := 
WAGO_FWUPDATE_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(WAGO_FWUPDATE_BUILDCONFIG) \
BIN_DIR=$(WAGO_FWUPDATE_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-fwupdate.extract:
	@$(call targetinfo)
	@mkdir -p $(WAGO_FWUPDATE_BUILDROOT_DIR)
	
	# Link source directory
	@if [ ! -L $(WAGO_FWUPDATE_DIR) ]; then \
	  ln -s $(WAGO_FWUPDATE_SRC_DIR) $(WAGO_FWUPDATE_DIR); \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

WAGO_FWUPDATE_CONF_TOOL	:= NO

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-fwupdate.compile:
	@$(call targetinfo)

# Currently no sources to compile
ifeq (0,1)
	@$(call world/compile, WAGO_FWUPDATE)
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-fwupdate.install:
	@$(call targetinfo)

# Currently nothing (e.g. public include files) to install
ifeq (0,1)
	@mkdir -p $(PTXCONF_SYSROOT_TARGET)/usr/include/$(WAGO_FWUPDATE)/
	@install -m 0644 $(WAGO_FWUPDATE_SRC_DIR)/inc/*.h \
		$(PTXCONF_SYSROOT_TARGET)/usr/include/$(WAGO_FWUPDATE)
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-fwupdate.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wago-fwupdate)
	@$(call install_fixup, wago-fwupdate,PRIORITY,optional)
	@$(call install_fixup, wago-fwupdate,SECTION,base)
	@$(call install_fixup, wago-fwupdate,AUTHOR,"<PEn>")
	@$(call install_fixup, wago-fwupdate,DESCRIPTION,missing)

	@$(call install_copy, wago-fwupdate, 0, 0, 0700, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate, /etc/config-tools/fwupdate)
	@$(call install_copy, wago-fwupdate, 0, 0, 0600, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate_basic_defines, /usr/sbin/fwupdate_basic_defines)
	@$(call install_replace, wago-fwupdate, /usr/sbin/fwupdate_basic_defines, @FW_UPDATE_GROUP@, $(PTXCONF_WAGO_FW_UPDATE_GROUP))
	@$(call install_copy, wago-fwupdate, 0, 0, 0600, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate_private_storage, /usr/sbin/fwupdate_private_storage)
	@$(call install_copy, wago-fwupdate, 0, 0, 0600, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate_common, /usr/sbin/fwupdate_common)
	@$(call install_copy, wago-fwupdate, 0, 0, 0700, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate_background_service, /usr/sbin/fwupdate_background_service)
	@$(call install_copy, wago-fwupdate, 0, 0, 0700, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate_status, /usr/sbin/fwupdate_status)
	@$(call install_copy, wago-fwupdate, 0, 0, 0700, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate_log, /usr/sbin/fwupdate_log)
	@$(call install_copy, wago-fwupdate, 0, 0, 0700, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate_control, /usr/sbin/fwupdate_control)
	@$(call install_copy, wago-fwupdate, 0, 0, 0700, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate_mode, /usr/sbin/fwupdate_mode)
	@$(call install_copy, wago-fwupdate, 0, 0, 0700, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate_report, /usr/sbin/fwupdate_report)
	@$(call install_copy, wago-fwupdate, 0, 0, 0700, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate_storage, /usr/sbin/fwupdate_storage)
	@$(call install_copy, wago-fwupdate, 0, 0, 0700, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate_service, /etc/init.d/fwupdate_service)
	@$(call install_link, wago-fwupdate, ../init.d/fwupdate_service, /etc/rc.d/disabled/S15_fwupdate_service)
	
	@$(call install_alternative, wago-fwupdate, 0, 0, 0755, \
		/usr/sbin/barebox-state_failsafe.sh)

	@$(call install_finish, wago-fwupdate)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-fwupdate.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/$(WAGO_FWUPDATE)/
	@if [ -d "$(WAGO_FWUPDATE_DIR)" -a -a "$(WAGO_FWUPDATE_DIR)/Makefile" ]; then \
	  $(WAGO_FWUPDATE_MAKE_ENV) $(WAGO_FWUPDATE_PATH) $(MAKE) $(MFLAGS) -C $(WAGO_FWUPDATE_DIR) clean; \
	fi
	@$(call clean_pkg, WAGO_FWUPDATE)
	@rm -rf $(WAGO_FWUPDATE_BUILDROOT_DIR)

# vim: syntax=make

