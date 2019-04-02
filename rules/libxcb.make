# -*-makefile-*-
#
# Copyright (C) 2007 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBXCB) += libxcb

#
# Paths and names
#
LIBXCB_VERSION		:= 1.13
LIBXCB_MD5		:= c2b6cf928afa16b0047c974e7aaa783f
LIBXCB			:= libxcb-$(LIBXCB_VERSION)
LIBXCB_SUFFIX		:= tar.bz2
LIBXCB_URL		:= http://xcb.freedesktop.org/dist/$(LIBXCB).$(LIBXCB_SUFFIX)
LIBXCB_SOURCE		:= $(SRCDIR)/$(LIBXCB).$(LIBXCB_SUFFIX)
LIBXCB_DIR		:= $(BUILDDIR)/$(LIBXCB)
LIBXCB_LICENSE		:= MIT

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBXCB_CONF_ENV := $(CROSS_ENV) ac_cv_prog_BUILD_DOCS=no

ifndef PTXCONF_XORG_SERVER_EXT_XDMCP
LIBXCB_CONF_ENV += ac_cv_lib_Xdmcp_XdmcpWrap=no
endif

#
# autoconf
#
LIBXCB_CONF_TOOL	:= autoconf
LIBXCB_CONF_OPT		:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-static \
	--enable-composite \
	--enable-damage \
	--enable-dpms \
	--enable-dri2 \
	--enable-dri3 \
	--enable-glx \
	--enable-present \
	--enable-randr \
	--enable-record \
	--enable-render \
	--enable-resource \
	--enable-screensaver \
	--enable-shape \
	--enable-shm \
	--enable-sync \
	--enable-xevie \
	--enable-xfixes \
	--enable-xfree86-dri \
	--enable-xinerama \
	--enable-xinput \
	--enable-xkb \
	--enable-xprint \
	--disable-selinux \
	--enable-xtest \
	--enable-xv \
	--enable-xvmc \
	--without-doxygen

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libxcb.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libxcb)
	@$(call install_fixup, libxcb,PRIORITY,optional)
	@$(call install_fixup, libxcb,SECTION,base)
	@$(call install_fixup, libxcb,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libxcb,DESCRIPTION,missing)

	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-composite)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-damage)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-dpms)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-dri2)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-dri3)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-glx)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-present)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-randr)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-record)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-render)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-res)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-screensaver)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-shape)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-shm)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-sync)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-xevie)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-xf86dri)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-xfixes)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-xinerama)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-xinput)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-xkb)
#	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-xlib)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-xprint)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-xtest)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-xv)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb-xvmc)
	@$(call install_lib, libxcb, 0, 0, 0644, libxcb)

	@$(call install_finish, libxcb)

	@$(call touch)

# vim: syntax=make
