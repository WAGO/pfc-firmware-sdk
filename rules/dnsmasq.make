# -*-makefile-*-
#
# Copyright (C) 2003 by Benedikt Spranger
#               2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_DNSMASQ) += dnsmasq

#
# Paths and names
#
DNSMASQ_VERSION		:= 2.83
DNSMASQ_MD5		:= 512119b86bc5243d80db843e6b78199b
DNSMASQ			:= dnsmasq-$(DNSMASQ_VERSION)
DNSMASQ_SUFFIX		:= tar.xz
DNSMASQ_URL		:= http://www.thekelleys.org.uk/dnsmasq/$(DNSMASQ).$(DNSMASQ_SUFFIX)
DNSMASQ_SOURCE		:= $(SRCDIR)/$(DNSMASQ).$(DNSMASQ_SUFFIX)
DNSMASQ_DIR		:= $(BUILDDIR)/$(DNSMASQ)
DNSMASQ_LICENSE	:= GPL-2.0-only OR GPL-3.0-only
DNSMASQ_LICENSE_FILES	:= \
	file://COPYING;md5=0636e73ff0215e8d672dc4c32c317bb3 \
	file://COPYING-v3;md5=d32239bcb673463ab874e80d47fae504 \
	file://src/dnsmasq.c;startline=2;endline=15;md5=0da70ade5c9cf3c1edf1f8dc6ab8aa9f


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

DNSMASQ_CONF_TOOL	:= NO

DNSMASQ_COPT	:=
DNSMASQ_COPT	+= $(call ptx/ifdef,PTXCONF_DNSMASQ_DBUS,-DHAVE_DBUS,)
DNSMASQ_COPT	+= $(call ptx/ifdef,PTXCONF_DNSMASQ_TFTP,,-DNO_TFTP)
DNSMASQ_COPT	+= $(call ptx/ifdef,PTXCONF_GLOBAL_IPV6,,-DNO_IPV6)
DNSMASQ_COPT	+= $(call ptx/ifdef,PTXCONF_DNSMASQ_DNSSEC,-DHAVE_DNSSEC,)
DNSMASQ_COPT	+= $(call ptx/ifdef,PTXCONF_DNSMASQ_SCRIPT,,-DNO_SCRIPT)
DNSMASQ_COPT	+= $(call ptx/ifdef,PTXCONF_DNSMASQ_SCRIPT_LUA,-DHAVE_LUASCRIPT,)
DNSMASQ_COPT	+= $(call ptx/ifdef,PTXCONF_DNSMASQ_DHCP,,-DNO_DHCP)

DNSMASQ_MAKE_OPT	:= \
	$(CROSS_ENV_CC) \
	PREFIX=/usr \
	AWK=awk \
	COPTS='$(DNSMASQ_COPT)' \
	"CFLAGS+=-Wall -Wextra -ggdb3 -O2"

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

DNSMASQ_INSTALL_OPT	:= \
	$(DNSMASQ_MAKE_OPT) \
	install

$(STATEDIR)/dnsmasq.install:
	@$(call targetinfo)
	@$(call world/install, DNSMASQ)
	@install -vD -m 644 "$(DNSMASQ_DIR)/dnsmasq.conf.example" \
		"$(DNSMASQ_PKGDIR)/etc/dnsmasq.conf"
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/dnsmasq.targetinstall:
	@$(call targetinfo)

	@$(call install_init, dnsmasq)
	@$(call install_fixup, dnsmasq,PRIORITY,optional)
	@$(call install_fixup, dnsmasq,SECTION,base)
	@$(call install_fixup, dnsmasq,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, dnsmasq,DESCRIPTION,"Low Requirements DNS/DHCP daemon")

	@$(call install_copy, dnsmasq, 0, 0, 0755, -, /usr/sbin/dnsmasq)

ifdef PTXCONF_DNSMASQ_INETD
	@$(call install_alternative, dnsmasq, 0, 0, 0644, /etc/inetd.conf.d/dnsmasq)
endif

ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_DNSMASQ_STARTSCRIPT
	@$(call install_alternative, dnsmasq, 0, 0, 0755, /etc/init.d/dnsmasq)

ifneq ($(call remove_quotes,$(PTXCONF_DNSMASQ_BBINIT_LINK)),)
	@$(call install_link, dnsmasq, \
		../init.d/dnsmasq, \
		/etc/rc.d/$(PTXCONF_DNSMASQ_BBINIT_LINK))
endif
endif
endif
ifdef PTXCONF_DNSMASQ_SYSTEMD_UNIT
	@$(call install_alternative, dnsmasq, 0, 0, 0644, \
		/usr/lib/systemd/system/dnsmasq.service)
	@$(call install_link, dnsmasq, ../dnsmasq.service, \
		/usr/lib/systemd/system/network.target.wants/dnsmasq.service)
endif

	@$(call install_alternative, dnsmasq, 0, 0, 0644, /etc/dnsmasq.conf)

ifdef PTXCONF_DNSMASQ_DHCP
#	# for the 'dnsmasq.leases' file
	@$(call install_copy, dnsmasq, 0, 0, 0755, /var/lib/misc)
endif
	@$(call install_finish, dnsmasq)

	@$(call touch)

# vim: syntax=make
