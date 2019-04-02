# -*-makefile-*-
#
# Copyright (C) 2012 by Jan Sondhauss <jan.sondhauss$(SILENCE)wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBWIDECPULIMIT) += libwidecpulimit

#
# Paths and names
#
LIBWIDECPULIMIT_VERSION	:= 1.0.0.0
LIBWIDECPULIMIT			:= libwidecpulimit
LIBWIDECPULIMIT_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/wide/$(LIBWIDECPULIMIT)
LIBWIDECPULIMIT_DIR		:= $(BUILDDIR)/$(LIBWIDECPULIMIT)
LIBWIDECPULIMIT_LICENSE	:= unknown

SILENCE=@

# parameters passed from ptxdist
# override the configured optimization when building the release version.
PTXCONF_LIBWIDECPULIMIT_BUILD_CFG_OPTIMIZATION=O1
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	PTXCONF_LIBWIDECPULIMIT_BUILD_CFG_OPTIMIZATION=O3
endif

LIBWIDECPULIMIT_PACKAGE_NAME := $(LIBWIDECPULIMIT)_$(LIBWIDECPULIMIT_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBWIDECPULIMIT_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/libwidecpulimit.get:
	$(SILENCE)$(call targetinfo)
	$(SILENCE)$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libwidecpulimit.extract:
	$(SILENCE)$(call targetinfo)
	$(SILENCE)mkdir -p $(LIBWIDECPULIMIT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	$(SILENCE)rsync -a --exclude=.libs/ --exclude=objs/ --exclude="*.o" --exclude="*.a" --exclude="*.so" --exclude="*.so" $(LIBWIDECPULIMIT_SRC) $(BUILDDIR)
endif
	$(SILENCE)$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libwidecpulimit.prepare:
	$(SILENCE)$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(SILENCE)$(call xslt_compile, $(LIBWIDECPULIMIT_DIR)/source/xml/libwidecpulimit.xml)
endif
	$(SILENCE)$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

LIBWIDECPULIMIT_PATH:= PATH=$(CROSS_PATH)
LIBWIDECPULIMIT_ENV:= $(CROSS_ENV)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	LIBWIDECPULIMIT_ENV+=CFLAGS="$(CROSS_CFLAGS) -DNDEBUG"
endif

LIBWIDECPULIMIT_BUILD_PARAMS:= 			\
	LIBWIDECPULIMIT_DEBUG_LEVEL=$(PTXCONF_LIBWIDECPULIMIT_BUILD_CFG_DEBUG_LEVEL)\
	LIBWIDECPULIMIT_COMPILER_OPTIMIZATION=$(PTXCONF_LIBWIDECPULIMIT_BUILD_CFG_OPTIMIZATION)

$(STATEDIR)/libwidecpulimit.compile:
	$(SILENCE)$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	$(SILENCE)cd $(LIBWIDECPULIMIT_DIR) && \
		$(LIBWIDECPULIMIT_PATH)  \
		$(LIBWIDECPULIMIT_ENV)   \
        $(LIBWIDECPULIMIT_BUILD_PARAMS) \
		$(MAKE) $(PARALLELMFLAGS) all
endif
	$(SILENCE)$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libwidecpulimit.install:
	$(SILENCE)$(call targetinfo)
	$(SILENCE)$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libwidecpulimit.targetinstall:
	$(SILENCE)$(call targetinfo)
	$(SILENCE)$(call install_init, libwidecpulimit)
	$(SILENCE)$(call install_fixup, libwidecpulimit,PRIORITY,optional)
	$(SILENCE)$(call install_fixup, libwidecpulimit,SECTION,base)
	$(SILENCE)$(call install_fixup, libwidecpulimit,AUTHOR,"Jan Sondhauss <jan.sondhauss$(SILENCE)wago.com>")
	$(SILENCE)$(call install_fixup, libwidecpulimit,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Extract precompiled binaries from archive
	$(SILENCE)rm -rf $(LIBWIDECPULIMIT_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	$(SILENCE)cd $(LIBWIDECPULIMIT_PLATFORMCONFIGPACKAGEDIR)/tmp && \
		ar -xov $(LIBWIDECPULIMIT_PLATFORMCONFIGPACKAGEDIR)/$(LIBWIDECPULIMIT_PACKAGE_NAME).ipk
	$(SILENCE)$(call install_archive, libwidecpulimit, 0, 0, $(LIBWIDECPULIMIT_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	$(SILENCE)$(call install_copy, libwidecpulimit, 0, 0, 0755, $(LIBWIDECPULIMIT_DIR)/lib/libwidecpulimit.so.0.0.0, /usr/lib/libwidecpulimit.so.0.0.0)
	$(SILENCE)$(call install_link, libwidecpulimit, libwidecpulimit.so.0.0.0, /usr/lib/libwidecpulimit.so.0)
	$(SILENCE)$(call install_link, libwidecpulimit, libwidecpulimit.so.0.0.0, /usr/lib/libwidecpulimit.so)
	$(SILENCE)$(call install_link, libwidecpulimit, ../libwidecpulimit.so.0.0.0, /usr/lib/wide/libwidecpulimit.so)
endif
	$(SILENCE)$(call install_finish, libwidecpulimit)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/$(SILENCE)platform$(SILENCE)/packages/
	$(SILENCE)cp $(PKGDIR)/$(LIBWIDECPULIMIT_PACKAGE_NAME).ipk $(LIBWIDECPULIMIT_PLATFORMCONFIGPACKAGEDIR)/
endif
	$(SILENCE)$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

PTX_ROOT=$(PTXCONF_SYSROOT_TARGET)/../root
PTX_ROOT_DEBUG=$(PTXCONF_SYSROOT_TARGET)/../root-debug

$(STATEDIR)/libwidecpulimit.clean:
	$(SILENCE)$(call targetinfo)
	$(SILENCE)-rm -rf $(LIBWIDECPULIMIT_DIR)
	$(SILENCE)-rm -f $(PTX_ROOT)/usr/lib/libwidecpulimit*
	$(SILENCE)-rm -f $(PTX_ROOT_DEBUG)/usr/lib/libwidecpulimit*
	$(SILENCE)$(call xslt_clean, $(LIBWIDECPULIMIT_DIR)/source/xml/libwidecpulimit.xml)
	$(SILENCE)$(call clean_pkg, LIBWIDECPULIMIT)

# vim: syntax=make
