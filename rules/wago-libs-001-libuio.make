# -*-makefile-*-
# $Id: template-make 8509 2008-06-12 12:45:40Z mkl $
#
# Copyright (C) 2010 by WAGO
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBUIO) += libuio

#
# Paths and names
#
LIBUIO_VERSION 		:= 0
LIBUIO			:= libuio
LIBUIO_SUFFIX		:= tar.gz
#LIBUIO_URL		:= file://
LIBUIO_SOURCE		:= $(SRCDIR)/$(LIBUIO).$(LIBUIO_SUFFIX)
LIBUIO_DIR		:= $(BUILDDIR)/$(LIBUIO)
LIBUIO_MD5      := d41d8cd98f00b204e9800998ecf8427e

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(LIBUIO_SOURCE):
	@$(call targetinfo, $@)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libuio.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(LIBUIO_DIR))
	cp -r $(SRCDIR)/$(LIBUIO) $(LIBUIO_DIR)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBUIO_PATH	:= PATH=$(CROSS_PATH)
LIBUIO_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
LIBUIO_AUTOCONF := $(CROSS_AUTOCONF_USR)

$(STATEDIR)/libuio.prepare:
	@$(call targetinfo)
	@$(call clean, $(LIBUIO_DIR)/config.cache)
	cd $(LIBUIO_DIR) && \
	./autogen.sh && \
		$(LIBUIO_PATH) $(LIBUIO_ENV) \
		./configure $(LIBUIO_AUTOCONF)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/libuio.compile:
	@$(call targetinfo)
	cd $(LIBUIO_DIR) && $(LIBUIO_PATH) $(MAKE) $(PARALLELMFLAGS)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libuio.install:
	@$(call targetinfo)
	@$(call world/install, LIBUIO)
#	cp $(SRCDIR)/$(LIBUIO)/src/libUIO.h $(PTXCONF_SYSROOT_TARGET)/usr/include
	cp $(SRCDIR)/$(LIBUIO)/src/libUIO.h $(PTXCONF_SYSROOT_TARGET)/usr/include/libuio.h
#	cp -d $(LIBUIO_DIR)/src/.libs/libuio.a $(PTXCONF_SYSROOT_TARGET)/usr/lib
#	cp -d $(LIBUIO_DIR)/src/.libs/libuio.so $(PTXCONF_SYSROOT_TARGET)/usr/lib
#	cp -d $(LIBUIO_DIR)/src/.libs/libuio.so.0.0.0 $(PTXCONF_SYSROOT_TARGET)/usr/lib
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libuio.targetinstall:$(libuio_install_deps_default)
	@$(call targetinfo)

	@$(call install_init, libuio)
	@$(call install_fixup, libuio,PRIORITY,optional)
	@$(call install_fixup, libuio,SECTION,base)
	@$(call install_fixup, libuio,AUTHOR,"WAGO <www.wago.com>")
	@$(call install_fixup, libuio,DESCRIPTION,missing)

	echo libuiodir: $(LIBUIO_DIR)
	@$(call install_copy, libuio, 0, 0, 0755, $(LIBUIO_DIR)/src/.libs/libuio.so.0.0.0, /lib/libuio.so.0.0.0)

#   *.a not needed on target 
#	@$(call install_copy, libuio, 0, 0, 0644, $(LIBUIO_DIR)/src/.libs/libuio.a, /lib/libuio.a)
#	@$(call install_copy, libuio, 0, 0, 0644, $(LIBUIO_DIR)/src/.libs/libuio.lai, /lib/libuio.lai)

	@$(call install_link, libuio, libuio.so.0.0.0, /lib/libuio.so.0)
	@$(call install_link, libuio, libuio.so.0.0.0, /lib/libuio.so)

	@$(call install_finish, libuio)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

libuio_clean:
	rm -rf $(STATEDIR)/libuio.*
	rm -rf $(PKGDIR)/libuio_*
	rm -rf $(LIBUIO_DIR)

# vim: syntax=make
