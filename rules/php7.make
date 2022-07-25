# -*-makefile-*-
#
# Copyright (C) 2006-2008 by Robert Schwebel
#               2009, 2012 by Marc Kleine-Budde <mkl@pengutronix.de>
#               2015 by Bruno Thomsen <bth@kamstrup.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PHP7) += php7

#
# Paths and names
#
PHP7_VERSION	:= 7.4.28
PHP7_MD5	:= ca4f40f41d028465bc810c007c3ed935
PHP7		:= php-$(PHP7_VERSION)
PHP7_SUFFIX	:= tar.xz
PHP7_SOURCE	:= $(SRCDIR)/$(PHP7).$(PHP7_SUFFIX)
PHP7_DIR	:= $(BUILDDIR)/$(PHP7)
PHP7_LICENSE		:= PHP License

#
# Note: older releases are moved to the 'museum', but the 'de.php.net'
# response with a HTML file instead of the archive. So, try the 'museum'
# URL first
#
PHP7_URL := \
	https://www.php.net/distributions/$(PHP7).$(PHP7_SUFFIX) \
	http://museum.php.net/php7/$(PHP7).$(PHP7_SUFFIX) \
	http://de.php.net/distributions/$(PHP7).$(PHP7_SUFFIX) \
	http://de2.php.net/get/$(PHP7).$(PHP7_SUFFIX)/from/this/mirror

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/php7.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(PHP7_DIR))
	@$(call extract, PHP7)
	@$(call patchin, PHP7)
	@cd $(PHP7_DIR) && ./buildconf --force
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

PHP7_CONF_ENV := \
	$(CROSS_ENV) \
	CFLAGS="$(CROSS_CFLAGS) -Os" \
	PHP_LDFLAGS="-ldl" \
	ac_cv_prog_cc_cross=yes \
	ac_cv_c_bigendian_php=$(call ptx/yesno, PTXCONF_ENDIAN_BIG)

#
# autoconf
#
PHP7_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--disable-phar \
	--with-config-file-path=/etc/php7 \
	--enable-opcache=no \
	--disable-fileinfo


# FIXME: PHP7 doesn't interprete "with_foo=no" correctly, so we cannot
# give --without-foo options. Should be fixed in PHP7's configure.in.

# ------------
# SAPI Modules
# ------------

ifdef PTXCONF_PHP7_SAPI_AOLSERVER
PHP7_AUTOCONF += --with-aolserver=FIXME
else
#PHP7_AUTOCONF += --without-aolserver
endif

ifdef PTXCONF_PHP7_SAPI_APXS2FILTER
PHP7_AUTOCONF += --with-apxs2filter=/usr/bin/apxs
else
#PHP7_AUTOCONF += --without-apxs2filter
endif

ifdef PTXCONF_PHP7_SAPI_APXS2
PHP7_AUTOCONF += --with-apxs2=/usr/bin/apxs
else
# PHP7_AUTOCONF += --without-apxs2
endif

ifdef PTXCONF_PHP7_SAPI_CAUDIUM
PHP7_AUTOCONF += --with-caudium
else
# PHP7_AUTOCONF += --without-caudium
endif

ifdef PTXCONF_PHP7_SAPI_CLI
PHP7_AUTOCONF += --enable-cli
else
PHP7_AUTOCONF += --disable-cli
endif

ifdef PTXCONF_PHP7_SAPI_CONTINUITY
PHP7_AUTOCONF += --with-continuity
else
#PHP7_AUTOCONF += --without-continuity
endif

ifdef PTXCONF_PHP7_SAPI_EMBEDDED
PHP7_AUTOCONF += --enable-embed
else
#PHP7_AUTOCONF += --disable-embed
endif

ifdef PTXCONF_PHP7_SAPI_ISAPI
PHP7_AUTOCONF += --with-isapi
else
#PHP7_AUTOCONF += --without-isapi
endif

ifdef PTXCONF_PHP7_SAPI_MILTER
PHP7_AUTOCONF += --with-milter
else
#PHP7_AUTOCONF += --without-milter
endif

ifdef PTXCONF_PHP7_SAPI_NSAPI
PHP7_AUTOCONF += --with-nsapi
else
#PHP7_AUTOCONF += --without-nsapi
endif

