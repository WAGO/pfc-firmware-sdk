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
PACKAGES-$(PTXCONF_QFLASH) += qflash

#
# Paths and names
#
QFLASH_VERSION	:= 1.0.9
QFLASH			:= qflash
QFLASH_DIR		:= $(BUILDDIR)/$(QFLASH)

QFLASH_URL		:= file://$(PTXDIST_WORKSPACE)/wago_intern/$(QFLASH)
QFLASH_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/$(QFLASH)
QFLASH_DIR		:= $(BUILDDIR)/$(QFLASH)

QFLASH_LICENSE	:= unknown

QFLASH_PACKAGE_NAME := $(QFLASH)_$(QFLASH_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
QFLASH_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/qflash.extract:
	@$(call targetinfo, $@)
	@mkdir -p $(QFLASH_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call clean, $(QFLASH_DIR))

	cp -rd $(QFLASH_SRC) $(QFLASH_DIR)
endif
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

QFLASH_PATH	:= PATH=$(CROSS_PATH)
QFLASH_MAKE_ENV	:= $(CROSS_ENV) PKG_CONFIG_PATH=$(PTXDIST_SYSROOT_TARGET)/usr/lib/pkg-config
QFLASH_MAKE_ENV	:= $(CROSS_ENV)

$(STATEDIR)/qflash.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/prepare, QFLASH)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/qflash.compile:
	@$(call targetinfo, $@)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@cd $(QFLASH_DIR) && \
		$(QFLASH_MAKE_ENV) \
		$(MAKE)	CROSS_COMPILE=$(COMPILER_PREFIX) SYSROOT=$(PTXDIST_SYSROOT_TARGET)
endif
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/qflash.install:

	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/install, QFLASH)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup headers for later use in configs/@platform@/packages
	cd $(PKGDIR)/$(QFLASH)/ && \
	tar -czvf $(QFLASH).tgz * && \
	mv $(QFLASH).tgz $(QFLASH_PLATFORMCONFIGPACKAGEDIR)/ 
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Recover header from archive in configs/@platform@/packages
	mkdir -p $(PKGDIR)/$(QFLASH)
	tar -xzvf $(QFLASH_PLATFORMCONFIGPACKAGEDIR)/$(QFLASH).tgz -C $(PKGDIR)/$(QFLASH)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/qflash.targetinstall:
	@$(call targetinfo)

	@$(call install_init, qflash)

	@$(call install_fixup, qflash,PRIORITY,optional)
	@$(call install_fixup, qflash,SECTION,base)
	@$(call install_fixup, qflash,AUTHOR,"Qualcomm>)
	@$(call install_fixup, qflash,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(QFLASH_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(QFLASH_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(QFLASH_PLATFORMCONFIGPACKAGEDIR)/$(QFLASH_PACKAGE_NAME).ipk  
	@$(call install_archive, qflash, 0, 0, $(QFLASH_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else

	@$(call install_copy, qflash, 0, 0, 0750, /usr/bin/qflash)
	@$(call install_copy, qflash, 0, 0, 0750, $(QFLASH_DIR)/QFlash, /usr/bin/qflash/qflash)
	@$(call install_link, qflash, /usr/bin/qflash, /usr/lib/qflash/qflash)
	@$(call install_copy, qflash, 0, 0, 0640, $(QFLASH_DIR)/config.ini, /usr/bin/qflash/config.ini)
	@$(call install_copy, qflash, 0, 0, 0640, $(QFLASH_DIR)/readme, /usr/bin/qflash/readme)
endif
	@$(call install_finish, qflash)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(QFLASH_PACKAGE_NAME).ipk $(QFLASH_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/qflash.clean:
	@$(call targetinfo)
	@-cd $(QFLASH_DIR) && \
		$(QFLASH_MAKE_ENV) $(QFLASH_PATH) $(MAKE) clean
	@$(call clean_pkg, QFLASH)

# vim: syntax=make
