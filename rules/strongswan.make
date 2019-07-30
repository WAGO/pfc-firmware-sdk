# -*-makefile-*-
#
# Copyright (C) 2013 by Christoph Fritz <chf.fritz@googlemail.com>
#
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_STRONGSWAN) += strongswan

#
# Paths and names
#
STRONGSWAN_VERSION	:= 5.7.2
STRONGSWAN_MD5		:= 618de96dc2a506f82a162a5abf9263d4
STRONGSWAN		:= strongswan-$(STRONGSWAN_VERSION)
STRONGSWAN_SUFFIX	:= tar.bz2
STRONGSWAN_URL		:= https://download.strongswan.org/$(STRONGSWAN).$(STRONGSWAN_SUFFIX)
STRONGSWAN_SOURCE	:= $(SRCDIR)/$(STRONGSWAN).$(STRONGSWAN_SUFFIX)
STRONGSWAN_DIR		:= $(BUILDDIR)/$(STRONGSWAN)
STRONGSWAN_LICENSE	:= GPL

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

STRONGSWAN_CONF_TOOL	:= autoconf
STRONGSWAN_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-static \
	--enable-aes \
	--$(call ptx/endis, PTXCONF_STRONGSWAN_AFALG)-af-alg \
	--disable-bliss \
	--disable-blowfish \
	--disable-ccm \
	--disable-chapoly \
	--enable-cmac \
	--disable-ctr \
	--disable-des \
	--enable-fips-prf \
	--enable-gcm \
	--disable-gcrypt \
	--enable-gmp \
	--disable-curve25519 \
	--enable-hmac \
	--disable-md4 \
	--disable-md5 \
	--disable-mgf1 \
	--disable-newhope \
	--enable-nonce \
	--disable-ntru \
	--$(call ptx/endis, PTXCONF_STRONGSWAN_OPENSSL)-openssl \
	--disable-padlock \
	--enable-random \
	--disable-rc2 \
	--disable-rdrand \
	--disable-aesni \
	--enable-sha1 \
	--enable-sha2 \
	--disable-sha3 \
	--enable-xcbc \
	--enable-dnskey \
	--enable-pem \
	--enable-pgp \
	--enable-pkcs1 \
	--enable-pkcs7 \
	--enable-pkcs8 \
	--disable-pkcs12 \
	--enable-pubkey \
	--disable-sshkey \
	--enable-x509 \
	--$(call ptx/endis, PTXCONF_STRONGSWAN_LIBCURL)-curl \
	--disable-files \
	--disable-ldap \
	--disable-soup \
	--disable-unbound \
	--disable-winhttp \
	--disable-mysql \
	--disable-sqlite \
	--disable-addrblock \
	--enable-acert \
	--disable-agent \
	--enable-constraints \
	--disable-coupling \
	--disable-dnscert \
	--disable-eap-sim \
	--disable-eap-sim-file \
	--disable-eap-sim-pcsc \
	--disable-eap-aka \
	--disable-eap-aka-3gpp \
	--disable-eap-aka-3gpp2 \
	--disable-eap-simaka-sql \
	--disable-eap-simaka-pseudonym \
	--disable-eap-simaka-reauth \
	--disable-eap-identity \
	--disable-eap-md5 \
	--disable-eap-gtc \
	--disable-eap-mschapv2 \
	--disable-eap-tls \
	--disable-eap-ttls \
	--disable-eap-peap \
	--disable-eap-tnc \
	--disable-eap-dynamic \
	--disable-eap-radius \
	--disable-ext-auth \
	--disable-ipseckey \
	--disable-keychain \
	--disable-pkcs11 \
	--disable-tpm \
	--enable-revocation \
	--disable-whitelist \
	--enable-xauth-generic \
	--disable-xauth-eap \
	--disable-xauth-pam \
	--disable-xauth-noauth \
	--enable-kernel-netlink \
	--disable-kernel-pfkey \
	--disable-kernel-pfroute \
	--disable-kernel-iph \
	--disable-kernel-libipsec \
	--disable-kernel-wfp \
	--enable-socket-default \
	--disable-socket-dynamic \
	--disable-socket-win \
	--enable-stroke \
	--disable-smp \
	--disable-sql \
	--disable-uci \
	--disable-vici \
	--disable-android-dns \
	--enable-attr \
	--disable-attr-sql \
	--disable-dhcp \
	--disable-osx-attr \
	--disable-p-cscf \
	--enable-resolve \
	--disable-unity \
	--disable-imc-test \
	--disable-imv-test \
	--disable-imc-scanner \
	--disable-imv-scanner \
	--disable-imc-os \
	--disable-imv-os \
	--disable-imc-attestation \
	--disable-imv-attestation \
	--disable-imc-swima \
	--disable-imv-swima \
	--disable-imc-hcd \
	--disable-imv-hcd \
	--disable-tnc-ifmap \
	--disable-tnc-imc \
	--disable-tnc-imv \
	--disable-tnc-pdp \
	--disable-tnccs-11 \
	--disable-tnccs-20 \
	--disable-tnccs-dynamic \
	--disable-android-log \
	--disable-bypass-lan \
	--disable-certexpire \
	--disable-connmark \
	--disable-counters \
	--disable-forecast \
	--disable-duplicheck \
	--disable-error-notify \
	--disable-farp \
	--disable-ha \
	--disable-led \
	--disable-load-tester \
	--disable-lookip \
	--disable-radattr \
	--disable-systime-fix \
	--disable-test-vectors \
	--enable-updown \
	--disable-aikgen \
	--enable-charon \
	--disable-cmd \
	--disable-conftest \
	--disable-fast \
	--disable-fuzzing \
	--disable-libipsec \
	--disable-manager \
	--disable-medcli \
	--disable-medsrv \
	--disable-nm \
	--enable-pki \
	--enable-scepclient \
	--enable-scripts \
	--disable-svc \
	--$(call ptx/endis, PTXCONF_STRONGSWAN_SYSTEMD_UNIT)-systemd \
	--disable-swanctl \
	--disable-tkm \
	--disable-bfd-backtraces \
	--disable-dbghelp-backtraces \
	--enable-ikev1 \
	--enable-ikev2 \
	--disable-integrity-test \
	--enable-load-warning \
	--disable-mediation \
	--disable-unwind-backtraces \
	--disable-ruby-gems \
	--disable-ruby-gems-install \
	--disable-python-eggs \
	--disable-python-eggs-install \
	--disable-perl-cpan \
	--disable-perl-cpan-install \
	--disable-tss-trousers \
	--disable-tss-tss2 \
	--disable-coverage \
	--disable-leak-detective \
	--disable-lock-profiler \
	--disable-log-thread-ids \
	--disable-monolithic \
	--disable-defaults \
	--enable-dependency-tracking \
	--enable-shared \
	--with-ipseclibdir=/usr/lib \
	--with-systemdsystemunitdir=/usr/lib/systemd/system

	#--disable-imc-swid \
	#--disable-imv-swid \
	#--disable-dumm \
# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

STRONGSWAN_PLUGINS := \
	libstrongswan-aes.so \
	libstrongswan-attr.so \
	libstrongswan-cmac.so \
	libstrongswan-constraints.so \
	libstrongswan-dnskey.so \
	libstrongswan-fips-prf.so \
	libstrongswan-gcm.so \
	libstrongswan-gmp.so \
	libstrongswan-hmac.so \
	libstrongswan-kernel-netlink.so \
	libstrongswan-nonce.so \
	libstrongswan-pem.so \
	libstrongswan-pgp.so \
	libstrongswan-pkcs1.so \
	libstrongswan-pkcs7.so \
	libstrongswan-pkcs8.so \
	libstrongswan-pubkey.so \
	libstrongswan-random.so \
	libstrongswan-resolve.so \
	libstrongswan-revocation.so \
	libstrongswan-sha1.so \
	libstrongswan-sha2.so \
	libstrongswan-socket-default.so \
	libstrongswan-stroke.so \
	libstrongswan-updown.so \
	libstrongswan-x509.so \
	libstrongswan-xauth-generic.so \
	libstrongswan-xcbc.so

ifdef PTXCONF_STRONGSWAN_LIBCURL
	STRONGSWAN_PLUGINS += libstrongswan-curl.so
endif
ifdef PTXCONF_STRONGSWAN_OPENSSL
	STRONGSWAN_PLUGINS += libstrongswan-openssl.so
