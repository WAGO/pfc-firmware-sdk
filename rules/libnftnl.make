# -*-makefile-*-
#
# Copyright (C) 2016 by Andreas Geisenhainer <andreas.geisenhainer@atsonline.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBNFTNL) += libnftnl

#
# Paths and names
#
LIBNFTNL_VERSION	:= 1.1.3
LIBNFTNL_MD5		:= e2a7af0a85c283b2cc837c09635b6bca
LIBNFTNL		:= libnftnl-$(LIBNFTNL_VERSION)
LIBNFTNL_SUFFIX		:= tar.bz2
LIBNFTNL_URL		:= http://ftp.netfilter.org/pub/libnftnl/$(LIBNFTNL).$(LIBNFTNL_SUFFIX)
LIBNFTNL_SOURCE		:= $(SRCDIR)/$(LIBNFTNL).$(LIBNFTNL_SUFFIX)
LIBNFTNL_DIR		:= $(BUILDDIR)/$(LIBNFTNL)
LIBNFTNL_LICENSE	:= GPL-2.0-only AND GPL-2.0-or-later
LIBNFTNL_LICENSE_FILES	:= file://COPYING;md5=79808397c3355f163c012616125c9e26

#
# autoconf
#
LIBNFTNL_CONF_TOOL	:= autoconf

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libnftnl.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libnftnl)
	@$(call install_fixup, libnftnl,PRIORITY,optional)
	@$(call install_fixup, libnftnl,SECTION,base)
	@$(call install_fixup, libnftnl,AUTHOR,"Andreas Geisenhainer <andreas.geisenhainer@atsonline.de>")
	@$(call install_fixup, libnftnl,DESCRIPTION,missing)

	@$(call install_lib, libnftnl, 0, 0, 0644, libnftnl)

	@$(call install_finish, libnftnl)

	@$(call touch)

# vim: syntax=make
