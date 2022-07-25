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
PACKAGES-$(PTXCONF_LIBDALDUMMY) += libdaldummy

#
# Paths and names
#
LIBDALDUMMY_VERSION	:= 1.0.0.1
LIBDALDUMMY			:= libdaldummy
LIBDALDUMMY_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/device/$(LIBDALDUMMY)
LIBDALDUMMY_DIR		:= $(BUILDDIR)/$(LIBDALDUMMY)
LIBDALDUMMY_LICENSE	:= unknown

LIBTOOL_CROSS:=$(PTXCONF_SYSROOT_CROSS)/bin/
LIBDALDUMMY_ENV:= $(CROSS_ENV) CROSS_LIBTOOL=$(LIBTOOL_CROSS)
LIBDALDUMMY_PATH:= PATH=$(CROSS_PATH)

# parameters passed from ptxdist
# override the configured optimization when building the release version.
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	PTXCONF_LIBDALDUMMY_BUILD_CFG_OPTIMIZATION=O3
endif

LIBDALDUMMY_PACKAGE_NAME := $(LIBDALDUMMY)_$(LIBDALDUMMY_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBDALDUMMY_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------
$(STATEDIR)/libdaldummy.get:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libdaldummy.extract:
	@$(call targetinfo)
	@mkdir -p $(LIBDALDUMMY_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=.libs/ --exclude=objs/ --exclude="*.o" --exclude="*.a" --exclude="*.so" --exclude="*.so" $(LIBDALDUMMY_SRC) $(BUILDDIR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libdaldummy.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	LIBDALDUMMY_ENV+= CFLAGS="$(CROSS_CFLAGS) -DNDEBUG"
endif

LIBDALDUMMY_BUILD_PARAMS:= 			\
	LIBDALDUMMY_DEBUG_LEVEL=$(PTXCONF_LIBDALDUMMY_BUILD_CFG_DEBUG_LEVEL)\
	LIBDALDUMMY_COMPILER_OPTIMIZATION=$(PTXCONF_LIBDALDUMMY_BUILD_CFG_OPTIMIZATION)

$(STATEDIR)/libdaldummy.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	cd $(LIBDALDUMMY_DIR) && \
		$(LIBDALDUMMY_PATH)  \
		$(LIBDALDUMMY_ENV)   \
		$(LIBDALDUMMY_BUILD_PARAMS) \
		make $(PARALLELMFLAGS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdaldummy.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdaldummy.targetinstall:
	@$(call targetinfo)
	@$(call install_init, libdaldummy)
	@$(call install_fixup, libdaldummy,PRIORITY,optional)
	@$(call install_fixup, libdaldummy,SECTION,base)
	@$(call install_fixup, libdaldummy,AUTHOR,"<WAGO GmbH \& Co. KG>")
	@$(call install_fixup, libdaldummy,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Extract precompiled binaries from archive
	rm -rf $(LIBDALDUMMY_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(LIBDALDUMMY_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(LIBDALDUMMY_PLATFORMCONFIGPACKAGEDIR)/$(LIBDALDUMMY_PACKAGE_NAME).ipk
	@$(call install_archive, libdaldummy, 0, 0, $(LIBDALDUMMY_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_copy, libdaldummy, 0, 0, 0755, $(LIBDALDUMMY_DIR)/libdaldummy.so.0.0.1, /usr/lib/libdaldummy.so.0.0.1)
	@$(call install_link, libdaldummy, libdaldummy.so.0.0.1, /usr/lib/libdaldummy.so.0)
	@$(call install_link, libdaldummy, libdaldummy.so.0.0.1, /usr/lib/libdaldummy.so)
	@$(call install_link, libdaldummy, ../libdaldummy.so.0.0.1, /usr/lib/wide/libdaldummy.so)
endif
	@$(call install_finish, libdaldummy)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(LIBDALDUMMY_PACKAGE_NAME).ipk $(LIBDALDUMMY_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

PTX_ROOT=$(PTXCONF_SYSROOT_TARGET)/../root
PTX_ROOT_DEBUG=$(PTXCONF_SYSROOT_TARGET)/../root-debug

$(STATEDIR)/libdaldummy.clean:
	@$(call targetinfo)
	@-rm -rf $(LIBDALDUMMY_DIR)
	@-rm -f $(PTX_ROOT)/usr/lib/libdaldummy*
	@-rm -f $(PTX_ROOT_DEBUG)/usr/lib/libdaldummy*
	@$(call clean_pkg, LIBDALDUMMY)

# vim: syntax=make
