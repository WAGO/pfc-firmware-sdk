# -*-makefile-*-
#
# Copyright (C) 2016 by <WAGO GmbH & Co. KG>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBMODBUSCONFIG) += libmodbusconfig

#
# Paths and names
#
LIBMODBUSCONFIG_VERSION_MAJOR := 0
LIBMODBUSCONFIG_VERSION_MINOR := 0
LIBMODBUSCONFIG_VERSION_MICRO := 1
LIBMODBUSCONFIG_VERSION := $(LIBMODBUSCONFIG_VERSION_MAJOR).$(LIBMODBUSCONFIG_VERSION_MINOR).$(LIBMODBUSCONFIG_VERSION_MICRO)
LIBMODBUSCONFIG		:= libModbusConfig
LIBMODBUSCONFIG_URL		:= file://$(PTXDIST_WORKSPACE)/wago_intern/device/modbus/config/
LIBMODBUSCONFIG_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/device/modbus/config/
LIBMODBUSCONFIG_DIR		:= $(BUILDDIR)/$(LIBMODBUSCONFIG)
LIBMODBUSCONFIG_MD5		:=
LIBMODBUSCONFIG_LICENSE	:= unknown

LIBMODBUSCONFIG_PACKAGE_NAME := libmodbusconfig_$(LIBMODBUSCONFIG_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBMODBUSCONFIG_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libmodbusconfig.extract:
	@$(call targetinfo)
	@$(call clean, $(LIBMODBUSCONFIG_DIR))
	@mkdir -p $(LIBMODBUSCONFIG_DIR)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	rsync -a --exclude=".project" --exclude=".cproject"  $(LIBMODBUSCONFIG_SRC) $(LIBMODBUSCONFIG_DIR)
	@$(call patchin, LIBMODBUSCONFIG)
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#LIBMODBUSCONFIG_PATH	:= PATH=$(CROSS_PATH)
LIBMODBUSCONFIG_CONF_TOOL	:= NO
LIBMODBUSCONFIG_MAKE_ENV	:= $(CROSS_ENV)

$(STATEDIR)/libmodbusconfig.prepare:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	
	@$(call world/prepare, LIBMODBUSCONFIG)
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/libmodbusconfig.compile:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call world/compile, LIBMODBUSCONFIG)
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

# Add the install artefacts (headers and libs) to the according lists.
# Use relative paths with the build directory as root.
LIBMODBUSCONFIG_HEADERS=MbConfig.h
LIBMODBUSCONFIG_LIBS=$(LIBMODBUSCONFIG).so.$(LIBMODBUSCONFIG_VERSION)


$(STATEDIR)/libmodbusconfig.install:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@mkdir -p $(PKGDIR)/$(LIBMODBUSCONFIG)

# Add additional headers into the loop
	for BASEDIR in $(PTXCONF_SYSROOT_TARGET) $(PKGDIR)/$(LIBMODBUSCONFIG); do \
	    install -D $(LIBMODBUSCONFIG_DIR)/MbConfig.h $${BASEDIR}/usr/include/MbConfig.h; \
    done

# Add additional libraries into the loop
	for BASEDIR in $(PTXCONF_SYSROOT_TARGET) $(PKGDIR)/$(LIBMODBUSCONFIG); do \
        install -D $(LIBMODBUSCONFIG_DIR)/$(LIBMODBUSCONFIG).so.$(LIBMODBUSCONFIG_VERSION) $${BASEDIR}/usr/lib/$(LIBMODBUSCONFIG).so; \
    done

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# Backup headers for later use in configs/@platform@/packages
	cd $(PKGDIR)/$(LIBMODBUSCONFIG)/ && \
    tar -czvf $(LIBMODBUSCONFIG).tgz * && \
	mv $(LIBMODBUSCONFIG).tgz $(LIBMODBUSCONFIG_PLATFORMCONFIGPACKAGEDIR)/
endif

endif

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# Recover header from archive in configs/@platform@/packages
	mkdir -p $(PKGDIR)/$(LIBMODBUSCONFIG)
	tar -xzvf $(LIBMODBUSCONFIG_PLATFORMCONFIGPACKAGEDIR)/$(LIBMODBUSCONFIG).tgz -C $(PKGDIR)/$(LIBMODBUSCONFIG)
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libmodbusconfig.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libmodbusconfig)

	@$(call install_fixup, libmodbusconfig,PRIORITY,optional)
	@$(call install_fixup, libmodbusconfig,SECTION,base)
	@$(call install_fixup, libmodbusconfig,AUTHOR,"<WAGO GmbH \& Co. KG>")
	@$(call install_fixup, libmodbusconfig,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
# Extract precompiled binaries from archive
	rm -rf $(LIBMODBUSCONFIG_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(LIBMODBUSCONFIG_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(LIBMODBUSCONFIG_PLATFORMCONFIGPACKAGEDIR)/$(LIBMODBUSCONFIG_PACKAGE_NAME).ipk  
	@$(call install_archive, libmodbusconfig, 0, 0, $(LIBMODBUSCONFIG_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE	

	@$(call install_copy, libmodbusconfig, 0, 0, 0755, $(LIBMODBUSCONFIG_DIR)/$(LIBMODBUSCONFIG).so.$(LIBMODBUSCONFIG_VERSION), /usr/lib/$(LIBMODBUSCONFIG).so.$(LIBMODBUSCONFIG_VERSION))
	
	@$(call install_link, libmodbusconfig, $(LIBMODBUSCONFIG).so.$(LIBMODBUSCONFIG_VERSION), /usr/lib/$(LIBMODBUSCONFIG).so.$(LIBMODBUSCONFIG_VERSION_MAJOR))
	@$(call install_link, libmodbusconfig, $(LIBMODBUSCONFIG).so.$(LIBMODBUSCONFIG_VERSION), /usr/lib/$(LIBMODBUSCONFIG).so)

	@$(call install_copy, libmodbusconfig, 0, 0, 0640, $(LIBMODBUSCONFIG_DIR)/modbus.conf, /etc/specific/modbus.conf)
	
endif

	@$(call install_finish, libmodbusconfig)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE

# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(LIBMODBUSCONFIG_PACKAGE_NAME).ipk $(LIBMODBUSCONFIG_PLATFORMCONFIGPACKAGEDIR)/
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libmodbusconfig.clean:
	@$(call targetinfo)
	@-cd $(LIBMODBUSCONFIG_DIR) && \
		$(LIBMODBUSCONFIG_MAKE_ENV) $(LIBMODBUSCONFIG_PATH) $(MAKE) clean
	@$(call clean_pkg, LIBMODBUSCONFIG)

# vim: syntax=make
