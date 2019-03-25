# -*-makefile-*-
#
# Copyright (C) 2012 by Jan Sondhauss <jan.sondhauss@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBTIMEZONEINFO) += libtimezoneinfo

#
# Paths and names
#
LIBTIMEZONEINFO_VERSION	:= 0.0.0.2
LIBTIMEZONEINFO			:= libtimezoneinfo
LIBTIMEZONEINFO_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/wide/$(LIBTIMEZONEINFO)
LIBTIMEZONEINFO_DIR		:= $(BUILDDIR)/$(LIBTIMEZONEINFO)
LIBTIMEZONEINFO_LICENSE	:= unknown

LIBTOOL_CROSS:=$(PTXCONF_SYSROOT_CROSS)/bin/
LIBTIMEZONEINFO_PATH:= PATH=$(CROSS_PATH)
LIBTIMEZONEINFO_ENV:= $(CROSS_ENV) CROSS_LIBTOOL=$(LIBTOOL_CROSS)

# parameters passed from ptxdist
# override the configured optimization when building the release version.
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	PTXCONF_LIBTIMEZONEINFO_BUILD_CFG_OPTIMIZATION=O3
endif

LIBTIMEZONEINFO_PACKAGE_NAME := $(LIBTIMEZONEINFO)_$(LIBTIMEZONEINFO_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBTIMEZONEINFO_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/libtimezoneinfo.get:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libtimezoneinfo.extract:
	@$(call targetinfo)
	@mkdir -p $(LIBTIMEZONEINFO_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=.libs/ --exclude=objs/ --exclude="*.o" --exclude="*.a" --exclude="*.so" --exclude="*.so" $(LIBTIMEZONEINFO_SRC) $(BUILDDIR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libtimezoneinfo.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	LIBTIMEZONEINFO_ENV+= CFLAGS="$(CROSS_CFLAGS) -DNDEBUG"
endif

LIBTIMEZONEINFO_BUILD_PARAMS:= 			\
	LIBTIMEZONEINFO_DEBUG_LEVEL=$(PTXCONF_LIBTIMEZONEINFO_BUILD_CFG_DEBUG_LEVEL)\
	LIBTIMEZONEINFO_COMPILER_OPTIMIZATION=$(PTXCONF_LIBTIMEZONEINFO_BUILD_CFG_OPTIMIZATION)

$(STATEDIR)/libtimezoneinfo.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	cd $(LIBTIMEZONEINFO_DIR) && \
		$(LIBTIMEZONEINFO_PATH)  \
		$(LIBTIMEZONEINFO_ENV)   \
      $(LIBTIMEZONEINFO_BUILD_PARAMS) \
		make $(PARALLELMFLAGS) 
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libtimezoneinfo.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libtimezoneinfo.targetinstall:
	@$(call targetinfo)
	@$(call install_init, libtimezoneinfo)
	@$(call install_fixup, libtimezoneinfo,PRIORITY,optional)
	@$(call install_fixup, libtimezoneinfo,SECTION,base)
	@$(call install_fixup, libtimezoneinfo,AUTHOR,"Lars Schmidt <lars.schmidt@wago.com>")
	@$(call install_fixup, libtimezoneinfo,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(LIBTIMEZONEINFO_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(LIBTIMEZONEINFO_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(LIBTIMEZONEINFO_PLATFORMCONFIGPACKAGEDIR)/$(LIBTIMEZONEINFO_PACKAGE_NAME).ipk  
	@$(call install_archive, libtimezoneinfo, 0, 0, $(LIBTIMEZONEINFO_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_copy, libtimezoneinfo, 0, 0, 0755, $(LIBTIMEZONEINFO_DIR)/lib/.libs/libtimezoneinfo.so.0.0.0, /usr/lib/libtimezoneinfo.so.0.0.0)
	@$(call install_link, libtimezoneinfo, libtimezoneinfo.so.0.0.0, /usr/lib/libtimezoneinfo.so.0)
	@$(call install_link, libtimezoneinfo, libtimezoneinfo.so.0.0.0, /usr/lib/libtimezoneinfo.so)
	@$(call install_link, libtimezoneinfo, ../libtimezoneinfo.so.0.0.0, /usr/lib/wide/libtimezoneinfo.so)
endif
	@$(call install_finish, libtimezoneinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(LIBTIMEZONEINFO_PACKAGE_NAME).ipk $(LIBTIMEZONEINFO_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

PTX_ROOT=$(PTXCONF_SYSROOT_TARGET)/../root
PTX_ROOT_DEBUG=$(PTXCONF_SYSROOT_TARGET)/../root-debug

$(STATEDIR)/libtimezoneinfo.clean:
	@$(call targetinfo)
	@-rm -rf $(LIBTIMEZONEINFO_DIR)
	@-rm -f $(PTX_ROOT)/usr/lib/libtimezoneinfo*
	@-rm -f $(PTX_ROOT_DEBUG)/usr/lib/libtimezoneinfo*
	@$(call clean_pkg, LIBTIMEZONEINFO)

# vim: syntax=make
