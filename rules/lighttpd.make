# -*-makefile-*-
#
# Copyright (C) 2007 by Daniel Schnell
#		2008, 2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIGHTTPD) += lighttpd

#
# WAGO: -added self-signed certificate generation
#       -added installation routine for a default lighttpd-htpasswd.user and its backup
#

#
# Paths and names
#
LIGHTTPD_VERSION	:= 1.4.54
LIGHTTPD_MD5		:= 7abc776243c811e9872f73ab38b7f8b5
LIGHTTPD		:= lighttpd-$(LIGHTTPD_VERSION)
LIGHTTPD_SUFFIX		:= tar.xz
LIGHTTPD_URL		:= http://download.lighttpd.net/lighttpd/releases-1.4.x/$(LIGHTTPD).$(LIGHTTPD_SUFFIX)
LIGHTTPD_SOURCE		:= $(SRCDIR)/$(LIGHTTPD).$(LIGHTTPD_SUFFIX)
LIGHTTPD_DIR		:= $(BUILDDIR)/$(LIGHTTPD)
LIGHTTPD_PROJECTROOT	=  $(call ptx/get_alternative, projectroot, etc/lighttpd)
LIGHTTPD_LICENSE	:= BSD-3-Clause
LIGHTTPD_LICENSE_FILES := file://COPYING;md5=e4dac5c6ab169aa212feb5028853a579

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIGHTTPD_PATH	:= PATH=$(CROSS_PATH)
LIGHTTPD_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
LIGHTTPD_CONF_TOOL	:= autoconf
LIGHTTPD_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--libdir=/usr/lib/lighttpd \
	--$(call ptx/endis, PTXCONF_GLOBAL_LARGE_FILE)-lfs \
	$(GLOBAL_IPV6_OPTION) \
	--disable-mmap \
	--enable-extra-warnings \
	--without-libev \
	--without-mysql \
	--without-pgsql \
	--without-dbi \
	--without-sasl \
	--without-ldap \
	--without-pam \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_ATTR)-attr \
	--without-valgrind \
	--without-libunwind \
	--without-krb5 \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_OPENSSL)-openssl \
	--without-wolfssl \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_PCRE)-pcre \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_ZLIB)-zlib \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_BZ2LIB)-bzip2 \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_FAM)-fam \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_WEBDAV_PROPS)-webdav-props \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_WEBDAV_PROPS)-libxml \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_WEBDAV_PROPS)-sqlite \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_WEBDAV_LOCKS)-webdav-locks \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_WEBDAV_LOCKS)-uuid \
	--without-gdbm \
	--without-geoip \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_MEMCACHED)-memcached \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_LUA)-lua

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

lighttpd_compile: $(STATEDIR)/lighttpd.compile

$(STATEDIR)/lighttpd.compile:
	@$(call targetinfo)
	$(LIGHTTPD_PATH) make -C $(LIGHTTPD_DIR)

ifdef PTXCONF_LIGHTTPD_HTTPS_GEN_CERT
# Generate a long-lasting self-signed certificate for testing
# openssl suffers from y2038 bug => use number of days until the "doomsday"
# to move the expiration date as far as possible into the future
	@mkdir -p $(LIGHTTPD_DIR) && \
	export PASSPHRASE=$$(head -c 128 /dev/urandom | uuencode - | grep -v '^end' | tr '\n' 'd') && \
DAYS_VALID=$$(echo "2038 01 18" | awk '{dt=mktime($$0 " 00 00 00")-systime(); print int(dt/86400);}') && \
echo $$DAYS_VALID && echo "" | awk '{print systime();}' && \
SUBJ=/C='DE'/ST='NRW'/L='Minden'/O='Wago Kontakttechnik GmbH & Co. KG'/OU='AUTOMATION'/CN='Self-signed certificate for testing' && \
	openssl genrsa -des3 -out $(LIGHTTPD_DIR)/server.key -passout env:PASSPHRASE 3072 && \
	cp $(LIGHTTPD_DIR)/server.key $(LIGHTTPD_DIR)/server.key.org && \
	openssl rsa -in $(LIGHTTPD_DIR)/server.key.org -out $(LIGHTTPD_DIR)/server.key -passin env:PASSPHRASE && \
	openssl req -new -x509 -subj "$${SUBJ}" -days $${DAYS_VALID} -extensions v3_req -key $(LIGHTTPD_DIR)/server.key -out $(LIGHTTPD_DIR)/server.crt -sha256 && \
	cat $(LIGHTTPD_DIR)/server.crt $(LIGHTTPD_DIR)/server.key > $(LIGHTTPD_DIR)/https-cert.pem && echo $${DAYS_VALID} && \
	openssl x509 -text -noout -in $(LIGHTTPD_DIR)/https-cert.pem && \
	openssl dhparam -out $(LIGHTTPD_DIR)/dh3072.pem -outform PEM -2 3072
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------
#$(STATEDIR)/lighttpd.install:
#	@$(call targetinfo)
#	@$(call world/install, LIGHTTPD)
#	@install -vD -m 0644 "$(LIGHTTPD_DIR)/doc/config/conf.d/mime.conf" \
#		"$(LIGHTTPD_PKGDIR)/etc/lighttpd/conf.d/mime.conf"
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

