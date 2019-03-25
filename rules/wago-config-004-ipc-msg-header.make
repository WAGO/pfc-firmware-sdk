# -*-makefile-*-
# $Id$
#
# Copyright (C) 2008 by Stefanie Meihöfer
# 
# This package installs the ipc_msg_com.h file on which codesys
# and the config-tools depend on.
# 
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CT_IPC_MSG_HEADER) += ct_ipc_msg_header

#TODO: refactor package: current solution (2 packages from same source dir
# is all but elegant)
# Integrate into config-tools? There must have been a reason to extract the headers
# into an own folder!

#
# Paths and names
#
CT_IPC_MSG_HEADER_VERSION 	:= 1.0.0
CT_IPC_MSG_HEADER		:= ct_ipc_msg_header
CT_IPC_MSG_HEADER_URL	:= file://$(PTXDIST_WORKSPACE)/local_src/config-tools
CT_IPC_MSG_HEADER_DIR	:= $(BUILDDIR)/config-tools

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/ct_ipc_msg_header.get: 
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/ct_ipc_msg_header.extract: 
	@$(call targetinfo)
	@$(call touch)

$(STATEDIR)/ct_ipc_msg_header.extract.post:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

CT_IPC_MSG_HEADER_PATH	:= PATH=$(CROSS_PATH)
CT_IPC_MSG_HEADER_ENV 	:= $(CROSS_ENV) _SYSROOT=$(PTXCONF_SYSROOT_TARGET)

$(STATEDIR)/ct_ipc_msg_header.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/ct_ipc_msg_header.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ct_ipc_msg_header.install: $(ct_ipc_msg_header_install_deps_default)
	@$(call targetinfo)

	cp $(PTXDIST_WORKSPACE)/local_src/config-tools/wago_ct_ipc_msg_com.h $(PTXDIST_PLATFORMDIR)/sysroot-target/usr/include/
	cp $(PTXDIST_WORKSPACE)/local_src/config-tools/wago_ct_get_rts_info.h $(PTXDIST_PLATFORMDIR)/sysroot-target/usr/include/

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ct_ipc_msg_header.targetinstall:
	@$(call targetinfo)

# Packet contains no target artifacts.

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

ct_ipc_msg_header_clean:
	rm -rf $(STATEDIR)/ct_ipc_msg_header.*
	rm -rf  $(PTXDIST_PLATFORMDIR)/sysroot-target/usr/include/ipc_msg_com.h


# vim: syntax=make
