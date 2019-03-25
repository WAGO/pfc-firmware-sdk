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
PACKAGES-$(PTXCONF_CANDEMO) +=candemo

#
# Paths and names
#
CANDEMO_VERSION	:= 0.3
CANDEMO		:= candemo
CANDEMO_URL	:= file://$(SRCDIR)/candemo
CANDEMO_DIR	:= $(BUILDDIR)/$(CANDEMO)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/candemo.get:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/candemo.extract:
	@$(call targetinfo, $@)
	@$(call clean, $(CANDEMO_DIR))
	@cp -rd $(SRCDIR)/$(CANDEMO) $(CANDEMO_DIR)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

CANDEMO_PATH	:= PATH=$(CROSS_PATH)
CANDEMO_ENV 	:= $(CROSS_ENV)

$(STATEDIR)/candemo.prepare:
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/candemo.compile:
	@$(call targetinfo, $@)
	@cd $(CANDEMO_DIR)/master && \
		$(CANDEMO_ENV) $(CANDEMO_PATH) DIST_DIR=$(PTXDIST_PLATFORMDIR) \
		env \
		DIST_DIR=$(PTXDIST_PLATFORMDIR) CROSS_COMPILE=$(COMPILER_PREFIX) \
		$(MAKE)		
	@cd $(CANDEMO_DIR)/slave && \
		$(CANDEMO_ENV) $(CANDEMO_PATH) DIST_DIR=$(PTXDIST_PLATFORMDIR) \
		env \
		DIST_DIR=$(PTXDIST_PLATFORMDIR) CROSS_COMPILE=$(COMPILER_PREFIX) \
		$(MAKE)			
	@cd $(CANDEMO_DIR)/layer2 && \
		$(CANDEMO_ENV) $(CANDEMO_PATH) DIST_DIR=$(PTXDIST_PLATFORMDIR) \
		env \
		DIST_DIR=$(PTXDIST_PLATFORMDIR) CROSS_COMPILE=$(COMPILER_PREFIX) \
		$(MAKE)			
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/candemo.install:
	@$(call targetinfo, $@)

	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/candemo.targetinstall:
	@$(call targetinfo, $@)

	@$(call install_init, candemo)
	@$(call install_fixup,candemo,PRIORITY,optional)
	@$(call install_fixup,candemo,VERSION,$(CANDEMO_VERSION))	
	@$(call install_fixup,candemo,SECTION,base)
	@$(call install_fixup,candemo,AUTHOR,"Axel Wueste")
	@$(call install_fixup,candemo,DESCRIPTION,missing)
	
#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish
#
	@$(call install_copy, candemo, 0, 0, 0755, $(CANDEMO_DIR)/master/candemo_master, /usr/sbin/candemo_master)
	@$(call install_copy, candemo, 0, 0, 0755, $(CANDEMO_DIR)/slave/candemo_slave, /usr/sbin/candemo_slave)
	@$(call install_copy, candemo, 0, 0, 0755, $(CANDEMO_DIR)/layer2/candemo_layer2, /usr/sbin/candemo_layer2)
	@$(call install_finish,candemo)

	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

candemo_clean:
	rm -rf $(STATEDIR)/candemo.*
	
# vim: syntax=make
