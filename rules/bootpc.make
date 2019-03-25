# -*-makefile-*-
# $Id: bootpc.make 8285 2008-06-03 20:24:41Z rsc $
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
PACKAGES-$(PTXCONF_BOOTPC) += bootpc

#
# Paths and names
# 
BOOTPC_VERSION		= 0.64
BOOTPC_MD5		= 0563ff0c8276d08cf869b6d2a533b3bf
BOOTPC			= bootpc-$(BOOTPC_VERSION)
BOOTPC_SUFFIX		= tar.gz
BOOTPC_URL		= http://ibiblio.org/pub/linux/system/network/admin/$(BOOTPC).$(BOOTPC_SUFFIX)
BOOTPC_SOURCE		= $(SRCDIR)/$(BOOTPC).$(BOOTPC_SUFFIX)
BOOTPC_DIR		= $(BUILDDIR)/$(BOOTPC)
BOOTPC_LICENSE	:= GPLv2

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

bootpc_get: $(STATEDIR)/bootpc.get

$(STATEDIR)/bootpc.get: $(bootpc_get_deps_default)
	@$(call targetinfo)
	@$(call touch)

$(BOOTPC_SOURCE):
	@$(call targetinfo)
	@$(call get, BOOTPC)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

bootpc_extract: $(STATEDIR)/bootpc.extract

$(STATEDIR)/bootpc.extract: $(bootpc_extract_deps_default)
	@$(call targetinfo)
	@$(call clean, $(BOOTPC_DIR))
	@$(call extract, BOOTPC)
	@$(call patchin, BOOTPC)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

bootpc_prepare: $(STATEDIR)/bootpc.prepare

BOOTPC_PATH	=  PATH=$(CROSS_PATH)
BOOTPC_ENV 	=  $(CROSS_ENV)

#
# autoconf
#
BOOTPC_AUTOCONF =  $(CROSS_AUTOCONF_USR)

$(STATEDIR)/bootpc.prepare: $(bootpc_prepare_deps_default)
	@$(call targetinfo)
	@$(call clean, $(BOOTPC_DIR)/config.cache)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

bootpc_compile: $(STATEDIR)/bootpc.compile

$(STATEDIR)/bootpc.compile: $(bootpc_compile_deps_default)
	@$(call targetinfo)
	cd $(BOOTPC_DIR) && $(BOOTPC_ENV) $(BOOTPC_PATH) make
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

bootpc_install: $(STATEDIR)/bootpc.install

$(STATEDIR)/bootpc.install: $(bootpc_install_deps_default)
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

bootpc_targetinstall: $(STATEDIR)/bootpc.targetinstall

$(STATEDIR)/bootpc.targetinstall: $(bootpc_targetinstall_deps_default)
	@$(call targetinfo)

	@$(call install_init, bootpc)
	@$(call install_fixup, bootpc,PRIORITY,optional)
	@$(call install_fixup, bootpc,SECTION,base)
	@$(call install_fixup, bootpc,AUTHOR,"Robert Schwebel <r.schwebel\@pengutronix.de>")
	@$(call install_fixup, bootpc,DESCRIPTION,missing)

	@$(call install_copy, bootpc, 0, 0, 0755, $(BOOTPC_DIR)/bootpc, /sbin/bootpc)
ifdef PTXCONF_BOOTPC_STARTUP_DEFAULT
	@$(call install_copy, bootpc, 0, 0, 0755, $(PTXDIST_WORKSPACE)/projectroot/sbin/bootpc-startup.default, /sbin/bootpc-startup)
endif
	@$(call install_finish, bootpc)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

bootpc_clean:
	rm -rf $(STATEDIR)/bootpc.*
	rm -rf $(PKGDIR)/bootpc_*
	rm -rf $(BOOTPC_DIR)

# vim: syntax=make