endif
ifdef PTXCONF_STRONGSWAN_AFALG
	STRONGSWAN_PLUGINS += libstrongswan-af-alg.so
endif

$(STATEDIR)/strongswan.targetinstall:
	@$(call targetinfo)

	@$(call install_init, strongswan)
	@$(call install_fixup, strongswan,PRIORITY,optional)
	@$(call install_fixup, strongswan,SECTION,base)
	@$(call install_fixup, strongswan,AUTHOR,"Christoph Fritz <chf@fritzc.com>")
	@$(call install_fixup, strongswan,DESCRIPTION,missing)

	@$(call install_alternative, strongswan, 0, 0, 0640, /etc/strongswan.conf)
	@$(call install_alternative, strongswan, 0, 0, 0640, /etc/ipsec.conf)
	@$(call install_alternative, strongswan, 0, 0, 0600, /etc/ipsec.secrets)

	@$(call install_copy, strongswan, 0, 0, 0750, /etc/strongswan.d)

	@for i in $(shell cd $(STRONGSWAN_PKGDIR) && find etc/strongswan.d -type f); do \
		$(call install_copy, strongswan, 0, 0, 0640, -, /$$i); \
	done

	@$(call install_copy, strongswan, 0, 0, 0750, /etc/strongswan.d/charon)

	@for i in $(shell cd $(STRONGSWAN_PKGDIR) && find etc/strongswan.d/charon -type f); do \
		$(call install_copy, strongswan, 0, 0, 0640, -, /$$i); \
	done

	#@$(call install_copy, strongswan, 0, 0, 0755, -, /usr/bin/pki)
	@$(call install_copy, strongswan, 0, 0, 0755, -, /usr/sbin/ipsec)

	#@$(call install_tree, strongswan, 0, 0, -, /usr/libexec/ipsec)
	@for i in $(shell cd $(STRONGSWAN_PKGDIR) && find usr/libexec/ipsec -type f); do \
		$(call install_copy, strongswan, 0, 0, 0750, -, /$$i); \
	done

	@$(call install_lib, strongswan, 0, 0, 0644, libcharon)
	#@$(call install_lib, strongswan, 0, 0, 0644, libhydra)
	@$(call install_lib, strongswan, 0, 0, 0644, libstrongswan)

	#@$(call install_tree, strongswan, 0, 0, -, /usr/libexec/ipsec)
	@$(foreach plugin, $(STRONGSWAN_PLUGINS), \
		$(call install_copy, strongswan, 0, 0, 0644, -, \
			/usr/lib/plugins/$(plugin));)

ifdef PTXCONF_STRONGSWAN_SYSTEMD_UNIT
	@$(call install_alternative, strongswan, 0, 0, 0644, \
		/usr/lib/systemd/system/strongswan.service)
	@$(call install_link, strongswan, ../strongswan.service, \
		/usr/lib/systemd/system/multi-user.target.wants/strongswan.service)
endif

	@$(call install_copy, strongswan, 0, 0, 0640, /etc/ipsec.d)
	@$(call install_copy, strongswan, 0, 0, 0640, /etc/ipsec.d/aacerts)
	@$(call install_copy, strongswan, 0, 0, 0640, /etc/ipsec.d/acerts)
	#@$(call install_copy, strongswan, 0, 0, 0644, /etc/ipsec.d/cacerts)
	@$(call install_link, strongswan, /etc/certificates, /etc/ipsec.d/cacerts)
	#@$(call install_copy, strongswan, 0, 0, 0640, /etc/ipsec.d/certs)
	@$(call install_link, strongswan, /etc/certificates, /etc/ipsec.d/certs)
	@$(call install_copy, strongswan, 0, 0, 0640, /etc/ipsec.d/crls)
	@$(call install_copy, strongswan, 0, 0, 0640, /etc/ipsec.d/ocspcerts)
	#@$(call install_copy, strongswan, 0, 0, 0600, /etc/ipsec.d/private)
	@$(call install_link, strongswan, /etc/certificates/keys, /etc/ipsec.d/private)
	@$(call install_copy, strongswan, 0, 0, 0640, /etc/ipsec.d/reqs)

	@$(call install_finish, strongswan)

	@$(call touch)

# vim: syntax=make
