# -*-makefile-*-
#
# Copyright (C) 2014 <WK>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBWAGOLIBKBUS) += libwagolibkbus

#
# Paths and names
#
LIBWAGOLIBKBUS_VERSION  := 1.0.0.0
LIBWAGOLIBKBUS          := libwagolibkbus
LIBWAGOLIBKBUS_SRC      := $(PTXDIST_WORKSPACE)/wago_intern/wide/$(LIBWAGOLIBKBUS)
LIBWAGOLIBKBUS_DIR      := $(BUILDDIR)/$(LIBWAGOLIBKBUS)
LIBWAGOLIBKBUS_LICENSE	:= unknown

# parameters passed from ptxdist
# override the configured optimization when building the release version.
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	PTXCONF_LIBWAGOLIBKBUS_BUILD_CFG_OPTIMIZATION=O3
endif

LIBWAGOLIBKBUS_PACKAGE_NAME := $(LIBWAGOLIBKBUS)_$(LIBWAGOLIBKBUS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBWAGOLIBKBUS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/libwagolibkbus.get:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libwagolibkbus.extract:
	@$(call targetinfo)
	@mkdir -p $(LIBWAGOLIBKBUS_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=.libs/ --exclude=objs/ --exclude="*.o" --exclude="*.a" --exclude="*.so" --exclude="*.so" $(LIBWAGOLIBKBUS_SRC) $(BUILDDIR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libwagolibkbus.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

LIBTOOL_CROSS:=$(PTXCONF_SYSROOT_CROSS)/bin/
LIBWAGOLIBKBUS_PATH:= PATH=$(CROSS_PATH)
LIBWAGOLIBKBUS_ENV:= $(CROSS_ENV) CROSS_LIBTOOL=$(LIBTOOL_CROSS)

LIBWAGOLIBKBUS_BUILD_PARAMS:= 			\
	LIBWAGOLIBKBUS_DEBUG_LEVEL=$(PTXCONF_LIBWAGOLIBKBUS_BUILD_CFG_DEBUG_LEVEL)\
	LIBWAGOLIBKBUS_COMPILER_OPTIMIZATION=$(PTXCONF_LIBWAGOLIBKBUS_BUILD_CFG_OPTIMIZATION)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	LIBWAGOLIBKBUS_ENV+= CFLAGS="$(CROSS_CFLAGS) -DNDEBUG"
endif

$(STATEDIR)/libwagolibkbus.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	cd $(LIBWAGOLIBKBUS_DIR) && \
		$(LIBWAGOLIBKBUS_PATH)  \
		$(LIBWAGOLIBKBUS_ENV)   \
      $(LIBWAGOLIBKBUS_BUILD_PARAMS) \
		make $(PARALLELMFLAGS) 
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libwagolibkbus.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libwagolibkbus.targetinstall:
	@$(call targetinfo)
	@$(call install_init, libwagolibkbus)
	@$(call install_fixup, libwagolibkbus,PRIORITY,optional)
	@$(call install_fixup, libwagolibkbus,SECTION,base)
	@$(call install_fixup, libwagolibkbus,AUTHOR,"<WK>")
	@$(call install_fixup, libwagolibkbus,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(LIBWAGOLIBKBUS_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(LIBWAGOLIBKBUS_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(LIBWAGOLIBKBUS_PLATFORMCONFIGPACKAGEDIR)/$(LIBWAGOLIBKBUS_PACKAGE_NAME).ipk  
	@$(call install_archive, libwagolibkbus, 0, 0, $(LIBWAGOLIBKBUS_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_copy, libwagolibkbus, 0, 0, 0755, $(LIBWAGOLIBKBUS_DIR)/lib/.libs/libwagolibkbus.so.0.0.0, /usr/lib/libwagolibkbus.so.0.0.0)
	@$(call install_link, libwagolibkbus, libwagolibkbus.so.0.0.0, /usr/lib/libwagolibkbus.so.0)
	@$(call install_link, libwagolibkbus, libwagolibkbus.so.0.0.0, /usr/lib/libwagolibkbus.so)
	@$(call install_link, libwagolibkbus, ../libwagolibkbus.so.0.0.0, /usr/lib/wide/libwagolibkbus.so)
endif
	@$(call install_finish, libwagolibkbus)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(LIBWAGOLIBKBUS_PACKAGE_NAME).ipk $(LIBWAGOLIBKBUS_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

PTX_ROOT=$(PTXCONF_SYSROOT_TARGET)/../root
PTX_ROOT_DEBUG=$(PTXCONF_SYSROOT_TARGET)/../root-debug

$(STATEDIR)/libwagolibkbus.clean:
	@$(call targetinfo)
	@-rm -rf $(LIBWAGOLIBKBUS_DIR)
	@-rm -f $(PTX_ROOT)/usr/lib/libwagolibkbus*
	@-rm -f $(PTX_ROOT_DEBUG)/usr/lib/libwagolibkbus*
	@$(call clean_pkg, LIBWAGOLIBKBUS)

# vim: syntax=make
