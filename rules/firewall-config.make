# -*-makefile-*-
#
# Copyright (C) 2017 by WAGO Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_FIREWALL_CONFIG) += firewall-config

#
# Paths and names
#
FIREWALL_CONFIG_VERSION        := 1.0.0
FIREWALL_CONFIG_MD5            :=
FIREWALL_CONFIG                := firewall.elf
FIREWALL_CONFIG_URL            := file://local_src/config-tools/$(FIREWALL_CONFIG)
FIREWALL_CONFIG_BUILDCONFIG    := Release
#FIREWALL_CONFIG_BUILDCONFIG    := Debug
FIREWALL_CONFIG_SRC_DIR        := $(PTXDIST_WORKSPACE)/local_src/config-tools/firewall
#FIREWALL_CONFIG_BUILDROOT_DIR  := $(BUILDDIR)/config-tools/firewall
FIREWALL_CONFIG_BUILDROOT_DIR  := $(BUILDDIR)/firewall-config
FIREWALL_CONFIG_DIR            := $(FIREWALL_CONFIG_BUILDROOT_DIR)/src
FIREWALL_CONFIG_BUILD_DIR      := $(FIREWALL_CONFIG_BUILDROOT_DIR)/bin/$(FIREWALL_CONFIG_BUILDCONFIG)
FIREWALL_CONFIG_LICENSE        := unknown
FIREWALL_CONFIG_CONF_TOOL      := NO
FIREWALL_CONFIG_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(FIREWALL_CONFIG_BUILDCONFIG) \
BIN_DIR=$(FIREWALL_CONFIG_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build
FIREWALL_CONFIG_IPTABLES        := $(call ptx/get_alternative, projectroot, etc/firewall/iptables)
FIREWALL_CONFIG_EBTABLES        := $(call ptx/get_alternative, projectroot, etc/firewall/ebtables)
FIREWALL_CONFIG_SERVICES        := $(call ptx/get_alternative, projectroot, etc/firewall/services)
FIREWALL_CONFIG_TEMPLATES       := $(call ptx/get_alternative, projectroot, etc/firewall/templates)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/firewall-config.extract:
	@$(call targetinfo)
	mkdir -p $(FIREWALL_CONFIG_BUILDROOT_DIR)
	@if [ ! -L $(FIREWALL_CONFIG_DIR) ]; then \
		ln -s $(FIREWALL_CONFIG_SRC_DIR) $(FIREWALL_CONFIG_DIR); \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#$(STATEDIR)/firewall-config.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, FIREWALL_CONFIG)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/firewall-config.compile:
#	@$(call targetinfo)
#	@$(call world/compile, FIREWALL_CONFIG)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/firewall-config.install:
	@$(call targetinfo)
	@$(call world/install, FIREWALL_CONFIG)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/firewall-config.targetinstall:
	@$(call targetinfo)

	@$(call install_init, firewall-config)
	@$(call install_fixup, firewall-config,PRIORITY,optional)
	@$(call install_fixup, firewall-config,SECTION,base)
	@$(call install_fixup, firewall-config,AUTHOR,"WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, firewall-config,DESCRIPTION,missing)

	@$(call install_copy, firewall-config, 0, 0, 0755, /etc/config-tools);
	@$(call install_copy, firewall-config, 0, 0, 0750, $(PTXDIST_SYSROOT_TARGET)/usr/bin/firewall.elf, /usr/bin/firewall);
	@$(call install_link, firewall-config, /usr/bin/firewall, /etc/config-tools/firewall)

	@$(call install_alternative, firewall-config, 0, 0, 0750, /etc/config-tools/firewall_apply.sh)

	@$(call install_copy, firewall-config, 0, 0, 0700, /etc/firewall)

	@$(call install_alternative, firewall-config, 0, 0, 0750, /etc/firewall/firewall)
	@$(call install_alternative, firewall-config, 0, 0, 0750, /etc/firewall/validate_if.sh)
	@$(call install_alternative, firewall-config, 0, 0, 0750, /etc/firewall/fwbackup.sh)
	@$(call install_alternative, firewall-config, 0, 0, 0750, /etc/firewall/fwrestore.sh)
	@$(call install_alternative, firewall-config, 0, 0, 0750, /etc/firewall/ipsecfirewall.sh)
	@$(call install_alternative, firewall-config, 0, 0, 0750, /etc/firewall/permissions.sh)
	@$(call install_alternative, firewall-config, 0, 0, 0640, /etc/firewall/firewall.conf)
	@$(call install_alternative, firewall-config, 0, 0, 0640, /etc/firewall/params_gen.xml)
	@$(call install_alternative, firewall-config, 0, 0, 0640, /etc/firewall/params.xml)
	@$(call install_alternative, firewall-config, 0, 0, 0640, /etc/firewall/params.xsd)
	@$(call install_alternative, firewall-config, 0, 0, 0640, /etc/firewall/patterns.xsd)
	@$(call install_alternative, firewall-config, 0, 0, 0640, /etc/firewall/transform.xsl)

	@$(call install_copy, firewall-config, 0, 0, 0700, /etc/firewall/ebtables)
	@cd $(FIREWALL_CONFIG_EBTABLES) && \
        for object in $$( find ./* -maxdepth 0 -type f -print ); do \
		if test -f $$object; then \
			$(call install_copy, firewall-config, 0, 0, 0600, $(FIREWALL_CONFIG_EBTABLES)/$$object, /etc/firewall/ebtables/$$object); \
		fi; \
	done;

	@$(call install_copy, firewall-config, 0, 0, 0700, /etc/firewall/iptables)
	@cd $(FIREWALL_CONFIG_IPTABLES) && \
	for object in $$( find ./* -maxdepth 0 -type f -print ); do \
		if test -f $$object; then \
			$(call install_copy, firewall-config, 0, 0, 0600, $(FIREWALL_CONFIG_IPTABLES)/$$object, /etc/firewall/iptables/$$object); \
		fi; \
	done;

	@$(call install_copy, firewall-config, 0, 0, 0700, /etc/firewall/services)
	@cd $(FIREWALL_CONFIG_SERVICES) && \
        for object in $$( find ./* -maxdepth 0 -type f -print ); do \
		if test -f $$object; then \
			$(call install_copy, firewall-config, 0, 0, 0600, $(FIREWALL_CONFIG_SERVICES)/$$object, /etc/firewall/services/$$object); \
		fi; \
	done;

	@$(call install_copy, firewall-config, 0, 0, 0700, /etc/firewall/templates)
	@cd $(FIREWALL_CONFIG_TEMPLATES) && \
	for object in $$( find ./* -maxdepth 0 -type f -print ); do \
		if test -f $$object; then \
			$(call install_copy, firewall-config, 0, 0, 0600, $(FIREWALL_CONFIG_TEMPLATES)/$$object, /etc/firewall/templates/$$object); \
		fi; \
	done;

	@$(call install_alternative, firewall-config, 0, 0, 0700, /etc/config-tools/events/codesys/firewall)
	@$(call install_alternative, firewall-config, 0, 0, 0700, /etc/config-tools/events/dhcp/firewall)
	@$(call install_alternative, firewall-config, 0, 0, 0700, /etc/config-tools/events/dns/firewall)
	@$(call install_alternative, firewall-config, 0, 0, 0700, /etc/config-tools/events/modbus/firewall)
	@$(call install_alternative, firewall-config, 0, 0, 0700, /etc/config-tools/events/snmp/firewall)
	@$(call install_alternative, firewall-config, 0, 0, 0700, /etc/config-tools/events/ssh/firewall)
	@$(call install_alternative, firewall-config, 0, 0, 0700, /etc/config-tools/events/ssl/firewall)
ifneq ($(PTXCONF_PLATFORM), wago-pac100)
	@$(call install_alternative, firewall-config, 0, 0, 0700, /etc/config-tools/events/telnet/firewall)
	@$(call install_alternative, firewall-config, 0, 0, 0700, /etc/config-tools/events/tftp/firewall)
endif
	@$(call install_alternative, firewall-config, 0, 0, 0700, /etc/config-tools/events/iocheckport/firewall)
	@$(call install_alternative, firewall-config, 0, 0, 0700, /etc/config-tools/events/dnp3/firewall)
	@$(call install_alternative, firewall-config, 0, 0, 0700, /etc/config-tools/events/iec60870_5_104/firewall)
	@$(call install_alternative, firewall-config, 0, 0, 0700, /etc/config-tools/events/iec61850_mms/firewall)

ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, firewall-config, 0, 0, 0700, /etc/init.d/firewall)

ifneq ($(PTXCONF_FIREWALL_CONFIG_RC_LINK),"")
	@$(call install_link, firewall-config, ../init.d/firewall, /etc/rc.d/$(PTXCONF_FIREWALL_CONFIG_RC_LINK))
endif

else
	$(error "Please supply an init file for your init system!")
endif

	@$(call install_finish, firewall-config)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/firewall-config.clean:
	@$(call targetinfo)
	@if [ -d $(FIREWALL_CONFIG_DIR) ]; then \
		$(FIREWALL_CONFIG_MAKE_ENV) $(FIREWALL_CONFIG_PATH) $(MAKE) $(MFLAGS) -C $(FIREWALL_CONFIG_DIR) clean; \
	fi
	@$(call clean_pkg, FIREWALL_CONFIG)
	@rm -rf $(FIREWALL_CONFIG_BUILDROOT_DIR)

# vim: syntax=make
