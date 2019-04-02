# -*-makefile-*-
# $Id$
#
# Copyright (C) 2008 by WAGO Kontakttechnik GmbH & KO.KG
# 
# This package installs the set_thread_priority script which
# allows to set the priority of several threads by its name.
# 
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CT_SET_THREAD_PRIORITY) += ct_set_thread_priority

#
# Paths and names
#
CT_SET_THREAD_PRIORITY_VERSION  := 1.0.0
CT_SET_THREAD_PRIORITY          := ct_set_thread_priority
CT_SET_THREAD_PRIORITY_SRC      := $(SRCDIR)/$(CT_SET_THREAD_PRIORITY)
CT_SET_THREAD_PRIORITY_URL      :=
CT_SET_THREAD_PRIORITY_DIR      := $(BUILDDIR)/$(CT_SET_THREAD_PRIORITY)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/ct_set_thread_priority.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/ct_set_thread_priority.extract:
	@$(call targetinfo)
	@cp -r --no-target-directory $(CT_SET_THREAD_PRIORITY_SRC) $(CT_SET_THREAD_PRIORITY_DIR)
	@$(call touch)

$(STATEDIR)/ct_set_thread_priority.extract.post:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/ct_set_thread_priority.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/ct_set_thread_priority.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ct_set_thread_priority.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

CT_STP_INIT_LOC=/etc/init.d/set_thread_priority
CT_STP_SCRIPT_LOC=/usr/sbin/set_thread_priority.sh
CT_STP_PLIST_LOC=/etc/specific/known_processes.lst

$(STATEDIR)/ct_set_thread_priority.targetinstall:
	@$(call targetinfo)

	@$(call install_init, ct_set_thread_priority)
	@$(call install_fixup,ct_set_thread_priority,PRIORITY,optional)
	@$(call install_fixup,ct_set_thread_priority,SECTION,base)
	@$(call install_fixup,ct_set_thread_priority,AUTHOR,"Wago Kontakttechnik")
	@$(call install_fixup,ct_set_thread_priority,DESCRIPTION,missing)

# Install the init.d script	
	@$(call install_copy, ct_set_thread_priority, 0, 0, 0755, $(PTXDIST_WORKSPACE)/projectroot/etc/init.d/set_thread_priority, $(CT_STP_INIT_LOC), n);

# Install set priority script
	@$(call install_copy, ct_set_thread_priority, 0, 0, 0755, $(CT_SET_THREAD_PRIORITY_DIR)/set_priority.sh, $(CT_STP_SCRIPT_LOC), n);
# Install the known_process file
	@$(call install_copy, ct_set_thread_priority, 0, 0, 0644, $(CT_SET_THREAD_PRIORITY_DIR)/known_processes.lst, $(CT_STP_PLIST_LOC), n);


	@$(call install_link, ct_set_thread_priority, ../init.d/set_thread_priority, /etc/rc.d/S90_set_thread_priority);

	@$(call install_finish,ct_set_thread_priority)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)ct_set_thread_priority.clean:
	rm -rf $(STATEDIR)/ct_set_thread_priority.*
	@$(call clean) 


# vim: syntax=make
