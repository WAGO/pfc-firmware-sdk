# -*-makefile-*-
#
# Copyright (C) 2012 by <WAGO GmbH & Co. KG>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBSERCOMM) += libsercomm

#
# Paths and names
#
LIBSERCOMM_VERSION	:= 1.0.0.0
LIBSERCOMM			:= libsercomm
LIBSERCOMM_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/wide/$(LIBSERCOMM)
LIBSERCOMM_DIR		:= $(BUILDDIR)/$(LIBSERCOMM)
LIBSERCOMM_LICENSE	:= unknown

LIBSERCOMM_PACKAGE_NAME := $(LIBSERCOMM)_$(LIBSERCOMM_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBSERCOMM_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# parameters passed from ptxdist
# override the configured optimization when building the release version.
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	PTXCONF_LIBSERCOMM_BUILD_CFG_OPTIMIZATION=O2
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------
$(STATEDIR)/libsercomm.get:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libsercomm.extract:
	@$(call targetinfo)
	@mkdir -p $(LIBSERCOMM_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=.libs/ --exclude=objs/ --exclude="*.o" --exclude="*.a" --exclude="*.so" --exclude="*.so" $(LIBSERCOMM_SRC) $(BUILDDIR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libsercomm.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

LIBSERCOMM_PATH:= PATH=$(CROSS_PATH)
LIBSERCOMM_ENV:= $(CROSS_ENV) CROSS_LIBTOOL=$(LIBTOOL_CROSS)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	LIBSERCOMM_ENV+=CFLAGS="$(CROSS_CFLAGS) -DNDEBUG"
endif

LIBSERCOMM_BUILD_PARAMS := \
	LIBSERCOMM_DEBUG_LEVEL=$(PTXCONF_LIBSERCOMM_BUILD_CFG_DEBUG_LEVEL) \
    LIBSERCOMM_COMPILER_OPTIMIZATION=$(PTXCONF_LIBSERCOMM_BUILD_CFG_OPTIMIZATION)

$(STATEDIR)/libsercomm.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	cd $(LIBSERCOMM_DIR) && \
		$(LIBSERCOMM_PATH) \
		$(LIBSERCOMM_ENV) \
		$(LIBSERCOMM_BUILD_PARAMS) \
		make $(PARALLELMFLAGS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libsercomm.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libsercomm.targetinstall:
	@$(call targetinfo)
	@$(call install_init, libsercomm)
	@$(call install_fixup, libsercomm,PRIORITY,optional)
	@$(call install_fixup, libsercomm,SECTION,base)
	@$(call install_fixup, libsercomm,AUTHOR,"<WAGO GmbH \& Co. KG>")
	@$(call install_fixup, libsercomm,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Extract precompiled binaries from archive
	rm -rf $(LIBSERCOMM_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(LIBSERCOMM_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(LIBSERCOMM_PLATFORMCONFIGPACKAGEDIR)/$(LIBSERCOMM_PACKAGE_NAME).ipk
	@$(call install_archive, libsercomm, 0, 0, $(LIBSERCOMM_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_copy, libsercomm, 0, 0, 0755, $(LIBSERCOMM_DIR)/lib/.libs/libsercomm.so.0.0.0, /usr/lib/libsercomm.so.0.0.0)
	@$(call install_link, libsercomm, libsercomm.so.0.0.0, /usr/lib/libsercomm.so.0)
	@$(call install_link, libsercomm, libsercomm.so.0.0.0, /usr/lib/libsercomm.so)
	@$(call install_link, libsercomm, ../libsercomm.so.0.0.0, /usr/lib/wide/libsercomm.so)
endif
	@$(call install_finish, libsercomm)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(LIBSERCOMM_PACKAGE_NAME).ipk $(LIBSERCOMM_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

PTX_ROOT=$(PTXCONF_SYSROOT_TARGET)/../root
PTX_ROOT_DEBUG=$(PTXCONF_SYSROOT_TARGET)/../root-debug

$(STATEDIR)/libsercomm.clean:
	@$(call targetinfo)
	@-rm -rf $(LIBSERCOMM_DIR)
	@-rm -f $(PTX_ROOT)/usr/lib/libsercomm*
	@-rm -f $(PTX_ROOT_DEBUG)/usr/lib/libsercomm*
	@$(call clean_pkg, LIBSERCOMM)

# vim: syntax=make
