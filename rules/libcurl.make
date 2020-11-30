# -*-makefile-*-
#
# Copyright (C) 2008 by Daniel Schnell
#		2008, 2009, 2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBCURL) += libcurl

#
# Paths and names
#
LIBCURL_VERSION	:= 7.72.0
LIBCURL_MD5	:= 84c557176d185661655b00c17f150fc7
LIBCURL		:= curl-$(LIBCURL_VERSION)
LIBCURL_SUFFIX	:= tar.bz2
LIBCURL_URL	:= https://curl.haxx.se/download/$(LIBCURL).$(LIBCURL_SUFFIX)
LIBCURL_SOURCE	:= $(SRCDIR)/$(LIBCURL).$(LIBCURL_SUFFIX)
LIBCURL_DIR	:= $(BUILDDIR)/$(LIBCURL)
LIBCURL_LICENSE	:= MIT

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
LIBCURL_CONF_TOOL	:= autoconf
LIBCURL_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-debug \
	--enable-optimize \
	--disable-warnings \
	--disable-werror \
	--disable-curldebug \
	--enable-symbol-hiding \
	--enable-hidden-symbols \
	--$(call ptx/endis, PTXCONF_LIBCURL_C_ARES)-ares \
	--enable-rt \
	--disable-code-coverage \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--$(call ptx/endis, PTXCONF_LIBCURL_HTTP)-http \
	--$(call ptx/endis, PTXCONF_LIBCURL_FTP)-ftp \
	--$(call ptx/endis, PTXCONF_LIBCURL_FILE)-file \
	--disable-ldap \
	--disable-ldaps \
	--disable-rtsp \
	--enable-proxy \
	--disable-dict \
	--disable-telnet \
	--$(call ptx/endis, PTXCONF_LIBCURL_TFTP)-tftp \
	--disable-pop3 \
	--disable-imap \
	--disable-smb \
	--$(call ptx/endis, PTXCONF_LIBCURL_SMTP)-smtp \
	--disable-gopher \
	--disable-manual \
	--enable-libcurl-option \
	--disable-libgcc \
	$(GLOBAL_IPV6_OPTION) \
	--enable-openssl-auto-load-config \
	--disable-versioned-symbols \
	--$(call ptx/disen, PTXCONF_LIBCURL_C_ARES)-threaded-resolver \
	--enable-pthreads \
	--$(call ptx/endis, PTXCONF_LIBCURL_VERBOSE)-verbose \
	--disable-sspi \
	--$(call ptx/endis, PTXCONF_LIBCURL_CRYPTO_AUTH)-crypto-auth \
	--disable-ntlm-wb \
	--enable-tls-srp \
	--enable-unix-sockets \
	--$(call ptx/endis, PTXCONF_LIBCURL_COOKIES)-cookies \
	--$(call ptx/endis, PTXCONF_LIBCURL_HTTP)-http-auth \
	--disable-doh \
	--disable-mime \
	--enable-dateparse \
	--enable-netrc \
	--enable-progress-meter \
	--disable-dnsshuffle \
	--disable-alt-svc \
	--with-zlib=$(SYSROOT) \
	--without-brotli \
	--without-gssapi \
	--with-default-ssl-backend=$(call ptx/ifdef, PTXCONF_LIBCURL_SSL,openssl,no) \
	--without-winssl \
	--without-schannel \
	--without-darwinssl \
	--without-secure-transport \
	--without-amissl \
	--with-ssl=$(call ptx/ifdef, PTXCONF_LIBCURL_SSL,$(SYSROOT)/usr,no) \
	--with-random=/dev/urandom \
	--without-gnutls \
	--without-mbedtls \
	--without-mesalink \
	--without-nss \
	--with-ca-bundle=$(PTXCONF_LIBCURL_SSL_CABUNDLE_PATH) \
	--with-ca-path=$(PTXCONF_LIBCURL_SSL_CAPATH_PATH) \
	--without-ca-fallback \
	--without-libpsl \
	--without-libmetalink \
	--$(call ptx/wwo, PTXCONF_LIBCURL_LIBSSH2)-libssh2 \
	--without-libssh \
	--without-librtmp \
	--without-winidn \
	--without-libidn2 \
	--without-nghttp2 \
	--without-zsh-functions-dir \
	--without-fish-functions-dir

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libcurl.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libcurl)
	@$(call install_fixup, libcurl,PRIORITY,optional)
	@$(call install_fixup, libcurl,SECTION,base)
	@$(call install_fixup, libcurl,AUTHOR,"Daniel Schnell <daniel.schnell@marel.com>")
	@$(call install_fixup, libcurl,DESCRIPTION,missing)

ifdef PTXCONF_LIBCURL_CURL
	@$(call install_copy, libcurl, 0, 0, 0755, -, /usr/bin/curl)
endif
	@$(call install_lib, libcurl, 0, 0, 0644, libcurl)

	@$(call install_finish, libcurl)

	@$(call touch)

# vim: syntax=make
