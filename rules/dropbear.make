# -*-makefile--
#
# Copyright (C) 2003 by Marc Kleine-Budde <kleine-budde@gmx.de> for
#                       Pengutronix e.K. <info@pengutronix.de>, Germany
#               2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_DROPBEAR) += dropbear

#
# Paths and names
#
DROPBEAR_VERSION	:= 2022.82
DROPBEAR_MD5		:= 7a4a5f2c6d23ff2e6627c97d7c1aeceb
DROPBEAR		:= dropbear-$(DROPBEAR_VERSION)
DROPBEAR_SUFFIX		:= tar.bz2
DROPBEAR_URL		:= http://matt.ucc.asn.au/dropbear/releases/$(DROPBEAR).$(DROPBEAR_SUFFIX)
DROPBEAR_SOURCE		:= $(SRCDIR)/$(DROPBEAR).$(DROPBEAR_SUFFIX)
DROPBEAR_DIR		:= $(BUILDDIR)/$(DROPBEAR)
DROPBEAR_LICENSE	:= \
	MIT AND BSD-2-Clause AND SSH-short AND (custom OR WTFPL) AND Unlicense
DROPBEAR_LICENSE_FILES	:= \
	file://LICENSE;md5=25cf44512b7bc8966a48b6b1a9b7605f \
	file://libtomcrypt/LICENSE;md5=71baacc459522324ef3e2b9e052e8180 \
	file://libtommath/LICENSE;md5=23e7e0a32e53a2b1d35f5fd9ef053402 \
	file://loginrec.c;startline=1;endline=26;md5=0d785ee11fab1cead2c7fee9c35574f1

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
DROPBEAR_CONF_TOOL	:= autoconf
DROPBEAR_CONF_OPT 	:= \
	$(CROSS_AUTOCONF_USR) \
	--enable-harden \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--$(call ptx/endis, PTXCONF_DROPBEAR_ZLIB)-zlib \
	--$(call ptx/endis, PTXCONF_DROPBEAR_PAM)-pam \
	--$(call ptx/disen, PTXCONF_DROPBEAR_DIS_OPENPTY)-openpty \
	--$(call ptx/disen, PTXCONF_DROPBEAR_DIS_SYSLOG)-syslog \
	--enable-shadow \
	--disable-plugin \
	--disable-fuzz \
	--enable-bundled-libtom \
	--$(call ptx/disen, PTXCONF_DROPBEAR_DIS_LASTLOG)-lastlog \
	--$(call ptx/disen, PTXCONF_DROPBEAR_DIS_UTMP)-utmp \
	--$(call ptx/disen, PTXCONF_DROPBEAR_DIS_UTMPX)-utmpx \
	--$(call ptx/disen, PTXCONF_DROPBEAR_DIS_WTMP)-wtmp \
	--$(call ptx/disen, PTXCONF_DROPBEAR_DIS_WTMPX)-wtmpx \
	--disable-loginfunc \
	--$(call ptx/disen, PTXCONF_DROPBEAR_DIS_PUTUTLINE)-pututline \
	--$(call ptx/disen, PTXCONF_DROPBEAR_DIS_PUTUTXLINE)-pututxline

DROPBEAR_LOCALOPTIONS	:= $(DROPBEAR_DIR)/localoptions.h

$(STATEDIR)/dropbear.prepare:
	@$(call targetinfo)
	@$(call world/prepare, DROPBEAR)

	@echo "/* localoptions.h created by ptxdist */" > $(DROPBEAR_LOCALOPTIONS)

# features
ifdef PTXCONF_DROPBEAR_MOTD
	@echo "ptxdist: print message of the day (MOTD)"
	@echo "#define DO_MOTD 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: do not print message of the day (MOTD)"
	@echo "#define DO_MOTD 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

ifdef PTXCONF_DROPBEAR_DIS_X11
	@echo "ptxdist: disabling x11 forwarding"
	@echo "#define DROPBEAR_X11FWD 0" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: enabling x11 forwarding"
	@echo "#define DROPBEAR_X11FWD 1" >> $(DROPBEAR_LOCALOPTIONS)
endif

