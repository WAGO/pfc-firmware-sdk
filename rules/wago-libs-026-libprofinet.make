# -*-makefile-*-
#
# Copyright (C) 2016 by Wago Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBPROFINET) += libprofinet

#
# Paths and names
#
LIBPROFINET_VERSION  := 1.0.2
LIBPROFINET_MD5      :=
LIBPROFINET          := libprofinet
LIBPROFINET_URL      := file://$(PTXDIST_WORKSPACE)/wago_intern/device/profinet
LIBPROFINET_LICENSE  := unknown

LIBPROFINET_BUILDCONFIG  := Release

LIBPROFINET_SRC_DIR := $(PTXDIST_WORKSPACE)/wago_intern/device/profinet
LIBPROFINET_BUILDROOT_DIR  := $(BUILDDIR)/$(LIBPROFINET)
LIBPROFINET_DIR            := $(LIBPROFINET_BUILDROOT_DIR)/src
LIBPROFINET_BUILD_DIR      := $(LIBPROFINET_BUILDROOT_DIR)/bin/$(LIBPROFINET_BUILDCONFIG)
LIBPROFINET_BIN := $(LIBPROFINET).so.$(LIBPROFINET_VERSION)
LIBPROFINET_CONF_TOOL := NO
LIBPROFINET_MAKE_ENV       := $(CROSS_ENV) \
                            BUILDCONFIG=$(LIBPROFINET_BUILDCONFIG) \
                            BIN_DIR=$(LIBPROFINET_BUILD_DIR) \
                            SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

LIBPROFINET_PACKAGE_NAME := $(LIBPROFINET)_$(LIBPROFINET_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libprofinet.extract:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	mkdir -p $(LIBPROFINET_BUILDROOT_DIR)
	@if [ ! -L $(LIBPROFINET_DIR) ]; then \
	  ln -s $(LIBPROFINET_SRC_DIR) $(LIBPROFINET_DIR); \
	fi
else
	mkdir -p $(LIBPROFINET_DIR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libprofinet.prepare:
	@$(call targetinfo)
	@$(call world/prepare, LIBPROFINET)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call xslt_compile, $(LIBPROFINET_DIR)/pfcspecific/src/xml/pndiag.xml)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/libprofinet.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, LIBPROFINET)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libprofinet.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/install, LIBPROFINET)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#find ommits empty directories created by ptxdist in background
	cd $(PKGDIR)/$(LIBPROFINET)/ && \
	find -L . -type f -or -xtype l | tar -czvf $(PTXDIST_PLATFORMCONFIGDIR)/packages/$(LIBPROFINET_PACKAGE_NAME).tgz -T -
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	mkdir -p $(PKGDIR)/$(LIBPROFINET)/
	tar -xzvf $(PTXDIST_PLATFORMCONFIGDIR)/packages/$(LIBPROFINET_PACKAGE_NAME).tgz -C $(PTXCONF_SYSROOT_TARGET)
	tar -xzvf $(PTXDIST_PLATFORMCONFIGDIR)/packages/$(LIBPROFINET_PACKAGE_NAME).tgz -C $(PKGDIR)/$(LIBPROFINET)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libprofinet.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libprofinet)
	@$(call install_fixup, libprofinet,PRIORITY,optional)
	@$(call install_fixup, libprofinet,SECTION,base)
	@$(call install_fixup, libprofinet,AUTHOR,"Wago")
	@$(call install_fixup, libprofinet,DESCRIPTION,missing)

	@$(call install_lib, libprofinet, 0, 0, 0644, libprofinet)
		
	@$(call install_copy, libprofinet, 0, 0, 0644, -, /usr/share/snmp/mibs/LLDP-EXT-DOT1-MIB.txt)
	@$(call install_copy, libprofinet, 0, 0, 0644, -, /usr/share/snmp/mibs/LLDP-EXT-DOT3-MIB.txt)
	@$(call install_copy, libprofinet, 0, 0, 0644, -, /usr/share/snmp/mibs/LLDP-EXT-IEC61158-TYPE10-V2.3.2.32-MIB.txt)
	@$(call install_copy, libprofinet, 0, 0, 0644, -, /usr/share/snmp/mibs/LLDP-MIB.txt)
	@$(call install_copy, libprofinet, 0, 0, 0644, -, /usr/share/snmp/mibs/PNIO-MIB.txt)
	
	
	
	@$(call install_finish, libprofinet)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libprofinet.clean:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@-$(call xslt_clean, $(LIBPROFINET_DIR)/src/pfcspecific/xml/pndiag.xml)
	@-if [ -d $(LIBPROFINET_DIR) ]; then \
		$(LIBPROFINET_MAKE_ENV) $(LIBPROFINET_PATH) $(MAKE) $(MFLAGS) -C $(LIBPROFINET_DIR) clean; \
	fi
endif
	@$(call clean_pkg, LIBPROFINET)
	@rm -rf $(LIBPROFINET_BUILDROOT_DIR)
# vim: syntax=make
