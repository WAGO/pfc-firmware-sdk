# -*-makefile-*-
# $Id$
#
# Copyright (C) 2008 by 
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_FLASHXLOADER) += flashxloader

#
# Paths and names
# src/flash-xloader/
FLASHXLOADER			:= flash-xloader
FLASHXLOADER_VERSION  		:= 0
FLASHXLOADER_URL		:= file://
FLASHXLOADER_SOURCE		:= $(SRCDIR)/$(FLASHXLOADER)
FLASHXLOADER_DIR		:= $(BUILDDIR)/$(FLASHXLOADER)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/flashxloader.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/flashxloader.extract:
	@$(call targetinfo)
	@$(call clean, $(FLASHXLOADER_DIR))
	cp -r $(SRCDIR)/$(FLASHXLOADER) $(FLASHXLOADER_DIR)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

FLASHXLOADER_PATH	:= PATH=$(CROSS_PATH)
FLASHXLOADER_ENV 	:= $(CROSS_ENV)

$(STATEDIR)/flashxloader.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/flashxloader.compile:
	@$(call targetinfo)
	cd $(FLASHXLOADER_DIR) && $(FLASHXLOADER_ENV) $(FLASHXLOADER_PATH) $(MAKE) CROSS_COMPILE=$(PTXCONF_COMPILER_PREFIX)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/flashxloader.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/flashxloader.targetinstall:
	@$(call targetinfo)

	@$(call install_init, flashxloader)
	@$(call install_fixup,flashxloader,PRIORITY,optional)
	@$(call install_fixup,flashxloader,SECTION,base)
	@$(call install_fixup,flashxloader,AUTHOR,"Wago")
	@$(call install_fixup,flashxloader,DESCRIPTION,missing)
#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish
#
	@$(call install_copy, flashxloader, 0, 0, 0755, $(FLASHXLOADER_DIR)/flash-xloader, /bin/flash-xloader)

	@$(call install_finish,flashxloader)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

flashxloader_clean:
	rm -rf $(STATEDIR)/flashxloader.*
	-cd $(FLASHXLOADER_DIR) && $(FLASHXLOADER_ENV) $(FLASHXLOADER_PATH) $(MAKE) clean

# vim: syntax=make
