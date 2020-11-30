# -*-makefile-*-
#
# Copyright (C) 2003 by Marc Kleine-Budde <kleine-budde@gmx.de> for
#                       GYRO net GmbH <info@gyro-net.de>, Hannover, Germany
#               2008, 2009 by Juergen Beisert <juergen@kreuzholzen.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PPP) += ppp

#
# Paths and names
#
PPP_VERSION	:= 2.4.8
PPP_MD5		:= fa325e90e43975a1bd7e1012c8676123
PPP		:= ppp-$(PPP_VERSION)
PPP_SUFFIX	:= tar.gz
PPP_URL		:= https://github.com/paulusmack/ppp/archive/$(PPP).$(PPP_SUFFIX)
PPP_SOURCE	:= $(SRCDIR)/$(PPP).$(PPP_SUFFIX)
PPP_DIR		:= $(BUILDDIR)/$(PPP)
PPP_LICENSE	:= BSD,GPL-2.0

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

PPP_KERNEL_VERSION := $(if $(KERNEL_HEADER_VERSION),$(KERNEL_HEADER_VERSION),$(KERNEL_VERSION))

ifdef PTXCONF_PPP
ifeq ($(PPP_KERNEL_VERSION),)
 $(warning ######################### ERROR ###########################)
 $(warning # Linux kernel version required in order to make ppp work #)
 $(warning #   Define a platform kernel or the kernel headers        #)
 $(warning ###########################################################)
 $(error )
endif
endif

PPP_CONF_ENV	:= \
	TARGET_OS=Linux \
	TARGET_OS_VER=$(PPP_KERNEL_VERSION) \
	TARGET_OS_ARCH=$(GENERIC_KERNEL_ARCH)

PPP_MAKE_ENV	:= $(CROSS_ENV)
PPP_MAKE_PAR	:= NO

#
# path to where the shared library based plugins get installed
# (and be searched at runtime)
#
PPP_SHARED_INST_PATH := /usr/lib/pppd/$(PPP_VERSION)

$(STATEDIR)/ppp.prepare:
	@$(call targetinfo)
	@cd $(PPP_DIR) && $(PPP_PATH) $(PPP_CONF_ENV) \
		./configure --prefix=/usr --sysconfdir=/etc

	@$(call disable_sh,$(PPP_DIR)/pppd/Makefile,USE_PAM=y)

ifdef PTXCONF_GLOBAL_IPV6
	@$(call enable_sh,$(PPP_DIR)/pppd/Makefile,HAVE_INET6=y)
else
	@$(call disable_sh,$(PPP_DIR)/pppd/Makefile,HAVE_INET6=y)
endif

ifndef PTXCONF_PPP_IPX
	@sed -i \
		-e 's/-DIPX_CHANGE//' \
		-e 's/ipxcp.o//' \
		$(PPP_DIR)/pppd/Makefile
endif

ifdef PTXCONF_PPP_FILTER
	@$(call enable_sh,$(PPP_DIR)/pppd/Makefile,FILTER=y)
else
	@$(call disable_sh,$(PPP_DIR)/pppd/Makefile,FILTER=y)
endif

ifdef PTXCONF_PPP_SRP
	@$(call enable_sh,$(PPP_DIR)/pppd/Makefile,USE_SRP=y)
else
	@$(call disable_sh,$(PPP_DIR)/pppd/Makefile,USE_SRP=y)
endif

ifndef PTXCONF_PPP_MULTILINK
	@$(call disable_sh,$(PPP_DIR)/pppd/Makefile,HAVE_MULTILINK=y)
	@sed -i \
		-e 's/-DHAVE_MULTILINK//' \
		-e 's/multilink.o//' \
		$(PPP_DIR)/pppd/Makefile
endif

ifdef PTXCONF_PPP_SHADOW
	@$(call enable_sh,$(PPP_DIR)/pppd/Makefile,HAS_SHADOW=y)
else
	@$(call disable_sh,$(PPP_DIR)/pppd/Makefile,HAS_SHADOW=y)
endif

ifdef PTXCONF_PPP_MS_CHAP
	@$(call enable_sh,$(PPP_DIR)/pppd/Makefile,CHAPMS=y)
else
	@$(call disable_sh,$(PPP_DIR)/pppd/Makefile,CHAPMS=y)
endif

ifdef PTXCONF_PPP_MPPE
	@$(call enable_sh,$(PPP_DIR)/pppd/Makefile,MPPE=y)
else
	@$(call disable_sh,$(PPP_DIR)/pppd/Makefile,MPPE=y)
endif

ifdef PTXCONF_PPP_MS_CBCP
	@$(call enable_sh,$(PPP_DIR)/pppd/Makefile,CBCP=y)
else
	@$(call disable_sh,$(PPP_DIR)/pppd/Makefile,CBCP=y)
endif

ifdef PTXCONF_PPP_NEEDS_CRYPT
	@$(call disable_sh,$(PPP_DIR)/pppd/Makefile,USE_CRYPT=y)
endif

ifdef PTXCONF_PPP_TDB
	@$(call enable_sh,$(PPP_DIR)/pppd/Makefile,USE_TDB=y)
else
	@$(call disable_sh,$(PPP_DIR)/pppd/Makefile,USE_TDB=y)
endif

ifdef PTXCONF_PPP_PLUGINS
	@$(call enable_sh,$(PPP_DIR)/pppd/Makefile,PLUGIN=y)
else
	@$(call disable_sh,$(PPP_DIR)/pppd/Makefile,PLUGIN=y)
endif
	@$(call touch)


PPP_INSTALL_OPT := \
	INSTROOT=$(PPP_PKGDIR) \
	DESTDIR=$(PPP_PKGDIR)/usr \
	install

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ppp.targetinstall:
	@$(call targetinfo)

	@$(call install_init, ppp)
	@$(call install_fixup, ppp,PRIORITY,optional)
	@$(call install_fixup, ppp,SECTION,base)
	@$(call install_fixup, ppp,AUTHOR,"Robert Schwebel (r.schwebel@pengutronix.de)")
	@$(call install_fixup, ppp,DESCRIPTION,missing)

ifdef PTXCONF_PPP_SUID
	@$(call install_copy, ppp, 0, 0, 4755, -, /usr/sbin/pppd)
else
	@$(call install_copy, ppp, 0, 0, 0755, -, /usr/sbin/pppd)
endif

ifdef PTXCONF_PPP_INST_CHAT
	@$(call install_copy, ppp, 0, 0, 0755, -, /usr/sbin/chat)
endif

ifdef PTXCONF_PPP_INST_DEFAULT_CONFIG_FILES
	@$(call install_alternative, ppp, 0, 0, 0600, /etc/ppp/options)
	@$(call install_alternative, ppp, 0, 0, 0750, /etc/ppp/ip-up)
	@$(call install_alternative, ppp, 0, 0, 0750, /etc/ppp/ip-down)
	@$(call install_alternative, ppp, 0, 0, 0600, /etc/ppp/pap-secrets)

	@$(call install_alternative, ppp, 0, 0, 0600, /etc/ppp/peers/provider)
	@$(call install_alternative, ppp, 0, 0, 0600, /etc/chatscripts/provider)
	@$(call install_alternative, ppp, 0, 0, 0600, /etc/chatscripts/disconnect)
endif

#	#
#	# busybox init
#	#
ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_PPP_STARTSCRIPT
	@$(call install_alternative, ppp, 0, 0, 0755, /etc/init.d/pppd)
	@$(call install_replace, ppp, /etc/init.d/pppd, \
		@PPPD_INTF@, $(PTXCONF_PPPD_INTF))

ifneq ($(call remove_quotes,$(PTXCONF_PPPD_BBINIT_LINK)),)
	@$(call install_link, ppp, \
		../init.d/pppd, \
		/etc/rc.d/$(PTXCONF_PPPD_BBINIT_LINK))
endif
endif
endif

ifdef PTXCONF_PPP_INST_PPPDUMP
	@$(call install_copy, ppp, 0, 0, 0755, -, /usr/sbin/pppdump)
endif

ifdef PTXCONF_PPP_INST_PPPSTATS
	@$(call install_copy, ppp, 0, 0, 0755, -, /usr/sbin/pppstats)
endif

ifdef PTXCONF_PPP_INST_PONOFF
	@$(call install_alternative, ppp, 0, 0, 0755, /usr/bin/pon)
	@$(call install_alternative, ppp, 0, 0, 0755, /usr/bin/poff)
endif

ifdef PTXCONF_PPP_PLUGINS
	@$(call install_copy, ppp, 0, 0, 0755, $(PPP_SHARED_INST_PATH))
endif

ifdef PTXCONF_PPP_OATM
	@$(call install_copy, ppp, 0, 0, 0644, -, \
		$(PPP_SHARED_INST_PATH)/pppoatm.so)
endif
ifdef PTXCONF_PPP_RADIUS
	@$(call install_copy, ppp, 0, 0, 0644, -, \
		$(PPP_SHARED_INST_PATH)/radius.so)
endif
ifdef PTXCONF_PPP_RADATTR
	@$(call install_copy, ppp, 0, 0, 0644, -, \
		$(PPP_SHARED_INST_PATH)/radattr.so)
endif
ifdef PTXCONF_PPP_RADREALMS
	@$(call install_copy, ppp, 0, 0, 0644, -, \
		$(PPP_SHARED_INST_PATH)/radrealms.so)
endif
ifdef PTXCONF_PPP_OE
	@$(call install_copy, ppp, 0, 0, 0644, -, \
		$(PPP_SHARED_INST_PATH)/rp-pppoe.so)
endif
ifdef PTXCONF_PPP_PLUGIN_RP_PPPOE_DISCOVERY
	@$(call install_copy, ppp, 0, 0, 0755, -, \
		$(PPP_SHARED_INST_PATH)/pppoe-discovery)
endif
ifdef PTXCONF_PPP_MINCONN
	@$(call install_copy, ppp, 0, 0, 0644, -, \
		$(PPP_SHARED_INST_PATH)/minconn.so)
endif
ifdef PTXCONF_PPP_PASSPROMPT
	@$(call install_copy, ppp, 0, 0, 0644, -, \
		$(PPP_SHARED_INST_PATH)/passprompt.so)
endif
ifdef PTXCONF_PPP_PASSWORDFD
	@$(call install_copy, ppp, 0, 0, 0644, -, \
		$(PPP_SHARED_INST_PATH)/passwordfd.so)
endif
ifdef PTXCONF_PPP_WINBIND
	@$(call install_copy, ppp, 0, 0, 0644, -, \
		$(PPP_SHARED_INST_PATH)/winbind.so)
endif
	@$(call install_finish, ppp)
	@$(call touch)

# vim: syntax=make
