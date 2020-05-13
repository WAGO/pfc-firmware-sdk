# -*-makefile-*-
#
# Copyright (C) 2017 by sebastian.smolorz@wago.com
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_DRM) += drm

#
# Paths and names
#
DRM                 := drm
DRM_VERSION         := 1.0.1
DRM_URL             := file://$(PTXDIST_WORKSPACE)/wago_intern/$(DRM)
DRM_BUILDCONFIG     := Release
DRM_SRC_DIR         := $(PTXDIST_WORKSPACE)/wago_intern/$(DRM)
DRM_BUILDROOT_DIR   := $(BUILDDIR)/$(DRM)
DRM_DIR             := $(DRM_BUILDROOT_DIR)
DRM_BUILD_DIR       := $(DRM_BUILDROOT_DIR)/bin/$(DRM_BUILDCONFIG)
DRM_LICENSE         := unknown
DRM_BIN             := $(DRM)
DRM_CONF_TOO        := NO
DRM_MAKE_ENV        := $(CROSS_ENV) \
    BUILDCONFIG=$(DRM_BUILDCONFIG) \
    BIN_DIR=$(DRM_BUILD_DIR) \
    SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build


DRM_PACKAGE_NAME                := $(DRM)_$(DRM_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
DRM_PLATFORMCONFIGPACKAGEDIR    := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/drm.extract:
	@$(call targetinfo)
	@$(call clean, $(DRM_DIR))
	@mkdir -p $(DRM_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=".project" --exclude=".cproject" $(DRM_SRC_DIR) $(BUILDDIR)
#	@$(call extract, DRM)
	@$(call patchin, DRM)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

# DRM_PATH	:= PATH=$(CROSS_PATH)
# DRM_CONF_TOOL	:= NO
# DRM_MAKE_ENV	:= $(CROSS_ENV)

$(STATEDIR)/drm.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/prepare, DRM)
	@$(call xslt_compile, $(DRM_DIR)/xml/drm_diag.xml)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/drm.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/compile, DRM)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/drm.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/install, DRM)


ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# Backup headers for later use in configs/@platform@/packages
	cd $(PKGDIR)/$(DRM)/ && \
	tar -czvf $(DRM).tgz * && \
	mv $(DRM).tgz $(DRM_PLATFORMCONFIGPACKAGEDIR)/
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# Recover header from archive in configs/@platform@/packages
	mkdir -p $(PKGDIR)/$(DRM)
	tar -xzvf $(DRM_PLATFORMCONFIGPACKAGEDIR)/$(DRM).tgz -C $(PKGDIR)/$(DRM)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/drm.targetinstall:
	@$(call targetinfo)

	@$(call install_init, drm)
	@$(call install_fixup, drm,PRIORITY,optional)
	@$(call install_fixup, drm,SECTION,base)
	@$(call install_fixup, drm,AUTHOR,"sebastian.smolorz@wago.com")
	@$(call install_fixup, drm,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# Extract precompiled binaries from archive
	rm -rf $(DRM_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(DRM_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(DRM_PLATFORMCONFIGPACKAGEDIR)/$(DRM_PACKAGE_NAME).ipk
	@$(call install_archive, drm, 0, 0, $(DRM_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE 
#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish
#
	@$(call install_copy, drm, 0, 0, 0755, $(DRM_BUILD_DIR)/drm.elf, /usr/bin/drm)

	@$(call install_copy, drm, 0, 0, 0444, \
		$(PTXDIST_WORKSPACE)/projectroot/usr/share/drm/drm.pub, \
		/usr/share/drm/drm.pub, n)

	@$(call install_copy, drm, 0, 0, 0755, /var/lib/device/drm)

	@$(call install_copy, drm, 0, 0, 0755, /var/lib/device/.drm)

	@$(call install_copy, drm, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/init.d/drm, \
		/etc/init.d/drm, n)

	@$(call install_link, drm, ../init.d/drm, /etc/rc.d/S15_drm)
endif
	@$(call install_finish, drm)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(DRM_PACKAGE_NAME).ipk $(DRM_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/drm.clean:
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@-$(call xslt_clean, $(DRM_DIR)/xml/drm_diag.xml)
endif
	@$(call targetinfo)
	@$(call clean_pkg, DRM)
	@-rm -rf $(DRM_DIR)

# vim: syntax=make