ifdef PTXCONF_DROPBEAR_DIS_TCP
	@echo "ptxdist: disabling tcp forwarding"
	@echo "#define DROPBEAR_CLI_LOCALTCPFWD 0" >> $(DROPBEAR_LOCALOPTIONS)
	@echo "#define DROPBEAR_CLI_REMOTETCPFWD 0" >> $(DROPBEAR_LOCALOPTIONS)
	@echo "#define DROPBEAR_SVR_LOCALTCPFWD 0" >> $(DROPBEAR_LOCALOPTIONS)
	@echo "#define DROPBEAR_SVR_REMOTETCPFWD 0" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: enabling tcp forwarding"
	@echo "#define DROPBEAR_CLI_LOCALTCPFWD 1" >> $(DROPBEAR_LOCALOPTIONS)
	@echo "#define DROPBEAR_CLI_REMOTETCPFWD 1" >> $(DROPBEAR_LOCALOPTIONS)
	@echo "#define DROPBEAR_SVR_LOCALTCPFWD 1" >> $(DROPBEAR_LOCALOPTIONS)
	@echo "#define DROPBEAR_SVR_REMOTETCPFWD 1" >> $(DROPBEAR_LOCALOPTIONS)
endif

ifdef PTXCONF_DROPBEAR_DIS_AGENT
	@echo "ptxdist: disabling auth agent forwarding"
	@echo "#define DROPBEAR_SVR_AGENTFWD 0" >> $(DROPBEAR_LOCALOPTIONS)
	@echo "#define DROPBEAR_CLI_AGENTFWD 0" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: enabling auth agent forwarding"
	@echo "#define DROPBEAR_SVR_AGENTFWD 1" >> $(DROPBEAR_LOCALOPTIONS)
	@echo "#define DROPBEAR_CLI_AGENTFWD 1" >> $(DROPBEAR_LOCALOPTIONS)
endif

ifdef PTXCONF_DROPBEAR_DIS_SFTPSERVER
	@echo "ptxdist: disabling ftps server"
	@echo "#define DROPBEAR_SFTPSERVER 0" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: enabling ftps server"
	@echo "#define DROPBEAR_SFTPSERVER 1" >> $(DROPBEAR_LOCALOPTIONS)
	@echo "#define SFTPSERVER_PATH \"$(PTXCONF_DROPBEAR_SFTPSERVER)\"" >> $(DROPBEAR_LOCALOPTIONS)
endif

# encryption
ifdef PTXCONF_DROPBEAR_AES128
	@echo "ptxdist: enabling aes128"
	@echo "#define DROPBEAR_AES128 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling aes128"
	@echo "#define DROPBEAR_AES128 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

ifdef PTXCONF_DROPBEAR_3DES
	@echo "ptxdist: enabling 3des"
	@echo "#define DROPBEAR_3DES 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling 3des"
	@echo "#define DROPBEAR_3DES 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

ifdef PTXCONF_DROPBEAR_AES256
	@echo "ptxdist: enabling aes256"
	@echo "#define DROPBEAR_AES256 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling aes256"
	@echo "#define DROPBEAR_AES256 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

ifdef PTXCONF_DROPBEAR_TWOFISH256
	@echo "ptxdist: enabling twofish256"
	@echo "#define DROPBEAR_TWOFISH256 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling twofish256"
	@echo "#define DROPBEAR_TWOFISH256 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

ifdef PTXCONF_DROPBEAR_TWOFISH128
	@echo "ptxdist: enabling twofish128"
	@echo "#define DROPBEAR_TWOFISH128 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling twofish128"
	@echo "#define DROPBEAR_TWOFISH128 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

ifdef PTXCONF_DROPBEAR_CHACHA20POLY1305
	@echo "ptxdist: enabling chacha20poly1305"
	@echo "#define DROPBEAR_CHACHA20POLY1305 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling chacha20poly1305"
	@echo "#define DROPBEAR_CHACHA20POLY1305 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

ifdef PTXCONF_DROPBEAR_DH_GROUP14_SHA1
	@echo "ptxdist: enabling dh group14 sha1"
	@echo "#define DROPBEAR_DH_GROUP14_SHA1 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling dh group14 sha1"
	@echo "#define DROPBEAR_DH_GROUP14_SHA1 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

ifdef PTXCONF_DROPBEAR_DH_GROUP14_SHA256
	@echo "ptxdist: enabling dh group14 sha256"
	@echo "#define DROPBEAR_DH_GROUP14_SHA256 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling dh group14 sha256"
	@echo "#define DROPBEAR_DH_GROUP14_SHA256 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

