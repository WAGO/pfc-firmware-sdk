# -*-makefile-*-
#
# Copyright (C) 2006 by Erwin Rol
# Copyright (C) 2009 by Robert Schwebel/Juergen Beisert
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_IPROUTE2) += iproute2

#
# Paths and names
#
IPROUTE2_VERSION	:= 4.20.0
IPROUTE2_MD5		:= f3dab4c812812bbb5873cb90f471bcbf
IPROUTE2		:= iproute2-$(IPROUTE2_VERSION)
IPROUTE2_SUFFIX		:= tar.xz
IPROUTE2_URL		:= $(call ptx/mirror, KERNEL, utils/net/iproute2/$(IPROUTE2).$(IPROUTE2_SUFFIX))
IPROUTE2_SOURCE		:= $(SRCDIR)/$(IPROUTE2).$(IPROUTE2_SUFFIX)
IPROUTE2_DIR		:= $(BUILDDIR)/$(IPROUTE2)
IPROUTE2_LICENSE	:= GPL-2.0-only

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

# iproute2's configure is handcrafted and doesn't take standard configure options
IPROUTE2_CONF_OPT := ''

$(STATEDIR)/iproute2.prepare:
	@$(call targetinfo)
	@$(call world/prepare, IPROUTE2)
# overwrite options we don't want, or may be misdetected
	@echo 'TC_CONFIG_ATM:=n'	>> $(IPROUTE2_DIR)/config.mk
ifndef PTXCONF_IPROUTE2_ARPD
	@echo 'HAVE_BERKELEY_DB:=n'	>> $(IPROUTE2_DIR)/config.mk
endif
	@$(call touch)

IPROUTE2_MAKE_OPT := \
	SBINDIR=/usr/sbin \
	DBM_INCLUDE=$(SYSROOT)/usr/include \
	LDFLAGS='-rdynamic'

IPROUTE2_INSTALL_OPT := \
	$(IPROUTE2_MAKE_OPT) \
	install

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

IPROUTE2_INSTALL_FILES-y =
IPROUTE2_INSTALL_FILES-$(PTXCONF_IPROUTE2_ARPD) +=	arpd
IPROUTE2_INSTALL_FILES-$(PTXCONF_IPROUTE2_BRIDGE) +=	bridge
IPROUTE2_INSTALL_FILES-$(PTXCONF_IPROUTE2_CTSTAT) +=	ctstat
IPROUTE2_INSTALL_FILES-$(PTXCONF_IPROUTE2_GENL) +=	genl
IPROUTE2_INSTALL_FILES-$(PTXCONF_IPROUTE2_IP) +=	ip
IPROUTE2_INSTALL_FILES-$(PTXCONF_IPROUTE2_IFCFG) +=	ifcfg
IPROUTE2_INSTALL_FILES-$(PTXCONF_IPROUTE2_IFSTAT) +=	ifstat
IPROUTE2_INSTALL_FILES-$(PTXCONF_IPROUTE2_LNSTAT) +=	lnstat
IPROUTE2_INSTALL_FILES-$(PTXCONF_IPROUTE2_NSTAT) +=	nstat
IPROUTE2_INSTALL_FILES-$(PTXCONF_IPROUTE2_ROUTEF) +=	routef
IPROUTE2_INSTALL_FILES-$(PTXCONF_IPROUTE2_ROUTEL) +=	routel
IPROUTE2_INSTALL_FILES-$(PTXCONF_IPROUTE2_RTACCT) +=	rtacct
IPROUTE2_INSTALL_FILES-$(PTXCONF_IPROUTE2_RTMON) +=	rtmon
IPROUTE2_INSTALL_FILES-$(PTXCONF_IPROUTE2_RTPR) +=	rtpr
IPROUTE2_INSTALL_FILES-$(PTXCONF_IPROUTE2_RTSTAT) +=	rtstat
IPROUTE2_INSTALL_FILES-$(PTXCONF_IPROUTE2_SS) +=	ss
IPROUTE2_INSTALL_FILES-$(PTXCONF_IPROUTE2_TC) +=	tc


$(STATEDIR)/iproute2.targetinstall:
	@$(call targetinfo)

	@$(call install_init, iproute2)
	@$(call install_fixup, iproute2,PRIORITY,optional)
	@$(call install_fixup, iproute2,SECTION,base)
	@$(call install_fixup, iproute2,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, iproute2,DESCRIPTION,missing)

	@for i in $(IPROUTE2_INSTALL_FILES-y); do \
		$(call install_copy, iproute2, 0, 0, 0755, -, /usr/sbin/$$i) \
	done

ifdef PTXCONF_IPROUTE2_TC
	@$(call install_copy, iproute2, 0, 0, 0644, -, /usr/lib/tc/normal.dist)
	@$(call install_copy, iproute2, 0, 0, 0644, -, /usr/lib/tc/pareto.dist)
	@$(call install_copy, iproute2, 0, 0, 0644, -, /usr/lib/tc/paretonormal.dist)
	@$(call install_copy, iproute2, 0, 0, 0644, -, /usr/lib/tc/experimental.dist)
endif

	@$(call install_alternative, iproute2, 0, 0, 0644, /etc/iproute2/ematch_map)
	@$(call install_alternative, iproute2, 0, 0, 0644, /etc/iproute2/group)
	@$(call install_alternative, iproute2, 0, 0, 0644, /etc/iproute2/nl_protos)
	@$(call install_alternative, iproute2, 0, 0, 0644, /etc/iproute2/rt_dsfield)
	@$(call install_alternative, iproute2, 0, 0, 0644, /etc/iproute2/rt_protos)
	@$(call install_alternative, iproute2, 0, 0, 0644, /etc/iproute2/rt_realms)
	@$(call install_alternative, iproute2, 0, 0, 0644, /etc/iproute2/rt_scopes)
	@$(call install_alternative, iproute2, 0, 0, 0644, /etc/iproute2/rt_tables)

	@$(call install_finish, iproute2)

	@$(call touch)

# vim: syntax=make
