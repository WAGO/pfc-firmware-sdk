# -*-makefile-*-
#
# Copyright (C) 2013 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_GST_LIBAV1) += gst-libav1

#
# Paths and names
#
GST_LIBAV1_VERSION	:= 1.12.5
GST_LIBAV1_MD5		:= 6b608cd31ef52b9c06ec0ecb13da96a4
GST_LIBAV1		:= gst-libav-$(GST_LIBAV1_VERSION)
GST_LIBAV1_SUFFIX	:= tar.xz
GST_LIBAV1_URL		:= http://gstreamer.freedesktop.org/src/gst-libav/$(GST_LIBAV1).$(GST_LIBAV1_SUFFIX)
GST_LIBAV1_SOURCE	:= $(SRCDIR)/$(GST_LIBAV1).$(GST_LIBAV1_SUFFIX)
GST_LIBAV1_DIR		:= $(BUILDDIR)/$(GST_LIBAV1)
GST_LIBAV1_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

GST_LIBAV1_ENV		:= \
	$(CROSS_ENV) \
	AS=$(CROSS_CC)

#
# autoconf
#
GST_LIBAV1_CONF_TOOL	:= autoconf
GST_LIBAV1_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--enable-orc \
	--disable-fatal-warnings \
	--enable-extra-check \
	--disable-valgrind \
	--disable-gcov \
	--disable-gtk-doc \
	--disable-gtk-doc-html \
	--disable-gtk-doc-pdf \
	--disable-gobject-cast-checks \
	--disable-glib-asserts \
	--disable-static-plugins \
	--disable-gpl \
	--with-package-origin="PTXdist" \
	--without-system-libav

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/gst-libav1.targetinstall:
	@$(call targetinfo)

	@$(call install_init, gst-libav1)
	@$(call install_fixup, gst-libav1,PRIORITY,optional)
	@$(call install_fixup, gst-libav1,SECTION,base)
	@$(call install_fixup, gst-libav1,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, gst-libav1,DESCRIPTION,missing)

	@$(call install_lib, gst-libav1, 0, 0, 0644, gstreamer-1.0/libgstlibav)

	@$(call install_finish, gst-libav1)

	@$(call touch)

# vim: syntax=make
