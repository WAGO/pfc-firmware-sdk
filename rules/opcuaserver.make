# -*-makefile-*-
#
# Copyright (C) 2016 by Falk Werner/Jobst Wellensiek
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_OPCUASERVER) += opcuaserver

#
# Paths and names
#
OPCUASERVER_VERSION        := 1.2.5
OPCUASERVER_MD5            :=
OPCUASERVER                := opcuaserver
OPCUASERVER_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/opcua/server
OPCUASERVER_URL            := file://$(OPCUASERVER_SRC_DIR)
OPCUASERVER_BUILDCONFIG    := Release
OPCUASERVER_BUILDROOT_DIR  := $(BUILDDIR)/$(OPCUASERVER)-$(OPCUASERVER_VERSION)
OPCUASERVER_DIR            := $(OPCUASERVER_BUILDROOT_DIR)/src
OPCUASERVER_BUILD_DIR      := $(OPCUASERVER_BUILDROOT_DIR)/bin/$(OPCUASERVER_BUILDCONFIG)
OPCUASERVER_LICENSE        := unknown
OPCUASERVER_PATH           := PATH=$(CROSS_PATH)
OPCUASERVER_CONF_TOOL      := NO
OPCUASERVER_PACKAGE_NAME := $(OPCUASERVER)_$(OPCUASERVER_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
OPCUASERVER_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages


OPCUASERVER_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(OPCUASERVER_BUILDCONFIG) \
BIN_DIR=$(OPCUASERVER_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

FIRMWARE_SVNREVISION = "$(shell svn info .. 2>/dev/null | grep '^Last Changed Rev' | awk '{print $$NF}')"

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/opcuaserver.extract:
	@$(call targetinfo)
	@mkdir -p $(OPCUASERVER_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES	
	@if [ ! -L $(OPCUASERVER_DIR) ]; then \
	  ln -s $(OPCUASERVER_SRC_DIR) $(OPCUASERVER_DIR); \
	fi
endif	
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract Post
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/opcuaserver.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#$(STATEDIR)/opcuaserver.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, OPCUASERVER)
#	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/opcuaserver.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, OPCUASERVER)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/opcuaserver.install:
	@$(call targetinfo)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#   BSP mode: install by extracting tgz file
	  @mkdir -p $(OPCUASERVER_PKGDIR) && \
	tar xvzf $(OPCUASERVER_PLATFORMCONFIGPACKAGEDIR)/$(OPCUASERVER_PACKAGE_NAME).tgz -C $(OPCUASERVER_PKGDIR)
else
#   normal mode, call "make install"
	@$(call world/install, OPCUASERVER)
	
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#   # save install directory to tgz for BSP mode
	@mkdir -p $(OPCUASERVER_PLATFORMCONFIGPACKAGEDIR)
	@cd $(OPCUASERVER_PKGDIR) && tar cvzf $(OPCUASERVER_PLATFORMCONFIGPACKAGEDIR)/$(OPCUASERVER_PACKAGE_NAME).tgz *
endif
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/opcuaserver.targetinstall:
	@$(call targetinfo)

	@$(call install_init, opcuaserver)
	@$(call install_fixup, opcuaserver,PRIORITY,optional)
	@$(call install_fixup, opcuaserver,SECTION,base)
	@$(call install_fixup, opcuaserver,AUTHOR,"Falk Werner/Jobst Wellensiek")
	@$(call install_fixup, opcuaserver,DESCRIPTION,missing)

#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish
#
#	@$(call install_copy, opcuaserver, 0, 0, 0755, $(OPCUASERVER_BUILD_DIR)/opcua-server.elf, /usr/bin/opcua-server)
	@$(call install_copy, opcuaserver, 0, 0, 0755, -, /usr/bin/opcua-server)
	@$(call install_copy, opcuaserver, 0, 0, 0755, -, /usr/bin/opcua-server-wrapper)
	@$(call install_copy, opcuaserver, 0, 0, 0755, -, /etc/config-tools/config-opcua)
	@$(call install_alternative, opcuaserver, 0, 0, 0755, /etc/init.d/opcua-server, n)
	@$(call install_link, opcuaserver, ../init.d/opcua-server, /etc/rc.d/S86_opcua-server)
	@$(call install_copy, opcuaserver, 0, 0, 0644, -, /etc/pam.d/opcua)
	@$(call install_copy, opcuaserver, 0, 0, 0644, -, /etc/specific/opcua-server/opcua-server.conf)
	@$(call install_copy, opcuaserver, 0, 0, 0750, /etc/specific/features/)
	@$(call install_copy, opcuaserver, 0, 0, 0644, -, /etc/specific/features/opcua-server)
	@$(call install_copy, opcuaserver, 117, 117, 0750, /etc/specific/opcua-server/pki)
	@$(call install_copy, opcuaserver, 117, 117, 0750, /etc/specific/opcua-server/pki/own)
	@$(call install_copy, opcuaserver, 117, 117, 0640, -, /etc/specific/opcua-server/pki/own/certificate.der)
	@$(call install_copy, opcuaserver, 117, 117, 0640, -, /etc/specific/opcua-server/pki/own/key.pem)
	@$(call install_copy, opcuaserver, 0, 0, 0744, -, /etc/config-tools/backup-restore/opcuaserver-backup)

	@$(call install_finish, opcuaserver)
	@$(call touch)
	$(PTXDIST_WORKSPACE)/scripts/opcuaserver-helpers/make-metaipk.sh
	#$(PTXDIST_WORKSPACE)/scripts/opcuaserver-helpers/make-metaipk-3s.sh

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/opcuaserver.clean:
	@$(call targetinfo)
	@if [ -d $(OPCUASERVER_DIR) ]; then \
	  $(OPCUASERVER_MAKE_ENV) $(OPCUASERVER_PATH) $(MAKE) $(MFLAGS) -C $(OPCUASERVER_DIR) clean; \
	fi
	@$(call clean_pkg, OPCUASERVER)
	@rm -rf $(OPCUASERVER_BUILDROOT_DIR)

# vim: syntax=make
