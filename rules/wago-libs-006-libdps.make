# -*-makefile-*-
#
# Copyright (C) 2012 by <WAGO Kontakttechnik GmbH & Co. KG>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#--- provided package --------------------------------------------------------

PACKAGES-$(PTXCONF_LIBDPS) += libdps

#--- paths and names --------------------------------------------------------- 

LIBDPS_VERSION					:= trunk
LIBDPS_NAME							:= libdps
LIBDPS                  := $(LIBDPS_NAME)-$(LIBDPS_VERSION)
LIBDPS_SUFFIX						:= 
LIBDPS_SRC						:= $(PTXDIST_WORKSPACE)/wago_intern/device/profibus/slave/$(LIBDPS_NAME)-$(LIBDPS_VERSION)
LIBDPS_DIR							:= $(BUILDDIR)/$(LIBDPS_NAME)-$(LIBDPS_VERSION)
LIBDPS_LICENSE					:=
LIBDPS_TARGET_PLATFORM	:= ptx-dist
LIBDPS_MAKE_PARAMETER		:= CROSS_COMPILE=$(COMPILER_PREFIX) \
																		TARGET_PLATFORM=$(LIBDPS_TARGET_PLATFORM) \
																		TARGET_ARCH=$(PTXCONF_ARCH_STRING) \
																		ARM_ARCH_VERSION=7 \
																		TARGET_OS=LINUX \
																		DEBUG=n \
																		SYSROOT_TARGET=$(PTXCONF_SYSROOT_TARGET)

