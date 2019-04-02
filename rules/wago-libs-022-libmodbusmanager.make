# -*-makefile-*-
#
# Copyright (C) 2014 by Timur Celik <timur.celik@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBMODBUSMANAGER) += libmodbusmanager

#
# Paths and names
#
LIBMODBUSMANAGER_VERSION	:= 0.4.5
LIBMODBUSMANAGER		:= libModbusManager
LIBMODBUSMANAGER_URL		:= $(PTXDIST_WORKSPACE)/wago_intern/device/modbus/manager
LIBMODBUSMANAGER_DIR		:= $(BUILDDIR)/$(LIBMODBUSMANAGER)
LIBMODBUSMANAGER_LICENSE	:= unknown
LIBMODBUSMANAGER_BIN		:= $(LIBMODBUSMANAGER).so.$(LIBMODBUSMANAGER_VERSION)

LIBMODBUSMANAGER_PACKAGE_NAME := libmodbusmanager_$(LIBMODBUSMANAGER_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBMODBUSMANAGER_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libmodbusmanager.extract:
	@$(call targetinfo)
	@$(call clean, $(LIBMODBUSMANAGER_DIR))
	
	@mkdir -p $(LIBMODBUSMANAGER_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	rsync -a --exclude=".*" $(LIBMODBUSMANAGER_URL)/* $(LIBMODBUSMANAGER_DIR)
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#LIBMODBUSMANAGER_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
#LIBMODBUSMANAGER_CONF_TOOL	:= autoconf
#LIBMODBUSMANAGER_CONF_OPT	:= $(CROSS_AUTOCONF_USR)

$(STATEDIR)/libmodbusmanager.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#	@$(call world/prepare, LIBMODBUSMANAGER)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

LIBMODBUSMANAGER_PATH      := PATH=$(CROSS_PATH)
LIBMODBUSMANAGER_MAKE_ENV  := $(CROSS_ENV) \
                              CFLAGS="$(CROSS_CFLAGS) -I$(KERNEL_HEADERS_INCLUDE_DIR)" \
                              VERSION=$(LIBMODBUSMANAGER_VERSION)
LIBMODBUSMANAGER_MAKE_OPT  := CC=$(CROSS_CC)

#LIBMODBUSMANAGER_MAKE_OPT += "DBGMODE=-g3"
#LIBMODBUSMANAGER_MAKE_OPT += "OPTIMIZE=-O2"

$(STATEDIR)/libmodbusmanager.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/compile, LIBMODBUSMANAGER)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libmodbusmanager.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/install, LIBMODBUSMANAGER)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup headers for later use in configs/@platform@/packages
	cd $(PKGDIR)/$(LIBMODBUSMANAGER)/ && \
	tar -czvf $(LIBMODBUSMANAGER).tgz * && \
	mv $(LIBMODBUSMANAGER).tgz $(LIBMODBUSMANAGER_PLATFORMCONFIGPACKAGEDIR)/ 
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Recover header from archive in configs/@platform@/packages
	mkdir -p $(PKGDIR)/$(LIBMODBUSMANAGER)
	tar -xzvf $(LIBMODBUSMANAGER_PLATFORMCONFIGPACKAGEDIR)/$(LIBMODBUSMANAGER).tgz -C $(PKGDIR)/$(LIBMODBUSMANAGER)
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libmodbusmanager.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libmodbusmanager)

	@$(call install_fixup, libmodbusmanager, PRIORITY, optional)
	@$(call install_fixup, libmodbusmanager, SECTION, base)
	@$(call install_fixup, libmodbusmanager, AUTHOR, "Timur Celik <timur.celik@wago.com>")
	@$(call install_fixup, libmodbusmanager, DESCRIPTION, missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 

	# Extract precompiled binaries from archive
	rm -rf $(LIBMODBUSMANAGER_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(LIBMODBUSMANAGER_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(LIBMODBUSMANAGER_PLATFORMCONFIGPACKAGEDIR)/$(LIBMODBUSMANAGER_PACKAGE_NAME).ipk  
	@$(call install_archive, libmodbusmanager, 0, 0, $(LIBMODBUSMANAGER_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE	

	@$(call install_copy, libmodbusmanager, 0, 0, 0755, $(LIBMODBUSMANAGER_DIR)/$(LIBMODBUSMANAGER_BIN), /usr/lib/$(LIBMODBUSMANAGER_BIN))
	@$(call install_link, libmodbusmanager, $(LIBMODBUSMANAGER_BIN), /usr/lib/$(LIBMODBUSMANAGER).so);

endif
	@$(call install_finish, libmodbusmanager)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(LIBMODBUSMANAGER_PACKAGE_NAME).ipk $(LIBMODBUSMANAGER_PLATFORMCONFIGPACKAGEDIR)/
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/libmodbusmanager.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, LIBMODBUSMANAGER)

