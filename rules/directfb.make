# -*-makefile-*-
#
# Copyright (C) 2006, 2009, 2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#               2010 by Erwin Rol <erwin@erwinrol.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_DIRECTFB) += directfb

#
# Paths and names
#

DIRECTFB_VERSION	:= 1.6.3
DIRECTFB_MD5	:= 641e8e999c017770da647f9b5b890906
DIRECTFB		:= DirectFB-$(DIRECTFB_VERSION)
DIRECTFB_SUFFIX		:= tar.gz
DIRECTFB_SOURCE		:= $(SRCDIR)/$(DIRECTFB).$(DIRECTFB_SUFFIX)
DIRECTFB_DIR		:= $(BUILDDIR)/$(DIRECTFB)

DIRECTFB_URL		:= \
	http://www.directfb.org/downloads/Core/DirectFB-1.5/$(DIRECTFB).$(DIRECTFB_SUFFIX) \
	http://www.directfb.org/downloads/Old/$(DIRECTFB).$(DIRECTFB_SUFFIX)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(DIRECTFB_SOURCE):
	@$(call targetinfo)
	@$(call get, DIRECTFB)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

DIRECTFB_PATH	:= PATH=$(CROSS_PATH)
DIRECTFB_ENV 	:= \
	$(CROSS_ENV) \
	DIRECTFB_CSOURCE=$(PTXCONF_SYSROOT_HOST)/bin/directfb-csource

DIRECTFB_INPUT-$(PTXCONF_DIRECTFB_INPUT_KEYBOARD)	+= keyboard
DIRECTFB_INPUT-$(PTXCONF_DIRECTFB_INPUT_LINUXINPUT)	+= linuxinput
DIRECTFB_INPUT-$(PTXCONF_DIRECTFB_INPUT_PS2MOUSE)	+= ps2mouse
DIRECTFB_INPUT-$(PTXCONF_DIRECTFB_INPUT_TSLIB)		+= tslib

DIRECTFB_GFX-$(PTXCONF_DIRECTFB_GFX_ATI128)		+= ati128
DIRECTFB_GFX-$(PTXCONF_DIRECTFB_GFX_CLE266)		+= cle266
DIRECTFB_GFX-$(PTXCONF_DIRECTFB_GFX_CYBER5K)		+= cyber5k
DIRECTFB_GFX-$(PTXCONF_DIRECTFB_GFX_DAVINCI)		+= davinci
DIRECTFB_GFX-$(PTXCONF_DIRECTFB_GFX_EP9X)		+= ep9x
DIRECTFB_GFX-$(PTXCONF_DIRECTFB_GFX_GL)			+= gl
DIRECTFB_GFX-$(PTXCONF_DIRECTFB_GFX_I810)		+= i810
DIRECTFB_GFX-$(PTXCONF_DIRECTFB_GFX_I830)		+= i830
DIRECTFB_GFX-$(PTXCONF_DIRECTFB_GFX_MACH64)		+= mach64
DIRECTFB_GFX-$(PTXCONF_DIRECTFB_GFX_MATROX)		+= matrox
DIRECTFB_GFX-$(PTXCONF_DIRECTFB_GFX_NEOMAGIC)		+= neomagic
DIRECTFB_GFX-$(PTXCONF_DIRECTFB_GFX_NSC)		+= nsc
DIRECTFB_GFX-$(PTXCONF_DIRECTFB_GFX_NVIDIA)		+= nvidia
DIRECTFB_GFX-$(PTXCONF_DIRECTFB_GFX_OMAP)		+= omap
DIRECTFB_GFX-$(PTXCONF_DIRECTFB_GFX_RADEON)		+= radeon
DIRECTFB_GFX-$(PTXCONF_DIRECTFB_GFX_SAVAGE)		+= savage
DIRECTFB_GFX-$(PTXCONF_DIRECTFB_GFX_SIS315)		+= sis315
DIRECTFB_GFX-$(PTXCONF_DIRECTFB_GFX_TDFX)		+= tdfx
DIRECTFB_GFX-$(PTXCONF_DIRECTFB_GFX_UNICHROME)		+= unichrome
DIRECTFB_GFX-$(PTXCONF_DIRECTFB_GFX_VMWARE)		+= vmware

#
# autoconf
#
DIRECTFB_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--without-tests \
	--with-tools \
	--disable-osx \
	--disable-network \
	--disable-multi \
	--disable-voodoo \
	--enable-sdl \
	--disable-vnc \
	--disable-zlib \
	--with-gfxdrivers=$(subst $(space),$(comma),$(DIRECTFB_GFX-y)) \
	--with-inputdrivers=$(subst $(space),$(comma),$(DIRECTFB_INPUT-y))
#	--disable-sdl \

ifdef PTXCONF_DIRECTFB_X11
DIRECTFB_AUTOCONF += --enable-x11
else
DIRECTFB_AUTOCONF += --disable-x11
endif

