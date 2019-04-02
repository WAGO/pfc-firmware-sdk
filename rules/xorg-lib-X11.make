# -*-makefile-*-
#
# Copyright (C) 2006 by Erwin Rol
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_XORG_LIB_X11) += xorg-lib-x11

#
# Paths and names
#
XORG_LIB_X11_VERSION	:= 1.6.5
XORG_LIB_X11_MD5	:= 0f618db70c4054ca67cee0cc156a4255
XORG_LIB_X11		:= libX11-$(XORG_LIB_X11_VERSION)
XORG_LIB_X11_SUFFIX	:= tar.bz2
XORG_LIB_X11_URL	:= $(call ptx/mirror, XORG, individual/lib/$(XORG_LIB_X11).$(XORG_LIB_X11_SUFFIX))
XORG_LIB_X11_SOURCE	:= $(SRCDIR)/$(XORG_LIB_X11).$(XORG_LIB_X11_SUFFIX)
XORG_LIB_X11_DIR	:= $(BUILDDIR)/$(XORG_LIB_X11)
XORG_LIB_X11_LICENSE	:= MIT

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

XORG_LIB_X11_CONF_ENV := \
	$(CROSS_ENV) \
	ac_cv_path_RAWCPP=$(PTXCONF_COMPILER_PREFIX)cpp

# configure states: "checking for working mmap...no"
# is this a correct fix?
XORG_LIB_X11_CONF_ENV += ac_cv_func_mmap_fixed_mapped=yes

#
# autoconf
#
XORG_LIB_X11_CONF_TOOL	:= autoconf
XORG_LIB_X11_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-malloc0returnsnull \
	--disable-specs \
	$(XORG_OPTIONS_TRANS) \
#	--$(call ptx/endis, PTXCONF_XORG_SERVER_OPT_SECURE_RPC)-secure-rpc \
	--$(call ptx/endis, PTXCONF_XORG_LIB_X11_I18N)-loadable-i18n \
	--$(call ptx/endis, PTXCONF_XORG_LIB_X11_CURSOR)-loadable-xcursor \
	--enable-xthreads \
	--enable-xcms \
	--enable-xlocale \
	--$(call ptx/endis, PTXCONF_XORG_LIB_X11_XF86BIGFONT)-xf86bigfont \
	--$(call ptx/endis, PTXCONF_XORG_LIB_X11_XKB)-xkb \
	--enable-composecache \
	$(XORG_OPTIONS_DOCS) \
	--without-perl \
	--without-lint

#
# if no value is given ignore the "--datadir" switch
#
ifneq ($(call remove_quotes,$(XORG_DATADIR)),)
XORG_LIB_X11_CONF_OPT += --datadir=$(XORG_DATADIR)
endif

# missing configure switches:
# --enable-xlocaledir     Enable XLOCALEDIR environment variable support
#

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/xorg-lib-x11.targetinstall:
	@$(call targetinfo)

	@$(call install_init, xorg-lib-x11)
	@$(call install_fixup, xorg-lib-x11,PRIORITY,optional)
	@$(call install_fixup, xorg-lib-x11,SECTION,base)
	@$(call install_fixup, xorg-lib-x11,AUTHOR,"Erwin Rol <ero@pengutronix.de>")
	@$(call install_fixup, xorg-lib-x11,DESCRIPTION,missing)

	@$(call install_lib, xorg-lib-x11, 0, 0, 0644, libX11)
	@$(call install_lib, xorg-lib-x11, 0, 0, 0644, libX11-xcb)

# Where is it gone?
#	@$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
#		/usr/lib/X11/XKeysymDB)

ifdef PTXCONF_XORG_LIB_X11_INSTALL_LOCALE
	@$(call install_copy, xorg-lib-x11, 0, 0, 0755, $(XORG_DATADIR)/X11/locale)
	@$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
		$(XORG_DATADIR)/X11/locale/locale.alias,n)

	@$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
		$(XORG_DATADIR)/X11/locale/locale.dir,n)

	@$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
		$(XORG_DATADIR)/X11/locale/compose.dir,n)

	@$(call install_copy, xorg-lib-x11, 0, 0, 0755, $(XORG_DATADIR)/X11/locale/C)
	@$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
		$(XORG_DATADIR)/X11/locale/C/Compose,n)
	@$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
		$(XORG_DATADIR)/X11/locale/C/XI18N_OBJS,n)
	@$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
		$(XORG_DATADIR)/X11/locale/C/XLC_LOCALE,n)

endif

ifdef PTXCONF_XORG_LIB_X11_INSTALL_LOCALE_8859_1
	@$(call install_copy, xorg-lib-x11, 0, 0, 0755, $(XORG_DATADIR)/X11/locale/iso8859-1)
	@$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
		$(XORG_DATADIR)/X11/locale/iso8859-1/Compose,n)
	@$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
		$(XORG_DATADIR)/X11/locale/iso8859-1/XI18N_OBJS,n)
	@$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
		$(XORG_DATADIR)/X11/locale/iso8859-1/XLC_LOCALE,n)
endif

ifdef PTXCONF_XORG_LIB_X11_INSTALL_LOCALE_8859_15
	@$(call install_copy, xorg-lib-x11, 0, 0, 0755, $(XORG_DATADIR)/X11/locale/iso8859-15)
	@$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
		$(XORG_DATADIR)/X11/locale/iso8859-15/Compose,n)
	@$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
		$(XORG_DATADIR)/X11/locale/iso8859-15/XI18N_OBJS,n)
	@$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
		$(XORG_DATADIR)/X11/locale/iso8859-15/XLC_LOCALE,n)
endif

ifdef PTXCONF_XORG_LIB_X11_INSTALL_LOCALE_CHN_MAIN
	@cd $(XORG_LIB_X11_DIR)/nls; \
	for file in `find . -name "*zh_CN*" -type d`; do \
		echo "scanning $$file"; \
		if [ -d $$file ]; then \
			$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
				$(XORG_DATADIR)/X11/locale/$$file/Compose,n); \
			$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
				$(XORG_DATADIR)/X11/locale/$$file/XI18N_OBJS,n); \
			$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
				$(XORG_DATADIR)/X11/locale/$$file/XLC_LOCALE,n); \
		fi; \
	done;
endif

ifdef PTXCONF_XORG_LIB_X11_INSTALL_LOCALE_CHN_HK
	@cd $(XORG_LIB_X11_DIR)/nls; \
	for file in `find . -name "*zh_HK*" -type d`; do \
		if [ -d $$file ]; then \
			$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
				$(XORG_DATADIR)/X11/locale/$$file/Compose,n); \
			$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
				$(XORG_DATADIR)/X11/locale/$$file/XI18N_OBJS,n); \
			$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
				$(XORG_DATADIR)/X11/locale/$$file/XLC_LOCALE,n); \
		fi; \
	done;
endif

ifdef PTXCONF_XORG_LIB_X11_INSTALL_LOCALE_CHN_TW
	@cd $(XORG_LIB_X11_DIR)/nls; \
	for file in `find . -name "*zh_TW*" -type d`; do \
		if [ -d $$file ]; then \
			$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
				$(XORG_DATADIR)/X11/locale/$$file/Compose,n); \
			$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
				$(XORG_DATADIR)/X11/locale/$$file/XI18N_OBJS,n); \
			$(call install_copy, xorg-lib-x11, 0, 0, 0644, -, \
				$(XORG_DATADIR)/X11/locale/$$file/XLC_LOCALE,n); \
		fi; \
	done;
endif

	@$(call install_finish, xorg-lib-x11)

	@$(call touch)

# vim: syntax=make
