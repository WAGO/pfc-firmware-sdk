# -*-makefile-*-
# $Id$
#
# Copyright (C) 2012 by <WAGO Kontakttechnik GmbH & Co. KG>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#

PACKAGES-$(PTXCONF_DPMDEMO) +=dpmdemo


#--- paths and names --------------------------------------------------------- 

DPMDEMO_VERSION	    				:= 1.0
DPMDEMO_NAME						:= dpmdemo
DPMDEMO_DIR							:= $(BUILDDIR)/$(DPMDEMO_NAME)
DPMDEMO_URL							:= file://$(SRCDIR)/$(DPMDEMO_NAME)
DPMDEMO_SRC     					:= $(SRCDIR)/$(DPMDEMO_NAME)


DPMDEMO_PATH						:= PATH=$(CROSS_PATH)
DPMDEMO_ENV							:= $(CROSS_ENV)
# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/$(DPMDEMO_NAME).get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/$(DPMDEMO_NAME).extract:
	@$(call targetinfo)
	@$(call clean, $(DPMDEMO_DIR))
	cp -rd $(DPMDEMO_SRC) $(DPMDEMO_DIR)
	@$(call touch)
	
$(STATEDIR)/$(DPMDEMO_NAME).extract.post:
	@$(call targetinfo)
	@$(call touch)
# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/$(DPMDEMO_NAME).prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/$(DPMDEMO_NAME).compile:
	@$(call targetinfo)
	@cd $(DPMDEMO_DIR)/project && \
		SYSROOT_TARGET=$(PTXDIST_PLATFORMDIR)/sysroot-target \
		$(DPMDEMO_ENV) $(DPMDEMO_PATH) DIST_DIR=$(PTXDIST_PLATFORMDIR) \
		env \
		CROSS_COMPILE=$(COMPILER_PREFIX) \
		$(MAKE)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/$(DPMDEMO_NAME).install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/$(DPMDEMO_NAME).targetinstall:
	@$(call targetinfo)
	@$(call install_init,	$(DPMDEMO_NAME))
	@$(call install_fixup, $(DPMDEMO_NAME),PRIORITY,optional)
	@$(call install_fixup, $(DPMDEMO_NAME),VERSION,$(DPMDEMO_VERSION))
	@$(call install_fixup, $(DPMDEMO_NAME),SECTION,base)
	@$(call install_fixup, $(DPMDEMO_NAME),AUTHOR,"Rolf Schulenburg")
#	@$(call install_fixup, $(DPMDEMO_NAME),DEPENDS,)
	@$(call install_fixup, $(DPMDEMO_NAME),DESCRIPTION,missing)
	@$(call install_copy, $(DPMDEMO_NAME), 0, 0, 0755,$(DPMDEMO_DIR)/project/$(DPMDEMO_NAME) , /usr/sbin/$(DPMDEMO_NAME))
	@$(call install_finish, $(DPMDEMO_NAME))
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/$(DPMDEMO_NAME).clean:
	@$(call targetinfo)
	@echo Deleting state files:
	@echo $(STATEDIR)/$(DPMDEMO_NAME).*
	@rm -rf $(STATEDIR)/$(DPMDEMO_NAME).*
	@echo
	@echo Deleting src dir:
	@echo $(DPMDEMO_DIR)
	@rm -rf $(DPMDEMO_DIR)
	@echo
	@echo Deleting installed files:
	@echo $(PTXCONF_SYSROOT_TARGET)/usr/sbin/$(DPMDEMO_NAME)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/sbin/$(DPMDEMO_NAME)
	@echo
