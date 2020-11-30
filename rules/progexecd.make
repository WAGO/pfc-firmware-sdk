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
PACKAGES-$(PTXCONF_PROGEXECD) += progexecd

#
# Paths and names
#
PROGEXECD_VERSION       := 2.0.0
PROGEXECD_MD5           :=
PROGEXECD               := progexecd
PROGEXECD_SUFFIX        := 
PROGEXECD_URL           := 
PROGEXECD_BUILDCONFIG   := Release
PROGEXECD_SRC_DIR       := $(PTXDIST_WORKSPACE)/wago_intern/$(PROGEXECD)
PROGEXECD_BUILDROOT_DIR := $(BUILDDIR)/$(PROGEXECD)
PROGEXECD_DIR           := $(PROGEXECD_BUILDROOT_DIR)/src
PROGEXECD_BUILD_DIR     := $(PROGEXECD_BUILDROOT_DIR)/bin/$(PROGEXECD_BUILDCONFIG)
PROGEXECD_LICENSE       := unknown
PROGEXECD_BIN           := 

PROGEXECD_CONF_TOOL     := NO
PROGEXECD_MAKE_ENV      := $(CROSS_ENV) \
BUILDCONFIG=$(PROGEXECD_BUILDCONFIG) \
BIN_DIR=$(PROGEXECD_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PROGEXECD_PACKAGE_NAME := $(PROGEXECD)_$(PROGEXECD_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PROGEXECD_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/progexecd.extract:
	@$(call targetinfo)
#	@$(call clean, $(PROGEXECD_BUILDROOT_DIR))
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@mkdir -p $(PROGEXECD_BUILDROOT_DIR)
	@if [ ! -L $(PROGEXECD_DIR) ]; then \
		ln -s $(PROGEXECD_SRC_DIR) $(PROGEXECD_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#$(STATEDIR)/progexecd.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, PROGEXECD)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/progexecd.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call world/compile, PROGEXECD)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/progexecd.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/install, PROGEXECD)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup headers for later use in configs/@platform@/packages
	cd $(PKGDIR)/$(PROGEXECD)/ && \
	tar -czvf $(PROGEXECD).tgz * && \
	mv $(PROGEXECD).tgz $(PROGEXECD_PLATFORMCONFIGPACKAGEDIR)/ 
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Recover header from archive in configs/@platform@/packages
	mkdir -p $(PKGDIR)/$(PROGEXECD)
	tar -xzvf $(PROGEXECD_PLATFORMCONFIGPACKAGEDIR)/$(PROGEXECD).tgz -C $(PKGDIR)/$(PROGEXECD)
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/progexecd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, progexecd)
	@$(call install_fixup, progexecd,PRIORITY,optional)
	@$(call install_fixup, progexecd,SECTION,base)
	@$(call install_fixup, progexecd,AUTHOR,"WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, progexecd,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(PROGEXECD_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(PROGEXECD_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(PROGEXECD_PLATFORMCONFIGPACKAGEDIR)/$(PROGEXECD_PACKAGE_NAME).ipk  
	@$(call install_archive, progexecd, 0, 0, $(PROGEXECD_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE	

	@$(call install_copy, progexecd, 0, 0, 0755, $(PROGEXECD_BUILD_DIR)/progexecd.elf, /usr/bin/progexecd)
	@$(call install_lib, progexecd, 0, 0, 0644, libprogexec)

ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_copy, progexecd, 0, 0, 0755, $(PROGEXECD_DIR)/progexec, /etc/init.d/progexec)

ifneq ($(call remove_quotes, $(PTXCONF_PROGEXEC_BBINIT_LINK)),)
	@$(call install_link, progexecd, ../init.d/progexec, /etc/rc.d/$(PTXCONF_PROGEXEC_BBINIT_LINK))
endif
endif

endif
	@$(call install_finish, progexecd)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(PROGEXECD_PACKAGE_NAME).ipk $(PROGEXECD_PLATFORMCONFIGPACKAGEDIR)/
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/progexecd.clean:
	@$(call targetinfo)
	@if [ -d $(PROGEXECD_DIR) ]; then \
	$(PROGEXECD_MAKE_ENV) $(PROGEXECD_PATH) $(MAKE) $(MFLAGS) -C $(PROGEXECD_DIR) clean; \
	fi
	@$(call clean_pkg, PROGEXECD)
	@rm -rf $(PROGEXECD_BUILDROOT_DIR)

# vim: syntax=make
