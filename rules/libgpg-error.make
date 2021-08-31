# -*-makefile-*-
#
# Copyright (C) 2009 by Erwin Rol
#               2010, 2013 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBGPG_ERROR) += libgpg-error

#
# Paths and names
#
LIBGPG_ERROR_VERSION	:= 1.36
LIBGPG_ERROR_MD5	:= eff437f397e858a9127b76c0d87fa5ed
LIBGPG_ERROR		:= libgpg-error-$(LIBGPG_ERROR_VERSION)
LIBGPG_ERROR_SUFFIX	:= tar.bz2
LIBGPG_ERROR_URL	:= \
	http://artfiles.org/gnupg.org/libgpg-error/$(LIBGPG_ERROR).$(LIBGPG_ERROR_SUFFIX) \
	https://www.gnupg.org/ftp/gcrypt/libgpg-error/$(LIBGPG_ERROR).$(LIBGPG_ERROR_SUFFIX) \
	ftp://ftp.gnupg.org/gcrypt/libgpg-error/$(LIBGPG_ERROR).$(LIBGPG_ERROR_SUFFIX)
LIBGPG_ERROR_SOURCE	:= $(SRCDIR)/$(LIBGPG_ERROR).$(LIBGPG_ERROR_SUFFIX)
LIBGPG_ERROR_DIR	:= $(BUILDDIR)/$(LIBGPG_ERROR)
LIBGPG_ERROR_LICENSE	:= GPL-2.0-only AND LGPL-2.0-only
LIBGPG_ERROR_LICENSE_FILES := \
	file://COPYING;md5=59530bdf33659b29e73d4adb9f9f6552 \
	file://COPYING.LIB;md5=2d5025d4aa3495befef8f17206a5b0a1

# workaround for linaro toolchain that uses a target triple instead of a quadruple
# Use '=' to delay $(shell ...) calls until this is needed
LIBGPG_ERROR_TARGET	= $(subst arm-linux-,arm-unknown-linux-,$(patsubst %-gnueabihf,%-gnueabi,$(patsubst i%86-pc-linux-gnu,i686-pc-linux-gnu,$(shell target=$(PTXCONF_GNU_TARGET); echo $${target/-*-linux/-$(if $(PTXCONF_ARCH_X86),pc,unknown)-linux}))))

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
LIBGPG_ERROR_CONF_TOOL	:= autoconf
LIBGPG_ERROR_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--enable-threads=posix \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--disable-nls \
	--disable-rpath \
	--disable-log-clock \
	--disable-werror \
	--enable-build-timestamp="$(PTXDIST_BUILD_TIMESTAMP)" \
	--disable-languages \
	--disable-doc \
	--disable-tests

$(STATEDIR)/libgpg-error.prepare:
	@$(call targetinfo)
	@cp -v $(LIBGPG_ERROR_DIR)/src/syscfg/lock-obj-pub.$(LIBGPG_ERROR_TARGET).h \
		$(LIBGPG_ERROR_DIR)/src/syscfg/lock-obj-pub.$(call remove_quotes, $(PTXCONF_GNU_TARGET)).h
	@$(call world/prepare, LIBGPG_ERROR)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libgpg-error.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libgpg-error)
	@$(call install_fixup, libgpg-error,PRIORITY,optional)
	@$(call install_fixup, libgpg-error,SECTION,base)
	@$(call install_fixup, libgpg-error,AUTHOR,"Erwin Rol")
	@$(call install_fixup, libgpg-error,DESCRIPTION,missing)

	@$(call install_lib, libgpg-error, 0, 0, 0644, libgpg-error)

	@$(call install_finish, libgpg-error)

	@$(call touch)

# vim: syntax=make
