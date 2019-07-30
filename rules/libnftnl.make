# -*-makefile-*-
#
# Copyright (C) 2016 by Andreas Geisenhainer <andreas.geisenhainer@atsonline.de>
#
# See CREDITS for details about who has contributed to this project.
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
LIBNFTNL_VERSION	:= 1.0.9
LIBNFTNL_MD5	:= 6c4f392faab5745933553b4354be5d8d
LIBNFTNL		:= libnftnl-$(LIBNFTNL_VERSION)
LIBNFTNL_SUFFIX	:= tar.bz2
LIBNFTNL_URL		:= http://ftp.netfilter.org/pub/libnftnl/$(LIBNFTNL).$(LIBNFTNL_SUFFIX)
LIBNFTNL_SOURCE	:= $(SRCDIR)/$(LIBNFTNL).$(LIBNFTNL_SUFFIX)
LIBNFTNL_DIR		:= $(BUILDDIR)/$(LIBNFTNL)
LIBNFTNL_LICENSE	:= GPL-2.0-only

#
# autoconf
#
LIBNFTNL_CONF_TOOL	:= autoconf
LIBNFTNL_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--$(call ptx/wwo, PTXCONF_LIBNFTNL_JSON)-json-parsing

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
