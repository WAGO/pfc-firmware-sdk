# -*-makefile-*-
#
# Copyright (C) 2008 by Juergen Beisert
#               2009-2011 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_QT4) += qt4

#
# Paths and names
#
QT4_VERSION	:= 4.8.6
QT4_MD5		:= 2edbe4d6c2eff33ef91732602f3518eb 
QT4		:= qt-everywhere-opensource-src-$(QT4_VERSION)
QT4_SUFFIX	:= tar.gz
QT4_URL		:= http://releases.qt-project.org/qt4/source/$(QT4).$(QT4_SUFFIX)
QT4_SOURCE	:= $(SRCDIR)/$(QT4).$(QT4_SUFFIX)
QT4_DIR		:= $(BUILDDIR)/$(QT4)
QT4_BUILD_OOT	:= YES
QT4_LICENSE	:= GPL3, LGPLv2.1


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/qt4.extract:
	@$(call targetinfo)
	@$(call clean, $(QT4_DIR))
	@$(call extract, QT4)
	@$(call patchin, QT4)
	@for file in $(QT4_DIR)/mkspecs/{qws/,}linux-ptx-g++/*.in; do \
		sed -e "s,@COMPILER_PREFIX@,$(PTXDIST_WORKSPACE)/selected_toolchain/$(COMPILER_PREFIX),g" \
		    -e "s,@CPPFLAGS@,$(CROSS_CPPFLAGS),g" \
		    -e "s,@CFLAGS@,$(CROSS_CFLAGS),g" \
		    -e "s,@CXXFLAGS@,$(CROSS_CXXFLAGS),g" \
		    -e "s,@INCDIR@,$(SYSROOT)/include $(SYSROOT)/usr/include,g" \
		    -e "s,@LIBS_X11@,-lXext -lX11 -lm -lz -lXau -lxcb -lXdmcp -lexpat -lgthread-2.0 -lXrender -lglib-2.0 -lpng12 -lfreetype -lXi -lXfixes -lXinerama -lfontconfig -lXrandr  -lXcursor,g" \
		    -e "s,@LIBDIR@,$(SYSROOT)/lib $(SYSROOT)/usr/lib,g" \
		    -e "s,@XINCDIR@,$(SYSROOT)/usr/include $(SYSROOT)/usr/include/xorg $(SYSROOT)/usr/include/X11,g" \
		    -e "s,@XLIBDIR@,$(SYSROOT)/usr/lib,g" \
		    -e "s#@LDFLAGS@#$(strip $(CROSS_LDFLAGS))#g" \
		    -e "s#@QMAKE_LIBS_OPENGL_ES1@#$(strip $(QT4_QMAKE_LIBS_OPENGL_ES1))#g" \
		    -e "s#@QMAKE_LIBS_OPENGL_ES1CL@#$(strip $(QT4_QMAKE_LIBS_OPENGL_ES1CL))#g" \
		    -e "s#@QMAKE_LIBS_OPENGL_ES2@#$(strip $(QT4_QMAKE_LIBS_OPENGL_ES2))#g" \
		    $$file > $${file%%.in}; \
	done
ifdef PTXCONF_QT4_BUILD_OPENGL
	@cp -f $(PVRDRIVER_DIR)/QT_conf/qmake.conf_sed  $(QT4_DIR)/mkspecs/qws/linux-ptx-g++/qmake.conf
endif
	rm -f $(PTXDIST_SYSROOT_TARGET)/usr/lib/libQt* && \
	rm -f $(ROOTDIR)/usr/lib/libQt*
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

# don't use CROSS_ENV. Qt uses mkspecs for instead.
# 'strip' is our special wrapper to avoid stripping for root-debug
QT4_ENV		:= $(CROSS_ENV_PKG_CONFIG)  STRIP=strip MAKEFLAGS="$(PARALLELMFLAGS)"
QT4_INSTALL_OPT	:= INSTALL_ROOT=$(QT4_PKGDIR)

ifdef PTXCONF_ARCH_ARM_V6
QT4_ARCH = armv6
else
QT4_ARCH = $(call remove_quotes, $(PTXCONF_ARCH_STRING))
endif
ifeq ($(QT4_ARCH),ppc)
QT4_ARCH = powerpc
endif

define ptx/qt-plugin
$(call ptx/ifdef, PTXCONF_QT4_$(strip $(1))_NONE,no,)$(call ptx/ifdef, PTXCONF_QT4_$(strip $(1))_PLUGIN,plugin,)$(call ptx/ifdef, PTXCONF_QT4_$(strip $(1))_BUILTIN,qt,)
endef
define ptx/qt
$(call ptx/ifdef, PTXCONF_QT4_$(strip $(1)),qt,no)
endef
define ptx/qt-no
$(call ptx/ifdef, PTXCONF_QT4_$(strip $(1)),y,no)
endef
define ptx/qt-system
$(call ptx/ifdef, PTXCONF_QT4_$(strip $(1))_NONE,no,)$(call ptx/ifdef, PTXCONF_QT4_$(strip $(1))_SYSTEM,system,)$(call ptx/ifdef, PTXCONF_QT4_$(strip $(1))_INTERNAL,qt,)
endef

#
# autoconf
#
# Important: Use "-no-fast" here. Otherwise qmake will be called during
# the compile stage when the environment is not properly set!
QT4_AUTOCONF := \
	-opensource \
	-confirm-license \
	-release \
	-no-rpath \
	-no-fast \
	-no-sql-ibase \
	-no-sql-mysql \
	-no-sql-odbc \
	-no-sql-psql \
	-no-sql-sqlite2 \
	-no-qt3support \
	-no-mmx \
	-no-3dnow \
	-no-sse \
	-no-sse2 \
	-no-sse3 \
	-no-ssse3 \
	-no-sse4.1 \
	-no-sse4.2 \
	-no-avx \
	-no-optimized-qmake \
	-no-nis \
	-no-cups \
	-no-pch \
	-reduce-relocations \
	-force-pkg-config \
	-prefix /usr \
	-no-armfpa \
	-make libs \
	-nomake docs \
	-no-iconv \
	-depths 16,24,32 \
	-v

#  -D QT_NO_QWS_CURSOR \
QT4_AUTOCONF-$(call ptx/ifdef, PTXCONF_QT4_ACCESSIBILITY,y,no)	+= accessibility
#QT4_AUTOCONF-$(call ptx/ifdef, PTXCONF_ICONV,y,no)		+= iconv
QT4_AUTOCONF-$(call ptx/ifdef, PTXCONF_GLOBAL_LARGE_FILE,y,no)	+= largefile
QT4_AUTOCONF-$(call ptx/ifdef, PTXCONF_ARCH_ARM_NEON,,no)	+= neon

QT4_AUTOCONF += -$(call ptx/ifdef, PTXCONF_ENDIAN_LITTLE, little, big)-endian


ifdef PTXCONF_QT4_PLATFORM_EMBEDDED
QT4_AUTOCONF += \
	-embedded $(QT4_ARCH) \
	-qt-decoration-styled \
	-depths all \
	-xplatform qws/linux-ptx-g++ \
	-D QT_QWS_CLIENTBLIT

# graphics drivers
QT4_AUTOCONF-$(call ptx/qt-plugin, GFX_AHI)		+= gfx-ahi
QT4_AUTOCONF-$(call ptx/qt-plugin, GFX_DIRECTFB)	+= gfx-directfb
QT4_AUTOCONF-$(call ptx/qt-plugin, GFX_EGLNULLWS)	+= gfx-eglnullws
QT4_AUTOCONF-$(call ptx/qt-plugin, GFX_LINUXFB)		+= gfx-linuxfb
QT4_AUTOCONF-$(call ptx/qt-plugin, GFX_POWERVR)		+= gfx-powervr
QT4_AUTOCONF-$(call ptx/qt-plugin, GFX_SVGA)		+= gfx-svgalib
QT4_AUTOCONF-$(call ptx/qt-plugin, GFX_TRANSFORMED)	+= gfx-transformed
QT4_AUTOCONF-$(call ptx/qt-plugin, GFX_QVFB)		+= gfx-qvfb
QT4_AUTOCONF-$(call ptx/qt-plugin, GFX_VNC)		+= gfx-vnc
QT4_AUTOCONF-$(call ptx/qt-plugin, GFX_MULTISCREEN)	+= gfx-multiscreen
QT4_AUTOCONF-$(call ptx/qt-plugin, GFX_HYBRID)		+= gfx-hybrid

# keyboard drivers
QT4_AUTOCONF-$(call ptx/qt, KBD_TTY)			+= kbd-tty
QT4_AUTOCONF-$(call ptx/qt, KBD_INPUT)			+= kbd-linuxinput
QT4_AUTOCONF-$(call ptx/qt, KBD_QVFB)			+= kbd-qvfb

# mouse drivers
QT4_AUTOCONF-$(call ptx/qt, MOUSE_PC)			+= mouse-pc
QT4_AUTOCONF-$(call ptx/qt, MOUSE_INPUT)		+= mouse-linuxinput
QT4_AUTOCONF-$(call ptx/qt, MOUSE_LINUXTP)		+= mouse-linuxtp
QT4_AUTOCONF-$(call ptx/qt, MOUSE_TSLIB)		+= mouse-tslib
QT4_AUTOCONF-$(call ptx/qt, MOUSE_QVFB)			+= mouse-qvfb

endif # PTXCONF_QT4_PLATFORM_EMBEDDED

ifdef PTXCONF_QT4_PLATFORM_X11
QT4_AUTOCONF += \
	-x11 \
	-arch $(QT4_ARCH) \
	-xplatform linux-ptx-g++ \
	-no-gtkstyle \
	-no-nas-sound \
	-no-openvg

QT4_AUTOCONF-$(call ptx/qt-no, X11_SM)			+= sm
QT4_AUTOCONF-$(call ptx/qt-no, X11_XSHAPE)		+= xshape
QT4_AUTOCONF-$(call ptx/qt-no, X11_XSYNC)		+= xsync
QT4_AUTOCONF-$(call ptx/qt-no, X11_XINERAMA)		+= xinerama
QT4_AUTOCONF-$(call ptx/qt-no, X11_XCURSOR)		+= xcursor
QT4_AUTOCONF-$(call ptx/qt-no, X11_XFIXES)		+= xfixes
QT4_AUTOCONF-$(call ptx/qt-no, X11_XRANDR)		+= xrandr
QT4_AUTOCONF-$(call ptx/qt-no, X11_XRENDER)		+= xrender
QT4_AUTOCONF-$(call ptx/qt-no, X11_MITSHM)		+= mitshm
QT4_AUTOCONF-$(call ptx/qt-no, X11_FONTCONFIG)		+= fontconfig
QT4_AUTOCONF-$(call ptx/qt-no, X11_XINPUT)		+= xinput
QT4_AUTOCONF-$(call ptx/qt-no, X11_XKB)			+= xkb

endif # PTXCONF_QT4_PLATFORM_X11

QT4_AUTOCONF-$(call ptx/qt-system, PNG)			+= libpng
QT4_AUTOCONF-$(call ptx/qt-system, MNG)			+= libmng
QT4_AUTOCONF-$(call ptx/qt-system, TIFF)		+= libtiff
QT4_AUTOCONF-$(call ptx/qt-system, GIF)			+= gif
QT4_AUTOCONF-$(call ptx/qt-system, JPG)			+= libjpeg
QT4_AUTOCONF-$(call ptx/qt-system, ZLIB)		+= zlib
QT4_AUTOCONF-$(call ptx/qt-system, FREETYPE)		+= freetype
ifdef PTXCONF_QT4_FREETYPE_SYSTEM
QT4_AUTOCONF += -I$(SYSROOT)/usr/include/freetype2
endif

QT4_AUTOCONF-$(call ptx/qt-no, STL)			+= stl
QT4_AUTOCONF-$(call ptx/qt-no, GLIB)			+= glib
QT4_AUTOCONF-$(call ptx/qt-no, OPENSSL)			+= openssl
QT4_AUTOCONF-$(call ptx/qt-no, OPENGL_EGL)		+= egl
QT4_AUTOCONF-$(call ptx/qt-no, BUILD_QTXMLPATTERNS)	+= xmlpatterns exceptions
QT4_AUTOCONF-$(call ptx/qt-no, BUILD_MULTIMEDIA)	+= multimedia audio-backend
QT4_AUTOCONF-$(call ptx/qt-no, BUILD_PHONON)		+= phonon phonon-backend
# QtWebKit need QtScript headers but not the lib.
QT4_AUTOCONF-$(call ptx/qt-no, BUILD_WEBKIT)		+= webkit script
QT4_AUTOCONF-$(call ptx/qt-no, BUILD_SCRIPTTOOLS)	+= scripttools
QT4_AUTOCONF-$(call ptx/qt-no, BUILD_SVG)		+= svg
QT4_AUTOCONF-$(call ptx/qt-no, BUILD_SCRIPT)		+= script
QT4_AUTOCONF-$(call ptx/qt-no, BUILD_DECLARATIVE)	+= declarative

QT4_AUTOCONF-$(call ptx/qt-plugin, SQLITE)		+= sql-sqlite
ifdef QT4_SQLITE_SYSTEM
QT4_AUTOCONF += -system-sqlite
endif

QT4_AUTOCONF-no := $(filter-out $(QT4_AUTOCONF-y),$(QT4_AUTOCONF-no))

QT4_AUTOCONF += $(foreach opt,$(QT4_AUTOCONF-y),-$(opt))
QT4_AUTOCONF += $(foreach opt,$(QT4_AUTOCONF-qt),-qt-$(opt))
QT4_AUTOCONF += $(foreach opt,$(QT4_AUTOCONF-plugin),-plugin-$(opt))
QT4_AUTOCONF += $(foreach opt,$(QT4_AUTOCONF-system),-system-$(opt))
QT4_AUTOCONF += $(foreach opt,$(QT4_AUTOCONF-no),-no-$(opt))

ifdef PTXCONF_QT4_DBUS_LOAD
QT4_AUTOCONF += -dbus
endif
ifdef PTXCONF_QT4_DBUS_LINK
QT4_AUTOCONF += -dbus-linked
endif
ifndef PTXCONF_QT4_DBUS
QT4_AUTOCONF += -no-qdbus
endif

ifdef PTXCONF_QT4_OPENGL_DESKTOP
QT4_AUTOCONF += -opengl
endif
ifdef PTXCONF_QT4_OPENGL_ES1
QT4_AUTOCONF += -opengl es1
endif
ifdef PTXCONF_QT4_OPENGL_ES2
QT4_AUTOCONF += -opengl es2
endif
ifdef PTXCONF_QT4_OPENGL_NONE
QT4_AUTOCONF += -no-opengl
endif

ifdef PTXCONF_QT4_SHARED
QT4_AUTOCONF += -shared
QT4_PLUGIN_EXT := so
else
QT4_AUTOCONF += -static
QT4_PLUGIN_EXT := a
endif

ifdef PTXCONF_QT4_BUILD_TOOLS
QT4_AUTOCONF += -make tools
else
QT4_AUTOCONF += -nomake tools
endif

ifdef PTXCONF_QT4_PREPARE_EXAMPLES
QT4_AUTOCONF += -make examples -make demos
else
QT4_AUTOCONF += -nomake examples -nomake demos
endif

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

QT4_BUILD-y := moc rcc corelib plugins
QT4_BUILD-$(PTXCONF_QT4_BUILD_XML)		+= xml
QT4_BUILD-$(PTXCONF_QT4_BUILD_GUI)		+= gui
QT4_BUILD-$(PTXCONF_QT4_BUILD_SQL)		+= sql
QT4_BUILD-$(PTXCONF_QT4_BUILD_NETWORK)		+= network
QT4_BUILD-$(PTXCONF_QT4_BUILD_SVG)		+= svg
QT4_BUILD-$(PTXCONF_QT4_BUILD_SCRIPT)		+= script
QT4_BUILD-$(PTXCONF_QT4_BUILD_QTESTLIB)		+= testlib
QT4_BUILD-$(PTXCONF_QT4_BUILD_DECLARATIVE)	+= declarative imports
QT4_BUILD-$(PTXCONF_QT4_BUILD_QTXMLPATTERNS)	+= xmlpatterns
QT4_BUILD-$(PTXCONF_QT4_BUILD_MULTIMEDIA)	+= multimedia
QT4_BUILD-$(PTXCONF_QT4_BUILD_PHONON)		+= phonon
QT4_BUILD-$(PTXCONF_QT4_BUILD_WEBKIT)		+= webkit script
QT4_BUILD-$(PTXCONF_QT4_BUILD_SCRIPTTOOLS)	+= scripttools
QT4_BUILD-$(PTXCONF_QT4_OPENGL_ES2) += opengl
# qmlviewer does not need xml but we cannot built sub-tools without it
QT4_BUILD-$(PTXCONF_QT4_BUILD_TOOLS)		+= xml

QT4_BUILD_TARGETS := $(foreach lib, $(QT4_BUILD-y),sub-$(lib))

ifdef PTXCONF_QT4_BUILD_TOOLS
QT4_BUILD_TOOLS_TARGETS = sub-tools
QT4_INSTALL_OPT += sub-tools-install_subtargets
endif

$(STATEDIR)/qt4.compile:
	@$(call targetinfo)
	@$(call compile, QT4, tools)
	@$(call compile, QT4, $(QT4_BUILD_TARGETS))

#	# These targets don't have the correct dependencies.
#	# We have to build them later
ifneq ($(strip $(QT4_BUILD_TOOLS_TARGETS)),)
	@$(call compile, QT4, $(QT4_BUILD_TOOLS_TARGETS))
endif
ifdef PTXCONF_QT4_PREPARE_EXAMPLES
#	# FIXME: use "-k" and " || true" for now.
#	# some examples will may fail to build because of missing libraries
#	# these cannot be installed but all are built
	@$(call compile, QT4, -k sub-examples) || true
	@$(call compile, QT4, -k sub-demos) || true
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

QT4_INSTALL_OPT += install_mkspecs install_qmake
QT4_INSTALL_OPT += $(foreach lib, $(QT4_BUILD-y),sub-$(lib)-install_subtargets)

$(STATEDIR)/qt4.install:
	@$(call targetinfo)
	@$(call install, QT4)
	@find "$(QT4_PKGDIR)" -name "*.la" -print0 | xargs -r -0 -- \
		sed -i -e "/^dependency_libs/s:\( \|-L\|-R\)$(QT4_PKGDIR)\(/lib\|/usr/lib\):\1$(SYSROOT)\2:g"
	@find "$(QT4_PKGDIR)" -name "*.prl" -print0 | xargs -r -0 -- \
		sed -i -e "/^QMAKE_PRL_LIBS/s:\( \|-L\|-R\)$(QT4_PKGDIR)\(/lib\|/usr/lib\):\1$(SYSROOT)\2:g"
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

QT4_LIBS-y := QtCore
QT4_LIBS-$(PTXCONF_QT4_BUILD_XML)		+= QtXml
QT4_LIBS-$(PTXCONF_QT4_BUILD_GUI)		+= QtGui
QT4_LIBS-$(PTXCONF_QT4_BUILD_SQL)		+= QtSql
QT4_LIBS-$(PTXCONF_QT4_BUILD_NETWORK)		+= QtNetwork
QT4_LIBS-$(PTXCONF_QT4_BUILD_SVG)		+= QtSvg
QT4_LIBS-$(PTXCONF_QT4_BUILD_SCRIPT)		+= QtScript
QT4_LIBS-$(PTXCONF_QT4_BUILD_QTESTLIB)		+= QtTest
QT4_LIBS-$(PTXCONF_QT4_BUILD_DECLARATIVE)	+= QtDeclarative
QT4_LIBS-$(PTXCONF_QT4_BUILD_ASSISTANTLIB)	+= QtCLucene QtHelp
QT4_LIBS-$(PTXCONF_QT4_DBUS)			+= QtDBus
QT4_LIBS-$(PTXCONF_QT4_BUILD_DESIGNERLIBS)	+= QtDesigner
QT4_LIBS-$(PTXCONF_QT4_BUILD_WEBKIT)		+= QtWebKit
QT4_LIBS-$(PTXCONF_QT4_BUILD_SCRIPTTOOLS)	+= QtScriptTools
QT4_LIBS-$(PTXCONF_QT4_BUILD_QTXMLPATTERNS)	+= QtXmlPatterns
QT4_LIBS-$(PTXCONF_QT4_BUILD_MULTIMEDIA)	+= QtMultimedia
QT4_LIBS-$(PTXCONF_QT4_BUILD_OPENGL)		+= QtOpenGL
QT4_LIBS-$(PTXCONF_QT4_BUILD_PHONON)		+= phonon

# builtin for static Qt
ifdef PTXCONF_QT4_SHARED
QT4_IMAGEFORMATS-$(PTXCONF_QT4_GIF_NONE)	+= gif
QT4_IMAGEFORMATS-$(PTXCONF_QT4_JPG_NONE)	+= jpeg
QT4_IMAGEFORMATS-$(PTXCONF_QT4_MNG_NONE)	+= mng
QT4_IMAGEFORMATS-$(PTXCONF_QT4_TIFF_NONE)	+= tiff
endif
QT4_IMAGEFORMATS-$(PTXCONF_QT4_ICO_NONE)	+= ico
ifdef PTXCONF_QT4_BUILD_SVG
QT4_IMAGEFORMATS-				+= svg
endif

QT4_GFXDRIVERS-$(call ptx/qt-plugin, GFX_DIRECTFB)	+= directfbscreen
QT4_GFXDRIVERS-$(call ptx/qt-plugin, GFX_EGLNULLWS)	+= eglnullws
QT4_GFXDRIVERS-$(call ptx/qt-plugin, GFX_LINUXFB)	+= screenlinuxfb
QT4_GFXDRIVERS-$(call ptx/qt-plugin, GFX_POWERVR)	+= gfxpvregl
QT4_GFXDRIVERS-$(call ptx/qt-plugin, GFX_TRANSFORMED)	+= gfxtransformed
QT4_GFXDRIVERS-$(call ptx/qt-plugin, GFX_QVFB)		+= screenvfb
QT4_GFXDRIVERS-$(call ptx/qt-plugin, GFX_VNC)		+= gfxvnc

ifdef PTXCONF_QT4_FONT_DEJAVU
QT4_FONTS += \
	DejaVuSans-Bold.ttf \
	DejaVuSans-BoldOblique.ttf \
	DejaVuSans-Oblique.ttf \
	DejaVuSans.ttf \
	DejaVuSansMono-Bold.ttf \
	DejaVuSansMono-BoldOblique.ttf \
	DejaVuSansMono-Oblique.ttf \
	DejaVuSansMono.ttf \
	DejaVuSerif-Bold.ttf \
	DejaVuSerif-BoldOblique.ttf \
	DejaVuSerif-Oblique.ttf \
	DejaVuSerif.ttf
endif
ifdef PTXCONF_QT4_FONT_UT
QT4_FONTS += \
	'UTBI____.pfa' \
	'UTB_____.pfa' \
	'UTI_____.pfa' \
	'UTRG____.pfa'
endif

ifdef PTXCONF_QT4_FONT_VERA
QT4_FONTS += \
	Vera.ttf \
	VeraBI.ttf \
	VeraBd.ttf \
	VeraIt.ttf \
	VeraMoBI.ttf \
	VeraMoBd.ttf \
	VeraMoIt.ttf \
	VeraMono.ttf \
	VeraSe.ttf \
	VeraSeBd.ttf
endif

ifdef PTXCONF_QT4_FONT_C0
QT4_FONTS += \
	c0419bt_.pfb \
	c0582bt_.pfb \
	c0583bt_.pfb \
	c0611bt_.pfb \
	c0632bt_.pfb \
	c0633bt_.pfb \
	c0648bt_.pfb \
	c0649bt_.pfb
endif

ifdef PTXCONF_QT4_FONT_COUR
QT4_FONTS += \
	cour.pfa \
	courb.pfa \
	courbi.pfa \
	couri.pfa
endif

ifdef PTXCONF_QT4_FONT_CURSOR
QT4_FONTS += \
	cursor.pfa
endif

ifdef PTXCONF_QT4_FONT_FIXED
QT4_FONTS += \
	fixed_120_50.qpf \
	fixed_70_50.qpf
endif

ifdef PTXCONF_QT4_FONT_HELVETICA
QT4_FONTS += \
	helvetica_100_50.qpf \
	helvetica_100_50i.qpf \
	helvetica_100_75.qpf \
	helvetica_100_75i.qpf \
	helvetica_120_50.qpf \
	helvetica_120_50i.qpf \
	helvetica_120_75.qpf \
	helvetica_120_75i.qpf \
	helvetica_140_50.qpf \
	helvetica_140_50i.qpf \
	helvetica_140_75.qpf \
	helvetica_140_75i.qpf \
	helvetica_180_50.qpf \
	helvetica_180_50i.qpf \
	helvetica_180_75.qpf \
	helvetica_180_75i.qpf \
	helvetica_240_50.qpf \
	helvetica_240_50i.qpf \
	helvetica_240_75.qpf \
	helvetica_240_75i.qpf \
	helvetica_80_50.qpf \
	helvetica_80_50i.qpf \
	helvetica_80_75.qpf \
	helvetica_80_75i.qpf
endif

ifdef PTXCONF_QT4_FONT_JAPANESE
QT4_FONTS += \
	japanese_230_50.qpf
endif

ifdef PTXCONF_QT4_FONT_L04
QT4_FONTS += \
	l047013t.pfa \
	l047016t.pfa \
	l047033t.pfa \
	l047036t.pfa \
	l048013t.pfa \
	l048016t.pfa \
	l048033t.pfa \
	l048036t.pfa \
	l049013t.pfa \
	l049016t.pfa \
	l049033t.pfa \
	l049036t.pfa
endif

$(STATEDIR)/qt4.targetinstall:
	@$(call targetinfo)

	@$(call install_init, qt4)
	@$(call install_fixup, qt4,PRIORITY,optional)
	@$(call install_fixup, qt4,SECTION,base)
	@$(call install_fixup, qt4,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, qt4,DESCRIPTION,missing)

ifdef PTXCONF_QT4_SHARED
	@$(foreach lib, $(QT4_LIBS-y), \
		$(call install_lib, qt4, 0, 0, 0644, lib$(lib));)

endif #PTXCONF_QT4_SHARED

ifdef PTXCONF_QT4_BUILD_DECLARATIVE
ifdef PTXCONF_QT4_BUILD_WEBKIT
	@$(call install_copy, qt4, 0, 0, 0644, -, \
		/usr/imports/QtWebKit/qmldir)
	@$(call install_copy, qt4, 0, 0, 0644, -, \
		/usr/imports/QtWebKit/libqmlwebkitplugin.$(QT4_PLUGIN_EXT))
endif
	@$(call install_copy, qt4, 0, 0, 0644, -, \
		/usr/imports/Qt/labs/folderlistmodel/qmldir)
	@$(call install_copy, qt4, 0, 0, 0644, -, \
		/usr/imports/Qt/labs/folderlistmodel/libqmlfolderlistmodelplugin.$(QT4_PLUGIN_EXT))

	@$(call install_copy, qt4, 0, 0, 0644, -, \
		/usr/imports/Qt/labs/gestures/qmldir)
	@$(call install_copy, qt4, 0, 0, 0644, -, \
		/usr/imports/Qt/labs/gestures/libqmlgesturesplugin.$(QT4_PLUGIN_EXT))

	@$(call install_copy, qt4, 0, 0, 0644, -, \
		/usr/imports/Qt/labs/particles/qmldir)
	@$(call install_copy, qt4, 0, 0, 0644, -, \
		/usr/imports/Qt/labs/particles/libqmlparticlesplugin.$(QT4_PLUGIN_EXT))
endif
ifdef PTXCONF_QT4_SQLITE_PLUGIN
	@$(call install_copy, qt4, 0, 0, 0644, -, \
		/usr/plugins/sqldrivers/libqsqlite.$(QT4_PLUGIN_EXT))
endif
	@$(foreach lib, $(QT4_GFXDRIVERS-plugin), \
		$(call install_copy, qt4, 0, 0, 0644, -, \
			/usr/plugins/gfxdrivers/libq$(lib).$(QT4_PLUGIN_EXT));)
ifdef PTXCONF_QT4_GFX_POWERVR_PLUGIN
	@$(call install_lib, qt4, 0, 0, 0644, libpvrQWSWSEGL)
endif

ifdef PTXCONF_QT4_DBUS
ifdef PTXCONF_QT4_BUILD_SCRIPT
	@$(call install_copy, qt4, 0, 0, 0644, -, \
		/usr/plugins/script/libqtscriptdbus.$(QT4_PLUGIN_EXT))
endif
endif

#	# not *_NONE is used here
	@$(foreach lib, $(QT4_IMAGEFORMATS-), \
		$(call install_copy, qt4, 0, 0, 0644, -, \
			/usr/plugins/imageformats/libq$(lib).$(QT4_PLUGIN_EXT));)
ifdef PTXCONF_QT4_BUILD_SVG
	@$(call install_copy, qt4, 0, 0, 0644, -, \
		/usr/plugins/iconengines/libqsvgicon.$(QT4_PLUGIN_EXT))
endif

ifndef PTXCONF_QT4_BUILD_NETWORK
	@$(call install_copy, qt4, 0, 0, 0644, -, \
		/usr/plugins/bearer/libqgenericbearer.$(QT4_PLUGIN_EXT))
	@$(call install_copy, qt4, 0, 0, 0644, -, \
		/usr/plugins/bearer/libqnmbearer.$(QT4_PLUGIN_EXT))
endif
ifdef PTXCONF_QT4_BUILD_PHONON
	@$(call install_copy, qt4, 0, 0, 0644, -, \
		/usr/plugins/phonon_backend/libphonon_gstreamer.$(QT4_PLUGIN_EXT))
endif
ifdef PTXCONF_QT4_INSTALL_QMLVIEWER
	@$(call install_copy, qt4, 0, 0, 0755, -, \
		/usr/bin/qmlviewer)
endif

	@$(foreach font, $(QT4_FONTS), \
		$(call install_copy, qt4, 0, 0, 0644, -, \
			/usr/lib/fonts/$(font));)

ifdef PTXCONF_PRELINK
	@$(call install_alternative, qt4, 0, 0, 0644, \
		/etc/prelink.conf.d/qt4)
endif

	@$(call install_finish, qt4)

	@$(call touch)

# vim: syntax=make