ifdef PTXCONF_DROPBEAR_DH_GROUP16_SHA256
	@echo "ptxdist: enabling dh group16 sha256"
	@echo "#define DROPBEAR_DH_GROUP16 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling dh group16 sha256"
	@echo "#define DROPBEAR_DH_GROUP16 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

ifdef PTXCONF_DROPBEAR_DH_GROUP1_SHA1
	@echo "ptxdist: enabling dh group1 sha1"
	@echo "#define DROPBEAR_DH_GROUP1 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling dh group1 sha1"
	@echo "#define DROPBEAR_DH_GROUP1 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

# ciphers
ifdef PTXCONF_DROPBEAR_CBC_CIPHERS
	@echo "ptxdist: enabling cbc ciphers"
	@echo "#define DROPBEAR_ENABLE_CBC_MODE 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling cbc ciphers"
	@echo "#define DROPBEAR_ENABLE_CBC_MODE 0" >> $(DROPBEAR_LOCALOPTIONS)
endif


ifdef PTXCONF_DROPBEAR_CTR_CIPHERS
	@echo "ptxdist: enabling ctr ciphers"
	@echo "#define DROPBEAR_ENABLE_CTR_MODE 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling ctr ciphers"
	@echo "#define DROPBEAR_ENABLE_CTR_MODE 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

ifdef PTXCONF_DROPBEAR_GCM_CIPHERS
	@echo "ptxdist: enabling gcm ciphers"
	@echo "#define DROPBEAR_ENABLE_GCM_MODE 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling gcm ciphers"
	@echo "#define DROPBEAR_ENABLE_GCM_MODE 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

# integrity
ifdef PTXCONF_DROPBEAR_SHA1
	@echo "ptxdist: enabling sha1"
	@echo "#define DROPBEAR_SHA1_HMAC 1" >> $(DROPBEAR_LOCALOPTIONS)
	@echo "#define DROPBEAR_DH_GROUP1 1" >> $(DROPBEAR_LOCALOPTIONS)
	@echo "#define DROPBEAR_DH_GROUP14_SHA1 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling sha1"
	@echo "#define DROPBEAR_SHA1_HMAC 0" >> $(DROPBEAR_LOCALOPTIONS)
	@echo "#define DROPBEAR_DH_GROUP1 0" >> $(DROPBEAR_LOCALOPTIONS)
	@echo "#define DROPBEAR_DH_GROUP14_SHA1 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

ifdef PTXCONF_DROPBEAR_SHA1_96
	@echo "ptxdist: enabling sha1-96"
	@echo "#define DROPBEAR_SHA1_96_HMAC 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling sha1-96"
	@echo "#define DROPBEAR_SHA1_96_HMAC 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

ifdef PTXCONF_DROPBEAR_SHA256
	@echo "ptxdist: enabling sha256"
	@echo "#define DROPBEAR_SHA2_256_HMAC 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling sha256"
	@echo "#define DROPBEAR_SHA2_256_HMAC 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

ifdef PTXCONF_DROPBEAR_SHA512
	@echo "ptxdist: enabling sha512"
	@echo "#define DROPBEAR_SHA2_512_HMAC 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling sha512"
	@echo "#define DROPBEAR_SHA2_512_HMAC 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

# host key / public key
ifdef PTXCONF_DROPBEAR_DSS
	@echo "ptxdist: enabling dss"
	@echo "#define DROPBEAR_DSS 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling dss"
	@echo "#define DROPBEAR_DSS 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

ifdef PTXCONF_DROPBEAR_RSA
	@echo "ptxdist: enabling rsa"
	@echo "#define DROPBEAR_RSA 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling rsa"
	@echo "#define DROPBEAR_RSA 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

ifdef PTXCONF_DROPBEAR_ECDSA
	@echo "ptxdist: enabling ecdsa"
	@echo "#define DROPBEAR_ECDSA 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling ecdsa"
	@echo "#define DROPBEAR_ECDSA 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

ifdef PTXCONF_DROPBEAR_ED25519
	@echo "ptxdist: enabling ed25519"
	@echo "#define DROPBEAR_ED25519 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling ed25519"
	@echo "#define DROPBEAR_ED25519 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

