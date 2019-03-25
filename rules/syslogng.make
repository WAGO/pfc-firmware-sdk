# -*-makefile-*-
#
# Copyright (C) 2006 by Robert Schwebel
#               2008, 2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_SYSLOGNG) += syslogng

#
# Paths and names
#
SYSLOGNG_VERSION	:= 3.3.4
SYSLOGNG_MD5		:= 8e1bc6bb55a6545dcf0329b982c82a44
SYSLOGNG		:= syslog-ng-$(SYSLOGNG_VERSION)
SYSLOGNG_SUFFIX		:= tar.gz
SYSLOGNG_URL		:= http://www.balabit.com/downloads/files/syslog-ng/sources/3.3/src/$(SYSLOGNG).$(SYSLOGNG_SUFFIX)
SYSLOGNG_SOURCE		:= $(SRCDIR)/$(SYSLOGNG).$(SYSLOGNG_SUFFIX)
SYSLOGNG_DIR		:= $(BUILDDIR)/$(SYSLOGNG)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

SYSLOGNG_PATH	:= PATH=$(CROSS_PATH)
SYSLOGNG_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
SYSLOGNG_AUTOCONF := \
	$(CROSS_AUTOCONF_ROOT) \
	$(GLOBAL_IPV6_OPTION) \
	--enable-dynamic-linking \
	--disable-debug \
	--disable-sun-streams \
	--with-pidfile-dir=/var/run/ \
	--disable-memtrace \
	--disable-largefile \
	--without-compile-date \
	--with-default-modules=$(PTXCONF_SYSLOGNG_DEFAULT_MODULES) \
	--libexecdir=/usr/libexec/ \
	--without-libmongo-client \


ifdef PTXCONF_SYSLOGNG_LINUX_CAPS
SYSLOGNG_AUTOCONF += --enable-linux-caps
else
SYSLOGNG_AUTOCONF += --disable-linux-caps
endif

ifdef PTXCONF_SYSLOGNG_ENV_WRAPPER
SYSLOGNG_AUTOCONF += --enable-env-wrapper
else
SYSLOGNG_AUTOCONF += --disable-env-wrapper
endif

ifdef PTXCONF_SYSLOGNG_SYSTEMD
SYSLOGNG_AUTOCONF += --enable-systemd
else
SYSLOGNG_AUTOCONF += --disable-systemd
endif

ifdef PTXCONF_SYSLOGNG_PCRE
SYSLOGNG_AUTOCONF += --enable-pcre
else
SYSLOGNG_AUTOCONF += --disable-pcre
endif

ifdef PTXCONF_SYSLOGNG_PACCT
SYSLOGNG_AUTOCONF += --enable-pacct
else
SYSLOGNG_AUTOCONF += --disable-pacct
endif

ifdef PTXCONF_SYSLOGNG_MONGODB
SYSLOGNG_AUTOCONF += --enable-mongodb
else
SYSLOGNG_AUTOCONF += --disable-mongodb
endif

ifdef PTXCONF_SYSLOGNG_SQL
SYSLOGNG_AUTOCONF += --enable-sql
else
SYSLOGNG_AUTOCONF += --disable-sql
endif

ifdef PTXCONF_SYSLOGNG_SSL
SYSLOGNG_AUTOCONF += --enable-ssl
else
SYSLOGNG_AUTOCONF += --disable-ssl
endif

ifdef PTXCONF_SYSLOGNG_JSON
SYSLOGNG_AUTOCONF += --enable-json
else
SYSLOGNG_AUTOCONF += --disable-json
endif

ifdef PTXCONF_SYSLOGNG_TCP_WRAPPER
SYSLOGNG_AUTOCONF += --enable-tcp-wrapper
else
SYSLOGNG_AUTOCONF += --disable-tcp-wrapper
endif

ifdef PTXCONF_SYSLOGNG_SPOOF_SOURCE
SYSLOGNG_AUTOCONF += \
	--enable-spoof-source \
	--with-libnet=$(SYSROOT)/usr/bin
else
SYSLOGNG_AUTOCONF += --disable-spoof-source
endif

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/syslogng.targetinstall:
	@$(call targetinfo)

	@$(call install_init, syslogng)
	@$(call install_fixup, syslogng,PRIORITY,optional)
	@$(call install_fixup, syslogng,SECTION,base)
	@$(call install_fixup, syslogng,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, syslogng,DESCRIPTION,missing)

#	# binary
	@$(call install_copy, syslogng, 0, 0, 0755, -, \
		/sbin/syslog-ng)
	@$(call install_copy, syslogng, 0, 0, 0755, -, \
		/usr/libexec/syslog-ng)
	@$(call install_copy, syslogng, 0, 0, 0755, -, \
		/lib/libsyslog-ng-3.3.4.so)
	@$(call install_copy, syslogng, 0, 0, 0755, -, \
		/lib/syslog-ng/libaffile.so)
	@$(call install_copy, syslogng, 0, 0, 0755, -, \
		/lib/syslog-ng/libafsocket.so)
	@$(call install_copy, syslogng, 0, 0, 0755, -, \
		/lib/syslog-ng/libafsocket-notls.so)
	@$(call install_copy, syslogng, 0, 0, 0755, -, \
		/lib/syslog-ng/libcsvparser.so)
	@$(call install_copy, syslogng, 0, 0, 0755, -, \
		/lib/syslog-ng/libsyslogformat.so)
	@$(call install_copy, syslogng, 0, 0, 0755, -, \
		/lib/syslog-ng/libafuser.so)

#	# config
ifdef PTXCONF_SYSLOGNG_CONFIG
	@$(call install_alternative, syslogng, 0, 0, 0640, /etc/syslog-ng.conf, n)
	@$(call install_alternative, syslogng, 0, 0, 0640, /etc/syslog_wago_plc.conf, n)
endif

#	# bb init: start scripts
ifdef PTXCONF_INITMETHOD_INITRAMFS
ifdef PTXCONF_SYSLOGNG_STARTSCRIPT
	@$(call install_alternative, syslogng, 0, 0, 0755, /etc/init.d/syslog-ng, n)
endif
endif

ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_SYSLOGNG_STARTSCRIPT
	@$(call install_alternative, syslogng, 0, 0, 0755, /etc/init.d/syslog-ng, n)

ifneq ($(call remove_quotes,$(PTXCONF_SYSLOGNG_BBINIT_LINK)),)
	@$(call install_link, syslogng, \
		../init.d/syslog-ng, \
		/etc/rc.d/$(PTXCONF_SYSLOGNG_BBINIT_LINK))
endif
endif
endif
#ifdef PTXCONF_INITMETHOD_SYSTEMD
#	@$(call install_copy, syslogng, 0, 0, 0755, $(SYSLOGNG_DIR)/contrib/systemd/syslog-ng.service, /etc/systemd/system/syslogng.service, n)
#	@$(call install_link, syslogng, ../syslogng.service, /etc/systemd/system/multi-user.target.wants/syslogng.service)
#endif

	@$(call install_finish, syslogng)

	@$(call touch)

# vim: syntax=make
