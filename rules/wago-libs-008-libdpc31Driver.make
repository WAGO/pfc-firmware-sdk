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

PACKAGES-$(PTXCONF_LIBDPC31DRIVER) += libdpc31driver

#--- paths and names --------------------------------------------------------- 

LIBDPC31DRIVER_VERSION					:= trunk
LIBDPC31DRIVER_NAME							:= libdpc31Driver
LIBDPC31DRIVER_SUFFIX						:= 
LIBDPC31DRIVER_SRC						:= $(PTXDIST_WORKSPACE)/wago_intern/device/profibus/slave/$(LIBDPC31DRIVER_NAME)-$(LIBDPC31DRIVER_VERSION)
LIBDPC31DRIVER_URL							:= file://$(LIBDPC31DRIVER_SRC)
LIBDPC31DRIVER_DIR							:= $(BUILDDIR)/$(LIBDPC31DRIVER_NAME)-$(LIBDPC31DRIVER_VERSION)
LIBDPC31DRIVER_LICENSE					:=
LIBDPC31DRIVER_TARGET_PLATFORM	:= ptx-dist
LIBDPC31DRIVER_MAKE_PARAMETER		:= CROSS_COMPILE=$(COMPILER_PREFIX) \
																		TARGET_PLATFORM=$(LIBDPC31DRIVER_TARGET_PLATFORM) \
																		TARGET_ARCH=$(PTXCONF_ARCH_STRING) \
																		ARM_ARCH_VERSION=7 \
																		TARGET_OS=LINUX \
																		DEBUG=n \
																		SYSROOT_TARGET=$(PTXCONF_SYSROOT_TARGET)

LIBDPC31DRIVER_PACKAGE_NAME := $(LIBDPC31DRIVER_NAME)_$(LIBDPC31DRIVER_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBDPC31DRIVER_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/libdpc31driver.get:
		@$(call targetinfo)
		@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libdpc31driver.extract:
	@$(call targetinfo)
	mkdir -p $(LIBDPC31DRIVER_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 		# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		rsync -a --exclude=output/ $(LIBDPC31DRIVER_SRC) $(BUILDDIR)
endif
	@$(call touch)
	
$(STATEDIR)/libdpc31driver.extract.post:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libdpc31driver.prepare:
		@$(call targetinfo)
		@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/libdpc31driver.compile:
		@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 		# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		cd $(LIBDPC31DRIVER_DIR)/project && $(MAKE) $(LIBDPC31DRIVER_MAKE_PARAMETER) static_lib
endif
		@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdpc31driver.install:
		@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
		# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		cp -d $(LIBDPC31DRIVER_DIR)/project/output/$(LIBDPC31DRIVER_TARGET_PLATFORM)/release/$(LIBDPC31DRIVER_NAME).a $(PTXCONF_SYSROOT_TARGET)/usr/lib
		mkdir -p $(PTXCONF_SYSROOT_TARGET)/usr/include/dpc31Driver
		cp -d $(LIBDPC31DRIVER_DIR)/sources/interfaces/dpc31Driver_API.h $(PTXCONF_SYSROOT_TARGET)/usr/include/dpc31Driver
		cp -d $(LIBDPC31DRIVER_DIR)/sources/interfaces/dpc31Driver_SYSI.h $(PTXCONF_SYSROOT_TARGET)/usr/include/dpc31Driver
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
		#Backup header files as archive for later use in configs/@platform@/packages
		cd $(PTXCONF_SYSROOT_TARGET) && \
		tar -czvf $(LIBDPC31DRIVER_PACKAGE_NAME).tgz \
		usr/lib/$(LIBDPC31DRIVER_NAME).a \
		usr/include/dpc31Driver/dpc31Driver_API.h \
		usr/include/dpc31Driver/dpc31Driver_SYSI.h && \
		mv $(LIBDPC31DRIVER_PACKAGE_NAME).tgz $(LIBDPC31DRIVER_PLATFORMCONFIGPACKAGEDIR)
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
        # Install header files from archive
		tar -xzvf $(LIBDPC31DRIVER_PLATFORMCONFIGPACKAGEDIR)/$(LIBDPC31DRIVER_PACKAGE_NAME).tgz -C $(PTXCONF_SYSROOT_TARGET)
endif
		@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdpc31driver.targetinstall:
		@$(call targetinfo)
		@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libdpc31driver.clean:
		@$(call targetinfo)
		@echo Deleting state files:
		@echo $(STATEDIR)/libdpc31driver.*
		@rm -rf $(STATEDIR)/libdpc31driver.*
		@echo
		@echo Deleting src dir:
		@echo $(LIBDPC31DRIVER_DIR)
		@rm -rf $(LIBDPC31DRIVER_DIR)
		@echo
		@echo Deleting installed files:
		@echo $(PTXCONF_SYSROOT_TARGET)/usr/lib/$(LIBDPC31DRIVER_NAME).a
		@rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/$(LIBDPC31DRIVER_NAME).a
		@echo $(PTXCONF_SYSROOT_TARGET)/usr/include/dpc31Driver
		@rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/dpc31Driver
		@echo