# key exchange algorithm
ifdef PTXCONF_DROPBEAR_ECDH
	@echo "ptxdist: enabling ecdh"
	@echo "#define DROPBEAR_ECDH 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling ecdh"
	@echo "#define DROPBEAR_ECDH 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

ifdef PTXCONF_DROPBEAR_CURVE25519
	@echo "ptxdist: enabling curve25519"
	@echo "#define DROPBEAR_CURVE25519 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling curve25519"
	@echo "#define DROPBEAR_CURVE25519 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

#	# DROPBEAR_KEXGUESS2 is not within defaultoptions.h nor
#	# local_options and only in sysoptions.h; we set it anyway
#	# to disable it (proprietary extension of dropbear)
ifdef PTXCONF_DROPBEAR_KEXGUESS2
	@echo "ptxdist: enabling kexguess2 extension"
	@echo "#define DROPBEAR_KEXGUESS2 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling kexguess2 extension"
	@echo "#define DROPBEAR_KEXGUESS2 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

# authentication types
ifdef PTXCONF_DROPBEAR_PASSWD
ifndef PTXCONF_DROPBEAR_PAM
	@echo "ptxdist: enabling passwd"
	@echo "#define DROPBEAR_SVR_PASSWORD_AUTH 1" >> $(DROPBEAR_LOCALOPTIONS)
	@echo "#define DROPBEAR_SVR_PAM_AUTH 0" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: enabling pam support"
	@echo "#define DROPBEAR_SVR_PASSWORD_AUTH 0" >> $(DROPBEAR_LOCALOPTIONS)
	@echo "#define DROPBEAR_SVR_PAM_AUTH 1" >> $(DROPBEAR_LOCALOPTIONS)
endif # PTXCONF_DROPBEAR_PAM
else
	@echo "ptxdist: disabling passwd"
	@echo "#define DROPBEAR_SVR_PASSWORD_AUTH 0" >> $(DROPBEAR_LOCALOPTIONS)
	@echo "#define DROPBEAR_SVR_PAM_AUTH 0" >> $(DROPBEAR_LOCALOPTIONS)
	@echo "#define DROPBEAR_CLI_PASSWORD_AUTH 0" >> $(DROPBEAR_LOCALOPTIONS)
endif # PTXCONF_DROPBEAR_PASSWD

ifdef PTXCONF_DROPBEAR_PUBKEY
	@echo "ptxdist: enabling pubkey"
	@echo "#define DROPBEAR_SVR_PUBKEY_AUTH 1" >> $(DROPBEAR_LOCALOPTIONS)
	@echo "#define DROPBEAR_CLI_PUBKEY_AUTH 1" >> $(DROPBEAR_LOCALOPTIONS)
else
	@echo "ptxdist: disabling pubkey"
	@echo "#define DROPBEAR_SVR_PUBKEY_AUTH 0" >> $(DROPBEAR_LOCALOPTIONS)
	@echo "#define DROPBEAR_CLI_PUBKEY_AUTH 0" >> $(DROPBEAR_LOCALOPTIONS)
endif

	@echo "ptxdist: disabling support for U2F-/FIDO2-backed keys"
	@echo "#define DROPBEAR_SK_ECDSA 0" >> $(DROPBEAR_LOCALOPTIONS)
	@echo "#define DROPBEAR_SK_ED25519 0" >> $(DROPBEAR_LOCALOPTIONS)

	@echo "ptxdist: disabling REEXEC"
	@echo "#define DROPBEAR_REEXEC 0" >> $(DROPBEAR_LOCALOPTIONS)

	@$(call touch)

DROPBEAR_MAKE_OPT	:= all scp
DROPBEAR_INSTALL_OPT	:= install inst_scp

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

ifdef PTXCONF_DROPBEAR_DSS
DROPBEAR_KEY_TYPES	+= dss
endif
ifdef PTXCONF_DROPBEAR_RSA
DROPBEAR_KEY_TYPES	+= rsa
endif
ifdef PTXCONF_DROPBEAR_ECDSA
DROPBEAR_KEY_TYPES	+= ecdsa
endif
ifdef PTXCONF_DROPBEAR_ED25519
DROPBEAR_KEY_TYPES	+= ed25519
endif