ifdef PTXCONF_PHP7_SAPI_PHTTPD
PHP7_AUTOCONF += --with-phttpd
else
#PHP7_AUTOCONF += --without-phttpd
endif

ifdef PTXCONF_PHP7_SAPI_PI3WEB
PHP7_AUTOCONF += --with-pi3web
else
#PHP7_AUTOCONF += --without-pi3web
endif

ifdef PTXCONF_PHP7_SAPI_ROXEN
PHP7_AUTOCONF += --with-roxen
else
#PHP7_AUTOCONF += --without-roxen
endif

ifdef PTXCONF_PHP7_SAPI_ROXEN_ZTS
PHP7_AUTOCONF += --with-roxen-zts
else
#PHP7_AUTOCONF += --without-roxen-zts
endif

ifdef PTXCONF_PHP7_SAPI_THTTPD
PHP7_AUTOCONF += --with-thttpd
else
#PHP7_AUTOCONF += --without-thttpd
endif

ifdef PTXCONF_PHP7_SAPI_TUX
PHP7_AUTOCONF += --with-tux
else
#PHP7_AUTOCONF += --without-tux
endif

ifdef PTXCONF_PHP7_SAPI_CGI
PHP7_AUTOCONF += --enable-cgi
else
PHP7_AUTOCONF += --disable-cgi
endif

ifdef PTXCONF_PHP7_SAPI_FPM
PHP7_AUTOCONF += --enable-fpm
else
PHP7_AUTOCONF += --disable-fpm
endif

# ---------------
# General Options
# ---------------

ifdef PTXCONF_PHP7_XML
PHP7_AUTOCONF += --enable-xml
else
PHP7_AUTOCONF += --disable-xml
endif

ifdef PTXCONF_PHP7_XML_LIBXML2
PHP7_AUTOCONF += \
	--with-libxml
else
PHP7_AUTOCONF += --without-libxml
endif

ifdef PTXCONF_PHP7_XML_LIBXML2_READER
PHP7_AUTOCONF += --enable-xmlreader
else
PHP7_AUTOCONF += --disable-xmlreader
endif

ifdef PTXCONF_PHP7_XML_LIBXML2_WRITER
PHP7_AUTOCONF += --enable-xmlwriter
else
PHP7_AUTOCONF += --disable-xmlwriter
endif

ifdef PTXCONF_PHP7_XML_LIBXML2_DOM
PHP7_AUTOCONF += --enable-dom
else
PHP7_AUTOCONF += --disable-dom
endif

ifdef PTXCONF_PHP7_XML_LIBXML2_XSLT
PHP7_AUTOCONF += --with-xsl=/usr
else
PHP7_AUTOCONF += --without-xsl
endif

ifdef PTXCONF_PHP7_XML_LIBXML2_SIMPLEXML
PHP7_AUTOCONF += --enable-simplexml
else
PHP7_AUTOCONF += --disable-simplexml
endif

ifdef PTXCONF_PHP7_XML_LIBXML2_XMLRPC
PHP7_AUTOCONF += --with-xmlrpc
endif

ifdef PTXCONF_PHP7_EXT_ZLIB
PHP7_AUTOCONF += --with-zlib=/usr
else
PHP7_AUTOCONF += --without-zlib
endif

ifdef PTXCONF_PHP7_EXT_ZIP
PHP7_AUTOCONF += --with-zip
else
PHP7_AUTOCONF += --without-zip
endif

ifdef PTXCONF_PHP7_EXT_OPENSSL
PHP7_AUTOCONF += --with-openssl=/usr
else
PHP7_AUTOCONF += --without-openssl
endif

ifdef PTXCONF_PHP7_EXT_CURL
PHP7_AUTOCONF += \
	--with-curl=/usr
else
PHP7_AUTOCONF += --without-curl
endif

ifdef PTXCONF_PHP7_EXT_MYSQL
PHP7_AUTOCONF += \
	--with-pdo-mysql=/usr \
	--with-mysqli=/usr/bin/mysql_config
else
PHP7_AUTOCONF += --without-pdo-mysql --without-mysqli
endif

