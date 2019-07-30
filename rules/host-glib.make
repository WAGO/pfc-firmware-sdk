# -*-makefile-*-
#
# Copyright (C) 2007 by Luotao Fu <lfu@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_GLIB) += host-glib

#
# Paths and names
#
HOST_GLIB_DIR	= $(HOST_BUILDDIR)/$(GLIB)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_GLIB_CONF_ENV	:= \
	$(HOST_ENV) \
	ac_cv_path_MSGFMT="" \
	ac_cv_path_XGETTEXT="" \
	ac_cv_prog_GTKDOC_CHECK="" \
	ac_cv_path_GTKDOC_REBASE="" \
	ac_cv_path_GTKDOC_MKPDF=""

#
# autoconf
#
HOST_GLIB_CONF_TOOL	:= autoconf
HOST_GLIB_CONF_OPT	:= \
	$(HOST_AUTOCONF) \
	--disable-maintainer-mode \
	--disable-debug \
	--disable-gc-friendly \
	--enable-mem-pools \
	--disable-installed-tests \
	--disable-always-build-tests \
	--disable-static \
	--enable-shared \
	--disable-included-printf \
	--disable-selinux \
	--disable-fam \
	--disable-xattr \
	--disable-libelf \
	--disable-libmount \
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

$(STATEDIR)/host-glib.install.post:
	@$(call targetinfo)
	@$(call world/install.post, HOST_GLIB)
	@sed -i "s:'/share':'$(PTXCONF_SYSROOT_HOST)/share':" "$(PTXCONF_SYSROOT_HOST)/bin/gdbus-codegen"
	@sed -i "s:^prefix=.*:prefix=$(PTXCONF_SYSROOT_HOST):" "$(PTXCONF_SYSROOT_HOST)/bin/glib-gettextize"
	@$(call touch)

# vim: syntax=make
