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
PACKAGES-$(PTXCONF_LIBMODCOM) += libmodcom

#
# Paths and names
#
LIBMODCOM_VERSION	:= 1.0.0.0
LIBMODCOM			:= libmodcom
LIBMODCOM_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/wide/$(LIBMODCOM)
LIBMODCOM_DIR		:= $(BUILDDIR)/$(LIBMODCOM)
LIBMODCOM_LICENSE	:= unknown

# parameters passed from ptxdist
# override the configured optimization when building the release version.
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	PTXCONF_LIBMODCOM_BUILD_CFG_OPTIMIZATION=O3
endif

LIBMODCOM_PACKAGE_NAME := $(LIBMODCOM)_$(LIBMODCOM_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBMODCOM_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/libmodcom.get:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libmodcom.extract:
	@$(call targetinfo)
	@mkdir -p $(LIBMODCOM_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=.libs/ --exclude=objs/ --exclude="*.o" --exclude="*.a" --exclude="*.so" --exclude="*.so" $(LIBMODCOM_SRC) $(BUILDDIR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libmodcom.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

LIBTOOL_CROSS:=$(PTXCONF_SYSROOT_CROSS)/bin/
LIBMODCOM_PATH:= PATH=$(CROSS_PATH)
LIBMODCOM_ENV:= $(CROSS_ENV) CROSS_LIBTOOL=$(LIBTOOL_CROSS)

LIBMODCOM_BUILD_PARAMS:= 			\
	LIBMODCOM_DEBUG_LEVEL=$(PTXCONF_LIBMODCOM_BUILD_CFG_DEBUG_LEVEL)\
	LIBMODCOM_COMPILER_OPTIMIZATION=$(PTXCONF_LIBMODCOM_BUILD_CFG_OPTIMIZATION)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	LIBMODCOM_ENV+= CFLAGS="$(CROSS_CFLAGS) -DNDEBUG"
endif

$(STATEDIR)/libmodcom.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	cd $(LIBMODCOM_DIR) && \
		$(LIBMODCOM_PATH)  \
		$(LIBMODCOM_ENV)   \
      $(LIBMODCOM_BUILD_PARAMS) \
		make $(PARALLELMFLAGS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libmodcom.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libmodcom.targetinstall:
	@$(call targetinfo)
	@$(call install_init, libmodcom)
	@$(call install_fixup, libmodcom,PRIORITY,optional)
	@$(call install_fixup, libmodcom,SECTION,base)
	@$(call install_fixup, libmodcom,AUTHOR,"<WAGO GmbH \& Co. KG>")
	@$(call install_fixup, libmodcom,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Extract precompiled binaries from archive
	rm -rf $(LIBMODCOM_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(LIBMODCOM_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(LIBMODCOM_PLATFORMCONFIGPACKAGEDIR)/$(LIBMODCOM_PACKAGE_NAME).ipk
	@$(call install_archive, libmodcom, 0, 0, $(LIBMODCOM_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_copy, libmodcom, 0, 0, 0755, $(LIBMODCOM_DIR)/lib/.libs/libmodcom.so.0.0.0, /usr/lib/libmodcom.so.0.0.0)
	@$(call install_link, libmodcom, libmodcom.so.0.0.0, /usr/lib/libmodcom.so.0)
	@$(call install_link, libmodcom, libmodcom.so.0.0.0, /usr/lib/libmodcom.so)
	@$(call install_link, libmodcom, ../libmodcom.so.0.0.0, /usr/lib/wide/libmodcom.so)
endif
	@$(call install_finish, libmodcom)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(LIBMODCOM_PACKAGE_NAME).ipk $(LIBMODCOM_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

PTX_ROOT=$(PTXCONF_SYSROOT_TARGET)/../root
PTX_ROOT_DEBUG=$(PTXCONF_SYSROOT_TARGET)/../root-debug

$(STATEDIR)/libmodcom.clean:
	@$(call targetinfo)
	@-rm -rf $(LIBMODCOM_DIR)
	@-rm -f $(PTX_ROOT)/usr/lib/libmodcom*
	@-rm -f $(PTX_ROOT_DEBUG)/usr/lib/libmodcom*
	@$(call clean_pkg, LIBMODCOM)

# vim: syntax=make
