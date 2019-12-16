# -*-makefile-*-
# Copyright (C) 2006 by Robert Schwebel
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PUREFTPD) += pureftpd

#
# Paths and names
#
PUREFTPD_VERSION	:= 1.0.47
PUREFTPD_MD5		:= a41fa531c0d21bd3416dd524d75495ae
PUREFTPD		:= pure-ftpd-$(PUREFTPD_VERSION)
PUREFTPD_SUFFIX		:= tar.bz2
PUREFTPD_URL		:= http://download.pureftpd.org/pub/pure-ftpd/releases/$(PUREFTPD).$(PUREFTPD_SUFFIX)
PUREFTPD_SOURCE		:= $(SRCDIR)/$(PUREFTPD).$(PUREFTPD_SUFFIX)
PUREFTPD_DIR		:= $(BUILDDIR)/$(PUREFTPD)
PUREFTPD_LICENSE	:= Custom License

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
PUREFTPD_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--without-ascii \
	--without-cookie \
	--without-throttling \
	--without-ratios \
	--without-quotas \
	--without-ftpwho \
	--with-welcomemsg \
	--without-virtualchroot \
	--without-nonroot \
	--without-peruserlimits \
	--without-debug \
	--with-language=english \
	--without-ldap \
	--without-mysql \
	--without-pgsql \
	--without-privsep \
	--without-capabilities \
	--with-standalone

#
# FIXME: configure probes host's /dev/urandom and /dev/random
# instead of target's one
#
# Can --with-probe-random-dev solve this?

# Enable or disable Linux-PAM support
PUREFTPD_AUTOCONF += --$(call ptx/wwo, PTXCONF_PUREFTPD_PAM)-pam

ifdef PTXCONF_PUREFTPD_SYSTEMD_UNIT
PUREFTPD_AUTOCONF += --with-inetd
else
PUREFTPD_AUTOCONF += --without-inetd
endif

ifdef PTXCONF_PUREFTPD_UPLOADSCRIPT
PUREFTPD_AUTOCONF += --with-uploadscript
else
PUREFTPD_AUTOCONF += --without-uploadscript
endif

ifdef PTXCONF_PUREFTPD_VIRTUALHOSTS
PUREFTPD_AUTOCONF += --with-virtualhosts
else
PUREFTPD_AUTOCONF += --without-virtualhosts
endif

ifdef PTXCONF_PUREFTPD_DIRALIASES
PUREFTPD_AUTOCONF += --with-diraliases
else
PUREFTPD_AUTOCONF += --without-diraliases
endif

ifdef PTXCONF_PUREFTPD_MINIMAL
PUREFTPD_AUTOCONF += --with-minimal
else
PUREFTPD_AUTOCONF += --without-minimal
endif

ifdef PTXCONF_PUREFTPD_SHRINK_MORE
PUREFTPD_AUTOCONF += --without-globbing
else
PUREFTPD_AUTOCONF += --with-globbing
endif

ifdef PTXCONF_PUREFTPD_SSL
# use https certificate from lighttpd
PUREFTPD_AUTOCONF += --with-tls
endif

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

PUREFTPD_ARGS := $(call remove_quotes,$(PTXCONF_PUREFTPD_ARGS))
ifdef PTXCONF_PUREFTPD_UPLOADSCRIPT
PUREFTPD_ARGS += -o
endif

$(STATEDIR)/pureftpd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pureftpd)
	@$(call install_fixup, pureftpd,PRIORITY,optional)
	@$(call install_fixup, pureftpd,SECTION,base)
	@$(call install_fixup, pureftpd,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, pureftpd,DESCRIPTION,missing)

	@$(call install_copy, pureftpd, 0, 0, 0755, -, \
		/usr/sbin/pure-ftpd)

ifdef PTXCONF_PUREFTPD_INSTALL_CUSTOM_CONFIG_FILE
# Custom config file is not processed by pureftpd directly.
# It has to be parsed by a wrapper or init script instead.
# Currently it may be parsed by /etc/init.d/pureftpd.
# FIXME: add solution for systemd (wrapper script).
	@$(call install_alternative, pureftpd, 0, 0, 0644, /etc/pure-ftpd.conf)
endif

ifdef PTXCONF_PUREFTPD_UPLOADSCRIPT
	@$(call install_copy, pureftpd, 0, 0, 0755, -, \
		/usr/sbin/pure-uploadscript)
endif

ifdef PTXCONF_PUREFTPD_PAM
	@$(call install_alternative, pureftpd, 0, 0, 644, /etc/pam.d/pure-ftpd)
endif

ifdef PTXCONF_PUREFTPD_SSL
	@$(call install_copy, pureftpd, 0, 0, 0740, /etc/ssl/private/)
	@$(call install_link, pureftpd, /etc/lighttpd/https-cert.pem, /etc/ssl/private/pure-ftpd.pem)
	@$(call install_link, pureftpd, /etc/lighttpd/dh3072.pem, /etc/ssl/private/pure-ftpd-dhparams.pem)
endif

#	#
#	# busybox init
#	#
ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_PUREFTPD_STARTSCRIPT
	@$(call install_alternative, pureftpd, 0, 0, 0755, /etc/init.d/pureftpd)

	@$(call install_replace, pureftpd, \
		/etc/init.d/pureftpd, \
		@DAEMON_ARGS@, "$(PUREFTPD_ARGS)")
	@$(call install_replace, pureftpd, \
		/etc/init.d/pureftpd, \
		@HELPER_ARGS@, $(PTXCONF_PUREFTPD_UPLOADSCRIPT_ARGS))
	@$(call install_replace, pureftpd, \
		/etc/init.d/pureftpd, \
		@HELPER_SCRIPT@, $(PTXCONF_PUREFTPD_UPLOADSCRIPT_SCRIPT))

ifneq ($(call remove_quotes,$(PTXCONF_PUREFTPD_BBINIT_LINK)),)
	@$(call install_link, pureftpd, \
		../init.d/pureftpd, \
		/etc/rc.d/$(PTXCONF_PUREFTPD_BBINIT_LINK))
endif
endif
endif

ifdef PTXCONF_PUREFTPD_SYSTEMD_UNIT
	@$(call install_alternative, pureftpd, 0, 0, 0644, \
		/lib/systemd/system/pure-ftpd.socket)
	@$(call install_link, pureftpd, ../pure-ftpd.socket, \
		/lib/systemd/system/sockets.target.wants/pure-ftpd.socket)

	@$(call install_alternative, pureftpd, 0, 0, 0644, \
		/lib/systemd/system/pure-ftpd@.service)
	@$(call install_replace, pureftpd, \
		/lib/systemd/system/pure-ftpd@.service, \
		@ARGS@, "$(PUREFTPD_ARGS)")

	@$(call install_alternative, pureftpd, 0, 0, 0644, \
		/lib/systemd/system/pure-uploadscript.service)
	@$(call install_replace, pureftpd, \
		/lib/systemd/system/pure-uploadscript.service, \
		@ARGS@, $(PTXCONF_PUREFTPD_UPLOADSCRIPT_ARGS))
	@$(call install_replace, pureftpd, \
		/lib/systemd/system/pure-uploadscript.service, \
		@SCRIPT@, $(PTXCONF_PUREFTPD_UPLOADSCRIPT_SCRIPT))
endif

	@$(call install_finish, pureftpd)

	@$(call touch)

# vim: syntax=make
