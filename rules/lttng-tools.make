# -*-makefile-*-
#
# Copyright (C) 2012 by Wolfram Sang <w.sang@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LTTNG_TOOLS) += lttng-tools

#
# Paths and names
#
LTTNG_TOOLS_VERSION	:= 2.6.0
LTTNG_TOOLS_MD5		:= 0478f60395f9564b4a19f45ce7b7f3df
LTTNG_TOOLS		:= lttng-tools-$(LTTNG_TOOLS_VERSION)
LTTNG_TOOLS_SUFFIX	:= tar.bz2
#LTTNG_TOOLS_URL		:= $(call ptx/mirror, SF, libcg/$(LTTNG_TOOLS).$(LTTNG_TOOLS_SUFFIX))
LTTNG_TOOLS_SOURCE	:= $(SRCDIR)/$(LTTNG_TOOLS).$(LTTNG_TOOLS_SUFFIX)
LTTNG_TOOLS_DIR		:= $(BUILDDIR)/$(LTTNG_TOOLS)
LTTNG_TOOLS_LICENSE	:= LGPLv2.1

#
# autoconf
#
LTTNG_TOOLS_CONF_TOOL := autoconf
LTTNG_TOOLS_CONF_OPT := \
	$(CROSS_AUTOCONF_USR) \



# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/lttng-tools.targetinstall:
	@$(call targetinfo)

	@$(call install_init, lttng-tools)
	@$(call install_fixup, lttng-tools,PRIORITY,optional)
	@$(call install_fixup, lttng-tools,SECTION,base)
	@$(call install_fixup, lttng-tools,AUTHOR,"jso@wago.com <JSo>")
	@$(call install_fixup, lttng-tools,DESCRIPTION,missing)

	@$(call install_copy, lttng-tools, 0, 0, 0755, $(LTTNG_TOOLS_DIR)/src/lib/lttng-ctl/.libs/liblttng-ctl.so.0.0.0, /usr/lib/liblttng-ctl.so.$(LTTNG_TOOLS_VERSION))
	@$(call install_link, lttng-tools, liblttng-ctl.so.$(LTTNG_TOOLS_VERSION), /usr/lib/liblttng-ctl.so.2)
	@$(call install_link, lttng-tools, liblttng-ctl.so.$(LTTNG_TOOLS_VERSION), /usr/lib/liblttng-ctl.so)

	@$(call install_copy, lttng-tools, 0, 0, 0755, $(LTTNG_TOOLS_DIR)/src/bin/lttng/lttng, /usr/bin/lttng)
	@$(call install_copy, lttng-tools, 0, 0, 0755, $(LTTNG_TOOLS_DIR)/src/bin/lttng-consumerd/lttng-consumerd, /usr/bin/lttng-consumerd)
	@$(call install_copy, lttng-tools, 0, 0, 0755, $(LTTNG_TOOLS_DIR)/src/bin/lttng-relayd/lttng-relayd, /usr/bin/lttng-relayd)
	@$(call install_copy, lttng-tools, 0, 0, 0755, $(LTTNG_TOOLS_DIR)/src/bin/lttng-sessiond/lttng-sessiond, /usr/bin/lttng-sessiond)

	@$(call install_finish, lttng-tools)

	@$(call touch)

# vim: syntax=make
