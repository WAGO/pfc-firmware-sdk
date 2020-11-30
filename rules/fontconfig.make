# -*-makefile-*-
#
# Copyright (C) 2006 by Robert Schwebel
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_FONTCONFIG) += fontconfig

#
# Paths and names
#
FONTCONFIG_VERSION	:= 2.13.92
FONTCONFIG_MD5		:= eda1551685c25c4588da39222142f063
FONTCONFIG		:= fontconfig-$(FONTCONFIG_VERSION)
FONTCONFIG_SUFFIX	:= tar.gz
FONTCONFIG_URL		:= http://fontconfig.org/release/$(FONTCONFIG).$(FONTCONFIG_SUFFIX)
FONTCONFIG_SOURCE	:= $(SRCDIR)/$(FONTCONFIG).$(FONTCONFIG_SUFFIX)
FONTCONFIG_DIR		:= $(BUILDDIR)/$(FONTCONFIG)
FONTCONFIG_LICENSE	:= MIT, Unicode-TOU, public_domain


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

FONTCONFIG_ENV	:=  \
	$(CROSS_ENV) \
	ac_cv_prog_HASDOCBOOK=no

#
# autoconf
#
FONTCONFIG_CONF_TOOL	:= autoconf
FONTCONFIG_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--$(call ptx/endis, PTXCONF_ICONV)-iconv \
	--disable-libxml2 \
	--disable-docs \
	--with-cache-dir=/var/cache/fontconfig \
	--with-default-fonts=$(XORG_FONTDIR) \
	--with-arch=$(PTXCONF_ARCH_STRING)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/fontconfig.targetinstall:
	@$(call targetinfo)

	@$(call install_init, fontconfig)
	@$(call install_fixup, fontconfig,PRIORITY,optional)
	@$(call install_fixup, fontconfig,SECTION,base)
	@$(call install_fixup, fontconfig,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, fontconfig,DESCRIPTION,missing)

	@$(call install_lib, fontconfig, 0, 0, 0644, libfontconfig)

	@$(call install_alternative, fontconfig, 0, 0, 0644, \
		/etc/fonts/fonts.conf)
	@$(call install_tree, fontconfig, 0, 0, -, /etc/fonts/conf.d)
	@$(call install_tree, fontconfig, 0, 0, -, /usr/share/fontconfig/conf.avail)

ifdef PTXCONF_FONTCONFIG_UTILS
	@$(call install_copy, fontconfig, 0, 0, 0755, -, \
		/usr/bin/fc-cache)

	@$(call install_copy, fontconfig, 0, 0, 0755, -, \
		/usr/bin/fc-list)

	@$(call install_copy, fontconfig, 0, 0, 0755, -, \
		/usr/bin/fc-match)

	@$(call install_copy, fontconfig, 0, 0, 0755, -, \
		/etc/fonts/fonts.conf)
endif

	@$(call install_finish, fontconfig)

	@$(call touch)

# vim: syntax=make
