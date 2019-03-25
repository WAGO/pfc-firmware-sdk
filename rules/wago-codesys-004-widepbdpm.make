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
PACKAGES-$(PTXCONF_LIBWIDEPBDPM) += libwidepbdpm

#
# Paths and names
#
LIBWIDEPBDPM_VERSION	:= 0.0.0
LIBWIDEPBDPM_SHORT_VERSION := 0
LIBWIDEPBDPM			:= codesyspbdpm
LIBWIDEPBDPM_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/device/profibus/master/$(LIBWIDEPBDPM)
LIBWIDEPBDPM_DIR		:= $(BUILDDIR)/$(LIBWIDEPBDPM)
LIBWIDEPBDPM_BUILD_OOT	:= YES
LIBWIDEPBDPM_LICENSE	:= unknown

LIBWIDEPBDPM_PACKAGE_NAME := libwidepbdpm_$(LIBWIDEPBDPM_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBWIDEPBDPM_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(STATEDIR)/libwidepbdpm.get:
#	@$(call targetinfo)
#	@$(call touch)


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libwidepbdpm.extract:
	@$(call targetinfo)
	mkdir -p $(LIBWIDEPBDPM_DIR)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	rsync -a --exclude=".*" --exclude=objs/ --exclude=objs_test/ --exclude=lib --exclude=lib_test  $(LIBWIDEPBDPM_SRC) $(BUILDDIR)
endif    

	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/libwidepbdpm.prepare:
	@$(call targetinfo)
	@$(call touch)
	
LIBWIDEPBDPM_ENV:= $(CROSS_ENV)
# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/libwidepbdpm.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@cd $(LIBWIDEPBDPM_DIR) && $(LIBWIDEPBDPM_ENV)\
		SYSROOT=$(PTXCONF_SYSROOT_TARGET)\
		CROSS_COMPILE=$(COMPILER_PREFIX)\
		$(MAKE) -f Makefile $(PARALLELMFLAGS)
        
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libwidepbdpm.install:
	@$(call targetinfo)
# NOTE: Mind the BUILD_VERSION_RELEASE/BINARIES switches when adding install artefacts!
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libwidepbdpm.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libwidepbdpm)
	@$(call install_fixup, libwidepbdpm, PRIORITY, optional)
	@$(call install_fixup, libwidepbdpm, SECTION, base)
	@$(call install_fixup, libwidepbdpm, AUTHOR, "Jan Sondhauss <jan.sondhauss@wago.com>")
	@$(call install_fixup, libwidepbdpm, DESCRIPTION, The libwidepbdpm library implements the codesys-specific part of the pbdpm.)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# Extract precompiled binaries from archive
	rm -rf $(LIBWIDEPBDPM_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(LIBWIDEPBDPM_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(LIBWIDEPBDPM_PLATFORMCONFIGPACKAGEDIR)/$(LIBWIDEPBDPM_PACKAGE_NAME).ipk
	@$(call install_archive, libwidepbdpm, 0, 0, $(LIBWIDEPBDPM_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)

else
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, libwidepbdpm, 0, 0, 0755, $(LIBWIDEPBDPM_DIR)/lib/libwidepbdpm.so.0.0.0, /usr/lib/libwidepbdpm.so.$(LIBWIDEPBDPM_VERSION))
	@$(call install_link, libwidepbdpm, ../libwidepbdpm.so.$(LIBWIDEPBDPM_VERSION), /usr/lib/wide/libwidepbdpm.so)

endif

	@$(call install_finish, libwidepbdpm)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(LIBWIDEPBDPM_PACKAGE_NAME).ipk $(LIBWIDEPBDPM_PLATFORMCONFIGPACKAGEDIR)/
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libwidepbdpm.clean:
	@$(call targetinfo)
	@rm -rf $(ROOTDIR)/usr/lib/libwidepbdpm*
	@rm -rf $(ROOTDIR)/usr/lib/wide/libwidepbdpm*
	@rm -rf $(ROOTDIR)/../root-debug/usr/lib/libwidepbdpm*
	@rm -rf $(ROOTDIR)/../root-debug/usr/lib/wide/libwidepbdpm*
	@$(call clean_pkg, LIBWIDEPBDPM)

# vim: syntax=make