$(STATEDIR)/dropbear.targetinstall:
	@$(call targetinfo)

	@$(call install_init, dropbear)
	@$(call install_fixup, dropbear,PRIORITY,optional)
	@$(call install_fixup, dropbear,SECTION,base)
	@$(call install_fixup, dropbear,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, dropbear,DESCRIPTION,missing)

#	# Replaced by WAGO specific solution
#	#	@$(call install_alternative, dropbear, 0, 0, 0644, /usr/lib/init/dropbear.sh)
#	#	@$(call install_replace, dropbear, /usr/lib/init/dropbear.sh, \
#	#		@KEYDIR@, $(PTXCONF_DROPBEAR_KEY_DIR))
#	#	@$(call install_replace, dropbear, /usr/lib/init/dropbear.sh, \
#	#		@KEYTYPES@, '$(DROPBEAR_KEY_TYPES)')

ifdef PTXCONF_DROPBEAR_DROPBEAR
	@$(call install_copy, dropbear, 0, 0, 0755, -, \
		/usr/sbin/dropbear)
endif

ifdef PTXCONF_DROPBEAR_DROPBEAR_KEY
	@$(call install_copy, dropbear, 0, 0, 0755, -, \
		/usr/bin/dropbearkey)

#	# Replaced by WAGO specific solution
#	#	@$(call install_alternative, dropbear, 0, 0, 0755, /etc/rc.once.d/dropbear, n)
endif

ifdef PTXCONF_DROPBEAR_DROPBEAR_CONVERT
	@$(call install_copy, dropbear, 0, 0, 0755, -, \
		/usr/bin/dropbearconvert)
endif

ifdef PTXCONF_DROPBEAR_SCP
	@$(call install_copy, dropbear, 0, 0, 0755, -, \
		/usr/bin/scp)
	@$(call install_copy, dropbear, 0, 0, 0755, -, \
		/usr/bin/dbclient)
endif

#       #
#       # install backup/restore script
#       #

ifdef PTXCONF_CT_SETTINGS_BACKUP
	@$(call install_alternative, dropbear, 0, 0, 0750, /etc/config-tools/backup-restore/backup_dropbear_host_keys)
endif

#	#
#	# busybox init: start script
#	#

ifdef PTXCONF_DROPBEAR_STARTSCRIPT
	@$(call install_alternative, dropbear, 0, 0, 0755, /etc/init.d/dropbear, n)

ifneq ($(call remove_quotes,$(PTXCONF_DROPBEAR_BBINIT_LINK)),)
	@$(call install_link, dropbear, \
		../init.d/dropbear, \
		/etc/rc.d/$(PTXCONF_DROPBEAR_BBINIT_LINK))
endif
endif

#	#
#	# configuration
#	#

	@$(call install_alternative, dropbear, 0, 0, 0600, /etc/dropbear/dropbear.conf)

ifdef PTXCONF_DROPBEAR_PAM
	@$(call install_alternative, dropbear, 0, 0, 0644, /etc/pam.d/sshd)
endif

#	#
#	# install keys
#	#

	@$(call install_copy, dropbear, 0, 0, 0755, $(PTXCONF_DROPBEAR_KEY_DIR))

ifdef PTXCONF_DROPBEAR_RSA
	@$(call install_copy, dropbear, 0, 0, 0400, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/dropbear/dropbear_rsa_host_key, \
		/etc/dropbear/dropbear_rsa_host_key)
endif

ifdef PTXCONF_DROPBEAR_DSS
	@$(call install_copy, dropbear, 0, 0, 0400, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/dropbear/dropbear_dss_host_key, \
		/etc/dropbear/dropbear_dss_host_key)
endif

ifdef PTXCONF_DROPBEAR_ECDSA
	@$(call install_copy, dropbear, 0, 0, 0400, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/dropbear/dropbear_ecdsa_host_key, \
		/etc/dropbear/dropbear_ecdsa_host_key)
endif

ifdef PTXCONF_DROPBEAR_ED25519
	@$(call install_copy, dropbear, 0, 0, 0400, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/dropbear/dropbear_ed25519_host_key, \
		/etc/dropbear/dropbear_ed25519_host_key)
endif

	@$(call install_finish, dropbear)

	@$(call touch)

# vim: syntax=make
