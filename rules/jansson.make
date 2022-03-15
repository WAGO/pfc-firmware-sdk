# -*-makefile-*-
#
# Copyright (C) 2014 by Christian Gieseler <cg@eks-engel.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_JANSSON) += jansson

#
# Paths and names
#
JANSSON_VERSION	:= 2.14
JANSSON_MD5	:= 3f90473d7d54ebd1cb6a2757396641df
JANSSON		:= jansson-$(JANSSON_VERSION)
JANSSON_SUFFIX	:= tar.bz2
JANSSON_URL	:= https://github.com/akheron/jansson/releases/download/v$(JANSSON_VERSION)/$(JANSSON).$(JANSSON_SUFFIX)
JANSSON_SOURCE	:= $(SRCDIR)/$(JANSSON).$(JANSSON_SUFFIX)
JANSSON_DIR	:= $(BUILDDIR)/lib$(JANSSON)
JANSSON_LICENSE	:= MIT
JANSSON_LICENSE_FILE := LICENSE

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#

JANSSON_CONF_TOOL	:= autoconf
JANSSON_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-windows-cryptoapi

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/jansson.targetinstall:
	@$(call targetinfo)

	@$(call install_init, jansson)
	@$(call install_fixup, jansson,PRIORITY,optional)
	@$(call install_fixup, jansson,SECTION,base)
	@$(call install_fixup, jansson,AUTHOR,"Christian Gieseler <cg@eks-engel.de>")
	@$(call install_fixup, jansson,DESCRIPTION,missing)

	@$(call install_lib, jansson, 0, 0, 0644, libjansson)
	@$(call install_copy, jansson, 0, 0, 0644, $(JANSSON_DIR)/$(JANSSON_LICENSE_FILE), /usr/share/licenses/oss/license.jansson_$(JANSSON_VERSION).txt)

	@$(call install_finish, jansson)

	@$(call touch)

# vim: syntax=make
