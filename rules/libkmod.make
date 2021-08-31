# -*-makefile-*-
#
# Copyright (C) 2011 by Juergen Beisert <jbe@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBKMOD) += libkmod

#
# Paths and names
#
LIBKMOD_VERSION	:= 27
LIBKMOD_MD5	:= 3973a74786670d3062d89a827e266581
LIBKMOD		:= kmod-$(LIBKMOD_VERSION)
LIBKMOD_SUFFIX	:= tar.xz
LIBKMOD_URL	:= $(call ptx/mirror, KERNEL, utils/kernel/kmod/$(LIBKMOD).$(LIBKMOD_SUFFIX))
LIBKMOD_SOURCE	:= $(SRCDIR)/$(LIBKMOD).$(LIBKMOD_SUFFIX)
LIBKMOD_DIR	:= $(BUILDDIR)/$(LIBKMOD)
# note: library: LGPLv2, tools: GPLv2
LIBKMOD_LICENSE	:= GPL-2.0-only AND LGPL-2.0-only
LIBKMOD_LICENSE_FILES := \
	file://tools/COPYING;md5=751419260aa954499f7abaabaa882bbe \
	file://libkmod/COPYING;md5=a6f89e2100d9b6cdffcea4f398e37343

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBKMOD_CONF_TOOL	:= autoconf
LIBKMOD_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--disable-experimental \
	--$(call ptx/endis, PTXCONF_LIBKMOD_TOOLS)-tools \
	--disable-manpages \
	--disable-test-modules \
	--$(call ptx/endis, PTXCONF_LIBKMOD_LOGGING)-logging \
	--$(call ptx/endis, PTXCONF_LIBKMOD_DEBUG)-debug \
	--disable-python \
	--disable-coverage \
	--without-xz \
	--$(call ptx/wwo, PTXCONF_LIBKMOD_ZLIB)-zlib \
	--without-openssl

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libkmod.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libkmod)
	@$(call install_fixup, libkmod,PRIORITY,optional)
	@$(call install_fixup, libkmod,SECTION,base)
	@$(call install_fixup, libkmod,AUTHOR,"Juergen Beisert <jbe@pengutronix.de>")
	@$(call install_fixup, libkmod,DESCRIPTION,"Linux kernel module handling")

	@$(call install_lib, libkmod, 0, 0, 0644, libkmod)

ifdef PTXCONF_LIBKMOD_TOOLS
	@$(call install_copy, libkmod, 0, 0, 0755, -, /usr/bin/kmod)
ifdef PTXCONF_LIBKMOD_INSMOD
	@$(call install_link, libkmod, ../bin/kmod, /usr/sbin/insmod)
endif
ifdef PTXCONF_LIBKMOD_RMMOD
	@$(call install_link, libkmod, ../bin/kmod, /usr/sbin/rmmod)
endif
ifdef PTXCONF_LIBKMOD_LSMOD
	@$(call install_link, libkmod, kmod, /usr/bin/lsmod)
endif
ifdef PTXCONF_LIBKMOD_MODINFO
	@$(call install_link, libkmod, ../bin/kmod, /usr/sbin/modinfo)
endif
ifdef PTXCONF_LIBKMOD_MODPROBE
	@$(call install_link, libkmod, ../bin/kmod, /usr/sbin/modprobe)
endif
ifdef PTXCONF_LIBKMOD_DEPMOD
	@$(call install_link, libkmod, ../bin/kmod, /usr/sbin/depmod)
endif
endif
	@$(call install_finish, libkmod)

	@$(call touch)

# vim: syntax=make
