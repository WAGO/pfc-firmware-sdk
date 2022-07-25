# -*-makefile-*-
#
# Copyright (C) 2013 by <cm>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_QCHECK) += qcheck

#
# Paths and names
#
QCHECK_VERSION	:= 1.0.0
QCHECK			    := qcheck

QCHECK_URL		:= file://$(PTXDIST_WORKSPACE)/wago_intern/$(QCHECK)
QCHECK_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/$(QCHECK)
QCHECK_DIR		:= $(BUILDDIR)/$(QCHECK)

QCHECK_LICENSE	:= unknown

QCHECK_PACKAGE_NAME := $(QCHECK)_$(QCHECK_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
QCHECK_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/qcheck.extract:
	@$(call targetinfo, $@)
	@$(call clean, $(QCHECK_DIR))
	@mkdir -p $(QCHECK_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	cp -rd $(QCHECK_SRC) $(BUILDDIR)
	@$(call patchin, QCHECK)
endif

	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#QCHECK_PATH	:= PATH=$(CROSS_PATH)
QCHECK_CONF_TOOL	:= NO
#QCHECK_MAKE_ENV	:= $(CROSS_ENV) PKG_CONFIG_PATH=$(PTXDIST_SYSROOT_TARGET)/usr/lib/pkg-config
QCHECK_MAKE_ENV	:= $(CROSS_ENV)

$(STATEDIR)/qcheck.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/qcheck.compile:
	@$(call targetinfo, $@)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@cd $(QCHECK_DIR) && \
		$(QCHECK_MAKE_ENV) \
		$(MAKE)	CROSS_COMPILE=$(COMPILER_PREFIX) SYSROOT=$(PTXDIST_SYSROOT_TARGET)
endif
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/qcheck.install:
	@$(call targetinfo, $@)
#	@$(call touch, $@)
#	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call world/install, QCHECK)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup headers for later use in configs/@platform@/packages
	cd $(PKGDIR)/$(QCHECK)/ && \
	tar -czvf $(QCHECK).tgz * && \
	mv $(QCHECK).tgz $(QCHECK_PLATFORMCONFIGPACKAGEDIR)/ 
endif
endif

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Recover header from archive in configs/@platform@/packages
	mkdir -p $(PKGDIR)/$(QCHECK)
	tar -xzvf $(QCHECK_PLATFORMCONFIGPACKAGEDIR)/$(QCHECK).tgz -C $(PKGDIR)/$(QCHECK)
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/qcheck.targetinstall:
	@$(call targetinfo)

	@$(call install_init, qcheck)

	@$(call install_fixup, qcheck,PRIORITY,optional)
	@$(call install_fixup, qcheck,SECTION,base)
	@$(call install_fixup, qcheck,AUTHOR,"Karsten Nünthel : WAGO GmbH \& Co. KG <www.wago.com>")
	@$(call install_fixup, qcheck,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(QCHECK_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(QCHECK_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(QCHECK_PLATFORMCONFIGPACKAGEDIR)/$(QCHECK_PACKAGE_NAME).ipk  
	@$(call install_archive, qcheck, 0, 0, $(QCHECK_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else

	@$(call install_copy, qcheck, 0, 0, 0750, /usr/bin/qcheck)
	@$(call install_copy, qcheck, 0, 0, 0750, $(QCHECK_DIR)/qcheck, /usr/bin/qcheck/qcheck)

endif
	@$(call install_finish, qcheck)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(QCHECK_PACKAGE_NAME).ipk $(QCHECK_PLATFORMCONFIGPACKAGEDIR)/
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/qcheck.clean:
	@$(call targetinfo)
	@-cd $(QCHECK_DIR) && \
		$(QCHECK_MAKE_ENV) $(QCHECK_PATH) $(MAKE) clean
	@$(call clean_pkg, QCHECK)

# vim: syntax=make
