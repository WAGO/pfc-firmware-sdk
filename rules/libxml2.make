# -*-makefile-*-
#
# Copyright (C) 2003 by Benedikt Spranger
#               2004-2009 by the ptxdist project
#               2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBXML2) += libxml2

#
# Paths and names
#
LIBXML2_VERSION	:= 2.9.13
LIBXML2_MD5	:= 824470f8cc325ae6b01f174b842c321f
LIBXML2		:= libxml2-$(LIBXML2_VERSION)
LIBXML2_SUFFIX	:= tar.xz
LIBXML2_SOURCE	:= $(SRCDIR)/$(LIBXML2).$(LIBXML2_SUFFIX)
LIBXML2_DIR	:= $(BUILDDIR)/$(LIBXML2)
LIBXML2_LICENSE	:= MIT AND ISC
# The file 'COPYING' is just a symlink on the file 'Copyright'
LIBXML2_LICENSE_FILES := \
	file://Copyright;md5=2044417e2e5006b65a8b9067b683fcf1 \
	file://hash.c;startline=6;endline=15;md5=e77f77b12cb69e203d8b4090a0eee879

LIBXML2_URL := \
	https://download.gnome.org/sources/libxml2/$(basename $(LIBXML2_VERSION))/$(LIBXML2).$(LIBXML2_SUFFIX)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBXML2_PATH	:= PATH=$(CROSS_PATH)
LIBXML2_ENV	:= $(CROSS_ENV)

#
# autoconf
#
LIBXML2_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--disable-static \
	--disable-rebuild-docs \
	$(GLOBAL_IPV6_OPTION) \
	--oldincludedir=$(SYSROOT)/usr/include \
	--$(call ptx/wwo, PTXCONF_LIBXML2_C14N)-c14n \
	--$(call ptx/wwo, PTXCONF_LIBXML2_CATALOG)-catalog \
	--$(call ptx/wwo, PTXCONF_LIBXML2_DEBUG)-debug \
	--$(call ptx/wwo, PTXCONF_LIBXML2_DOCBOOK)-docbook \
	--$(call ptx/wwo, PTXCONF_LIBXML2_FEXCEPTIONS)-fexceptions \
	--$(call ptx/wwo, PTXCONF_LIBXML2_FTP)-ftp \
	--$(call ptx/wwo, PTXCONF_LIBXML2_HISTORY)-history \
	--$(call ptx/wwo, PTXCONF_LIBXML2_HTML)-html \
	--$(call ptx/wwo, PTXCONF_LIBXML2_HTTP)-http \
	--without-icu \
	--$(call ptx/wwo, PTXCONF_LIBXML2_ISO8859X)-iso8859x \
	--$(call ptx/wwo, PTXCONF_LIBXML2_LEGACY)-legacy \
	--$(call ptx/wwo, PTXCONF_LIBXML2_MEM_DEBUG)-mem-debug \
	--$(call ptx/wwo, PTXCONF_LIBXML2_MINIMUM)-minimum \
	--$(call ptx/wwo, PTXCONF_LIBXML2_OUTPUT)-output \
	--$(call ptx/wwo, PTXCONF_LIBXML2_PATTERN)-pattern \
	--$(call ptx/wwo, PTXCONF_LIBXML2_PUSH)-push \
	--$(call ptx/wwo, PTXCONF_LIBXML2_READER)-reader \
	--$(call ptx/wwo, PTXCONF_LIBXML2_REGEXPS)-regexps \
	--$(call ptx/wwo, PTXCONF_LIBXML2_RUN_DEBUG)-run-debug \
	--$(call ptx/wwo, PTXCONF_LIBXML2_SAX1)-sax1 \
	--$(call ptx/wwo, PTXCONF_LIBXML2_SCHEMAS)-schemas \
	--$(call ptx/wwo, PTXCONF_LIBXML2_SCHEMATRON)-schematron \
	--$(call ptx/wwo, PTXCONF_LIBXML2_THREADS)-threads \
	--$(call ptx/wwo, PTXCONF_LIBXML2_THREADS_ALLOC)-thread-alloc \
	--$(call ptx/wwo, PTXCONF_LIBXML2_DOM)-tree \
	--$(call ptx/wwo, PTXCONF_LIBXML2_DTD)-valid \
	--$(call ptx/wwo, PTXCONF_LIBXML2_WRITER)-writer \
	--$(call ptx/wwo, PTXCONF_LIBXML2_XINCLUDE)-xinclude \
	--$(call ptx/wwo, PTXCONF_LIBXML2_XPATH)-xpath \
	--$(call ptx/wwo, PTXCONF_LIBXML2_XPTR)-xptr \
	--$(call ptx/wwo, PTXCONF_LIBXML2_MODULES)-modules \
	--$(call ptx/wwo, PTXCONF_LIBXML2_LZMA)-lzma \
	--without-coverage

ifdef PTXCONF_ICONV
# --with-iconv=yes -> does the right thing for libc-iconv
LIBXML2_AUTOCONF += --with-iconv=yes
else
LIBXML2_AUTOCONF += --with-iconv=no
endif

ifdef PTXCONF_LIBXML2_PYTHON
LIBXML2_AUTOCONF += --with-python=$(SYSROOT)/usr
else
LIBXML2_AUTOCONF += --with-python=no
endif

ifdef PTXCONF_LIBXML2_ZLIB
LIBXML2_AUTOCONF += --with-zlib=$(SYSROOT)/usr
else
LIBXML2_AUTOCONF += --without-zlib
endif

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libxml2.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libxml2)
	@$(call install_fixup, libxml2,PRIORITY,optional)
	@$(call install_fixup, libxml2,SECTION,base)
	@$(call install_fixup, libxml2,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libxml2,DESCRIPTION,missing)

	@$(call install_lib, libxml2, 0, 0, 0644, libxml2)

ifdef PTXCONF_LIBXML2_XMLLINT
	@$(call install_copy, libxml2, 0, 0, 0755, -, /usr/bin/xmllint)
endif

	@$(call install_finish, libxml2)

	@$(call touch)

# vim: syntax=make