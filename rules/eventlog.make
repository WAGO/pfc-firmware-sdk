# -*-makefile-*-
#
# Copyright (C) 2008, 2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_EVENTLOG) += eventlog

#
# Paths and names
#
EVENTLOG_VERSION	:= 0.2.12
EVENTLOG_MD5		:= 7d95392a6618437a0404a3fadb49f53e  
EVENTLOG		:= eventlog-$(EVENTLOG_VERSION)
EVENTLOG_SUFFIX		:= tar.gz
EVENTLOG_URL		:= http://my.balabit.com/downloads/syslog-ng/sources/3.3.4/source/eventlog-$(EVENTLOG_VERSION).$(EVENTLOG_SUFFIX)
EVENTLOG_SOURCE		:= $(SRCDIR)/$(EVENTLOG).$(EVENTLOG_SUFFIX)
EVENTLOG_DIR		:= $(BUILDDIR)/$(EVENTLOG)
EVENTLOG_LICENSE	:= Custom License (BSD-like)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

EVENTLOG_PATH	:= PATH=$(CROSS_PATH)
EVENTLOG_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
EVENTLOG_AUTOCONF := $(CROSS_AUTOCONF_USR)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/eventlog.targetinstall:
	@$(call targetinfo)

	@$(call install_init, eventlog)
	@$(call install_fixup, eventlog,PRIORITY,optional)
	@$(call install_fixup, eventlog,SECTION,base)
	@$(call install_fixup, eventlog,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, eventlog,DESCRIPTION,missing)

	@$(call install_lib, eventlog, 0, 0, 0644, libevtlog)

	@$(call install_finish, eventlog)

	@$(call touch)

# vim: syntax=make
