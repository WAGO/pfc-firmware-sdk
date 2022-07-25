# -*-makefile-*-
#
# Copyright (C) 2012 by <WAGO GmbH & Co. KG>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#--- provided package --------------------------------------------------------

PACKAGES-$(PTXCONF_LIBDPSWIDE) += libdpswide

#--- paths and names --------------------------------------------------------- 

LIBDPSWIDE_VERSION					:= trunk
LIBDPSWIDE_NAME							:= libdpsWide
LIBDPSWIDE_SUFFIX						:= 
#LIBDPSWIDE_URL							:= file://$(SRCDIR)/$(LIBDPSWIDE_NAME)-$(LIBDPSWIDE_VERSION)
LIBDPSWIDE_SRC						:= $(PTXDIST_WORKSPACE)/wago_intern/device/profibus/slave/$(LIBDPSWIDE_NAME)-$(LIBDPSWIDE_VERSION)
LIBDPSWIDE_DIR							:= $(BUILDDIR)/$(LIBDPSWIDE_NAME)-$(LIBDPSWIDE_VERSION)
LIBDPSWIDE_LICENSE					:=
LIBDPSWIDE_TARGET_PLATFORM	:= ptx-dist
LIBDPSWIDE_MAKE_PARAMETER		:= CROSS_COMPILE=$(COMPILER_PREFIX) \
																				TARGET_PLATFORM=$(LIBDPSWIDE_TARGET_PLATFORM) \
																				TARGET_ARCH=$(PTXCONF_ARCH_STRING) \
																				ARM_ARCH_VERSION=7 \
																				TARGET_OS=LINUX \
																				DEBUG=n \
																				SYSROOT_TARGET=$(PTXCONF_SYSROOT_TARGET)

