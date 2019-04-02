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
PACKAGES-$(PTXCONF_LIBCTCALLHELPER) += libctcallhelper

#
# Paths and names
#
LIBCTCALLHELPER_VERSION	:= 1.0.0.0
LIBCTCALLHELPER			:= libctcallhelper
LIBCTCALLHELPER_SRC		:= $(SRCDIR)/$(LIBCTCALLHELPER)
LIBCTCALLHELPER_DIR		:= $(BUILDDIR)/$(LIBCTCALLHELPER)
LIBCTCALLHELPER_LICENSE	:= unknown

LIBCTCALLHELPER_PACKAGE_NAME := $(LIBCTCALLHELPER)_$(LIBCTCALLHELPER_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBCTCALLHELPER_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# parameters passed from ptxdist
# override the configured optimization when building the release version.
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	PTXCONF_LIBCTCALLHELPER_BUILD_CFG_OPTIMIZATION=O2
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------
$(STATEDIR)/libctcallhelper.get:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libctcallhelper.extract:
	@$(call targetinfo)
	@mkdir -p $(LIBCTCALLHELPER_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=.libs/ --exclude=objs/ --exclude="*.o" --exclude="*.a" --exclude="*.so" --exclude="*.so" $(LIBCTCALLHELPER_SRC) $(BUILDDIR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libctcallhelper.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

LIBCTCALLHELPER_PATH:= PATH=$(CROSS_PATH)
LIBCTCALLHELPER_ENV:= $(CROSS_ENV) CROSS_LIBTOOL=$(LIBTOOL_CROSS)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	LIBCTCALLHELPER_ENV+=CFLAGS="$(CROSS_CFLAGS) -DNDEBUG"
endif

LIBCTCALLHELPER_BUILD_PARAMS := \
	LIBCTCALLHELPER_DEBUG_LEVEL=$(PTXCONF_LIBCTCALLHELPER_BUILD_CFG_DEBUG_LEVEL) \
    LIBCTCALLHELPER_COMPILER_OPTIMIZATION=$(PTXCONF_LIBCTCALLHELPER_BUILD_CFG_OPTIMIZATION)

$(STATEDIR)/libctcallhelper.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	cd $(LIBCTCALLHELPER_DIR) && \
		$(LIBCTCALLHELPER_PATH) \
		$(LIBCTCALLHELPER_ENV) \
		$(LIBCTCALLHELPER_BUILD_PARAMS) \
		make $(PARALLELMFLAGS) 
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libctcallhelper.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	cp $(LIBCTCALLHELPER_DIR)/inc/*.h $(PTXCONF_SYSROOT_TARGET)/usr/include/
	cp $(LIBCTCALLHELPER_DIR)/lib/.libs/* $(PTXCONF_SYSROOT_TARGET)/usr/lib/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	#Backup header files as archive for later use in configs/@platform@/packages
	cd $(PTXCONF_SYSROOT_TARGET) && \
	tar -czvf $(LIBCTCALLHELPER_PACKAGE_NAME).tgz \
	usr/include/ct_call_helper.h \
	usr/lib/libctcallhelper.la  \
	usr/lib/libctcallhelper.lai  \
	usr/lib/libctcallhelper.so  \
	usr/lib/libctcallhelper.so.0  \
	usr/lib/libctcallhelper.so.0.0.0 && \
	mv $(LIBCTCALLHELPER_PACKAGE_NAME).tgz $(LIBCTCALLHELPER_PLATFORMCONFIGPACKAGEDIR)
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Install header files from archive
	tar -xzvf $(LIBCTCALLHELPER_PLATFORMCONFIGPACKAGEDIR)/$(LIBCTCALLHELPER_PACKAGE_NAME).tgz -C $(PTXCONF_SYSROOT_TARGET)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libctcallhelper.targetinstall:
	@$(call targetinfo)
	@$(call install_init, libctcallhelper)
	@$(call install_fixup, libctcallhelper,PRIORITY,optional)
	@$(call install_fixup, libctcallhelper,SECTION,base)
	@$(call install_fixup, libctcallhelper,AUTHOR,"Jan Sondhauss <jan.sondhauss@wago.com>")
	@$(call install_fixup, libctcallhelper,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(LIBCTCALLHELPER_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(LIBCTCALLHELPER_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(LIBCTCALLHELPER_PLATFORMCONFIGPACKAGEDIR)/$(LIBCTCALLHELPER_PACKAGE_NAME).ipk  
	@$(call install_archive, libctcallhelper, 0, 0, $(LIBCTCALLHELPER_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_copy, libctcallhelper, 0, 0, 0755, $(LIBCTCALLHELPER_DIR)/lib/.libs/libctcallhelper.so.0.0.0, /usr/lib/libctcallhelper.so.0.0.0)
	@$(call install_link, libctcallhelper, libctcallhelper.so.0.0.0, /usr/lib/libctcallhelper.so.0)
	@$(call install_link, libctcallhelper, libctcallhelper.so.0.0.0, /usr/lib/libctcallhelper.so)
endif
	@$(call install_finish, libctcallhelper)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(LIBCTCALLHELPER_PACKAGE_NAME).ipk $(LIBCTCALLHELPER_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

PTX_ROOT=$(PTXCONF_SYSROOT_TARGET)/../root
PTX_ROOT_DEBUG=$(PTXCONF_SYSROOT_TARGET)/../root-debug

$(STATEDIR)/libctcallhelper.clean:
	@$(call targetinfo)
	@-rm -rf $(LIBCTCALLHELPER_DIR)
	@-rm -f $(PTX_ROOT)/usr/lib/libctcallhelper*
	@-rm -f $(PTX_ROOT_DEBUG)/usr/lib/libctcallhelper*
	@$(call clean_pkg, LIBCTCALLHELPER)

# vim: syntax=make
