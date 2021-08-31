# -*-makefile-*-
#
# Copyright (C) 2012 by Alexander Aring <aar@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBNL3) += libnl3

#
# Paths and names
#
LIBNL3_VERSION	:= 3.5.0
LIBNL3_MD5	:= 74ba57b1b1d6f9f92268aa8141d8e8e4
LIBNL3		:= libnl-$(LIBNL3_VERSION)
LIBNL3_RELEASE	:= libnl$(subst .,_,$(LIBNL3_VERSION))
LIBNL3_SUFFIX	:= tar.gz
LIBNL3_URL	:= https://github.com/thom311/libnl/releases/download/$(LIBNL3_RELEASE)/$(LIBNL3).$(LIBNL3_SUFFIX)
LIBNL3_SOURCE	:= $(SRCDIR)/$(LIBNL3).$(LIBNL3_SUFFIX)
LIBNL3_DIR	:= $(BUILDDIR)/$(LIBNL3)
LIBNL3_LICENSE	:= LGPL-2.1-only AND BSD-3-Clause
LIBNL3_LICENSE_FILES := \
	file://COPYING;md5=4fbd65380cdd255951079008b364516c \
	file://lib/xfrm/ae.c;startline=3;endline=32;md5=6813917a92b539b07534e1a79f5a6aca

ifdef PTXCONF_LIBNL3_ENABLE_CLI
LIBNL3_LICENSE	+= AND GPL-2.0-only
endif

#
# autoconf
#
LIBNL3_CONF_TOOL    := autoconf
LIBNL3_CONF_OPT	    := \
	$(CROSS_AUTOCONF_USR) \
	--enable-shared \
	--disable-static \
	--$(call ptx/endis, PTXCONF_LIBNL3_ENABLE_CLI)-cli \
	--enable-pthreads \
	--disable-debug

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

LIBNL3_INSTALL_FILES-y =
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=genl-ctrl-list
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=idiag-socket-details
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nf-ct-add
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nf-ct-list
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nf-exp-add
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nf-exp-delete
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nf-exp-list
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nf-log
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nf-monitor
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nf-queue
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-addr-add
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-addr-delete
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-addr-list
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-class-add
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-class-delete
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-class-list
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-classid-lookup
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-cls-add
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-cls-delete
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-cls-list
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-fib-lookup
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-link-enslave
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-link-ifindex2name
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-link-list
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-link-name2ifindex
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-link-release
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-link-set
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-link-stats
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-list-caches
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-list-sockets
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-monitor
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-neigh-add
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-neigh-delete
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-neigh-list
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-neightbl-list
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-pktloc-lookup
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-qdisc-add
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-qdisc-delete
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-qdisc-list
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-route-add
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-route-delete
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-route-get
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-route-list
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-rule-list
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-tctree-list
LIBNL3_INSTALL_FILES-$(PTXCONF_LIBNL3_ENABLE_CLI)   +=nl-util-addr

$(STATEDIR)/libnl3.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libnl3)
	@$(call install_fixup, libnl3,PRIORITY,optional)
	@$(call install_fixup, libnl3,SECTION,base)
	@$(call install_fixup, libnl3,AUTHOR,"Alexander Aring <aar@pengutronix.de>")
	@$(call install_fixup, libnl3,DESCRIPTION,missing)

	@$(call install_lib, libnl3, 0, 0, 0644, libnl-3)
	@$(call install_lib, libnl3, 0, 0, 0644, libnl-genl-3)
	@$(call install_lib, libnl3, 0, 0, 0644, libnl-idiag-3)
	@$(call install_lib, libnl3, 0, 0, 0644, libnl-nf-3)
	@$(call install_lib, libnl3, 0, 0, 0644, libnl-route-3)
	@$(call install_lib, libnl3, 0, 0, 0644, libnl-xfrm-3)

ifdef PTXCONF_LIBNL3_ENABLE_CLI
	@$(call install_lib, libnl3, 0, 0, 0644, libnl-cli-3)
	@$(call install_lib, libnl3, 0, 0, 0644, libnl/cli/cls/basic)
	@$(call install_lib, libnl3, 0, 0, 0644, libnl/cli/cls/cgroup)
	@$(call install_lib, libnl3, 0, 0, 0644, libnl/cli/qdisc/bfifo)
	@$(call install_lib, libnl3, 0, 0, 0644, libnl/cli/qdisc/blackhole)
	@$(call install_lib, libnl3, 0, 0, 0644, libnl/cli/qdisc/fq_codel)
	@$(call install_lib, libnl3, 0, 0, 0644, libnl/cli/qdisc/hfsc)
	@$(call install_lib, libnl3, 0, 0, 0644, libnl/cli/qdisc/htb)
	@$(call install_lib, libnl3, 0, 0, 0644, libnl/cli/qdisc/ingress)
	@$(call install_lib, libnl3, 0, 0, 0644, libnl/cli/qdisc/pfifo)
	@$(call install_lib, libnl3, 0, 0, 0644, libnl/cli/qdisc/plug)
endif

	@$(foreach tool,$(LIBNL3_INSTALL_FILES-y), \
		$(call install_copy, libnl3, 0, 0, 0755, -, /usr/bin/$(tool));)

	@$(call install_alternative, libnl3, 0, 0, 0644, /etc/libnl/classid)
	@$(call install_alternative, libnl3, 0, 0, 0644, /etc/libnl/pktloc)

	@$(call install_finish, libnl3)

	@$(call touch)

# vim: syntax=make
