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
PACKAGES-$(PTXCONF_PROFINETDEMO) += profinetdemo

#
# Paths and names
#
PROFINETDEMO_VERSION := 0.0.1
PROFINETDEMO   := profinetdemo
PROFINETDEMO_PKGDIR := $(PKGDIR)/$(PROFINETDEMO)-$(PROFINETDEMO_VERSION)
PROFINETDEMO_URL := file://$(SRCDIR)/profinetdemo
PROFINETDEMO_DIR := $(BUILDDIR)/$(PROFINETDEMO)
PROFINETDEMO_SUBDIR := idevice
PROFINETDEMO_MAKE_ENV 	:= $(CROSS_ENV)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/profinetdemo.get:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/profinetdemo.extract:
	@$(call targetinfo, $@)
	@$(call clean, $(PROFINETDEMO_DIR))
	cp -rd $(SRCDIR)/$(PROFINETDEMO) $(PROFINETDEMO_DIR)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/profinetdemo.prepare:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/profinetdemo.compile:
#	@$(call targetinfo, $@)
#	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/profinetdemo.install:
#	@$(call targetinfo, $@)
#	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/profinetdemo.targetinstall:
	@$(call targetinfo, $@)
ifndef PTXCONF_PROFINETDEMO_SKIP_TARGETINSTALL
	@$(call install_init,profinetdemo)
	@$(call install_fixup,profinetdemo,PRIORITY,optional)
	@$(call install_fixup,profinetdemo,VERSION,$(PROFINETDEMO_VERSION)) 
	@$(call install_fixup,profinetdemo,SECTION,base)
	@$(call install_fixup,profinetdemo,AUTHOR,"WAGO")
	@$(call install_fixup,profinetdemo,DESCRIPTION,missing)

#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish
#
	@$(call install_copy,profinetdemo, 0, 0, 0755, $(PROFINETDEMO_DIR)/idevice/profinetdemo_idevice, /usr/sbin/profinetdemo_idevice)
	@$(call install_finish,profinetdemo)
endif
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/profinetdemo.clean:
	@rm -rf $(STATEDIR)/profinetdemo.*
	@rm -rf $(PROFINETDEMO_DIR)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/sbin/$(PROFINETDEMO)
	@rm -rf $(PROFINETDEMO_PKGDIR)

# vim: syntax=make