ifdef PTXCONF_DIRECTFB_FBDEV
DIRECTFB_AUTOCONF += --enable-fbdev
else
DIRECTFB_AUTOCONF += --disable-fbdev
endif

ifdef PTXCONF_DIRECTFB_V4L
DIRECTFB_AUTOCONF += --enable-video4linux
else
DIRECTFB_AUTOCONF += --disable-video4linux
endif

ifdef PTXCONF_DIRECTFB_V4L2
DIRECTFB_AUTOCONF += --enable-video4linux2
else
DIRECTFB_AUTOCONF += --disable-video4linux2
endif

ifdef PTXCONF_DIRECTFB_DEBUG
DIRECTFB_AUTOCONF += --enable-debug
DIRECTFB_MODULE_DIRECTORY := /usr/lib/directfb-1.5-0
else
DIRECTFB_AUTOCONF += --disable-debug-support
DIRECTFB_MODULE_DIRECTORY := /usr/lib/directfb-1.5-0-pure
endif

ifdef PTXCONF_DIRECTFB_TRACE
DIRECTFB_AUTOCONF += --enable-trace
else
DIRECTFB_AUTOCONF += --disable-trace
endif

ifdef PTXCONF_DIRECTFB_WM_UNIQUE
DIRECTFB_AUTOCONF += --enable-unique
else
DIRECTFB_AUTOCONF += --disable-unique
endif

ifdef PTXCONF_DIRECTFB_IMAGE_GIF
DIRECTFB_AUTOCONF += --enable-gif
else
DIRECTFB_AUTOCONF += --disable-gif
endif

ifdef PTXCONF_DIRECTFB_IMAGE_PNG
DIRECTFB_AUTOCONF += --enable-png
else
DIRECTFB_AUTOCONF += --disable-png
endif

ifdef PTXCONF_DIRECTFB_IMAGE_JPEG
DIRECTFB_AUTOCONF += --enable-jpeg
else
DIRECTFB_AUTOCONF += --disable-jpeg
endif

ifdef PTXCONF_DIRECTFB_FONT_FREETYPE
DIRECTFB_AUTOCONF += --enable-freetype
else
DIRECTFB_AUTOCONF += --disable-freetype
endif

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

# /usr/bin/dfblayer
# /usr/bin/dfbfx
# /usr/bin/dfbg
# /usr/bin/dfbinspector
# /usr/bin/dfbinput
# /usr/bin/mkdgiff
# /usr/bin/dfbscreen
# /usr/bin/dfbpenmount
# /usr/bin/directfb-config
# /usr/bin/mkdfiff
# /usr/bin/directfb-csource
# /usr/bin/dfbmaster
# /usr/bin/dfbdump
# /usr/share/directfb-1.3.5/cursor.dat
# /usr/lib/directfb-1.3-0-pure/interfaces/IDirectFBFont/libidirectfbfont_dgiff.so
# /usr/lib/directfb-1.3-0-pure/interfaces/IDirectFBFont/libidirectfbfont_default.so
# /usr/lib/directfb-1.3-0-pure/interfaces/IDirectFBImageProvider/libidirectfbimageprovider_dfiff.so
# /usr/lib/directfb-1.3-0-pure/interfaces/IDirectFBVideoProvider/libidirectfbvideoprovider_gif.so
# /usr/lib/directfb-1.3-0-pure/systems/libdirectfb_devmem.so

$(STATEDIR)/directfb.targetinstall:
	@$(call targetinfo)

	@$(call install_init, directfb)
	@$(call install_fixup, directfb,PRIORITY,optional)
	@$(call install_fixup, directfb,SECTION,base)
	@$(call install_fixup, directfb,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, directfb,DESCRIPTION,missing)

ifdef PTXCONF_DIRECTFB_CONFIG_DIRECTFBRC
	@$(call install_alternative, directfb, 0, 0, 0644, /etc/directfbrc)
endif

ifdef PTXCONF_PRELINK
	@$(call install_alternative, directfb, 0, 0, 0644, \
		/etc/prelink.conf.d/directfb)
endif

	@$(call install_copy, directfb, 0, 0, 0755, -, /usr/bin/dfbinfo)

	@$(call install_lib, directfb, 0, 0, 0644, libdirectfb-1.6)
	@$(call install_lib, directfb, 0, 0, 0644, libfusion-1.6)
	@$(call install_lib, directfb, 0, 0, 0644, libdirect-1.6)

ifdef PTXCONF_DIRECTFB_WM_UNIQUE
	@$(call install_lib, directfb, 0, 0, 0644, libuniquewm-1.6)
endif

	@cd $(DIRECTFB_PKGDIR) && for plugin in `find ./$(DIRECTFB_MODULE_DIRECTORY) -name "*.so"`; do \
		$(call install_copy, directfb, 0, 0, 0644, -, /$$plugin); \
	done

	@$(call install_finish, directfb)

	@$(call touch)

# vim: syntax=make

