# -*-makefile-*-
#
# Copyright (C) 2002-2009 by Pengutronix e.K., Hildesheim, Germany
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_NCURSES) += ncurses

#
# Paths and names
#
NCURSES_VERSION	:= 6.2
NCURSES_MAJOR	:= $(word 1,$(subst ., ,$(NCURSES_VERSION)))
NCURSES_MD5	:= e812da327b1c2214ac1aed440ea3ae8d
NCURSES		:= ncurses-$(NCURSES_VERSION)
NCURSES_SUFFIX	:= tar.gz
NCURSES_URL	:= $(call ptx/mirror, GNU, ncurses/$(NCURSES).$(NCURSES_SUFFIX))
NCURSES_SOURCE	:= $(SRCDIR)/$(NCURSES).$(NCURSES_SUFFIX)
NCURSES_DIR	:= $(BUILDDIR)/$(NCURSES)
NCURSES_LICENSE	:= MIT
NCURSES_LICENSE_FILES := file://COPYING;md5=910e05334f7e0b7631da6b4ebb1e1aab

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

NCURSES_CONF_TOOL := autoconf
NCURSES_CONF_ENV := \
	$(CROSS_ENV) \
	TIC_PATH="$(PTXDIST_SYSROOT_HOST)/bin/tic"

NCURSES_SHARED_TARGET	:= y
NCURSES_SHARED_HOST	:=

# NOTE: reentrant enables opaque, which breaks other packages
# pthread enables reentrant, so don't enable it either
NCURSES_AUTOCONF_SHARED = \
	--without-ada \
	--without-cxx \
	--without-cxx-binding \
	--enable-db-install \
	--without-manpages \
	--without-tack \
	--without-tests \
	--with-curses-h \
	--with-pkg-config-libdir=/usr/lib/pkgconfig \
	--$(call ptx/endis,$(1))-pc-files \
	--enable-mixed-case \
	--without-libtool \
	--$(call ptx/wwo,$(1))-shared \
	--with-normal \
	--without-debug \
	--without-profile \
	--without-cxx-shared \
	--without-termlib \
	--without-ticlib \
	--without-gpm \
	--without-dlsym \
	--without-pcre2 \
	--without-sysmouse \
	--disable-rpath \
	--disable-relink \
	--disable-rpath-hack \
	--enable-overwrite \
	--$(call ptx/endis,PTXCONF_NCURSES_BIG_CORE)-big-core \
	--disable-big-strings \
	--$(call ptx/endis,PTXCONF_NCURSES_WIDE_CHAR)-widec \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--enable-ext-funcs \
	--enable-sp-funcs \
	--disable-term-driver \
	--enable-const \
	--enable-ext-colors \
	--disable-ext-mouse \
	--disable-ext-putwin \
	--disable-no-padding \
	--disable-signed-char \
	--disable-sigwinch \
	--without-pthread \
	--disable-reentrant \
	--without-develop \
	--disable-interop

NCURSES_CONF_OPT := \
	$(CROSS_AUTOCONF_USR) \
	$(call NCURSES_AUTOCONF_SHARED,NCURSES_SHARED_TARGET)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

ifdef PTXCONF_NCURSES_WIDE_CHAR
#
# we need a tweak, to force all programs to use the wide char
# library even if they request for the non wide char library
# Done by forcing the linker to use the right library instead
#
NCURSES_LIBRARY_LIST := ncurses

ifdef PTXCONF_NCURSES_FORM
NCURSES_LIBRARY_LIST += form
endif
ifdef PTXCONF_NCURSES_MENU
NCURSES_LIBRARY_LIST += menu
endif
ifdef PTXCONF_NCURSES_PANEL
NCURSES_LIBRARY_LIST += panel
endif

NCURSES_WIDE := w
endif

$(STATEDIR)/ncurses.install:
	@$(call targetinfo)
	@$(call world/install, NCURSES)

ifdef PTXCONF_NCURSES_WIDE_CHAR
# Note: This tweak only works if we build the application with these settings!
# Already built applications may continue to use the non wide library!
# For this, the links at runtime are required
#
	@for lib in $(NCURSES_LIBRARY_LIST); do \
		echo "INPUT(-l$${lib}w)" > $(NCURSES_PKGDIR)/$(CROSS_LIB_DIR)/lib$${lib}.so ; \
	done
	@echo "INPUT(-lncursesw)" > $(NCURSES_PKGDIR)/$(CROSS_LIB_DIR)/libcurses.so

	@ln -sf "ncursesw$(NCURSES_MAJOR)-config" \
		"$(NCURSES_PKGDIR)/usr/bin/ncurses$(NCURSES_MAJOR)-config"
ifdef PTXCONF_NCURSES_BACKWARD_COMPATIBLE_NON_WIDE_CHAR
	@for lib in $(NCURSES_LIBRARY_LIST); do \
		ln -vs "$${lib}w.pc" "$(NCURSES_PKGDIR)/usr/$(CROSS_LIB_DIR)/pkgconfig/$${lib}.pc"; \
	done
endif
endif
	@$(call touch)

