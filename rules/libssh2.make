# -*-makefile-*-
#
# Copyright (C) 2009, 2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBSSH2) += libssh2

#
# Paths and names
#
LIBSSH2_VERSION	:= 1.9.0
LIBSSH2_MD5	:= 1beefafe8963982adc84b408b2959927
LIBSSH2		:= libssh2-$(LIBSSH2_VERSION)
LIBSSH2_SUFFIX	:= tar.gz
LIBSSH2_URL	:= http://www.libssh2.org/download/$(LIBSSH2).$(LIBSSH2_SUFFIX)
LIBSSH2_SOURCE	:= $(SRCDIR)/$(LIBSSH2).$(LIBSSH2_SUFFIX)
LIBSSH2_DIR	:= $(BUILDDIR)/$(LIBSSH2)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBSSH2_PATH	:= PATH=$(CROSS_PATH)
LIBSSH2_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
LIBSSH2_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--disable-rpath \
	--enable-crypt-none \
	--enable-mac-none \
	--disable-gex-new \
	--disable-clear-memory \
	--enable-debug \
	--enable-hidden-symbols \
	--enable-examples-build \
	--disable-werror \
	--with-crypto=openssl \
	--with-libz \
	--with-libssl-prefix=$(SYSROOT)/usr \
	--with-libz-prefix=$(SYSROOT)/usr


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libssh2.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libssh2)
	@$(call install_fixup, libssh2,PRIORITY,optional)
	@$(call install_fixup, libssh2,SECTION,base)
	@$(call install_fixup, libssh2,AUTHOR,"Marc Kleine-Budde <mkl@pengutronix.de>")
	@$(call install_fixup, libssh2,DESCRIPTION,missing)

	@$(call install_lib, libssh2, 0, 0, 0644, libssh2)

	@$(call install_finish, libssh2)

	@$(call touch)

# vim: syntax=make