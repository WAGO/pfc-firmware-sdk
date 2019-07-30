# -*-makefile-*-
#
# Copyright (C) 2003 by Benedikt Spranger
#               2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
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
DNSMASQ_VERSION		:= 2.80
DNSMASQ_MD5		:= e040e72e6f377a784493c36f9e788502
DNSMASQ			:= dnsmasq-$(DNSMASQ_VERSION)
DNSMASQ_SUFFIX		:= tar.xz
DNSMASQ_URL		:= http://www.thekelleys.org.uk/dnsmasq/$(DNSMASQ).$(DNSMASQ_SUFFIX)
DNSMASQ_SOURCE		:= $(SRCDIR)/$(DNSMASQ).$(DNSMASQ_SUFFIX)
DNSMASQ_DIR		:= $(BUILDDIR)/$(DNSMASQ)


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

DNSMASQ_PATH := PATH=$(CROSS_PATH)
DNSMASQ_MAKE_ENV := $(CROSS_ENV)

DNSMASQ_COPT :=

ifndef PTXCONF_DNSMASQ_TFTP
DNSMASQ_COPT += -DNO_TFTP
endif

ifndef PTXCONF_GLOBAL_IPV6
DNSMASQ_COPT += -DNO_IPV6
endif

ifndef PTXCONF_DNSMASQ_DHCP
DNSMASQ_COPT += -DNO_DHCP
else
ifndef PTXCONF_DNSMASQ_SCRIPT
DNSMASQ_COPT += -DNO_SCRIPT
else
ifdef PTXCONF_DNSMASQ_SCRIPT_LUA
DNSMASQ_COPT += -DHAVE_LUASCRIPT
endif
endif
endif

ifdef DNSMASQ_DNSSEC
DNSMASQ_COPT += -DHAVE_DNSSEC
endif

DNSMASQ_MAKEVARS := PREFIX=/usr AWK=awk COPTS='$(DNSMASQ_COPT)'

$(STATEDIR)/dnsmasq.prepare:
	@$(call targetinfo)
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
	@$(call install_fixup, dnsmasq,DESCRIPTION,missing)

	@$(call install_copy, dnsmasq, 0, 0, 0755, -, /usr/sbin/dnsmasq)

ifdef PTXCONF_DNSMASQ_INETD
	@$(call install_alternative, dnsmasq, 0, 0, 0644, /etc/inetd.conf.d/dnsmasq, n)
endif

ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_DNSMASQ_STARTSCRIPT
	@$(call install_alternative, dnsmasq, 0, 0, 0755, /etc/init.d/dnsmasq, n)

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

	@$(call install_alternative, dnsmasq, 0, 0, 0644, /etc/dnsmasq.conf, n)

# for the 'dnsmasq.leases' file
	@$(call install_copy, dnsmasq, 0, 0, 0755, /var/lib/misc)

	@$(call install_finish, dnsmasq)

	@$(call touch)

# vim: syntax=make
