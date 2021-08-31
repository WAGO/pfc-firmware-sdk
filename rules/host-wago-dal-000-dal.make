# -*-makefile-*-
# $Id: template-make 8509 2008-06-12 12:45:40Z mkl $
#
# Copyright (C) 2012 by WAGO
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_DAL) += host-dal

#
# Paths and names
#
HOST_DAL_VERSION		:= 0.0.1
HOST_DAL					:= host-dal
HOST_DAL_URL			:= file://$(PTXDIST_WORKSPACE)/wago_intern/$(HOST_DAL)
HOST_DAL_DIR			:= $(BUILDDIR)/../build-host/$(HOST_DAL)-$(HOST_DAL_VERSION)
HOST_DAL_SRC     		:= $(PTXDIST_WORKSPACE)/wago_intern/dal

HOST_DAL_PACKAGE_NAME := $(HOST_DAL)_$(HOST_DAL_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
HOST_DAL_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------
$(STATEDIR)/host-dal.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/host-dal.extract:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	rsync -a --exclude=".*" --exclude="host-dal_tests" --exclude=".doc" --exclude=project/output/ --exclude="*.d" --exclude="*.o" --exclude="libhost-dal.a" $(HOST_DAL_SRC)/* $(HOST_DAL_DIR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

# extra vars for host unit tests
HOST_DAL_ENV := $(HOST_ENV) CPPUTEST_HOME=$(HOST_CPPUTEST_DIR) _SYSROOT_HOST=$(PTXCONF_SYSROOT_HOST)

$(STATEDIR)/host-dal.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
SYSROOT_TARGET = $(PTXDIST_PLATFORMDIR)/sysroot-host

$(STATEDIR)/host-dal.compile:
	@$(call targetinfo)
   	# build a host version of HOST_DAL to run the host-side unit tests on (build lib example first as it is needed for the tests) 
	cd $(HOST_DAL_DIR)/project && SYSROOT_TARGET=$(PTXCONF_SYSROOT_HOST) $(HOST_DAL_ENV) $(MAKE) -f Makefile  $(PARALLELMFLAGS) examples
	cd $(HOST_DAL_DIR) && SYSROOT_TARGET=$(PTXCONF_SYSROOT_HOST) $(HOST_DAL_ENV) $(MAKE) -f tests/Makefile  $(PARALLELMFLAGS)
	cd $(HOST_DAL_DIR)/project && SYSROOT_TARGET=$(PTXCONF_SYSROOT_HOST) $(HOST_DAL_ENV) $(MAKE) -f Makefile  $(PARALLELMFLAGS)

	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-dal.install:
	@$(call targetinfo)
	mkdir -p $(PTXCONF_SYSROOT_HOST)/usr/include/dal/events
	mkdir -p $(PTXCONF_SYSROOT_HOST)/usr/lib/
	cp $(HOST_DAL_DIR)/include/adi_application_interface.h  $(PTXCONF_SYSROOT_HOST)/usr/include/dal/
	cp $(HOST_DAL_DIR)/include/dal_limits.h  $(PTXCONF_SYSROOT_HOST)/usr/include/dal/
	cp $(HOST_DAL_DIR)/include/dal_caps.h  $(PTXCONF_SYSROOT_HOST)/usr/include/dal/
	cp $(HOST_DAL_DIR)/include/dal_types.h  $(PTXCONF_SYSROOT_HOST)/usr/include/dal/
	cp $(HOST_DAL_DIR)/include/sdi_stack_interface.h  $(PTXCONF_SYSROOT_HOST)/usr/include/dal/
	cp $(HOST_DAL_DIR)/include/events/events.h $(PTXCONF_SYSROOT_HOST)/usr/include/dal/events/
	cp $(HOST_DAL_DIR)/project/libdal.a $(PTXCONF_SYSROOT_HOST)/usr/lib/

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

$(STATEDIR)/host-dal.clean:
	rm -rf $(STATEDIR)/host-dal*
	rm -rf $(PKGDIR)/host-dal*
	rm -rf $(HOST_DAL_DIR)
	rm -rf $(PTXCONF_SYSROOT_HOST)/usr/include/host-dal
	rm -rf $(PTXCONF_SYSROOT_HOST)/usr/lib/libdal.a

