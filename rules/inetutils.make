# -*-makefile-*-
#
# Copyright (C) 2003 by Ixia Corporation (www.ixiacom.com)
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
PACKAGES-$(PTXCONF_INETUTILS) += inetutils

#
# Paths and names
#
INETUTILS_VERSION	:= 1.9.4
INETUTILS_MD5		:= 04852c26c47cc8c6b825f2b74f191f52
INETUTILS		:= inetutils-$(INETUTILS_VERSION)
INETUTILS_SUFFIX	:= tar.gz
INETUTILS_URL		:= $(call ptx/mirror, GNU, inetutils/$(INETUTILS).$(INETUTILS_SUFFIX))
INETUTILS_SOURCE	:= $(SRCDIR)/$(INETUTILS).$(INETUTILS_SUFFIX)
INETUTILS_DIR		:= $(BUILDDIR)/$(INETUTILS)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

INETUTILS_CONF_ENV	:= \
	$(CROSS_ENV) \
	inetutils_cv_path_cp=/bin/cp \
	inetutils_cv_path_login=/bin/login \
	inetutils_cv_path_procnet_dev=/proc/net/dev

#
# autoconf
#
INETUTILS_CONF_OPT := \
	$(CROSS_AUTOCONF_USR) \
	--sysconfdir=/etc \
	--libexecdir=/usr/sbin \
	--disable-libls \
	--disable-ftpd \
	--$(call ptx/endis, PTXCONF_INETUTILS_INETD)-inetd \
	--disable-rexecd \
	--$(call ptx/endis, PTXCONF_INETUTILS_RLOGIND)-rlogind \
	--$(call ptx/endis, PTXCONF_INETUTILS_RSHD)-rshd \
	--$(call ptx/endis, PTXCONF_INETUTILS_SYSLOGD)-syslogd \
	--disable-talkd \
	--disable-telnetd \
	--$(call ptx/endis, PTXCONF_INETUTILS_TFTPD)-tftpd \
	--disable-uucpd \
	--disable-ftp \
	--disable-dnsdomainname \
	--disable-hostname \
	--$(call ptx/endis, PTXCONF_INETUTILS_PING)-ping \
	--$(call ptx/endis, PTXCONF_INETUTILS_PING6)-ping6 \
	--$(call ptx/endis, PTXCONF_INETUTILS_RCP)-rcp \
	--disable-rexec \
	--disable-rlogin \
	--$(call ptx/endis, PTXCONF_INETUTILS_RSH)-rsh \
	--disable-logger \
	--disable-talk \
	--disable-telnet \
	--disable-tftp \
	--disable-whois \
	--disable-ifconfig \
	--disable-traceroute \
	$(GLOBAL_LARGE_FILE_OPTION) \
	$(GLOBAL_IPV6_OPTION) \
	--disable-rpath \
	--disable-readline \
	--without-wrap \
	--without-pam


## '--disable-*' is broken for:
# --disable-encryption
# --disable-authentication

#
# FIXME: Unhandled options:
# --disable-ncurses

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/inetutils.targetinstall:
	@$(call targetinfo)

	@$(call install_init, inetutils)
	@$(call install_fixup, inetutils,PRIORITY,optional)
	@$(call install_fixup, inetutils,SECTION,base)
	@$(call install_fixup, inetutils,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, inetutils,DESCRIPTION,missing)

ifdef PTXCONF_INETUTILS_INETD
	@$(call install_copy, inetutils, 0, 0, 0755, -, /usr/sbin/inetd)
endif
ifdef PTXCONF_INETUTILS_PING
	@$(call install_copy, inetutils, 0, 0, 4755, -, /usr/bin/ping)
endif
ifdef PTXCONF_INETUTILS_PING6
	@$(call install_copy, inetutils, 0, 0, 4755, -, /usr/bin/ping6)
endif
ifdef PTXCONF_INETUTILS_RCP
	@$(call install_copy, inetutils, 0, 0, 0755, -, /usr/bin/rcp)
endif
ifdef PTXCONF_INETUTILS_RLOGIND
	@$(call install_copy, inetutils, 0, 0, 0755, -, /usr/sbin/rlogind)
endif
ifdef PTXCONF_INETUTILS_RSH
	@$(call install_copy, inetutils, 0, 0, 0755, -, /usr/bin/rsh)
endif
ifdef PTXCONF_INETUTILS_RSHD
	@$(call install_copy, inetutils, 0, 0, 0755, -, /usr/sbin/rshd)
	@$(call install_alternative, inetutils, 0, 0, 0644, \
		/etc/inetd.conf.d/inetutils-rshd, n)
endif
ifdef PTXCONF_INETUTILS_SYSLOGD
	@$(call install_copy, inetutils, 0, 0, 0755, -, /usr/sbin/syslogd)
endif
ifdef PTXCONF_INETUTILS_SYSLOGD_STARTSCRIPT
	@$(call install_alternative, inetutils, 0, 0, 0755, \
		/etc/init.d/syslogd, n)

ifneq ($(call remove_quotes, $(PTXCONF_INETUTILS_SYSLOGD_BBINIT_LINK)),)
	@$(call install_link, inetutils, \
		../init.d/syslogd, \
		/etc/rc.d/$(PTXCONF_INETUTILS_SYSLOGD_BBINIT_LINK))
endif
endif
ifdef PTXCONF_INETUTILS_SYSLOGD_CONFIG
	@$(call install_alternative, inetutils, 0, 0, 0644, \
		/etc/syslog.conf, n)
endif
ifdef PTXCONF_INETUTILS_TFTPD
	@$(call install_copy, inetutils, 0, 0, 0755, -, /usr/sbin/tftpd)
ifneq ($(PTXCONF_INETUTILS_TFTPD_BASE_DIR),"")
#	# create the base dir
	@$(call install_copy, inetutils, 0, 0, 0755, \
		$(PTXCONF_INETUTILS_TFTPD_BASE_DIR) )
endif
	@$(call install_alternative, inetutils, 0, 0, 0644, /etc/inetd.conf.d/tftp, n)
	@$(call install_replace, inetutils, \
		/etc/inetd.conf.d/tftp, \
		@ROOT@, \
		$(PTXCONF_INETUTILS_TFTPD_BASE_DIR) )
endif

#	#
#	# busybox init: start script
#	#

ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_INETUTILS_INETD_STARTSCRIPT
	@$(call install_alternative, inetutils, 0, 0, 0755, /etc/init.d/inetd, n)

ifneq ($(call remove_quotes, $(PTXCONF_INETUTILS_INETD_BBINIT_LINK)),)
	@$(call install_link, inetutils, \
		../init.d/inetd, \
		/etc/rc.d/$(PTXCONF_INETUTILS_INETD_BBINIT_LINK))
endif
endif
endif

	@$(call install_finish, inetutils)

	@$(call touch)

# vim: syntax=make
