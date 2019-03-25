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

PACKAGES-$(PTXCONF_LIBDPC31STACK) += libdpc31stack

#--- paths and names --------------------------------------------------------- 

LIBDPC31STACK_VERSION						:= trunk
LIBDPC31STACK_NAME							:= libdpc31Stack
LIBDPC31STACK_SUFFIX						:= 
#LIBDPC31STACK_URL								:= file://$(SRCDIR)/$(LIBDPC31STACK_NAME)-$(LIBDPC31STACK_VERSION)
LIBDPC31STACK_SRC						:= $(PTXDIST_WORKSPACE)/wago_intern/device/profibus/slave/$(LIBDPC31STACK_NAME)-$(LIBDPC31STACK_VERSION)
LIBDPC31STACK_DIR								:= $(BUILDDIR)/$(LIBDPC31STACK_NAME)-$(LIBDPC31STACK_VERSION)
LIBDPC31STACK_LICENSE						:=
LIBDPC31STACK_TARGET_PLATFORM		:= ptx-dist
LIBDPC31STACK_MAKE_PARAMETER		:= CROSS_COMPILE=$(COMPILER_PREFIX) \
																		TARGET_PLATFORM=$(LIBDPC31STACK_TARGET_PLATFORM) \
																		TARGET_ARCH=$(PTXCONF_ARCH_STRING) \
																		ARM_ARCH_VERSION=7 \
																		TARGET_OS=LINUX \
																		DEBUG=n \
																		SYSROOT_TARGET=$(PTXCONF_SYSROOT_TARGET)

LIBDPC31STACK_PACKAGE_NAME := $(LIBDPC31STACK_NAME)_$(LIBDPC31STACK_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBDPC31STACK_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/libdpc31stack.get:
		@$(call targetinfo)
		@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libdpc31stack.extract:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libdpc31stack.prepare:
		@$(call targetinfo)
		mkdir -p $(LIBDPC31STACK_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 		# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		rsync -a --exclude=output/ $(LIBDPC31STACK_SRC) $(BUILDDIR)
endif
		@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/libdpc31stack.compile:
		@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 		# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		cd $(LIBDPC31STACK_DIR)/project && $(MAKE) $(LIBDPC31STACK_MAKE_PARAMETER) static_lib
endif
		@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdpc31stack.install:
		@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
		# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		cp -d $(LIBDPC31STACK_DIR)/project/output/$(LIBDPC31STACK_TARGET_PLATFORM)/release/$(LIBDPC31STACK_NAME).a $(PTXCONF_SYSROOT_TARGET)/usr/lib
		mkdir -p $(PTXCONF_SYSROOT_TARGET)/usr/include/dpc31Stack
		cp -d $(LIBDPC31STACK_DIR)/sources/interfaces/dpc31Stack_API.h $(PTXCONF_SYSROOT_TARGET)/usr/include/dpc31Stack
		cp -d $(LIBDPC31STACK_DIR)/sources/interfaces/dpc31Stack_SYSI.h $(PTXCONF_SYSROOT_TARGET)/usr/include/dpc31Stack	
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
		#Backup header files as archive for later use in configs/@platform@/packages
		cd $(PTXCONF_SYSROOT_TARGET) && \
		tar -czvf $(LIBDPC31STACK_PACKAGE_NAME).tgz \
		usr/lib/$(LIBDPC31STACK_NAME).a \
		usr/include/dpc31Stack/dpc31Stack_API.h \
		usr/include/dpc31Stack/dpc31Stack_SYSI.h && \
		mv $(LIBDPC31STACK_PACKAGE_NAME).tgz $(LIBDPC31STACK_PLATFORMCONFIGPACKAGEDIR)
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
        # Install header files from archive
		tar -xzvf $(LIBDPC31STACK_PLATFORMCONFIGPACKAGEDIR)/$(LIBDPC31STACK_PACKAGE_NAME).tgz -C $(PTXCONF_SYSROOT_TARGET)
endif
		@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdpc31stack.targetinstall:
		@$(call targetinfo)
		@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libdpc31stack.clean:
		@$(call targetinfo)
		@echo Deleting state files:
		@echo $(STATEDIR)/libdpc31stack.*
		@rm -rf $(STATEDIR)/libdpc31stack.*
		@echo
		@echo Deleting src dir:
		@echo $(LIBDPC31STACK_DIR)
		@rm -rf $(LIBDPC31STACK_DIR)
		@echo
		@echo Deleting installed files:
		@echo $(PTXCONF_SYSROOT_TARGET)/usr/lib/$(LIBDPC31STACK_NAME).a
		@rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/$(LIBDPC31STACK_NAME).a
		@echo $(PTXCONF_SYSROOT_TARGET)/usr/include/dpc31Stack
		@rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/dpc31Stack
		@echo
