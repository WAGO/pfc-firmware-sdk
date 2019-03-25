# -*-makefile-*-
#
# Copyright (C) 2007,2012 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PIXMAN) += pixman

#
# Paths and names
#
PIXMAN_VERSION	:= 0.34.0
PIXMAN_MD5	:= 002a4fcb644ddfcb4b0e4191576a0d59
PIXMAN		:= pixman-$(PIXMAN_VERSION)
PIXMAN_SUFFIX	:= tar.bz2
PIXMAN_URL	:= $(call ptx/mirror, XORG, individual/lib/$(PIXMAN).$(PIXMAN_SUFFIX))
PIXMAN_SOURCE	:= $(SRCDIR)/$(PIXMAN).$(PIXMAN_SUFFIX)
PIXMAN_DIR	:= $(BUILDDIR)/$(PIXMAN)
PIXMAN_LICENSE	:= MIT

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
PIXMAN_CONF_TOOL	:= autoconf
PIXMAN_CONF_OPT		:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-static \
	--disable-openmp \
	--disable-loongson-mmi \
	--$(call ptx/endis, PTXCONF_ARCH_X86)-mmx \
	--$(call ptx/endis, PTXCONF_ARCH_X86)-sse2 \
	--$(call ptx/endis, PTXCONF_ARCH_X86)-ssse3 \
	--disable-vmx \
	--$(call ptx/endis, PTXCONF_ARCH_ARM_V6)-arm-simd \
	--$(call ptx/endis, PTXCONF_ARCH_ARM_NEON)-arm-neon \
	--disable-arm-iwmmxt \
	--disable-arm-iwmmxt2 \
	--disable-mips-dspr2 \
	--enable-gcc-inline-asm \
	--disable-static-testprogs \
	--disable-timers \
	--disable-gtk \
	--disable-libpng

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pixman.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pixman)
	@$(call install_fixup, pixman,PRIORITY,optional)
	@$(call install_fixup, pixman,SECTION,base)
	@$(call install_fixup, pixman,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, pixman,DESCRIPTION,missing)

	@$(call install_lib, pixman, 0, 0, 0644, libpixman-1)

	@$(call install_finish, pixman)

	@$(call touch)

# vim: syntax=make
