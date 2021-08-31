# -*-makefile-*-
#
# Copyright (C) 2010 by Robert Schwebel <r.schwebel@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBCAP) += libcap

#
# Paths and names
#
LIBCAP_VERSION	:= 2.41
LIBCAP_MD5	:= 1da3bac88e17186b78d1ca59f154c53a
LIBCAP		:= libcap-$(LIBCAP_VERSION)
LIBCAP_SUFFIX	:= tar.xz
LIBCAP_URL	:= \
	$(call ptx/mirror, KERNEL, libs/security/linux-privs/libcap2/$(LIBCAP).$(LIBCAP_SUFFIX))
LIBCAP_SOURCE	:= $(SRCDIR)/$(LIBCAP).$(LIBCAP_SUFFIX)
LIBCAP_DIR	:= $(BUILDDIR)/$(LIBCAP)
LIBCAP_LICENSE	:= BSD-3-Clause AND GPL-2.0-only
LIBCAP_LICENSE_FILES := file://License;md5=3f84fd6f29d453a56514cb7e4ead25f1

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBCAP_MAKE_OPT	:= \
	prefix=/usr lib=lib \
	CC=$(CROSS_CC) \
	BUILD_CC=$(HOSTCC) \
	DYNAMIC=yes \
	GOLANG=no \
	LIBATTR=$(call ptx/yesno, PTXCONF_LIBCAP_SETCAP) \
	PAM_CAP=$(call ptx/ifdef, PTXCONF_GLOBAL_PAM, yes, no)

LIBCAP_INSTALL_OPT :=  \
	$(LIBCAP_MAKE_OPT) \
	RAISE_SETFCAP=no \
	install

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libcap.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  libcap)
	@$(call install_fixup, libcap,PRIORITY,optional)
	@$(call install_fixup, libcap,SECTION,base)
	@$(call install_fixup, libcap,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libcap,DESCRIPTION,missing)

	@$(call install_copy, libcap, 0, 0, 0755, -, /usr/sbin/getpcaps)
	@$(call install_copy, libcap, 0, 0, 0755, -, /usr/sbin/capsh)
	@$(call install_lib,  libcap, 0, 0, 0644, libcap)
ifdef PTXCONF_GLOBAL_PAM
	@$(call install_copy, libcap, 0, 0, 0755, -, \
		/usr/lib/security/pam_cap.so)
endif
ifdef PTXCONF_LIBCAP_SETCAP
	@$(call install_copy, libcap, 0, 0, 0755, -, /usr/sbin/setcap)
	@$(call install_copy, libcap, 0, 0, 0755, -, /usr/sbin/getcap)
endif
	@$(call install_finish, libcap)

	@$(call touch)

# vim: syntax=make