ifdef PTXCONF_PHP7_EXT_SOAP
PHP7_AUTOCONF += --enable-soap
else
PHP7_AUTOCONF += --disable-soap
endif

ifdef PTXCONF_PHP7_EXT_SOCKETS
PHP7_AUTOCONF += --enable-sockets
else
PHP7_AUTOCONF += --disable-sockets
endif

ifdef PTXCONF_PHP7_EXT_SQLITE3
PHP7_AUTOCONF += \
	--with-sqlite3=/usr \
	--with-pdo-sqlite=/usr
# broken config system: sqlite3 (local copy) uses it
# but it is only linked to if used by external dependencies
#PHP7_CONF_ENV += PHP_LDFLAGS="-ldl -lpthread"
else
PHP7_AUTOCONF += --without-sqlite3 --without-pdo-sqlite
endif

ifdef PTXCONF_PHP7_EXT_SHMOP
PHP7_AUTOCONF += --enable-shmop --enable-sysvsem
else
PHP7_AUTOCONF += --disable-shmop --disable-sysvsem
endif

ifdef PTXCONF_PHP7_EXT_PEAR
PHP7_AUTOCONF += --with-pear=FIXME
else
PHP7_AUTOCONF += --without-pear
endif

ifdef PTXCONF_PHP7_EXT_MBSTRING
PHP7_AUTOCONF += --enable-mbstring
else
PHP7_AUTOCONF += --disable-mbstring
endif

ifdef PTXCONF_PHP7_EXT_ICONV
PHP7_AUTOCONF += --with-iconv=/usr
else
PHP7_AUTOCONF += --without-iconv
endif

ifdef PTXCONF_PHP7_EXT_GD
PHP7_AUTOCONF += --enable-gd
PHP7_AUTOCONF += --with-jpeg
PHP7_AUTOCONF += --with-xpm
endif


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/php7.targetinstall:
	@$(call targetinfo)

	@$(call install_init, php7)
	@$(call install_fixup, php7,PRIORITY,optional)
	@$(call install_fixup, php7,SECTION,base)
	@$(call install_fixup, php7,AUTHOR,"Adam Bagsik <adam.bagsik@wago.com>")
	@$(call install_fixup, php7,DESCRIPTION,missing)

ifdef PTXCONF_PHP7_SAPI_APXS2
	@$(call install_copy, php7, 0, 0, 0644, -, \
		/usr/modules/libPHP7.so)
endif

ifdef PTXCONF_PHP7_SAPI_CLI
	@$(call install_copy, php7, 0, 0, 0755, $(PHP7_PKGDIR)/usr/bin/php, \
		/usr/bin/php7)
	@$(call install_link, php7, php7, /usr/bin/php)
endif

ifdef PTXCONF_PHP7_SAPI_CGI
	@$(call install_copy, php7, 0, 0, 0755, $(PHP7_PKGDIR)/usr/bin/php-cgi, /usr/bin/php7-cgi)
	@$(call install_link, php7, php7-cgi, /usr/bin/php-cgi)
endif

ifdef PTXCONF_PHP7_INI
	@$(call install_alternative, php7, 0, 102, 0640, /etc/php7/php.ini)
endif

ifdef PTXCONF_PHP7_SAPI_FPM
	@$(call install_copy, php7, 0, 0, 0755, $(PHP7_PKGDIR)/usr/sbin/php-fpm, /usr/sbin/php7-fpm)

	@$(call install_alternative, php7, 0, 0, 0640, /etc/php7/php-fpm.conf)
	@$(call install_alternative, php7, 0, 0, 0640, /etc/php7/php-fpm.d/www.conf)
ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_PHP7_SAPI_FPM_STARTSCRIPT
	@$(call install_alternative, php7, 0, 0, 0755, /etc/init.d/php-fpm)

ifneq ($(call remove_quotes,$(PTXCONF_PHP7_SAPI_FPM_BBINIT_LINK)),)
	@$(call install_link, php7, \
		../init.d/php-fpm, \
		/etc/rc.d/$(PTXCONF_PHP7_SAPI_FPM_BBINIT_LINK))
endif
endif
endif
endif

	@$(call install_finish, php7)

	@$(call touch)

# vim: syntax=make