LIBDPS_PACKAGE_NAME := libdps_$(LIBDPS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBDPS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/libdps.get:
		@$(call targetinfo)
		@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libdps.extract:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libdps.prepare:
		@$(call targetinfo)
		mkdir -p $(LIBDPS_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 		# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		rsync -a --exclude=output/ $(LIBDPS_SRC) $(BUILDDIR)
endif
		@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

LIBDPS_ENV := $(CROSS_ENV)

$(STATEDIR)/libdps.compile:
		@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 		# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		@$(call xslt_compile, $(LIBDPS_DIR)/project/config/dps_diagnostics.xml)
		cd $(LIBDPS_DIR)/project && $(LIBDPS_ENV) $(MAKE) $(LIBDPS_MAKE_PARAMETER) shared_lib
endif
		@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdps.install:
		@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
		# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		cp -d $(LIBDPS_DIR)/project/output/$(LIBDPS_TARGET_PLATFORM)/release/$(LIBDPS_NAME).so.$(LIBDPS_VERSION) $(PTXCONF_SYSROOT_TARGET)/usr/lib
		mkdir -p $(PTXCONF_SYSROOT_TARGET)/usr/include/dps
		cp -d $(LIBDPS_DIR)/sources/interfaces/dps_API.h $(PTXCONF_SYSROOT_TARGET)/usr/include/dps
		cp -d $(LIBDPS_DIR)/sources/interfaces/dps_SYSI.h $(PTXCONF_SYSROOT_TARGET)/usr/include/dps
		cp -d $(LIBDPS_DIR)/sources/interfaces/dpsTypes.h $(PTXCONF_SYSROOT_TARGET)/usr/include/dps
		cp -d $(LIBDPS_DIR)/project/config/dps.conf $(PTXCONF_SYSROOT_TARGET)/etc/specific
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
		#Backup header files as archive for later use in configs/@platform@/packages
		cd $(PTXCONF_SYSROOT_TARGET) && \
		tar -czvf $(LIBDPS_PACKAGE_NAME).tgz \
		usr/lib/$(LIBDPS_NAME).so.$(LIBDPS_VERSION) \
		usr/include/dps/dps_API.h \
		usr/include/dps/dps_SYSI.h \
		usr/include/dps/dpsTypes.h\
		etc/specific/dps.conf && \
		mv $(LIBDPS_PACKAGE_NAME).tgz $(LIBDPS_PLATFORMCONFIGPACKAGEDIR)
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
        # Install header files from archive
		tar -xzvf $(LIBDPS_PLATFORMCONFIGPACKAGEDIR)/$(LIBDPS_PACKAGE_NAME).tgz -C $(PTXCONF_SYSROOT_TARGET)
endif
		@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdps.targetinstall:
		@$(call targetinfo)
		@$(call install_init,  libdps)
		@$(call install_fixup, libdps,PACKAGE,libdps)
		@$(call install_fixup, libdps,PRIORITY,optional)
		@$(call install_fixup, libdps,VERSION,$(LIBDPS_VERSION))
		@$(call install_fixup, libdps,SECTION,base)
		@$(call install_fixup, libdps,AUTHOR,"WAGO")
		@$(call install_fixup, libdps,DEPENDS,)
		@$(call install_fixup, libdps,DESCRIPTION,missing)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
		# Extract precompiled binaries from archive
		rm -rf $(LIBDPS_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
		cd $(LIBDPS_PLATFORMCONFIGPACKAGEDIR)/tmp && \
		ar -xov $(LIBDPS_PLATFORMCONFIGPACKAGEDIR)/$(LIBDPS_PACKAGE_NAME).ipk  
		@$(call install_archive, libdps, 0, 0, $(LIBDPS_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
		# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		@$(call install_copy, libdps, 0, 0, 0755, $(PTXCONF_SYSROOT_TARGET)/usr/lib/$(LIBDPS_NAME).so.$(LIBDPS_VERSION), /usr/lib/$(LIBDPS_NAME).so.$(LIBDPS_VERSION))
		@$(call install_copy, libdps, 0, 0, 0664, $(PTXCONF_SYSROOT_TARGET)/etc/specific/dps.conf, /etc/specific/dps.conf)
		@$(call install_link,  libdps, ../$(LIBDPS_NAME).so.$(LIBDPS_VERSION), /usr/lib/dal/$(LIBDPS_NAME).so)
endif
		@$(call install_finish, libdps)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
		# Backup binaries in configs/@platform@/packages/
		cp $(PKGDIR)/$(LIBDPS_PACKAGE_NAME).ipk $(LIBDPS_PLATFORMCONFIGPACKAGEDIR)/
endif
		@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libdps.clean:
		@$(call targetinfo)
		@echo Deleting state files:
		@echo $(STATEDIR)/libdps.*
		@rm -rf $(STATEDIR)/libdps.*
		@echo
		@echo Deleting src dir:
		@echo $(LIBDPS_DIR)
		@rm -rf $(LIBDPS_DIR)
		@echo
		@echo Deleting installed files:
		
		@echo $(PTXDIST_PLATFORMDIR)/root/usr/lib/$(LIBDPS_NAME).so.$(LIBDPS_VERSION)
		@rm -rf $(PTXDIST_PLATFORMDIR)/root/usr/lib/$(LIBDPS_NAME).so.$(LIBDPS_VERSION)
		@echo $(PTXDIST_PLATFORMDIR)/root-debug/usr/lib/$(LIBDPS_NAME).so.$(LIBDPS_VERSION)
		@rm -rf $(PTXDIST_PLATFORMDIR)/root-debug/usr/lib/$(LIBDPS_NAME).so.$(LIBDPS_VERSION)
		@echo $(PTXCONF_SYSROOT_TARGET)/usr/lib/$(LIBDPS_NAME).so.$(LIBDPS_VERSION)
		@rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/$(LIBDPS_NAME).so.$(LIBDPS_VERSION)
		@echo $(PTXCONF_SYSROOT_TARGET)/usr/include/dps
		@rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/dps
		
		@echo $(PTXDIST_PLATFORMDIR)/root/etc/specific/dps.conf
		@rm -rf $(PTXDIST_PLATFORMDIR)/root/etc/specific/dps.conf
		@echo $(PTXDIST_PLATFORMDIR)/root-debug/etc/specific/dps.conf
		@rm -rf $(PTXDIST_PLATFORMDIR)/root-debug/etc/specific/dps.conf
		@echo $(PTXCONF_SYSROOT_TARGET)/etc/specific/dps.conf
		@rm -rf $(PTXCONF_SYSROOT_TARGET)/etc/specific/dps.conf
				
		@echo $(PTXDIST_PLATFORMDIR)/root/usr/lib/dal/$(LIBDPS_NAME).so
		@rm -rf $(PTXDIST_PLATFORMDIR)/root/usr/lib/dal/$(LIBDPS_NAME).so
		@echo $(PTXDIST_PLATFORMDIR)/root-debug/usr/lib/dal/$(LIBDPS_NAME).so
		@rm -rf $(PTXDIST_PLATFORMDIR)/root-debug/usr/lib/dal/$(LIBDPS_NAME).so
				
		@echo $(PKGDIR)/$(LIBDPS_NAME)*
		@rm -rf $(PKGDIR)/$(LIBDPS_NAME)*
				
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
		@echo Clean XSLT
		@$(call xslt_clean, $(LIBDPS_DIR)/project/config/dps_diagnostics.xml)
endif
		@echo clean done



