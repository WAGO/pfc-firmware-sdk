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

PACKAGES-$(PTXCONF_LIBUTIL) += libutil

#--- paths and names --------------------------------------------------------- 

LIBUTIL_VERSION             := 1.3.0
LIBUTIL_NAME                := libutil
LIBUTIL_URL                 := file://$(SRCDIR)/$(LIBUTIL_NAME)
LIBUTIL_DIR                 := $(BUILDDIR)/$(LIBUTIL_NAME)-$(LIBUTIL_VERSION)
LIBUTIL_PROJ_DIR            := LIBUTIL_DIR
LIBUTIL_LICENSE             :=
LIBUTIL_TARGET_PLATFORM		:= ptx-dist
LIBUTIL_MAKE_PARAMETER		:= CROSS_COMPILE=$(COMPILER_PREFIX) \
															TARGET_PLATFORM=$(LIBUTIL_TARGET_PLATFORM) \
															TARGET_ARCH=$(PTXCONF_ARCH_STRING) \
															ARM_ARCH_VERSION=7 \
															TARGET_OS=LINUX \
															DEBUG=n \
															SYSROOT_TARGET=$(PTXCONF_SYSROOT_TARGET)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/$(LIBUTIL_NAME).get:
		@$(call targetinfo)
		@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/$(LIBUTIL_NAME).extract:
	@$(call targetinfo)
	@mkdir -p $(LIBUTIL_DIR)
	@rsync -a --exclude=output/ $(SRCDIR)/$(LIBUTIL_NAME)/ $(BUILDDIR)/$(LIBUTIL_NAME)-$(LIBUTIL_VERSION)
	@$(call touch)
	
$(STATEDIR)/$(LIBUTIL_NAME).extract.post:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/$(LIBUTIL_NAME).prepare:
		@$(call targetinfo)
		@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/$(LIBUTIL_NAME).compile:
		@$(call targetinfo)
		cd $(LIBUTIL_DIR) && $(MAKE) $(LIBUTIL_MAKE_PARAMETER) static_lib
		@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/$(LIBUTIL_NAME).install:
		@$(call targetinfo)
		mkdir -p $(PTXCONF_SYSROOT_TARGET)/usr/lib/util
		cp -d $(LIBUTIL_DIR)/output/$(LIBUTIL_TARGET_PLATFORM)/release/$(LIBUTIL_NAME).a $(PTXCONF_SYSROOT_TARGET)/usr/lib/util
		mkdir -p $(PTXCONF_SYSROOT_TARGET)/usr/include/util
		cp -d $(LIBUTIL_DIR)/includes/util_api.h $(PTXCONF_SYSROOT_TARGET)/usr/include/util
		@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/$(LIBUTIL_NAME).targetinstall:
		@$(call targetinfo)
		@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/$(LIBUTIL_NAME).clean:
		@$(call targetinfo)
		@echo Deleting state files:
		@echo $(STATEDIR)/$(LIBUTIL_NAME).*
		@rm -rf $(STATEDIR)/$(LIBUTIL_NAME).*
		@echo
		@echo Deleting src dir:
		@echo $(LIBUTIL_DIR)
		@rm -rf $(LIBUTIL_DIR)
		@echo
		@echo Deleting installed files:
		@echo $(PTXCONF_SYSROOT_TARGET)/usr/lib/$(LIBUTIL_NAME).a
		@rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/$(LIBUTIL_NAME).a
		@echo $(PTXCONF_SYSROOT_TARGET)/usr/include/util
		@rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/util
		@echo
