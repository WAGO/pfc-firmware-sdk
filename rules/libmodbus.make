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
PACKAGES-$(PTXCONF_LIBMODBUS) += libmodbus

#
# Paths and names
#
LIBMODBUS_VERSION_MAJOR=3
LIBMODBUS_VERSION_MINOR=0
LIBMODBUS_VERSION_MICRO=5
LIBMODBUS_VERSION := $(LIBMODBUS_VERSION_MAJOR).$(LIBMODBUS_VERSION_MINOR).$(LIBMODBUS_VERSION_MICRO)
LIBMODBUS_LD_CURRENT=750
LIBMODBUS_LD_AGE=0
LIBMODBUS_LD_REVISION=39
LIBMODBUS_LD_VERSION=$(LIBMODBUS_LD_CURRENT).$(LIBMODBUS_LD_AGE).$(LIBMODBUS_LD_REVISION)
LIBMODBUS          := libmodbus-$(LIBMODBUS_VERSION)
LIBMODBUS_SUFFIX   := tar.gz
LIBMODBUS_SOURCE   := $(SRCDIR)/$(LIBMODBUS).$(LIBMODBUS_SUFFIX)
LIBMODBUS_URL      := http://svsv01003.wago.local/wago-ptxdist-src/$(LIBMODBUS).$(LIBMODBUS_SUFFIX)
LIBMODBUS_DIR      := $(BUILDDIR)/$(LIBMODBUS)
LIBMODBUS_PATH     := PATH=$(CROSS_PATH)
LIBMODBUS_ENV      := $(CROSS_ENV)
LIBMODBUS_MD5      := 4d539942e45afd05565235934ca99b35


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
LIBMODBUS_CONF_TOOL	:= autoconf

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libmodbus.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libmodbus)
	@$(call install_fixup, libmodbus,PACKAGE,libmodbus)
	@$(call install_fixup, libmodbus,PRIORITY,optional)
	@$(call install_fixup, libmodbus,VERSION,$(LIBMODBUS_VERSION))
	@$(call install_fixup, libmodbus,SECTION,base)
	@$(call install_fixup, libmodbus,AUTHOR,”Timur Celik <timur.celik@wago.com>”)
	@$(call install_fixup, libmodbus,DEPENDS,)
	@$(call install_fixup, libmodbus,DESCRIPTION,missing)

	@$(call install_copy, libmodbus, 0, 0, 0755, $(LIBMODBUS_DIR)/src/.libs/libmodbus.so.$(LIBMODBUS_LD_VERSION), /usr/lib/libmodbus.so.$(LIBMODBUS_LD_VERSION))

	@$(call install_link, libmodbus, libmodbus.so.$(LIBMODBUS_LD_VERSION), /usr/lib/libmodbus.so.$(LIBMODBUS_LD_CURRENT))
	@$(call install_link, libmodbus, libmodbus.so.$(LIBMODBUS_LD_VERSION), /usr/lib/libmodbus.so)

	@$(call install_finish, libmodbus)
	@$(call touch)



# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libmodbus.clean:
	rm -rf $(STATEDIR)/libmodbus.*
	rm -rf $(PKGDIR)/libmodbus_*
	rm -rf $(LIBMODBUS_DIR)

# vim: syntax=make
