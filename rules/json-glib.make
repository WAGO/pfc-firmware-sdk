# -*-makefile-*-
#
# Copyright (C) 2016 by Bastian Stender <bst@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_JSON_GLIB) += json-glib

#
# Paths and names
#
JSON_GLIB_VERSION	:= 1.2.8
JSON_GLIB_MD5		:= ff31e7d0594df44318e12facda3d086e
JSON_GLIB		:= json-glib-$(JSON_GLIB_VERSION)
JSON_GLIB_SUFFIX	:= tar.xz
JSON_GLIB_URL		:= http://ftp.gnome.org/pub/GNOME/sources/json-glib/1.2/$(JSON_GLIB).$(JSON_GLIB_SUFFIX)
JSON_GLIB_SOURCE	:= $(SRCDIR)/$(JSON_GLIB).$(JSON_GLIB_SUFFIX)
JSON_GLIB_DIR		:= $(BUILDDIR)/$(JSON_GLIB)
JSON_GLIB_LICENSE	:= LGPL-2.1-only

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
JSON_GLIB_CONF_TOOL	:= autoconf
JSON_GLIB_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-glibtest \
	--enable-debug=minimum \
	--enable-maintainer-flags=no \
	--disable-installed-tests \
	--disable-always-build-tests \
	--disable-gcov \
	--disable-gtk-doc \
	--disable-gtk-doc-html \
	--disable-gtk-doc-pdf \
	--disable-man \
	--$(call ptx/endis, PTXCONF_JSON_GLIB_INTROSPECTION)-introspection \
	--disable-nls \
	--disable-rpath


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/json-glib.targetinstall:
	@$(call targetinfo)

	@$(call install_init, json-glib)
	@$(call install_fixup, json-glib, PRIORITY, optional)
	@$(call install_fixup, json-glib, SECTION, base)
	@$(call install_fixup, json-glib, AUTHOR, "Bastian Stender <bst@pengutronix.de>")
	@$(call install_fixup, json-glib, DESCRIPTION, \
		"A library providing (de)serialization support for the JSON format.")

	@$(call install_lib, json-glib, 0, 0, 0644, libjson-glib-1.0)

	@$(call install_finish, json-glib)

	@$(call touch)

# vim: syntax=make
