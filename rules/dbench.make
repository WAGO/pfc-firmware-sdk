# -*-makefile-*-
#
# Copyright (C) 2005 by Robert Schwebel
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_DBENCH) += dbench

#
# Paths and names
#
DBENCH_VERSION	:= 3.04
DBENCH_MD5	:= efd0c958da79c1cd941ecd3f63e637ae
DBENCH		:= dbench-$(DBENCH_VERSION)
DBENCH_SUFFIX	:= tar.gz
DBENCH_URL	:= http://samba.org/ftp/tridge/dbench/$(DBENCH).$(DBENCH_SUFFIX)
DBENCH_SOURCE	:= $(SRCDIR)/$(DBENCH).$(DBENCH_SUFFIX)
DBENCH_DIR	:= $(BUILDDIR)/$(DBENCH)


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

DBENCH_PATH		:= PATH=$(CROSS_PATH)
DBENCH_CONV_ENV		:= $(CROSS_ENV)
DBENCH_MAKE_ENV		:= $(CROSS_ENV)
DBENCH_INSTALL_OPT	:= prefix=$(PKGDIR)/$(DBENCH)/usr install

#
# autoconf
#
DBENCH_AUTOCONF := $(CROSS_AUTOCONF_USR)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/dbench.targetinstall:
	@$(call targetinfo)

	@$(call install_init, dbench)
	@$(call install_fixup, dbench,PRIORITY,optional)
	@$(call install_fixup, dbench,SECTION,base)
	@$(call install_fixup, dbench,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, dbench,DESCRIPTION,missing)

ifdef PTXCONF_DBENCH_DBENCH
	@$(call install_copy, dbench, 0, 0, 0755, -, /usr/bin/dbench)
	@$(call install_copy, dbench, 0, 0, 0555, -, /usr/share/client.txt)
endif
ifdef PTXCONF_DBENCH_TBENCH
	@$(call install_copy, dbench, 0, 0, 0755, -, /usr/bin/tbench)
endif
ifdef PTXCONF_DBENCH_TBENCH_SERVER
	@$(call install_copy, dbench, 0, 0, 0755, -, /usr/bin/tbench_srv)
endif
	@$(call install_finish, dbench)

	@$(call touch)

# vim: syntax=make
