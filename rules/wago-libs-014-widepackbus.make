# -*-makefile-*-
#
# Copyright (C) 2012 by <WAGO Kontakttechnik GmbH & Co. KG>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBWIDEPACKBUS) += libwidepackbus

#
# Paths and names
#
LIBWIDEPACKBUS_VERSION        := 0.1.0
LIBWIDEPACKBUS_MD5            :=
LIBWIDEPACKBUS                := libwidepackbus
LIBWIDEPACKBUS_BUILDCONFIG    := Release
LIBWIDEPACKBUS_BUILDROOT_DIR  := $(BUILDDIR)/$(LIBWIDEPACKBUS)
LIBWIDEPACKBUS_DIR            := $(LIBWIDEPACKBUS_BUILDROOT_DIR)
LIBWIDEPACKBUS_BUILD_DIR      := $(LIBWIDEPACKBUS_BUILDROOT_DIR)/bin/$(LIBWIDEPACKBUS_BUILDCONFIG)
LIBWIDEPACKBUS_BASE           := $(PTXDIST_WORKSPACE)/wago_intern/wide/$(LIBWIDEPACKBUS)
LIBWIDEPACKBUS_SRC            := $(LIBWIDEPACKBUS_BASE)/src
LIBWIDEPACKBUS_BIN            := $(LIBWIDEPACKBUS).so.$(LIBWIDEPACKBUS_VERSION)
LIBWIDEPACKBUS_LICENSE        := unknown
LIBWIDEPACKBUS_MAKE_ENV       := $(CROSS_ENV) \
 BUILDCONFIG=$(LIBWIDEPACKBUS_BUILDCONFIG) \
 BIN_DIR=$(LIBWIDEPACKBUS_BUILD_DIR) \
 SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

#LIBWIDEPACKBUS_MAKE_ENV          := $(CROSS_ENV)
#LIBWIDEPACKBUS_PATH         := PATH=$(CROSS_PATH)
#LIBWIDEPACKBUS_AUTOCONF     := $(CROSS_AUTOCONF)
#LIBWIDEPACKBUS_CONF_OPT		:= $(CROSS_AUTOCONF_USR) SYSROOT_TARGET=$(PTXCONF_SYSROOT_TARGET)

LIBWIDEPACKBUS_PACKAGE_NAME := $(LIBWIDEPACKBUS)_$(LIBWIDEPACKBUS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBWIDEPACKBUS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------
$(STATEDIR)/libwidepackbus.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------
$(STATEDIR)/libwidepackbus.extract:
	@$(call targetinfo)
	@mkdir -p $(LIBWIDEPACKBUS_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call clean, $(LIBWIDEPACKBUS_DIR))
	cp -rd $(LIBWIDEPACKBUS_BASE) $(LIBWIDEPACKBUS_DIR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------
$(STATEDIR)/libwidepackbus.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/prepare, LIBWIDEPACKBUS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
$(STATEDIR)/libwidepackbus.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/compile, LIBWIDEPACKBUS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------
$(STATEDIR)/libwidepackbus.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/install, LIBWIDEPACKBUS)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup headers for later use in configs/@platform@/packages
	cd $(PKGDIR)/$(LIBWIDEPACKBUS)/ && \
	tar -czvf $(LIBWIDEPACKBUS).tgz * && \
	mv $(LIBWIDEPACKBUS).tgz $(LIBWIDEPACKBUS_PLATFORMCONFIGPACKAGEDIR)/
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Recover header from archive in configs/@platform@/packages
	mkdir -p $(PKGDIR)/$(LIBWIDEPACKBUS)
	tar -xzvf $(LIBWIDEPACKBUS_PLATFORMCONFIGPACKAGEDIR)/$(LIBWIDEPACKBUS).tgz -C $(PKGDIR)/$(LIBWIDEPACKBUS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------
$(STATEDIR)/libwidepackbus.targetinstall:
	@$(call targetinfo)
	@$(call install_init, libwidepackbus)
	@$(call install_fixup, libwidepackbus,PRIORITY,optional)
	@$(call install_fixup, libwidepackbus,SECTION,base)
	@$(call install_fixup, libwidepackbus,AUTHOR,"<WAGO Kontakttechnik GmbH \& Co. KG>")
	@$(call install_fixup, libwidepackbus,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Extract precompiled binaries from archive
	rm -rf $(LIBWIDEPACKBUS_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(LIBWIDEPACKBUS_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(LIBWIDEPACKBUS_PLATFORMCONFIGPACKAGEDIR)/$(LIBWIDEPACKBUS_PACKAGE_NAME).ipk
	@$(call install_archive, libwidepackbus, 0, 0, $(LIBWIDEPACKBUS_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_lib, libwidepackbus, 0, 0, 0644, libwidepackbus)

#	@$(call install_copy, libwidepackbus, 0, 0, 0755, $(LIBWIDEPACKBUS_DIR)/libwidepackbus/.libs/libwidepackbus.so.0.0.0, /usr/lib/libwidepackbus.so.0.0.0)
#	@$(call install_link, libwidepackbus, libwidepackbus.so.0.0.0, /usr/lib/libwidepackbus.so.0)
#	@$(call install_link, libwidepackbus, libwidepackbus.so.0.0.0, /usr/lib/libwidepackbus.so)

	@$(call install_link, libwidepackbus, ../$(LIBWIDEPACKBUS_BIN), /usr/lib/wide/libwidepackbus.so)
endif
	@$(call install_finish, libwidepackbus)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(LIBWIDEPACKBUS_PACKAGE_NAME).ipk $(LIBWIDEPACKBUS_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------
PTX_ROOT=$(PTXCONF_SYSROOT_TARGET)/../root
PTX_ROOT_DEBUG=$(PTXCONF_SYSROOT_TARGET)/../root-debug

$(STATEDIR)/libwidepackbus.clean:
	@$(call targetinfo)
	@-rm -rf $(LIBWIDEPACKBUS_DIR)
	@-rm -f $(PTX_ROOT)/usr/lib/libwidepackbus*
	@-rm -f $(PTX_ROOT_DEBUG)/usr/lib/libwidepackbus*
	@rm -rf $(LIBWIDEPACKBUS_BUILDROOT_DIR)
	@$(call clean_pkg, LIBWIDEPACKBUS)

# vim: syntax=make