LIBDPSWIDE_PACKAGE_NAME := libdpswide_$(LIBDPSWIDE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBDPSWIDE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/libdpswide.get:
		@$(call targetinfo)
		@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libdpswide.extract:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libdpswide.prepare:
		@$(call targetinfo)
		mkdir -p $(LIBDPSWIDE_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 		# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		rsync -a --exclude=output/ $(LIBDPSWIDE_SRC) $(BUILDDIR)
endif
		@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

LIBDPSWIDE_ENV := $(CROSS_ENV)

$(STATEDIR)/libdpswide.compile:
		@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 		# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		cd $(LIBDPSWIDE_DIR)/project && $(LIBDPSWIDE_ENV) $(MAKE) $(LIBDPSWIDE_MAKE_PARAMETER) shared_lib
endif
		@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdpswide.install:
		@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
		# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		cp -d $(LIBDPSWIDE_DIR)/project/output/$(LIBDPSWIDE_TARGET_PLATFORM)/release/$(LIBDPSWIDE_NAME).so.$(LIBDPSWIDE_VERSION) $(PTXCONF_SYSROOT_TARGET)/usr/lib
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
		#Backup header files as archive for later use in configs/@platform@/packages
		cd $(PTXCONF_SYSROOT_TARGET) && \
		tar -czvf $(LIBDPSWIDE_PACKAGE_NAME).tgz \
		usr/lib/$(LIBDPSWIDE_NAME).so.$(LIBDPSWIDE_VERSION) && \
		mv $(LIBDPSWIDE_PACKAGE_NAME).tgz $(LIBDPSWIDE_PLATFORMCONFIGPACKAGEDIR)
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
        # Install header files from archive
		tar -xzvf $(LIBDPSWIDE_PLATFORMCONFIGPACKAGEDIR)/$(LIBDPSWIDE_PACKAGE_NAME).tgz -C $(PTXCONF_SYSROOT_TARGET)
endif
		@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdpswide.targetinstall:
		@$(call targetinfo)
		@$(call install_init,  libdpswide)
		@$(call install_fixup, libdpswide,PACKAGE,libdpswide)
		@$(call install_fixup, libdpswide,PRIORITY,optional)
		@$(call install_fixup, libdpswide,VERSION,$(LIBDPSWIDE_VERSION))
		@$(call install_fixup, libdpswide,SECTION,base)
		@$(call install_fixup, libdpswide,AUTHOR,"WAGO")
		@$(call install_fixup, libdpswide,DEPENDS,)
		@$(call install_fixup, libdpswide,DESCRIPTION,missing)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
		# Extract precompiled binaries from archive
		rm -rf $(LIBDPSWIDE_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
		cd $(LIBDPSWIDE_PLATFORMCONFIGPACKAGEDIR)/tmp && \
		ar -xov $(LIBDPSWIDE_PLATFORMCONFIGPACKAGEDIR)/$(LIBDPSWIDE_PACKAGE_NAME).ipk  
		@$(call install_archive, libdpswide, 0, 0, $(LIBDPSWIDE_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
		# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		@$(call install_copy, libdpswide, 0, 0, 0755, $(PTXCONF_SYSROOT_TARGET)/usr/lib/$(LIBDPSWIDE_NAME).so.$(LIBDPSWIDE_VERSION), /usr/lib/$(LIBDPSWIDE_NAME).so.$(LIBDPSWIDE_VERSION))
		@$(call install_link,  libdpswide, ../$(LIBDPSWIDE_NAME).so.$(LIBDPSWIDE_VERSION), /usr/lib/wide/$(LIBDPSWIDE_NAME).so)
endif
		@$(call install_finish, libdpswide)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
		# Backup binaries in configs/@platform@/packages/
		cp $(PKGDIR)/$(LIBDPSWIDE_PACKAGE_NAME).ipk $(LIBDPSWIDE_PLATFORMCONFIGPACKAGEDIR)/
endif
		@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libdpswide.clean:
		@$(call targetinfo)
		@echo Deleting state files:
		@echo $(STATEDIR)/libdpswide.*
		@rm -rf $(STATEDIR)/libdpswide.*
		@echo
		@echo Deleting src dir:
		@echo $(LIBDPSWIDE_DIR)
		@rm -rf $(LIBDPSWIDE_DIR)
		@echo
		@echo Deleting installed files:
		
		@echo $(PTXDIST_PLATFORMDIR)/root/usr/lib/$(LIBDPSWIDE_NAME).so.$(LIBDPSWIDE_VERSION)
		@rm -rf $(PTXDIST_PLATFORMDIR)/root/usr/lib/$(LIBDPSWIDE_NAME).so.$(LIBDPSWIDE_VERSION)
		@echo $(PTXDIST_PLATFORMDIR)/root-debug/usr/lib/$(LIBDPSWIDE_NAME).so.$(LIBDPSWIDE_VERSION)
		@rm -rf $(PTXDIST_PLATFORMDIR)/root-debug/usr/lib/$(LIBDPSWIDE_NAME).so.$(LIBDPSWIDE_VERSION)
		
		@echo $(PTXDIST_PLATFORMDIR)/root/usr/lib/wide/$(LIBDPSWIDE_NAME).so
		@rm -rf $(PTXDIST_PLATFORMDIR)/root/usr/lib/wide/$(LIBDPSWIDE_NAME).so
		@echo $(PTXDIST_PLATFORMDIR)/root-debug/usr/lib/wide/$(LIBDPSWIDE_NAME).so
		@rm -rf $(PTXDIST_PLATFORMDIR)/root-debug/usr/lib/wide/$(LIBDPSWIDE_NAME).so
		
		@echo $(PTXCONF_SYSROOT_TARGET)/usr/lib/$(LIBDPSWIDE_NAME).so.$(LIBDPSWIDE_VERSION)
		@rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/$(LIBDPSWIDE_NAME).so.$(LIBDPSWIDE_VERSION)
		@echo $(PTXCONF_SYSROOT_TARGET)/usr/include/dpsWide
		@rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/dpsWide
		
		@echo $(PKGDIR)/$(LIBDPSWIDE_NAME)*
		@rm -rf $(PKGDIR)/$(LIBDPSWIDE_NAME)*
		@echo