LIGHTTPD_MODULES-y :=
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_ACCESS)		+= mod_access
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_ACCESSLOG)	+= mod_accesslog
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_ALIAS)		+= mod_alias
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_AUTH)		+= mod_auth
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_AUTH)		+= mod_authn_file
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_CML)		+= mod_cml
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_COMPRESS)	+= mod_compress
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_FASTCGI)	+= mod_fastcgi
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_MAGNET)		+= mod_magnet
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_OPENSSL)		+= mod_openssl
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_REWRITE)	+= mod_rewrite
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_SETENV)		+= mod_setenv
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_TRIGGER_B4_DL)	+= mod_trigger_b4_dl
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_WEBDAV)		+= mod_webdav
LIGHTTPD_MODULES-y += $(call remove_quotes,$(PTXCONF_LIGHTTPD_MOD_EXTRA))

LIGHTTPD_MODULE_STRING := $(subst $(space),$(comma),$(addsuffix \",$(addprefix \",$(LIGHTTPD_MODULES-y))))

# add modules that are always loaded
LIGHTTPD_MODULES_INSTALL := mod_indexfile mod_dirlisting mod_staticfile $(LIGHTTPD_MODULES-y)

$(STATEDIR)/lighttpd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, lighttpd)
	@$(call install_fixup, lighttpd,PRIORITY,optional)
	@$(call install_fixup, lighttpd,SECTION,base)
	@$(call install_fixup, lighttpd,AUTHOR,"Daniel Schnell <danielsch@marel.com>")
	@$(call install_fixup, lighttpd,DESCRIPTION,missing)

#	#
#	# Executables
#	#
	@$(call install_copy, lighttpd, 0, 0, 0755, -, \
		/usr/sbin/lighttpd)
	@$(call install_copy, lighttpd, 0, 0, 0755, -, \
		/usr/sbin/lighttpd-angel)

#	#
#	# Modules
#	#
ifdef PTXCONF_LIGHTTPD_INSTALL_SELECTED_MODULES
	@$(foreach mod,$(LIGHTTPD_MODULES_INSTALL), \
		$(call install_lib, lighttpd, 0, 0, 0644, lighttpd/$(mod));)
else
	@$(call install_tree, lighttpd, 0, 0, -, /usr/lib/lighttpd)
endif

#	#
#	# Configuration files
#	#
	@cd $(LIGHTTPD_PROJECTROOT) && \
	for file in *.conf; do \
			$(call install_copy, lighttpd, 0, 0, 0600, \
			$(LIGHTTPD_PROJECTROOT)/$$file, \
			/etc/lighttpd/$$file, n); \
	done;

# Config directory for FCGI plugins
	@$(call install_copy, lighttpd, 0, 0, 0755, \
		/etc/lighttpd/fastcgi.confd);

ifdef PTXCONF_LIGHTTPD_MOD_FASTCGI_PHP
	@$(call install_alternative, lighttpd, 0, 0, 0600, \
		/etc/lighttpd/conf.d/mod_fastcgi_php.conf)
endif

ifdef PTXCONF_LIGHTTPD_HTTPS

#	#
#	# Default mode (https or https+http)
#	#
ifdef PTXCONF_LIGHTTPD_INSTALL_CONF_LINK_HTTPS
	@$(call install_link, lighttpd, mode_https.conf, \
		/etc/lighttpd/mode.conf)
else
	@$(call install_link, lighttpd, mode_http+https.conf, \
		/etc/lighttpd/mode.conf)
endif

#	#
#	# TLS configuration (strong or standard)
#	#
ifdef PTXCONF_LIGHTTPD_INSTALL_CONF_LINK_HTTPS_STRONG
	@$(call install_link, lighttpd, tls-strong.conf, \
		/etc/lighttpd/tls.conf)
else
	@$(call install_link, lighttpd, tls-standard.conf, \
		/etc/lighttpd/tls.conf)
endif

endif # PTXCONF_LIGHTTPD_HTTPS

#	#
#	# Default mode (http)
#	#
ifdef PTXCONF_LIGHTTPD_INSTALL_CONF_LINK_HTTP
	@$(call install_link, lighttpd, mode_http.conf, \
		/etc/lighttpd/mode.conf)
endif

#	#
#	# Certificates and keys
#	#
ifdef PTXCONF_LIGHTTPD_HTTPS_GEN_CERT
	@$(call install_copy, lighttpd, 0, 0, 0400, \
		$(LIGHTTPD_DIR)/https-cert.pem, \
		/etc/lighttpd/https-cert.pem, n)
	@$(call install_copy, lighttpd, 0, 0, 0400, \
		$(LIGHTTPD_DIR)/dh3072.pem, \
		/etc/lighttpd/dh3072.pem, n)
else
	@$(call install_alternative, lighttpd, 0, 0, 0400, \
		/etc/lighttpd/https-cert.pem, n)
	@$(call install_alternative, lighttpd, 0, 0, 0400, \
		/etc/lighttpd/dh3072.pem, n)
endif

#	#
#	# Websockets
#	#
	@$(call install_alternative, lighttpd, 0, 0, 0600, \
		/etc/lighttpd/websocket_off.conf, n)
	@$(call install_alternative, lighttpd, 0, 0, 0600, \
		/etc/lighttpd/websocket_on.conf, n)

ifdef PTXCONF_CDS3_TSCWEBSOCKETSERVER
	@$(call install_link, lighttpd, websocket_on.conf, \
		/etc/lighttpd/websocket.conf)
else
	@$(call install_link, lighttpd, websocket_off.conf, \
		/etc/lighttpd/websocket.conf)
endif

	@$(call install_link, lighttpd, redirect_wbm.conf, \
		/etc/lighttpd/redirect_default.conf)

#	#
#	# WBM user database
#	#
	@$(call install_alternative, lighttpd, 0, 102, 0640, \
		/etc/lighttpd/lighttpd-htpasswd.user)

#	#
#	# busybox init: start script
#	#
ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_LIGHTTPD_STARTSCRIPT
	@$(call install_alternative, lighttpd, 0, 0, 0755, /etc/init.d/lighttpd)

ifneq ($(call remove_quotes, $(PTXCONF_LIGHTTPD_BBINIT_LINK)),)
	@$(call install_link, lighttpd, \
		../init.d/lighttpd, \
		/etc/rc.d/$(PTXCONF_LIGHTTPD_BBINIT_LINK))
endif
endif
endif

ifdef PTXCONF_LIGHTTPD_SYSTEMD_UNIT
	@$(call install_alternative, lighttpd, 0, 0, 0644, \
		/usr/lib/systemd/system/lighttpd.service)
	@$(call install_link, lighttpd, ../lighttpd.service, \
		/usr/lib/systemd/system/multi-user.target.wants/lighttpd.service)
endif

ifdef PTXCONF_LIGHTTPD_GENERIC_SITE
ifdef PTXCONF_LIGHTTPD_MOD_FASTCGI_PHP
	@$(call install_copy, lighttpd, www, www, 0644, \
		$(PTXDIST_TOPDIR)/projectroot/var/www/lighttpd.html, \
		/var/www/index.html)

	@$(call install_copy, lighttpd, www, www, 0644, \
		$(PTXDIST_TOPDIR)/projectroot/var/www/bottles.php, \
		/var/www/bottles.php)

	@$(call install_copy, lighttpd, www, www, 0644, \
		$(PTXDIST_TOPDIR)/projectroot/var/www/more_bottles.php, \
		/var/www/more_bottles.php)
else
	@$(call install_copy, lighttpd, www, www, 0644, \
		$(PTXDIST_TOPDIR)/projectroot/var/www/httpd.html, \
		/var/www/index.html)
endif
endif

	@$(call install_finish, lighttpd)
	@$(call touch)

# vim: syntax=make
