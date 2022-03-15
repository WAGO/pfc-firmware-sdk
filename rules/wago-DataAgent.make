# -*-makefile-*-
#
# Copyright (C) 2016 by M&M Software GmbH
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_DATAAGENT) += dataagent

#
# Paths and names
#
DATAAGENT_VERSION       := 1.11
DATAAGENT               := DataAgent
DATAAGENT_URL           := file://wago_intern/$(DATAAGENT)
DATAAGENT_BUILDCONFIG   := Release
DATAAGENT_SRC_DIR       := $(PTXDIST_WORKSPACE)/wago_intern/$(DATAAGENT)
DATAAGENT_BUILDROOT_DIR := $(BUILDDIR)/$(DATAAGENT)
DATAAGENT_DIR           := $(DATAAGENT_BUILDROOT_DIR)/src
DATAAGENT_BUILD_DIR     := $(DATAAGENT_BUILDROOT_DIR)/bin/$(DATAAGENT_BUILDCONFIG)
DATAAGENT_LICENSE	:= unknown
DATAAGENT_CONF_TOOL	:= NO
DATAAGENT_MAKE_ENV	:= $(CROSS_ENV) \
BUILDCONFIG=$(DATAAGENT_BUILDCONFIG) \
BIN_DIR=$(DATAAGENT_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

DATAAGENT_PACKAGE_NAME := dataagent_$(DATAAGENT_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
DATAAGENT_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------
#$(DATAAGENT_SOURCE):
#	@$(call targetinfo)
#	@$(call get, DATAAGENT)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------
$(STATEDIR)/dataagent.extract: 
	@$(call targetinfo)

	@mkdir -p $(DATAAGENT_BUILDROOT_DIR)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES	
	@if [ ! -L $(DATAAGENT_DIR) ]; then \
		ln -s $(DATAAGENT_SRC_DIR) $(DATAAGENT_DIR); \
	fi
endif

	@$(call touch)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/dataagent.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/dataagent.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/dataagent.compile:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/compile, DATAAGENT)
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/dataagent.install:
	@$(call targetinfo)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#   # BSP mode: install ipk contents to packages/DataAgent
	@mkdir -p $(DATAAGENT_PKGDIR) && \
    tar xvzf $(DATAAGENT_PLATFORMCONFIGPACKAGEDIR)/$(DATAAGENT_PACKAGE_NAME).tgz -C $(DATAAGENT_PKGDIR)
else

#   # "normal" mode: call "make install"
	@$(call world/install, DATAAGENT)


ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#   # save install directory contents for BSP
	@cd $(DATAAGENT_PKGDIR) && tar cvzf $(DATAAGENT_PLATFORMCONFIGPACKAGEDIR)/$(DATAAGENT_PACKAGE_NAME).tgz *
endif

endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------
$(STATEDIR)/dataagent.targetinstall:
	@$(call targetinfo)
	@$(call install_init, dataagent)

	@$(call install_fixup, dataagent,PRIORITY,optional)
	@$(call install_fixup, dataagent,SECTION,base)
	@$(call install_fixup, dataagent,AUTHOR,"M\&M Software GmbH")
	@$(call install_fixup, dataagent,DESCRIPTION,missing)

	@$(call install_copy, dataagent, 0, 118, 0770, /etc/dataagent/)
	@$(call install_copy, dataagent, 118, 118, 0640, -, /etc/dataagent/dataagent.config)
	@$(call install_copy, dataagent, 0, 0, 0755, -, /usr/bin/dataagent)
	@$(call install_copy, dataagent, 0, 0, 0750, /etc/specific/features/)
	@$(call install_copy, dataagent, 0, 0, 0644, -, /etc/specific/features/feature_dataagent)

	@$(call install_copy, dataagent, 0, 0, 0755, -, /etc/init.d/dataagent)

	@$(call install_copy, dataagent, 0, 0, 0750, -, /etc/config-tools/backup-restore/dataagent_backup_restore)
	@$(call install_copy, dataagent, 0, 0, 0750, -, /etc/config-tools/config_cloudconnectivity)

ifdef PTXCONF_DATAAGENT_PARAMETER_PROVIDER_DAEMON
	@$(call install_copy, dataagent, 0, 0, 0755, -, /usr/bin/pp_cloudconnect)
ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_DATAAGENT_PARAMETER_PROVIDER_DAEMON_STARTSCRIPT
	@$(call install_copy, dataagent, 0, 0, 0755, -, /etc/init.d/pp_cloudconnectivity)

ifneq ($(call remove_quotes, $(PTXCONF_DATAAGENT_PARAMETER_PROVIDER_DAEMON_STARTSCRIPT_BBINIT_LINK)),)
	@$(call install_link, dataagent, \
	  /etc/init.d/pp_cloudconnectivity, \
	  /etc/rc.d/$(PTXCONF_DATAAGENT_PARAMETER_PROVIDER_DAEMON_STARTSCRIPT_BBINIT_LINK))
endif
endif # PTXCONF_DATAAGENT_PARAMETER_PROVIDER_DAEMON_STARTSCRIPT
endif # PTXCONF_INITMETHOD_BBINIT
endif # PTXCONF_DATAAGENT_PARAMETER_PROVIDER_DAEMON

	
	

	@$(call install_finish, dataagent)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------
$(STATEDIR)/dataagent.clean:
	@$(call targetinfo)
	@if [ -e $(DATAAGENT_DIR)/Makefile ]; then \
	  $(CT_BUILD_EXAMPLE_MAKE_ENV) $(DATAAGENT_PATH) $(MAKE) $(MFLAGS) -C $(DATAAGENT_DIR) clean; \
	fi
	@$(call clean_pkg, DATAAGENT)
	@rm -rf $(DATAAGENT_BUILDROOT_DIR)


# vim: syntax=make
