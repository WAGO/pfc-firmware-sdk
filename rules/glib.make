# -*-makefile-*-
#
# Copyright (C) 2006-2009 by Robert Schwebel <r.schwebel@pengutronix.de>
#                            Pengutronix <info@pengutronix.de>, Germany
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_GLIB) += glib

#
# Paths and names
#
GLIB_VERSION	:= 2.56.4
GLIB_MD5	:= 17c3dca43d99a4882384f1a7b530b80b
GLIB		:= glib-$(GLIB_VERSION)
GLIB_SUFFIX	:= tar.xz
GLIB_SOURCE	:= $(SRCDIR)/$(GLIB).$(GLIB_SUFFIX)
GLIB_DIR	:= $(BUILDDIR)/$(GLIB)

GLIB_URL	:= http://ftp.gnome.org/pub/GNOME/sources/glib/$(basename $(GLIB_VERSION))/glib-$(GLIB_VERSION).$(GLIB_SUFFIX)

GLIB_LICENSE	:= LGPL-2.0-or-later

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

# If one of GTKDOC tools is found at configure stage, it might be used,
# no matter whether we use --disable-gtk-doc as this option controls
# generating documentation, while in tarballs it is already generated
# and providing empty paths to GTKDOC tools avoids doc installation.
GLIB_CONF_ENV	:= \
	$(CROSS_ENV) \
	glib_cv_uscore=no \
	glib_cv_stack_grows=no \
	ac_cv_path_MSGFMT="" \
	ac_cv_path_XGETTEXT="" \
	ac_cv_prog_GTKDOC_CHECK="" \
	ac_cv_path_GTKDOC_REBASE="" \
	ac_cv_path_GTKDOC_MKPDF=""

#
# autoconf
#
# --with-libiconv=no does also find the libc iconv implementation! So it
# is the right choice for no locales and locales-via-libc
#

GLIB_CONF_TOOL	:= autoconf
GLIB_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-maintainer-mode \
	--enable-debug=minimum \
	--disable-gc-friendly \
	--enable-mem-pools \
	--disable-installed-tests \
	--disable-always-build-tests \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--disable-static \
	--enable-shared \
	--disable-included-printf \
	--disable-selinux \
	--disable-fam \
	--disable-xattr \
	--disable-libelf \
	--$(call ptx/endis, PTXCONF_GLIB_LIBMOUNT)-libmount \
	--disable-gtk-doc \
	--disable-gtk-doc-html \
	--disable-gtk-doc-pdf \
	--disable-man \
	--disable-dtrace \
	--disable-systemtap \
	--disable-coverage \
	--with-libiconv=no \
	--with-threads=posix \
	--with-pcre=system

# workaround for broken libtool
GLIB_CFLAGS:= -Wl,-rpath-link,$(GLIB_DIR)/gmodule/.libs

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/glib.install:
	@$(call targetinfo)
	@$(call world/install, GLIB)
	@mkdir -p $(PTXCONF_SYSROOT_TARGET)/usr/include/glib-2.0
	@cp $(GLIB_DIR)/glib/glibconfig.h $(PTXCONF_SYSROOT_TARGET)/usr/include
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/glib.targetinstall:
	@$(call targetinfo)

	@$(call install_init, glib)
	@$(call install_fixup, glib,PRIORITY,optional)
	@$(call install_fixup, glib,SECTION,base)
	@$(call install_fixup, glib,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, glib,DESCRIPTION,missing)

	@$(call install_copy, glib, 0, 0, 0755, /usr/lib/gio/modules)

	@for i in libgio-2.0 libglib-2.0 libgmodule-2.0 libgobject-2.0 libgthread-2.0; do \
		$(call install_lib, glib, 0, 0, 0644, $$i); \
	done

ifdef PTXCONF_GLIB_GDBUS
	@$(call install_copy, glib, 0, 0, 0755, -, /usr/bin/gdbus)
endif
	@$(call install_finish, glib)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/glib.clean:
	@$(call targetinfo)
	@$(call clean_pkg, glib)
	rm -f $(STATEDIR)/glib.*
	rm -rf $(GLIB_DIR)
	rm -Rf $(PTXCONF_SYSROOT_TARGET)/usr/include/glib-2.0

# vim: syntax=make
