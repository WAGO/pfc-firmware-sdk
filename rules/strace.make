# -*-makefile-*-
#
# Copyright (C) 2003 by Auerswald GmbH & Co. KG, Schandelah, Germany
#               2003-2008 by Pengutronix e.K., Hildesheim, Germany
#               2009, 2010, 2012 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_STRACE) += strace

#
# Paths and names
#
STRACE_VERSION	:= 4.26
STRACE_MD5	:= daa51acc0c7c696221ec03cf0b30a7af
STRACE		:= strace-$(STRACE_VERSION)
STRACE_SUFFIX	:= tar.xz
STRACE_URL	:= https://strace.io/files/$(STRACE_VERSION)/$(STRACE).$(STRACE_SUFFIX)
STRACE_SOURCE	:= $(SRCDIR)/$(STRACE).$(STRACE_SUFFIX)
STRACE_DIR	:= $(BUILDDIR)/$(STRACE)
STRACE_LICENSE	:= BSD-3-Clause

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

STRACE_CONF_TOOL	:= autoconf
STRACE_CONF_OPT		:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-gcc-Werror \
	--disable-code-coverage \
	--disable-arm-oabi \
	--disable-stacktrace \
	--disable-mpers \
	--disable-valgrind \
	--disable-valgrind-memcheck \
	--disable-valgrind-helgrind \
	--disable-valgrind-drd \
	--disable-valgrind-sgcheck \
	--without-gcov \
	--without-libdw \
	--without-libunwind \
	--without-libiberty


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/strace.targetinstall:
	@$(call targetinfo)

	@$(call install_init, strace)
	@$(call install_fixup, strace,PRIORITY,optional)
	@$(call install_fixup, strace,SECTION,base)
	@$(call install_fixup, strace,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, strace,DESCRIPTION,missing)

	@$(call install_copy, strace, 0, 0, 0755, -, /usr/bin/strace)

	@$(call install_finish, strace)

	@$(call touch)

# vim: syntax=make
