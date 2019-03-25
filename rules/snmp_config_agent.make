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
PACKAGES-$(PTXCONF_SNMPCONFIGAGENT) += snmpconfigagent

#
# Paths and names
#
SNMPCONFIGAGENT_VERSION       := 1.0.0
SNMPCONFIGAGENT_MD5           :=
SNMPCONFIGAGENT               := snmpconfigagent
SNMPCONFIGAGENT_SUFFIX        := 
SNMPCONFIGAGENT_URL           := 
SNMPCONFIGAGENT_BUILDCONFIG   := Release
SNMPCONFIGAGENT_SRC_DIR       := $(PTXDIST_WORKSPACE)/wago_intern/SnmpConfigAgent
SNMPCONFIGAGENT_BUILDROOT_DIR := $(BUILDDIR)/SnmpConfigAgent
SNMPCONFIGAGENT_DIR           := $(SNMPCONFIGAGENT_BUILDROOT_DIR)/src
SNMPCONFIGAGENT_BUILD_DIR     := $(SNMPCONFIGAGENT_BUILDROOT_DIR)/bin/$(SNMPCONFIGAGENT_BUILDCONFIG)
SNMPCONFIGAGENT_LICENSE       := unknown
SNMPCONFIGAGENT_BIN           := 

SNMPCONFIGAGENT_CONF_TOOL     := NO
SNMPCONFIGAGENT_MAKE_ENV      := $(CROSS_ENV) \
BUILDCONFIG=$(SNMPCONFIGAGENT_BUILDCONFIG) \
BIN_DIR=$(SNMPCONFIGAGENT_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

SNMPCONFIGAGENT_PACKAGE_NAME := $(SNMPCONFIGAGENT)_$(SNMPCONFIGAGENT_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
SNMPCONFIGAGENT_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/snmpconfigagent.extract:
	@$(call targetinfo)
#	@$(call clean, $(SNMPCONFIGAGENT_BUILDROOT_DIR))
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@mkdir -p $(SNMPCONFIGAGENT_BUILDROOT_DIR)
	@if [ ! -L $(SNMPCONFIGAGENT_DIR) ]; then \
		ln -s $(SNMPCONFIGAGENT_SRC_DIR) $(SNMPCONFIGAGENT_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#$(STATEDIR)/snmpconfigagent.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, SNMPCONFIGAGENT)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/snmpconfigagent.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call world/compile, SNMPCONFIGAGENT)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/snmpconfigagent.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/install, SNMPCONFIGAGENT)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup headers for later use in configs/@platform@/packages
	cd $(PKGDIR)/$(SNMPCONFIGAGENT)/ && \
	tar -czvf $(SNMPCONFIGAGENT).tgz * && \
	mv $(SNMPCONFIGAGENT).tgz $(SNMPCONFIGAGENT_PLATFORMCONFIGPACKAGEDIR)/ 
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Recover header from archive in configs/@platform@/packages
	mkdir -p $(PKGDIR)/$(SNMPCONFIGAGENT)
	tar -xzvf $(SNMPCONFIGAGENT_PLATFORMCONFIGPACKAGEDIR)/$(SNMPCONFIGAGENT).tgz -C $(PKGDIR)/$(SNMPCONFIGAGENT)
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/snmpconfigagent.targetinstall:
	@$(call targetinfo)

	@$(call install_init, snmpconfigagent)
	@$(call install_fixup, snmpconfigagent,PRIORITY,optional)
	@$(call install_fixup, snmpconfigagent,SECTION,base)
	@$(call install_fixup, snmpconfigagent,AUTHOR,"WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, snmpconfigagent,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(SNMPCONFIGAGENT_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(SNMPCONFIGAGENT_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(SNMPCONFIGAGENT_PLATFORMCONFIGPACKAGEDIR)/$(SNMPCONFIGAGENT_PACKAGE_NAME).ipk  
	@$(call install_archive, snmpconfigagent, 0, 0, $(SNMPCONFIGAGENT_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE	

	@$(call install_copy, snmpconfigagent, 0, 0, 0755, $(SNMPCONFIGAGENT_BUILD_DIR)/snmp_config_agentd.elf, /usr/sbin/snmp_config_agentd)

ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_copy, snmpconfigagent, 0, 0, 0755, $(SNMPCONFIGAGENT_DIR)/res/snmpconfigagent, /etc/init.d/snmpconfigagent)
	# SnmpConfigAgent is only useful together with net-snmp daemon/agent.
	# Therefore, SnmpConfigAgent init script is called and controlled by net-snmp init script.
	#@$(call install_link, snmpconfigagent, ../init.d/snmpconfigagent, /etc/rc.d/S99_snmpconfigagent)
endif

endif
	@$(call install_finish, snmpconfigagent)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(SNMPCONFIGAGENT_PACKAGE_NAME).ipk $(SNMPCONFIGAGENT_PLATFORMCONFIGPACKAGEDIR)/
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/snmpconfigagent.clean:
	@$(call targetinfo)
	@if [ -d $(SNMPCONFIGAGENT_DIR) ]; then \
	$(SNMPCONFIGAGENT_MAKE_ENV) $(SNMPCONFIGAGENT_PATH) $(MAKE) $(MFLAGS) -C $(SNMPCONFIGAGENT_DIR) clean; \
	fi
	@$(call clean_pkg, SNMPCONFIGAGENT)
	@rm -rf $(SNMPCONFIGAGENT_BUILDROOT_DIR)

# vim: syntax=make
