# -*-makefile-*-
#
# Copyright (C) 2007 by Robert Schwebel
#               2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_DBUS_GLIB) += dbus-glib

#
# Paths and names
#
DBUS_GLIB_VERSION	:= 0.104
DBUS_GLIB_MD5		:= 5497d2070709cf796f1878c75a72a039
DBUS_GLIB		:= dbus-glib-$(DBUS_GLIB_VERSION)
DBUS_GLIB_SUFFIX	:= tar.gz
DBUS_GLIB_URL		:= http://dbus.freedesktop.org/releases/dbus-glib/$(DBUS_GLIB).$(DBUS_GLIB_SUFFIX)
DBUS_GLIB_SOURCE	:= $(SRCDIR)/$(DBUS_GLIB).$(DBUS_GLIB_SUFFIX)
DBUS_GLIB_DIR		:= $(BUILDDIR)/$(DBUS_GLIB)
DBUS_GLIB_LICENSE	:= AFL-2.1 AND GPL-2.0-or-later

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
DBUS_GLIB_CONF_TOOL	:= autoconf
DBUS_GLIB_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-tests \
	--disable-ansi \
	--disable-gcov \
	--disable-bash-completion \
	--disable-asserts \
	--enable-checks \
	--disable-gtk-doc \
	--with-dbus-binding-tool=$(PTXDIST_SYSROOT_HOST)/bin/dbus-binding-tool \
	--with-introspect-xml=$(PTXDIST_SYSROOT_HOST)/share/dbus-glib/dbus-bus-introspect.xml

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/dbus-glib.targetinstall:
	@$(call targetinfo)

	@$(call install_init, dbus-glib)
	@$(call install_fixup, dbus-glib,PRIORITY,optional)
	@$(call install_fixup, dbus-glib,SECTION,base)
	@$(call install_fixup, dbus-glib,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, dbus-glib,DESCRIPTION,missing)

	@$(call install_lib, dbus-glib, 0, 0, 0644, libdbus-glib-1)

	@$(call install_finish, dbus-glib)

	@$(call touch)

# vim: syntax=make
