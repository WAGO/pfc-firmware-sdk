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
PACKAGES-$(PTXCONF_LTTNG_UST) += lttng-ust

#
# Paths and names
#
LTTNG_UST_VERSION	:= 2.6.0
LTTNG_UST_MD5		:= 74ad4826eac76c9dbf50fd3e46523191
LTTNG_UST		:= lttng-ust-$(LTTNG_UST_VERSION)
LTTNG_UST_SUFFIX	:= tar.bz2
#LTTNG_UST_URL		:= $(call ptx/mirror, SF, libcg/$(LTTNG_UST).$(LTTNG_UST_SUFFIX))
LTTNG_UST_SOURCE	:= $(SRCDIR)/$(LTTNG_UST).$(LTTNG_UST_SUFFIX)
LTTNG_UST_DIR		:= $(BUILDDIR)/$(LTTNG_UST)
LTTNG_UST_LICENSE	:= LGPLv2.1

#
# autoconf
#
LTTNG_UST_CONF_TOOL := autoconf
LTTNG_UST_CONF_OPT := \
	$(CROSS_AUTOCONF_USR) \


# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/lttn-ust.install:
	@$(call targetinfo)
#	cp $(LIBCGROUPCPULOAD_DIR)/cgroupcpuload.h $(PTXDIST_SYSROOT_TARGET)/usr/include

	cp -r $(LTTNG_UST_DIR)/include/lttng $(PTXDIST_SYSROOT_TARGET)/usr/include/

	cp $(LTTNG_UST_DIR)/liblttng-ust/.libs/liblttng-ust.so.0.0.0 $(PTXDIST_SYSROOT_TARGET)/usr/lib/liblttng-ust.so.$(LTTNG_UST_VERSION)
	ln -s $(PTXDIST_SYSROOT_TARGET)/usr/lib/lttng-ust.so.$(LTTNG_UST_VERSION) $(PTXDIST_SYSROOT_TARGET)/usr/lib/liblttng-ust.so.2
	ln -s $(PTXDIST_SYSROOT_TARGET)/usr/lib/lttng-ust.so.$(LTTNG_UST_VERSION) $(PTXDIST_SYSROOT_TARGET)/usr/lib/liblttng-ust.so

	cp $(LTTNG_UST_DIR)/liblttng-ust/.libs/liblttng-ust-tracepoint.so.0.0.0 $(PTXDIST_SYSROOT_TARGET)/usr/lib/liblttng-ust-tracepoint.so.$(LTTNG_UST_VERSION)
	ln -s $(PTXDIST_SYSROOT_TARGET)/usr/lib/lttng-ust-tracepoint.so.$(LTTNG_UST_VERSION) $(PTXDIST_SYSROOT_TARGET)/usr/lib/liblttng-ust-tracepoint.so.2
	ln -s $(PTXDIST_SYSROOT_TARGET)/usr/lib/lttng-ust-tracepoint.so.$(LTTNG_UST_VERSION) $(PTXDIST_SYSROOT_TARGET)/usr/lib/liblttng-ust-tracepoint.so
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/lttng-ust.targetinstall:
	@$(call targetinfo)

	@$(call install_init, lttng-ust)
	@$(call install_fixup, lttng-ust,PRIORITY,optional)
	@$(call install_fixup, lttng-ust,SECTION,base)
	@$(call install_fixup, lttng-ust,AUTHOR,"jso@wago.com <JSo>")
	@$(call install_fixup, lttng-ust,DESCRIPTION,missing)

	@$(call install_copy, lttng-ust, 0, 0, 0755, $(LTTNG_UST_DIR)/liblttng-ust/.libs/liblttng-ust.so.0.0.0, /usr/lib/liblttng-ust.so.$(LTTNG_UST_VERSION))
	@$(call install_link, lttng-ust, liblttng-ust.so.$(LTTNG_UST_VERSION), /usr/lib/liblttng-ust.so.2)
	@$(call install_link, lttng-ust, liblttng-ust.so.$(LTTNG_UST_VERSION), /usr/lib/liblttng-ust.so)

	@$(call install_copy, lttng-ust, 0, 0, 0755, $(LTTNG_UST_DIR)/liblttng-ust/.libs/liblttng-ust-tracepoint.so.0.0.0, /usr/lib/liblttng-ust-tracepoint.so.$(LTTNG_UST_VERSION))
	@$(call install_link, lttng-ust, liblttng-ust-tracepoint.so.$(LTTNG_UST_VERSION), /usr/lib/liblttng-ust-tracepoint.so.2)
	@$(call install_link, lttng-ust, liblttng-ust-tracepoint.so.$(LTTNG_UST_VERSION), /usr/lib/liblttng-ust-tracepoint.so)

	@$(call install_copy, lttng-ust, 0, 0, 0755, $(LTTNG_UST_DIR)/liblttng-ust-libc-wrapper/.libs/liblttng-ust-libc-wrapper.so.0.0.0, /usr/lib/liblttng-ust-libc-wrapper.so.$(LTTNG_UST_VERSION))
	@$(call install_link, lttng-ust, liblttng-ust-libc-wrapper.so.$(LTTNG_UST_VERSION), /usr/lib/liblttng-ust-libc-wrapper.so.2)
	@$(call install_link, lttng-ust, liblttng-ust-libc-wrapper.so.$(LTTNG_UST_VERSION), /usr/lib/liblttng-ust-libc-wrapper.so)

	@$(call install_copy, lttng-ust, 0, 0, 0755, $(LTTNG_UST_DIR)/liblttng-ust-libc-wrapper/.libs/liblttng-ust-pthread-wrapper.so.0.0.0, /usr/lib/liblttng-ust-pthread-wrapper.so.$(LTTNG_UST_VERSION))
	@$(call install_link, lttng-ust, liblttng-ust-pthread-wrapper.so.$(LTTNG_UST_VERSION), /usr/lib/liblttng-ust-pthread-wrapper.so.2)
	@$(call install_link, lttng-ust, liblttng-ust-pthread-wrapper.so.$(LTTNG_UST_VERSION), /usr/lib/liblttng-ust-pthread-wrapper.so)

	@$(call install_copy, lttng-ust, 0, 0, 0755, $(LTTNG_UST_DIR)/liblttng-ust-ctl/.libs/liblttng-ust-ctl.so.2.0.0, /usr/lib/liblttng-ust-ctl.so.2.0.0)
	@$(call install_link, lttng-ust, liblttng-ust-ctl.so.$(LTTNG_UST_VERSION), /usr/lib/liblttng-ust-ctl.so.2)
	@$(call install_link, lttng-ust, liblttng-ust-ctl.so.$(LTTNG_UST_VERSION), /usr/lib/liblttng-ust-ctl.so)

	@$(call install_copy, lttng-ust, 0, 0, 0755, $(LTTNG_UST_DIR)/liblttng-ust-dl/.libs/liblttng-ust-dl.so.0.0.0, /usr/lib/liblttng-ust-dl.so.$(LTTNG_UST_VERSION))
	@$(call install_link, lttng-ust, liblttng-ust-dl.so.$(LTTNG_UST_VERSION), /usr/lib/liblttng-ust-dl.so.2)
	@$(call install_link, lttng-ust, liblttng-ust-dl.so.$(LTTNG_UST_VERSION), /usr/lib/liblttng-ust-dl.so)

	@$(call install_finish, lttng-ust)

	@$(call touch)


$(STATEDIR)/lttng-ust.clean:
	@$(call targetinfo)
#	@-rm -rf $(PTXDIST_SYSROOT_TARGET)/usr/include/lttng
	@-rm -f $(PTXDIST_SYSROOT_TARGET)/usr/lib/lttng-ust*
	@$(call clean_pkg, LTTNG_UST)

# vim: syntax=make
