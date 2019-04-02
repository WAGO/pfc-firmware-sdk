# -*-makefile-*-
#
# Copyright (C) 2015 by Mark Lindner <unknow@unknown.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBCONFIG) += libconfig

#
# Paths and names
#
LIBCONFIG_VERSION	:= 1.4.9
LIBCONFIG_MD5		:= b6ee0ce2b3ef844bad7cac2803a90634
LIBCONFIG		:= libconfig-$(LIBCONFIG_VERSION)
LIBCONFIG_SUFFIX	:= tar.gz
LIBCONFIG_URL		:= http://www.hyperrealm.com/libconfig/$(LIBCONFIG).$(LIBCONFIG_SUFFIX)
LIBCONFIG_SOURCE	:= $(SRCDIR)/$(LIBCONFIG).$(LIBCONFIG_SUFFIX)
LIBCONFIG_DIR		:= $(BUILDDIR)/$(LIBCONFIG)
LIBCONFIG_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBCONFIG_PATH := PATH=$(CROSS_PATH)
LIBCONFIG_ENV  := $(CROSS_ENV)

LIBCONFIG_COMPILE_ENV := \
	$(CROSS_ENV_CFLAGS) \
	$(CROSS_ENV_CPPFLAGS) \
	$(CROSS_ENV_LDFLAGS) \
	$(CROSS_ENV_AR)

#
# autoconf
#
LIBCONFIG_AUTOCONF := \
	$(CROSS_AUTOCONF_USR)

# 	--enable-protochain \
# 	--disable-optimizer-dbg \
# 	--disable-yydebug \
# 	--with-libnl \
# 	--without-dag \
# 	--without-septel

# ifdef PTXCONF_ARCH_MINGW
# LIBCONFIG_AUTOCONF += --with-pcap=null
# LIBCONFIG_ENV += ac_cv_lbl_gcc_fixincludes=yes
# else
# LIBCONFIG_AUTOCONF += --with-pcap=linux
# endif

# ifdef PTXCONF_LIBCONFIG_USE_DBUS
# LIBCONFIG_AUTOCONF += --enable-dbus 
# else
# LIBCONFIG_AUTOCONF += --disable-dbus
# endif

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libconfig.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  libconfig)
	@$(call install_fixup, libconfig,PRIORITY,optional)
	@$(call install_fixup, libconfig,SECTION,base)
	@$(call install_fixup, libconfig,AUTHOR,"Heinrich Toews <heinrich.toews@wago.com>")
	@$(call install_fixup, libconfig,DESCRIPTION,missing)

	@$(call install_lib, libconfig, 0, 0, 0644, libconfig)
	@$(call install_copy, libconfig, 0, 0, 0755, $(LIBCONFIG_DIR)/examples/c/example1, /usr/bin/example1)
	@$(call install_copy, libconfig, 0, 0, 0755, $(LIBCONFIG_DIR)/examples/c/example2, /usr/bin/example2)
	@$(call install_copy, libconfig, 0, 0, 0755, $(LIBCONFIG_DIR)/examples/c/example3, /usr/bin/example3)

	@$(call install_finish, libconfig)

	@$(call touch)

# vim: syntax=make
