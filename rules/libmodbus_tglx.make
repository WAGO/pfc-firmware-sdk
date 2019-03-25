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
PACKAGES-$(PTXCONF_LIBMODBUS_TGLX) += libmodbus_tglx

#
# Paths and names
#
LIBMODBUS_TGLX_VERSION	:= 0.9.20
LIBMODBUS_TGLX		:= libModbus-$(LIBMODBUS_TGLX_VERSION)
LIBMODBUS_TGLX_SUFFIX	:= tar.gz
LIBMODBUS_TGLX_URL	:= http://svsv01003.wago.local/wago-ptxdis-src/$(LIBMODBUS_TGLX).$(LIBMODBUS_TGLX_SUFFIX)
LIBMODBUS_TGLX_SOURCE	:= $(SRCDIR)/$(LIBMODBUS_TGLX).$(LIBMODBUS_TGLX_SUFFIX)
LIBMODBUS_TGLX_DIR	:= $(BUILDDIR)/$(LIBMODBUS_TGLX)
LIBMODBUS_TGLX_MD5      := 25f181ae89ea033a592cc8957c89134b

LIBMODBUS_TGLX_LD_CURRENT=1
LIBMODBUS_TGLX_LD_AGE=0
LIBMODBUS_TGLX_LD_REVISION=19
LIBMODBUS_TGLX_LD_VERSION=$(LIBMODBUS_TGLX_LD_CURRENT).$(LIBMODBUS_TGLX_LD_AGE).$(LIBMODBUS_TGLX_LD_REVISION)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libmodbus_tglx.extract: $(STATEDIR)/autogen-tools

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBMODBUS_TGLX_PATH	:= PATH=$(CROSS_PATH)
LIBMODBUS_TGLX_ENV 	:= $(CROSS_ENV)

#
# autoconf
#

# libmbs relies on libModbus being installed to $(SYSROOT)/lib. 
# ptxproj.2011 and .2013 share libmbs source code so that we cannot fix paths in libmbs Makefile.
# Work-around: Remove hard-coded prefix/libdir options from CROSS_AUTOCONF_USR to preserve old behavior.
LIBMODBUS_TGLX_CONF_OPT = $(filter-out --prefix=/usr --libdir=/usr/lib,$(CROSS_AUTOCONF_USR)) --prefix= \
                          CFLAGS="$(CFLAGS) -std=gnu11 -I$(KERNEL_HEADERS_INCLUDE_DIR) -pthread -D_GNU_SOURCE -D__USE_GNU -fPIC -g3 -O2" \
                          LDFLAGS="$(LDFLAGS) -lrt -lpthread" \
                          --disable-silent-rules --enable-shared --enable-static

$(STATEDIR)/libmodbus_tglx.prepare:
	@$(call targetinfo)
	@$(call world/prepare, LIBMODBUS_TGLX)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libmodbus_tglx.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libmodbus_tglx)
	@$(call install_fixup, libmodbus_tglx,PRIORITY,optional)
	@$(call install_fixup, libmodbus_tglx,SECTION,base)
	@$(call install_fixup, libmodbus_tglx,AUTHOR,"Robert Schwebel <r.schwebel\@pengutronix.de>")
	@$(call install_fixup, libmodbus_tglx,DESCRIPTION,missing)

	echo libmodbusdir: $(LIBMODBUS_TGLX_DIR)
	@$(call install_copy, libmodbus_tglx, 0, 0, 0755, $(LIBMODBUS_TGLX_DIR)/src/.libs/libModbus-0.9.20.so.$(LIBMODBUS_TGLX_LD_VERSION), \
		/lib/libModbus-0.9.20.so.$(LIBMODBUS_TGLX_LD_VERSION))

	@$(call install_link, libmodbus_tglx, libModbus-0.9.20.so.$(LIBMODBUS_TGLX_LD_VERSION), /lib/libModbus-0.9.20.so.$(LIBMODBUS_TGLX_LD_CURRENT))
	@$(call install_link, libmodbus_tglx, libModbus-0.9.20.so.$(LIBMODBUS_TGLX_LD_VERSION), /lib/libModbus.so)


	@$(call install_finish, libmodbus_tglx)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libmodbus_tglx.clean:
	rm -rf $(STATEDIR)/libmodbus_tglx.*
	rm -rf $(PKGDIR)/libmodbus_tglx_*
	rm -rf $(LIBMODBUS_TGLX_DIR)

# vim: syntax=make
