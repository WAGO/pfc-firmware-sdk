# -*-makefile-*-
#
# Copyright (C) 2011 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_TRACE_CMD) += trace-cmd

#
# Paths and names
#
TRACE_CMD_VERSION	:= 2.6.2
TRACE_CMD_MD5           := 679260b17bc971fa22667c88f909b975
TRACE_CMD		:= trace-cmd-v$(TRACE_CMD_VERSION)
TRACE_CMD_SUFFIX	:= tar.gz
TRACE_CMD_URL		:= http://git.kernel.org/pub/scm/linux/kernel/git/rostedt/trace-cmd.git;tag=trace-cmd-v$(TRACE_CMD_VERSION)
TRACE_CMD_SOURCE	:= $(SRCDIR)/$(TRACE_CMD).$(TRACE_CMD_SUFFIX)
TRACE_CMD_DIR		:= $(BUILDDIR)/$(TRACE_CMD)
TRACE_CMD_LICENSE	:= GPL-2.0

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

TRACE_CMD_CONF_TOOL	:= NO

TRACE_CMD_MAKE_ENV	:= \
	$(CROSS_ENV) \
	CROSS_COMPILE=$(COMPILER_PREFIX) \
	NO_PYTHON=1 \
	prefix=/usr

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

TRACE_CMD_PLUGINS	:= \
	blk \
	cfg80211 \
	function \
	hrtimer \
	jbd2 \
	kmem \
	kvm \
	mac80211 \
	sched_switch \
	scsi \
	xen


$(STATEDIR)/trace-cmd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, trace-cmd)
	@$(call install_fixup, trace-cmd,PRIORITY,optional)
	@$(call install_fixup, trace-cmd,SECTION,base)
	@$(call install_fixup, trace-cmd,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, trace-cmd,DESCRIPTION,missing)

	@$(call install_copy, trace-cmd, 0, 0, 0755, -, /usr/bin/trace-cmd)

	@$(foreach plugin, $(TRACE_CMD_PLUGINS), \
		$(call install_lib, trace-cmd, 0, 0, 0644, \
			trace-cmd/plugins/plugin_$(plugin));)

	@$(call install_finish, trace-cmd)

	@$(call touch)

# vim: syntax=make
