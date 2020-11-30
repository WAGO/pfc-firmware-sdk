# -*-makefile-*-
#
# Copyright (C) 2007 by Luotao Fu <lfu@pengutronix.de>
#               2008, 2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_LOCALEDEF) += host-localedef

#
# Paths and names
#
HOST_LOCALEDEF_VERSION	:= 2.27
HOST_LOCALEDEF_MD5	:= 274e794c999b4ebbccdf72bfb4c49a68
HOST_LOCALEDEF		:= localedef-glibc-$(HOST_LOCALEDEF_VERSION)
HOST_LOCALEDEF_SUFFIX	:= tar.bz2
HOST_LOCALEDEF_URL	:= $(call ptx/mirror, GNU, glibc/glibc-$(HOST_LOCALEDEF_VERSION).$(HOST_LOCALEDEF_SUFFIX))
HOST_LOCALEDEF_SOURCE	:= $(SRCDIR)/glibc-$(HOST_LOCALEDEF_VERSION).$(HOST_LOCALEDEF_SUFFIX)
HOST_LOCALEDEF_DIR	:= $(HOST_BUILDDIR)/$(HOST_LOCALEDEF)
HOST_LOCALEDEF_BUILD_OOT:= YES
HOST_LOCALEDEF_LICENSE	:= LGPL-2.1-or-later AND GPL-2.0-or-later

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_LOCALEDEF_CONF_TOOL	:= autoconf
HOST_LOCALEDEF_CONF_OPT		:= \
	--prefix=/usr \
	--enable-sanity-checks \
	--disable-profile \
	--disable-static-pie \
	--disable-timezone-tools \
	--disable-hardcoded-path-in-tests \
	--enable-stackguard-randomization \
	--enable-hidden-plt \
	--enable-bind-now \
	--enable-stack-protector \
	--disable-static-nss \
	--disable-force-install \
	--disable-all-warnings \
	--disable-werror \
	--disable-multi-arch \
	--disable-experimental-malloc \
	--disable-nss-crypt \
	--disable-obsolete-rpc \
	--disable-obsolete-nsl \
	--disable-systemtap \
	--disable-build-nscd \
	--disable-nscd \
	--disable-pt_chown \
	--disable-tunables \
	--disable-mathvec

HOST_LOCALEDEF_MAKE_OPT		:= locale/others
HOST_LOCALEDEF_INSTALL_OPT	:= locale/install-others

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-localedef.install:
	@$(call targetinfo)
	@install -vD -m755 $(HOST_LOCALEDEF_DIR)-build/locale/localedef \
		$(HOST_LOCALEDEF_PKGDIR)/bin/localedef
	@$(call touch)

# vim: syntax=make