$(STATEDIR)/ncurses.install.post:
	@$(call targetinfo)
	@$(call world/install.post, NCURSES)
	@cp -dp -- "$(NCURSES_PKGDIR)/usr/bin/"*config* "$(PTXDIST_SYSROOT_CROSS)/bin"
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ncurses.targetinstall:
	@$(call targetinfo)

	@$(call install_init, ncurses)
	@$(call install_fixup, ncurses,PRIORITY,optional)
	@$(call install_fixup, ncurses,SECTION,base)
	@$(call install_fixup, ncurses,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, ncurses,DESCRIPTION,missing)

	@$(call install_lib, ncurses, 0, 0, 0644, libncurses$(NCURSES_WIDE))

ifdef PTXCONF_NCURSES_BACKWARD_COMPATIBLE_NON_WIDE_CHAR
	@$(call install_link, ncurses, libncursesw.so.$(NCURSES_VERSION), \
		/$(CROSS_LIB_DIR)/libncurses.so.$(NCURSES_VERSION))
	@$(call install_link, ncurses, libncursesw.so.$(NCURSES_VERSION), \
		/$(CROSS_LIB_DIR)/libncurses.so.$(NCURSES_MAJOR))
	@$(call install_link, ncurses, libncursesw.so.$(NCURSES_VERSION), \
		/$(CROSS_LIB_DIR)/libncurses.so)
endif

ifdef PTXCONF_NCURSES_FORM
	@$(call install_lib, ncurses, 0, 0, 0644, libform$(NCURSES_WIDE))
ifdef PTXCONF_NCURSES_BACKWARD_COMPATIBLE_NON_WIDE_CHAR
	@$(call install_link, ncurses, libformw.so.$(NCURSES_VERSION), \
		/$(CROSS_LIB_DIR)/libform.so.$(NCURSES_VERSION))
	@$(call install_link, ncurses, libformw.so.$(NCURSES_VERSION), \
		/$(CROSS_LIB_DIR)/libform.so.$(NCURSES_MAJOR))
	@$(call install_link, ncurses, libformw.so.$(NCURSES_VERSION), \
		/$(CROSS_LIB_DIR)/libform.so)
endif
endif


ifdef PTXCONF_NCURSES_MENU
	@$(call install_lib, ncurses, 0, 0, 0644, libmenu$(NCURSES_WIDE))
ifdef PTXCONF_NCURSES_BACKWARD_COMPATIBLE_NON_WIDE_CHAR
	@$(call install_link, ncurses, libmenuw.so.$(NCURSES_VERSION), \
		/$(CROSS_LIB_DIR)/libmenu.so.$(NCURSES_VERSION))
	@$(call install_link, ncurses, libmenuw.so.$(NCURSES_VERSION), \
		/$(CROSS_LIB_DIR)/libmenu.so.$(NCURSES_MAJOR))
	@$(call install_link, ncurses, libmenuw.so.$(NCURSES_VERSION), \
		/$(CROSS_LIB_DIR)/libmenu.so)
endif
endif


ifdef PTXCONF_NCURSES_PANEL
	@$(call install_lib, ncurses, 0, 0, 0644, libpanel$(NCURSES_WIDE))
ifdef PTXCONF_NCURSES_BACKWARD_COMPATIBLE_NON_WIDE_CHAR
	@$(call install_link, ncurses, libpanelw.so.$(NCURSES_VERSION), \
		/$(CROSS_LIB_DIR)/libpanel.so.$(NCURSES_VERSION))
	@$(call install_link, ncurses, libpanelw.so.$(NCURSES_VERSION), \
		/$(CROSS_LIB_DIR)/libpanel.so.$(NCURSES_MAJOR))
	@$(call install_link, ncurses, libpanelw.so.$(NCURSES_VERSION), \
		/$(CROSS_LIB_DIR)/libpanel.so)
endif
endif


ifdef PTXCONF_NCURSES_TERMCAP
	@$(call install_copy, ncurses, 0, 0, 0644, -, /usr/share/terminfo/x/xterm, n);
	@$(call install_copy, ncurses, 0, 0, 0644, -, /usr/share/terminfo/x/xterm-color, n);
	@$(call install_copy, ncurses, 0, 0, 0644, -, /usr/share/terminfo/x/xterm-256color, n);
	@$(call install_copy, ncurses, 0, 0, 0644, -, /usr/share/terminfo/x/xterm-xfree86, n);
	@$(call install_copy, ncurses, 0, 0, 0644, -, /usr/share/terminfo/v/vt100, n);
	@$(call install_copy, ncurses, 0, 0, 0644, -, /usr/share/terminfo/v/vt102, n);
	@$(call install_copy, ncurses, 0, 0, 0644, -, /usr/share/terminfo/v/vt200, n);
	@$(call install_copy, ncurses, 0, 0, 0644, -, /usr/share/terminfo/v/vt220, n);
	@$(call install_copy, ncurses, 0, 0, 0644, -, /usr/share/terminfo/a/ansi, n);
	@$(call install_copy, ncurses, 0, 0, 0644, -, /usr/share/terminfo/l/linux, n);
	@$(call install_copy, ncurses, 0, 0, 0644, -, /usr/share/terminfo/s/screen, n);
	@$(call install_copy, ncurses, 0, 0, 0644, -, /usr/share/terminfo/s/screen-256color, n);
	@$(call install_copy, ncurses, 0, 0, 0644, -, /usr/share/terminfo/d/dumb, n);
endif

	@$(call install_finish, ncurses)

	@$(call touch)

# vim: syntax=make
