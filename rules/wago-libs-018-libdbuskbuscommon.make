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
PACKAGES-$(PTXCONF_LIBDBUSKBUSCOMMON) += libdbuskbuscommon

#
# Paths and names
#
LIBDBUSKBUSCOMMON_VERSION	:= 1.0.0.0
LIBDBUSKBUSCOMMON			:= libdbuskbuscommon
LIBDBUSKBUSCOMMON_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/device/kbus/$(LIBDBUSKBUSCOMMON)
LIBDBUSKBUSCOMMON_DIR		:= $(BUILDDIR)/$(LIBDBUSKBUSCOMMON)
LIBDBUSKBUSCOMMON_LICENSE	:= unknown


LIBTOOL_CROSS:=$(PTXCONF_SYSROOT_CROSS)/bin/
LIBDBUSKBUSCOMMON_PATH:= PATH=$(CROSS_PATH)

# parameters passed from ptxdist
# override the configured optimization when building the release version.
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	PTXCONF_LIBDBUSKBUSCOMMON_BUILD_CFG_OPTIMIZATION=O3
endif

LIBDBUSKBUSCOMMON_PACKAGE_NAME := $(LIBDBUSKBUSCOMMON)_$(LIBDBUSKBUSCOMMON_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBDBUSKBUSCOMMON_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libdbuskbuscommon.extract:
	@$(call targetinfo)
	@mkdir -p $(LIBDBUSKBUSCOMMON_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=lib/ --exclude=.libs/ --exclude=objs/ --exclude="*.o" --exclude="*.a" --exclude="*.so" --exclude="*.so" $(LIBDBUSKBUSCOMMON_SRC) $(BUILDDIR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libdbuskbuscommon.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

LIBDBUSKBUSCOMMON_ENV:= $(CROSS_ENV) SYSROOT=$(PTXCONF_SYSROOT_TARGET) CROSS_COMPILE=$(COMPILER_PREFIX) CROSS_LIBTOOL=$(LIBTOOL_CROSS) 

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	LIBDBUSKBUSCOMMON_ENV+=CFLAGS="$(CROSS_CFLAGS) -DNDEBUG"
endif

LIBDBUSKBUSCOMMON_BUILD_PARAMS:= \
	LIBDBUSKBUSCOMMON_DEBUG_LEVEL=$(PTXCONF_LIBDBUSKBUSCOMMON_BUILD_CFG_DEBUG_LEVEL) \
	LIBDBUSKBUSCOMMON_COMPILER_OPTIMIZATION=$(PTXCONF_LIBDBUSKBUSCOMMON_BUILD_CFG_OPTIMIZATION)

$(STATEDIR)/libdbuskbuscommon.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	cd $(LIBDBUSKBUSCOMMON_DIR) && \
		$(LIBDBUSKBUSCOMMON_PATH) \
		$(LIBDBUSKBUSCOMMON_ENV) \
		$(LIBDBUSKBUSCOMMON_BUILD_PARAMS) \
		make $(PARALLELMFLAGS) 
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdbuskbuscommon.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	cp $(LIBDBUSKBUSCOMMON_DIR)/include/ldkc_kbus_information.h $(PTXCONF_SYSROOT_TARGET)/usr/include/
	cp $(LIBDBUSKBUSCOMMON_DIR)/include/ldkc_kbus_register_communication.h $(PTXCONF_SYSROOT_TARGET)/usr/include/
	-cp $(LIBDBUSKBUSCOMMON_DIR)/lib/.libs/libdbuskbuscommon.a $(PTXCONF_SYSROOT_TARGET)/usr/lib/
	-cp $(LIBDBUSKBUSCOMMON_DIR)/lib/libdbuskbuscommon.a $(PTXCONF_SYSROOT_TARGET)/usr/lib/
	cp $(LIBDBUSKBUSCOMMON_DIR)/lib/.libs/libdbuskbuscommon.la $(PTXCONF_SYSROOT_TARGET)/usr/lib/
	cp $(LIBDBUSKBUSCOMMON_DIR)/lib/.libs/libdbuskbuscommon.lai $(PTXCONF_SYSROOT_TARGET)/usr/lib/
	cp $(LIBDBUSKBUSCOMMON_DIR)/lib/.libs/libdbuskbuscommon.so $(PTXCONF_SYSROOT_TARGET)/usr/lib/
	cp $(LIBDBUSKBUSCOMMON_DIR)/lib/.libs/libdbuskbuscommon.so.0 $(PTXCONF_SYSROOT_TARGET)/usr/lib/
	cp $(LIBDBUSKBUSCOMMON_DIR)/lib/.libs/libdbuskbuscommon.so.0.0.0 $(PTXCONF_SYSROOT_TARGET)/usr/lib/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	#Backup header files as archive for later use in configs/@platform@/packages
	cd $(PTXCONF_SYSROOT_TARGET) && \
	tar -czvf $(LIBDBUSKBUSCOMMON_PACKAGE_NAME).tgz \
	usr/include/ldkc_kbus_information.h \
	usr/include/ldkc_kbus_register_communication.h \
	usr/lib/libdbuskbuscommon.la \
	usr/lib/libdbuskbuscommon.lai \
	usr/lib/libdbuskbuscommon.so \
	usr/lib/libdbuskbuscommon.so.0 \
	usr/lib/libdbuskbuscommon.so.0.0.0 && \
	mv $(LIBDBUSKBUSCOMMON_PACKAGE_NAME).tgz $(LIBDBUSKBUSCOMMON_PLATFORMCONFIGPACKAGEDIR)
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Install header files from archive
	tar -xzvf $(LIBDBUSKBUSCOMMON_PLATFORMCONFIGPACKAGEDIR)/$(LIBDBUSKBUSCOMMON_PACKAGE_NAME).tgz -C $(PTXCONF_SYSROOT_TARGET)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdbuskbuscommon.targetinstall:
	@$(call targetinfo)
	@$(call install_init, libdbuskbuscommon)
	@$(call install_fixup, libdbuskbuscommon,PRIORITY,optional)
	@$(call install_fixup, libdbuskbuscommon,SECTION,base)
	@$(call install_fixup, libdbuskbuscommon,AUTHOR,"Jan Sondhauss <jan.sondhauss@wago.com>")
	@$(call install_fixup, libdbuskbuscommon,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(LIBDBUSKBUSCOMMON_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(LIBDBUSKBUSCOMMON_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(LIBDBUSKBUSCOMMON_PLATFORMCONFIGPACKAGEDIR)/$(LIBDBUSKBUSCOMMON_PACKAGE_NAME).ipk  
	@$(call install_archive, libdbuskbuscommon, 0, 0, $(LIBDBUSKBUSCOMMON_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_copy, libdbuskbuscommon, 0, 0, 0755, $(LIBDBUSKBUSCOMMON_DIR)/lib/.libs/libdbuskbuscommon.so.0.0.0, /usr/lib/libdbuskbuscommon.so.0.0.0)
	@$(call install_link, libdbuskbuscommon, libdbuskbuscommon.so.0.0.0, /usr/lib/libdbuskbuscommon.so.0)
	@$(call install_link, libdbuskbuscommon, libdbuskbuscommon.so.0.0.0, /usr/lib/libdbuskbuscommon.so)
endif
	@$(call install_finish, libdbuskbuscommon)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(LIBDBUSKBUSCOMMON_PACKAGE_NAME).ipk $(LIBDBUSKBUSCOMMON_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

PTX_ROOT=$(PTXCONF_SYSROOT_TARGET)/../root
PTX_ROOT_DEBUG=$(PTXCONF_SYSROOT_TARGET)/../root-debug

$(STATEDIR)/libdbuskbuscommon.clean:
	@$(call targetinfo)
	@-rm -rf $(LIBDBUSKBUSCOMMON_DIR)
	@-rm -f $(PTX_ROOT)/usr/lib/libdbuskbuscommon*
	@-rm -f $(PTX_ROOT_DEBUG)/usr/lib/libdbuskbuscommon*
	@-rm -f $(PTXCONF_SYSROOT_TARGET)/usr/lib/libdbuskbuscommon*
	@-rm -f $(PTXCONF_SYSROOT_TARGET)/usr/include/ldkc_*
	@$(call clean_pkg, LIBDBUSKBUSCOMMON)

# vim: syntax=make
