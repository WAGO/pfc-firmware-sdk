# -*-makefile-*-
#
# Copyright (C) 2015 by <HFS>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_WAGO_PFC_DIAGNOSTIC) += host-wago-pfc-diagnostic

#
# Paths and names
#
HOST_WAGO_PFC_DIAGNOSTIC_VERSION	:= 
HOST_WAGO_PFC_DIAGNOSTIC_MD5	:=
HOST_WAGO_PFC_DIAGNOSTIC		:= wago-pfc-diagnostic
HOST_WAGO_PFC_DIAGNOSTIC_SUFFIX	:= 
HOST_WAGO_PFC_DIAGNOSTIC_URL	:= file://$(PTXDIST_WORKSPACE)/local_src/$(HOST_WAGO_PFC_DIAGNOSTIC)
#HOST_WAGO_PFC_DIAGNOSTIC_SOURCE	:= $(PTXDIST_WORKSPACE)/local_src/$(HOST_WAGO_PFC_DIAGNOSTIC)
HOST_WAGO_PFC_DIAGNOSTIC_DIR	:= $(HOST_BUILDDIR)/$(HOST_WAGO_PFC_DIAGNOSTIC)
HOST_WAGO_PFC_DIAGNOSTIC_DIAG_DIR   := $(HOST_WAGO_PFC_DIAGNOSTIC_DIR)/src/diagnostic

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/host-wago-pfc-diagnostic.extract:
	@$(call targetinfo)
	@$(call clean, $(HOST_WAGO_PFC_DIAGNOSTIC_DIR))
	mkdir -p $(HOST_WAGO_PFC_DIAGNOSTIC_DIR)
	rsync -a --exclude=".*" --exclude=objs/ --exclude="*.d" --exclude="*.o" \
  $(PTXDIST_WORKSPACE)/local_src/$(HOST_WAGO_PFC_DIAGNOSTIC)/* $(HOST_WAGO_PFC_DIAGNOSTIC_DIR)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#HOST_WAGO_PFC_DIAGNOSTIC_CONF_ENV	:= $(HOST_ENV)

#
# autoconf
#
#HOST_WAGO_PFC_DIAGNOSTIC_CONF_TOOL	:= autoconf
#HOST_WAGO_PFC_DIAGNOSTIC_CONF_OPT	:= $(HOST_AUTOCONF)

$(STATEDIR)/host-wago-pfc-diagnostic.prepare:
	@$(call targetinfo)
#	@$(call clean, $(HOST_WAGO_PFC_DIAGNOSTIC_DIR)/config.cache)
#	cd $(HOST_WAGO_PFC_DIAGNOSTIC_DIR) && \
#		$(HOST_WAGO_PFC_DIAGNOSTIC_PATH) $(HOST_WAGO_PFC_DIAGNOSTIC_ENV) \
#		./configure $(HOST_WAGO_PFC_DIAGNOSTIC_CONF_OPT)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/host-wago-pfc-diagnostic.compile:
	@$(call targetinfo)
#	@$(call world/compile, HOST_WAGO_PFC_DIAGNOSTIC)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-wago-pfc-diagnostic.install:
	@$(call targetinfo)
#	@$(call world/install, HOST_WAGO_PFC_DIAGNOSTIC)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/host-wago-pfc-diagnostic.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, HOST_WAGO_PFC_DIAGNOSTIC)

# vim: syntax=make
