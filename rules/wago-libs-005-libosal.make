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

PACKAGES-$(PTXCONF_LIBOSAL) += libosal

#--- paths and names --------------------------------------------------------- 

LIBOSAL_VERSION             := 1.3.0
LIBOSAL_NAME                := libosal
LIBOSAL_URL                 := file://$(SRCDIR)/$(LIBOSAL_NAME)-$(LIBOSAL_VERSION)
LIBOSAL_DIR                 := $(BUILDDIR)/$(LIBOSAL_NAME)-$(LIBOSAL_VERSION)
LIBOSAL_LICENSE             :=
LIBOSAL_TARGET_PLATFORM     := ptx-dist
LIBOSAL_MAKE_PARAMETER      := CROSS_COMPILE=$(COMPILER_PREFIX) \
															TARGET_PLATFORM=$(LIBOSAL_TARGET_PLATFORM) \
															TARGET_ARCH=$(PTXCONF_ARCH_STRING) \
															ARM_ARCH_VERSION=7 \
															TARGET_OS=LINUX \
															DEBUG=n \
															SYSROOT_TARGET=$(PTXCONF_SYSROOT_TARGET)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/$(LIBOSAL_NAME).get:
		@$(call targetinfo)
		@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/$(LIBOSAL_NAME).extract:
	@$(call targetinfo)
	mkdir -p $(LIBOSAL_DIR)
	rsync -a --exclude=output/ $(SRCDIR)/$(LIBOSAL_NAME)-$(LIBOSAL_VERSION) $(BUILDDIR)
	@$(call touch)

$(STATEDIR)/$(LIBOSAL_NAME).extract.post:
	@$(call targetinfo)
	@$(call touch)
	
# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/$(LIBOSAL_NAME).prepare:
		@$(call targetinfo)
		@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/$(LIBOSAL_NAME).compile:
		@$(call targetinfo)
		cd $(LIBOSAL_DIR) && $(MAKE) $(LIBOSAL_MAKE_PARAMETER) static_lib
		@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/$(LIBOSAL_NAME).install:
		@$(call targetinfo)
		cp -d $(LIBOSAL_DIR)/output/$(LIBOSAL_TARGET_PLATFORM)/release/$(LIBOSAL_NAME).a $(PTXCONF_SYSROOT_TARGET)/usr/lib
		rsync -a $(LIBOSAL_DIR)/includes/ $(PTXCONF_SYSROOT_TARGET)/usr/include/osal
		@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/$(LIBOSAL_NAME).targetinstall:
		@$(call targetinfo)
		@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/$(LIBOSAL_NAME).clean:
		@$(call targetinfo)
		@echo Deleting state files:
		@echo $(STATEDIR)/$(LIBOSAL_NAME).*
		@rm -rf $(STATEDIR)/$(LIBOSAL_NAME).*
		@echo
		@echo Deleting src dir:
		@echo $(LIBOSAL_DIR)
		@rm -rf $(LIBOSAL_DIR)
		@echo
		@echo Deleting installed files:
		@echo $(PTXCONF_SYSROOT_TARGET)/usr/lib/$(LIBOSAL_NAME).a
		@rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/$(LIBOSAL_NAME).a
		@echo $(PTXCONF_SYSROOT_TARGET)/usr/include/osal
		@rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/osal
		@echo
