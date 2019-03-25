# -*-makefile-*-
# $Id: template-make 8509 2008-06-12 12:45:40Z mkl $
#
# Copyright (C) 2010 by WAGO
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBPBDPM) += libpbdpm

#
# Paths and names
#
LIBPBDPM_VERSION	:= 0.0.1
LIBPBDPM_VERSION_SHORT	:= 0
LIBPBDPM			:= libpbdpm
LIBPBDPM_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/device/profibus/master/$(LIBPBDPM)
LIBPBDPM_DIR		:= $(BUILDDIR)/$(LIBPBDPM)

LIBPBDPM_PACKAGE_NAME := $(LIBPBDPM)_$(LIBPBDPM_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBPBDPM_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------
$(STATEDIR)/libpbdpm.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------
$(STATEDIR)/libpbdpm.extract:
	$(call targetinfo)
	mkdir -p $(LIBPBDPM_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	rsync -a --exclude=".*" --exclude=objs/ --exclude=objs_test/ --exclude=lib --exclude=lib_test  $(LIBPBDPM_SRC) $(BUILDDIR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------
#

$(STATEDIR)/libpbdpm.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call xslt_compile, $(LIBPBDPM_DIR)/data/diagnostics/dpm.xml)
endif
	@$(call touch)

LIBPBDPM_ENV:= $(CROSS_ENV)
# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/libpbdpm.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	cd $(LIBPBDPM_DIR) && $(LIBPBDPM_ENV)\
		SYSROOT=$(PTXCONF_SYSROOT_TARGET)\
		CROSS_COMPILE=$(COMPILER_PREFIX)\
		$(MAKE) -f Makefile $(PARALLELMFLAGS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

PBDPM_INCLUDES:= $(PTXCONF_SYSROOT_TARGET)/usr/include/pbdpm/

$(STATEDIR)/libpbdpm.install:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

# copy the Profibus headers
	mkdir -p $(PBDPM_INCLUDES)
	mkdir -p $(PBDPM_INCLUDES)cifx/
	cp -d $(LIBPBDPM_DIR)/source/Application/dpm/*.h $(PBDPM_INCLUDES)
	cp -d $(LIBPBDPM_DIR)/source/Application/ProfibusApi/interface/ProfibusApi_Typedefs.h $(PBDPM_INCLUDES)
	cp -d $(LIBPBDPM_DIR)/lib/libpbdpm.so.0.0.0 $(PTXCONF_SYSROOT_TARGET)/usr/lib/libpbdpm.so

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup headers for later use in configs/@platform@/packages
	cd $(PTXCONF_SYSROOT_TARGET) && \
	tar -czvf $(LIBPBDPM).tgz usr/include/pbdpm/* && \
	mv $(LIBPBDPM).tgz $(LIBPBDPM_PLATFORMCONFIGPACKAGEDIR)/ 
endif
endif

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Recover header from archive in configs/@platform@/packages
	mkdir -p $(PKGDIR)/$(LIBPBDPM)
	tar -xzvf $(LIBPBDPM_PLATFORMCONFIGPACKAGEDIR)/$(LIBPBDPM).tgz -C $(PTXCONF_SYSROOT_TARGET)
endif

		@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libpbdpm.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libpbdpm)
	@$(call install_fixup, libpbdpm,PRIORITY,optional)
	@$(call install_fixup, libpbdpm,SECTION,base)
	@$(call install_fixup, libpbdpm,AUTHOR,"WAGO <www.WAGO.com>")
	@$(call install_fixup, libpbdpm,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(LIBPBDPM_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(LIBPBDPM_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(LIBPBDPM_PLATFORMCONFIGPACKAGEDIR)/$(LIBPBDPM_PACKAGE_NAME).ipk  
	@$(call install_archive, libpbdpm, 0, 0, $(LIBPBDPM_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	echo libpbdpmdir: $(LIBPBDPM_DIR)
	@$(call install_copy, libpbdpm, 0, 0, 0755, $(LIBPBDPM_DIR)/lib/libpbdpm.so.0.0.0, /usr/lib/libpbdpm.so.$(LIBPBDPM_VERSION))
	@$(call install_link, libpbdpm, ../libpbdpm.so.$(LIBPBDPM_VERSION), /usr/lib/dal/libpbdpm.so)
	@$(call install_link, libpbdpm, libpbdpm.so.$(LIBPBDPM_VERSION), /usr/lib/libpbdpm.so)
	@$(call install_link, libpbdpm, libpbdpm.so.$(LIBPBDPM_VERSION), /usr/lib/libpbdpm.so.$(LIBPBDPM_VERSION_SHORT))

	@$(call install_copy, libpbdpm, 0, 0, 0755, $(LIBPBDPM_DIR)/data/opt/netx/channel0/X0601000.nxf, /opt/netx/channel0/X0601000.nxf)
	@$(call install_copy, libpbdpm, 0, 0, 0755, $(LIBPBDPM_DIR)/data/opt/netx/NETX51-BSL.bin, /opt/netx/NETX51-BSL.bin)
endif

	@$(call install_finish, libpbdpm)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(LIBPBDPM_PACKAGE_NAME).ipk $(LIBPBDPM_PLATFORMCONFIGPACKAGEDIR)/
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libpbdpm.clean:
	@rm -rf $(STATEDIR)/libpbdpm.*
	@rm -rf $(PKGDIR)/libpbdpm_*
	@rm -rf $(LIBPBDPM_DIR)
	@rm -rf $(PBDPM_INCLUDES)
	@$(call xslt_clean, $(LIBPBDPM_DIR)/data/diagnostics/dpm.xml)


