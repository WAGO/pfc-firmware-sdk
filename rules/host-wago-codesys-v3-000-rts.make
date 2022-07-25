## -*-makefile-*-
# $Id$
#
# Copyright (C) 2013 by WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_CODESYS3) += host-codesys3

HOST_CODESYS3_VERSION    := 3.5SP3
HOST_CODESYS3            := CODESYSControl_V3_WAGO_$(HOST_CODESYS3_VERSION)
HOST_CODESYS3_SUFFIX     := tar.gz
HOST_CODESYS3_DIR        := $(BUILDDIR)/../build-host/$(HOST_CODESYS3)
HOST_CODESYS3_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/plc/codesys/codesys-3.5.3.0/$(HOST_CODESYS3).$(HOST_CODESYS3_SUFFIX)
HOST_CODESYS3_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/plc/codesys/codesys-$(HOST_CODESYS3_VERSION)
HOST_CODESYS3_SPEC_DIR   := $(HOST_CODESYS3_SRC_DIR)/wago_specific/
HOST_CODESYS3_SRC        := $(HOST_CODESYS3_SRC_DIR)/$(HOST_CODESYS3).$(HOST_CODESYS3_SUFFIX)
HOST_CODESYS3_CMP_SRC_DIR := $(HOST_CODESYS3_SRC_DIR)/WagoComponents/
HOST_CODESYS3_CMP_DIR    := $(HOST_CODESYS3_DIR)/Platforms/Linux/Projects/Wago

HOST_CODESYS3_PACKAGE_NAME := plclinuxrt-v3_$(HOST_CODESYS3_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
HOST_CODESYS3_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/host-codesys3.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/host-codesys3.extract:
	@$(call targetinfo)
	mkdir -p $(HOST_CODESYS3_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
#	unzip $(HOST_CODESYS3_URL) Components/* Platforms/Linux/* -d $(HOST_CODESYS3_DIR)
#	@$(call extract, HOST_CODESYS3, $(host-codesys3_DIR))
	cd $(HOST_CODESYS3_DIR) && tar -xf $(HOST_CODESYS3_SRC) Components Platforms/Linux
#	rsync -a --exclude=Documentation \
#				--exclude=Templates/ \
#				--exclude=RtsConfigurator/ \
#				--exclude=Placeholder \
#				--exclude=BuildUtils/ \
#				--exclude=.svn/ \
#				--exclude=".*" \
#				--exclude="*.d" \
#				--exclude="*.o" \
#				--exclude="*.pdf"  \
#				--exclude="*tar.bz2" \
#				$(HOST_CODESYS3_SRC_DIR) $(HOST_CODESYS3_DIR)
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(HOST_CODESYS3_SPEC_DIR) $(HOST_CODESYS3_DIR)
#copy Wago Components
	mkdir -p $(HOST_CODESYS3_CMP_DIR)
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
                $(HOST_CODESYS3_CMP_SRC_DIR) $(HOST_CODESYS3_CMP_DIR)

	$(call patchin, HOST_CODESYS3)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# AGa: only include CROSS_ENV_PROGS to env as 3S' Makefile doesn't support providing CFLAGs and friends
#     via command line out of the box. Try to alter 3S' code as little as possible (override directive
#     would be way to go otherwise).
#
HOST_CODESYS3_ENV:= $(HOST_ENV) BASE_PATH=$(HOST_CODESYS3_DIR) \
                    DELIVERY_VERSION=testversion \
                    BUILD_CONFIG=wago_build.config \
                    SYSROOT=$(PTXCONF_SYSROOT_HOST) \
                    ARCH=x86


ifdef PTXCONF_HOST_CODESYS3_DEBUG
HOST_CODESYS3_ENV += DEBUG=1
endif

$(STATEDIR)/host-codesys3.prepare:
	@$(call targetinfo)
	cd $(HOST_CODESYS3_DIR)/Platforms/Linux/Projects && \
  $(MAKE) copy_makefile $(HOST_CODESYS3_ENV) && \
  $(MAKE) copy_sysfiles $(HOST_CODESYS3_ENV)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
HOST_CODESYS3_PATH:= PATH=$(CROSS_PATH)


$(STATEDIR)/host-codesys3.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	cd $(HOST_CODESYS3_DIR)/Platforms/Linux/Projects && \
		$(HOST_CODESYS3_PATH) \
		$(MAKE) $(PARALLELMFLAGS) clean && \
    $(MAKE) $(PARALLELMFLAGS) ext $(HOST_CODESYS3_ENV) && \
    $(MAKE) $(PARALLELMFLAGS) all $(HOST_CODESYS3_ENV)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-codesys3.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
# TODO: copy all needed headers to an intermediate directory
# See plclinux_rt[v2] for details
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#TODO: Backup header files as archive for later use in configs/@platform@/packages

endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
# TODO: enable when the header handling above has been implemented
#	tar -xzvf $(HOST_CODESYS3_PLATFORMCONFIGPACKAGEDIR)/$(HOST_CODESYS3_PACKAGE_NAME).tgz -C $(PTXCONF_SYSROOT_HOST)
#	tar -xzvf $(HOST_CODESYS3_PLATFORMCONFIGPACKAGEDIR)/$(HOST_CODESYS3_PACKAGE_NAME).tgz -C $(PKGDIR)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------
# Host, image and cross packages don’t need to install anything in the target
# file system. Therefore, PTXdist only respects the targetinstall and
# targetinstall.post stages for packages whose name doesn’t start with host-,
# image-, or cross-.

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/host-codesys3.clean:
	rm -rf $(STATEDIR)/host-codesys3.*
	rm -rf $(PKGDIR)/host-codesys3*
	rm -rf $(HOST_CODESYS3_DIR)
