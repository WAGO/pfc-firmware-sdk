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

PACKAGES-$(PTXCONF_LIBEXTMEMCPY) += libextmemcpy

#--- paths and names --------------------------------------------------------- 

LIBEXTMEMCPY_VERSION    := trunk
LIBEXTMEMCPY_NAME       := libextMemCpy
LIBEXTMEMCPY_URL        := file://$(SRCDIR)/$(LIBEXTMEMCPY_NAME)-$(LIBEXTMEMCPY_VERSION)
LIBEXTMEMCPY_DIR        := $(BUILDDIR)/$(LIBEXTMEMCPY_NAME)-$(LIBEXTMEMCPY_VERSION)
LIBEXTMEMCPY_LICENSE    :=
LIBEXTMEMCPY_TARGET_PLATFORM    := ptx-dist
LIBEXTMEMCPY_MAKE_PARAMETER     := CROSS_COMPILE=$(COMPILER_PREFIX) \
																	TARGET_PLATFORM=$(LIBEXTMEMCPY_TARGET_PLATFORM) \
																	TARGET_ARCH=$(PTXCONF_ARCH_STRING) \
																	ARM_ARCH_VERSION=7 \
																	TARGET_OS=LINUX \
																	DEBUG=n \
																	SYSROOT_TARGET=$(PTXCONF_SYSROOT_TARGET)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/libextmemcpy.get:
	@$(call targetinfo)
	@mkdir -p $(LIBEXTMEMCPY_DIR)
	@rsync -a --exclude=output/ $(SRCDIR)/$(LIBEXTMEMCPY_NAME)-$(LIBEXTMEMCPY_VERSION) $(BUILDDIR)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libextmemcpy.extract:
	@$(call targetinfo)
	@$(call touch)

$(STATEDIR)/libextmemcpy.extract.post:
	@$(call targetinfo)
	@$(call touch)
	
# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libextmemcpy.prepare:
		@$(call targetinfo)
		@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/libextmemcpy.compile:
		@$(call targetinfo)
		cd $(LIBEXTMEMCPY_DIR)/project && $(MAKE) $(LIBEXTMEMCPY_MAKE_PARAMETER) static_lib
		@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libextmemcpy.install:
		@$(call targetinfo)
		cp -d $(LIBEXTMEMCPY_DIR)/project/output/$(LIBEXTMEMCPY_TARGET_PLATFORM)/release/$(LIBEXTMEMCPY_NAME).a $(PTXCONF_SYSROOT_TARGET)/usr/lib
		mkdir -p $(PTXCONF_SYSROOT_TARGET)/usr/include/extMemCpy
		cp -d $(LIBEXTMEMCPY_DIR)/sources/extMemCpy/extMemCpy_API.h $(PTXCONF_SYSROOT_TARGET)/usr/include/extMemCpy
		cp -d $(LIBEXTMEMCPY_DIR)/sources/extMemCpy/extMemCpy_SYSI.h $(PTXCONF_SYSROOT_TARGET)/usr/include/extMemCpy
		@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libextmemcpy.targetinstall:
		@$(call targetinfo)
		@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libextmemcpy.clean:
		@$(call targetinfo)
		@echo Deleting state files:
		@echo $(STATEDIR)/libextmemcpy.*
		@rm -rf $(STATEDIR)/libextmemcpy.*
		@echo
		@echo Deleting src dir:
		@echo $(LIBEXTMEMCPY_DIR)
		@rm -rf $(LIBEXTMEMCPY_DIR)
		@echo
		@echo Deleting installed files:
		@echo $(PTXCONF_SYSROOT_TARGET)/usr/lib/$(LIBEXTMEMCPY_NAME).a
		@rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/$(LIBEXTMEMCPY_NAME).a
		@echo $(PTXCONF_SYSROOT_TARGET)/usr/include/extMemCpy
		@rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/extMemCpy
		@echo
