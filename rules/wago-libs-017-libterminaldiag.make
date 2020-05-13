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
PACKAGES-$(PTXCONF_LIBTERMINALDIAG) += libterminaldiag

#
# Paths and names
#
LIBTERMINALDIAG_VERSION	:= 1.0.0.0
LIBTERMINALDIAG			:= libterminaldiag
LIBTERMINALDIAG_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/wide/$(LIBTERMINALDIAG)
LIBTERMINALDIAG_DIR		:= $(BUILDDIR)/$(LIBTERMINALDIAG)
LIBTERMINALDIAG_LICENSE	:= unknown

LIBTOOL_CROSS:=$(PTXCONF_SYSROOT_CROSS)/bin/
LIBTERMINALDIAG_ENV:= $(CROSS_ENV) CROSS_LIBTOOL=$(LIBTOOL_CROSS)
LIBTERMINALDIAG_PATH:= PATH=$(CROSS_PATH)

# parameters passed from ptxdist
# override the configured optimization when building the release version.
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	PTXCONF_LIBTERMINALDIAG_BUILD_CFG_OPTIMIZATION=O3
endif

LIBTERMINALDIAG_PACKAGE_NAME := $(LIBTERMINALDIAG)_$(LIBTERMINALDIAG_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBTERMINALDIAG_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------
$(STATEDIR)/libterminaldiag.get:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libterminaldiag.extract:
	@$(call targetinfo)
	@mkdir -p $(LIBTERMINALDIAG_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=.libs/ --exclude=objs/ --exclude="*.o" --exclude="*.a" --exclude="*.so" --exclude="*.so" $(LIBTERMINALDIAG_SRC) $(BUILDDIR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libterminaldiag.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	LIBTERMINALDIAG_ENV+= CFLAGS="$(CROSS_CFLAGS) -DNDEBUG"
endif

LIBTERMINALDIAG_BUILD_PARAMS:= 			\
	LIBTERMINALDIAG_DEBUG_LEVEL=$(PTXCONF_LIBTERMINALDIAG_BUILD_CFG_DEBUG_LEVEL)\
	LIBTERMINALDIAG_COMPILER_OPTIMIZATION=$(PTXCONF_LIBTERMINALDIAG_BUILD_CFG_OPTIMIZATION)

$(STATEDIR)/libterminaldiag.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	cd $(LIBTERMINALDIAG_DIR) && \
		$(LIBTERMINALDIAG_PATH)  \
		$(LIBTERMINALDIAG_ENV)   \
		$(LIBTERMINALDIAG_BUILD_PARAMS) \
		make $(PARALLELMFLAGS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libterminaldiag.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libterminaldiag.targetinstall:
	@$(call targetinfo)
	@$(call install_init, libterminaldiag)
	@$(call install_fixup, libterminaldiag,PRIORITY,optional)
	@$(call install_fixup, libterminaldiag,SECTION,base)
	@$(call install_fixup, libterminaldiag,AUTHOR,"<WAGO Kontakttechnik GmbH \& Co. KG>")
	@$(call install_fixup, libterminaldiag,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Extract precompiled binaries from archive
	rm -rf $(LIBTERMINALDIAG_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(LIBTERMINALDIAG_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(LIBTERMINALDIAG_PLATFORMCONFIGPACKAGEDIR)/$(LIBTERMINALDIAG_PACKAGE_NAME).ipk
	@$(call install_archive, libterminaldiag, 0, 0, $(LIBTERMINALDIAG_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_copy, libterminaldiag, 0, 0, 0755, $(LIBTERMINALDIAG_DIR)/lib/.libs/libterminaldiag.so.0.0.0, /usr/lib/libterminaldiag.so.0.0.0)
	@$(call install_link, libterminaldiag, libterminaldiag.so.0.0.0, /usr/lib/libterminaldiag.so.0)
	@$(call install_link, libterminaldiag, libterminaldiag.so.0.0.0, /usr/lib/libterminaldiag.so)
	@$(call install_link, libterminaldiag, ../libterminaldiag.so.0.0.0, /usr/lib/wide/libterminaldiag.so)
endif
	@$(call install_finish, libterminaldiag)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(LIBTERMINALDIAG_PACKAGE_NAME).ipk $(LIBTERMINALDIAG_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

PTX_ROOT=$(PTXCONF_SYSROOT_TARGET)/../root
PTX_ROOT_DEBUG=$(PTXCONF_SYSROOT_TARGET)/../root-debug

$(STATEDIR)/libterminaldiag.clean:
	@$(call targetinfo)
	@-rm -rf $(LIBTERMINALDIAG_DIR)
	@-rm -f $(PTX_ROOT)/usr/lib/libterminaldiag*
	@-rm -f $(PTX_ROOT_DEBUG)/usr/lib/libterminaldiag*
	@$(call clean_pkg, LIBTERMINALDIAG)

# vim: syntax=make
