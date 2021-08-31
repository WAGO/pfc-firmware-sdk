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
HOST_PACKAGES-$(PTXCONF_HOST_DBUS_GLIB) += host-dbus-glib

#
# Paths and names
#
HOST_DBUS_GLIB_DIR	= $(HOST_BUILDDIR)/$(DBUS_GLIB)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_DBUS_GLIB_CONF_TOOL	:= autoconf
HOST_DBUS_GLIB_CONF_OPT		:= \
	$(HOST_AUTOCONF) \
	--disable-tests \
	--disable-ansi \
	--disable-gcov \
	--disable-bash-completion \
	--disable-asserts \
	--enable-checks \
	--disable-gtk-doc

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-dbus-glib.install:
	@$(call targetinfo)
	@$(call install, HOST_DBUS_GLIB)
	install -D -m 644 "$(HOST_DBUS_GLIB_DIR)/dbus-bus-introspect.xml" \
		"$(HOST_DBUS_GLIB_PKGDIR)/share/dbus-glib/dbus-bus-introspect.xml"
	@$(call touch)

# vim: syntax=make
