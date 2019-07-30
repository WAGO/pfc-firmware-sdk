# -*-makefile-*-
#
# Copyright (C) 2003 by Marc Kleine-Budde <kleine-budde@gmx.de> for
#                       Pengutronix e.K. <info@pengutronix.de>, Germany
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
PACKAGES-$(PTXCONF_DROPBEAR) += dropbear

#
# Paths and names
#
DROPBEAR_VERSION	:= 2018.76
DROPBEAR_MD5		:= c3912f7fcdcc57c99937e4a79480d2c2
DROPBEAR		:= dropbear-$(DROPBEAR_VERSION)
DROPBEAR_SUFFIX		:= tar.bz2
DROPBEAR_URL		:= http://matt.ucc.asn.au/dropbear/releases/$(DROPBEAR).$(DROPBEAR_SUFFIX)
DROPBEAR_SOURCE		:= $(SRCDIR)/$(DROPBEAR).$(DROPBEAR_SUFFIX)
DROPBEAR_DIR		:= $(BUILDDIR)/$(DROPBEAR)

# ----------------------------------------------------------------------------
# Help functions
# ----------------------------------------------------------------------------

#
# enable_dropbear_option
#
# Enables a dropbear option define, set to '1'
#
# $1: file
# $2: parameter
#
enable_dropbear_option =                                                                                      \
        FILENAME="$(strip $(1))";                                                               \
        PARAMETER="$(strip $(2))";                                                              \
        sed -i -e                                                                               \
                "s,\#define [ ]*$$PARAMETER .*,\#define $$PARAMETER 1,"                         \
                "$$FILENAME"

#
# disable_dropbear_option
#
# Disables a dropbear option define, set to '0'
#
# $1: file
# $2: parameter
#
disable_dropbear_option =                                                                                     \
        FILENAME="$(strip $(1))";                                                               \
        PARAMETER="$(strip $(2))";                                                              \
        sed -i -e                                                                               \
                "s,\#define [ ]*$$PARAMETER .*,\#define $$PARAMETER 0,"                         \
                "$$FILENAME"

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
DROPBEAR_CONF_TOOL	:= autoconf
DROPBEAR_CONF_OPT 	:= \
	$(CROSS_AUTOCONF_USR) \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--$(call ptx/endis, PTXCONF_DROPBEAR_ZLIB)-zlib \
	--disable-pam \
	--$(call ptx/disen, PTXCONF_DROPBEAR_DIS_OPENPTY)-openpty \
	--$(call ptx/disen, PTXCONF_DROPBEAR_DIS_SYSLOG)-syslog \
	--enable-shadow \
	--enable-bundled-libtom \
	--$(call ptx/disen, PTXCONF_DROPBEAR_DIS_LASTLOG)-lastlog \
	--$(call ptx/disen, PTXCONF_DROPBEAR_DIS_UTMP)-utmp \
	--$(call ptx/disen, PTXCONF_DROPBEAR_DIS_UTMPX)-utmpx \
	--$(call ptx/disen, PTXCONF_DROPBEAR_DIS_WTMP)-wtmp \
	--$(call ptx/disen, PTXCONF_DROPBEAR_DIS_WTMPX)-wtmpx \
	--disable-loginfunc \
	--$(call ptx/disen, PTXCONF_DROPBEAR_DIS_PUTUTLINE)-pututline \
	--$(call ptx/disen, PTXCONF_DROPBEAR_DIS_PUTUTXLINE)-pututxline

$(STATEDIR)/dropbear.prepare:
	@$(call targetinfo)
	@$(call world/prepare, DROPBEAR)

ifdef PTXCONF_DROPBEAR_DIS_X11
	@echo "ptxdist: disabling x11 forwarding"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_X11FWD)
else
	@echo "ptxdist: enabling x11 forwarding"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_X11FWD)
endif

ifdef PTXCONF_DROPBEAR_DIS_TCP
	@echo "ptxdist: disabling tcp"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_CLI_LOCALTCPFWD)
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_CLI_REMOTETCPFWD)
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_SVR_LOCALTCPFWD)
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_SVR_REMOTETCPFWD)
else
	@echo "ptxdist: enabling tcp"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_CLI_LOCALTCPFWD)
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_CLI_REMOTETCPFWD)
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_SVR_LOCALTCPFWD)
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_SVR_REMOTETCPFWD)
endif

ifdef PTXCONF_DROPBEAR_DIS_AGENT
	@echo "ptxdist: disabling agent"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_SVR_AGENTFWD)
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_CLI_AGENTFWD)
else
	@echo "ptxdist: enabling agent"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_SVR_AGENTFWD)
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_CLI_AGENTFWD)
endif


ifdef PTXCONF_DROPBEAR_AES128
	@echo "ptxdist: enabling aes128"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_AES128)
else
	@echo "ptxdist: disabling aes128"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_AES128)
endif

ifdef PTXCONF_DROPBEAR_3DES
	@echo "ptxdist: enabling 3des"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_3DES)
else
	@echo "ptxdist: disabling 3des"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_3DES)
endif

ifdef PTXCONF_DROPBEAR_AES256
	@echo "ptxdist: enabling aes256"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_AES256)
else
	@echo "ptxdist: disabling aes256"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_AES256)
endif

ifdef PTXCONF_DROPBEAR_BLOWFISH
	@echo "ptxdist: enabling blowfish"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_BLOWFISH)
else
	@echo "ptxdist: disabling blowfish"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_BLOWFISH)
endif

ifdef PTXCONF_DROPBEAR_TWOFISH256
	@echo "ptxdist: enabling twofish256"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_TWOFISH256)
else
	@echo "ptxdist: disabling twofish256"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_TWOFISH256)
endif

ifdef PTXCONF_DROPBEAR_TWOFISH128
	@echo "ptxdist: enabling twofish128"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_TWOFISH128)
else
	@echo "ptxdist: disabling twofish128"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_TWOFISH128)
endif

ifdef PTXCONF_DROPBEAR_CBC_CIPHERS
	@echo "ptxdist: enabling cbc ciphers"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_ENABLE_CBC_MODE)
else
	@echo "ptxdist: disabling cbc ciphers"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_ENABLE_CBC_MODE)
endif

ifdef PTXCONF_DROPBEAR_CTR_CIPHERS
	@echo "ptxdist: enabling ctr ciphers"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_ENABLE_CTR_MODE)
else
	@echo "ptxdist: disabling ctr ciphers"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_ENABLE_CTR_MODE)
endif

ifdef PTXCONF_DROPBEAR_SHA1
	@echo "ptxdist: enabling sha1"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_SHA1_HMAC)
else
	@echo "ptxdist: disabling sha1"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_SHA1_HMAC)
endif

ifdef PTXCONF_DROPBEAR_SHA1_96
	@echo "ptxdist: enabling sha1-96"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_SHA1_96_HMAC)
else
	@echo "ptxdist: disabling sha1-96"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_SHA1_96_HMAC)
endif

ifdef PTXCONF_DROPBEAR_SHA256
	@echo "ptxdist: enabling sha256"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_SHA2_256_HMAC)
else
	@echo "ptxdist: disabling sha256"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_SHA2_256_HMAC)
endif

ifdef PTXCONF_DROPBEAR_SHA512
	@echo "ptxdist: enabling sha512"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_SHA2_512_HMAC)
else
	@echo "ptxdist: disabling sha512"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_SHA2_512_HMAC)
endif

ifdef PTXCONF_DROPBEAR_MD5
	@echo "ptxdist: enabling md5"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_MD5_HMAC)
else
	@echo "ptxdist: disabling md5"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_MD5_HMAC)
endif


ifdef PTXCONF_DROPBEAR_RSA
	@echo "ptxdist: enabling rsa"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_RSA)
else
	@echo "ptxdist: disabling rsa"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_RSA)
endif

ifdef PTXCONF_DROPBEAR_DSS
	@echo "ptxdist: enabling dss"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_DSS)
else
	@echo "ptxdist: disabling dss"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_DSS)
endif

ifdef PTXCONF_DROPBEAR_ECDSA
	@echo "ptxdist: enabling ecdsa"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_ECDSA)
else
	@echo "ptxdist: disabling ecdsa"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_ECDSA)
endif

ifdef PTXCONF_DROPBEAR_DH_GROUP14_SHA1
	@echo "ptxdist: enabling dh group14 sha1"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_DH_GROUP14_SHA1)
else
	@echo "ptxdist: disabling dh group14 sha1"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_DH_GROUP14_SHA1)
endif

ifdef PTXCONF_DROPBEAR_DH_GROUP14_SHA256
	@echo "ptxdist: enabling dh group14 sha256"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_DH_GROUP14_SHA256)
else
	@echo "ptxdist: disabling dh group14 sha256"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_DH_GROUP14_SHA256)
endif

ifdef PTXCONF_DROPBEAR_DH_GROUP16_SHA256
	@echo "ptxdist: enabling dh group16 sha256"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_DH_GROUP16)
else
	@echo "ptxdist: disabling dh group16 sha256"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_DH_GROUP16)
endif

ifdef PTXCONF_DROPBEAR_ECDH
	@echo "ptxdist: enabling ecdh"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_ECDH)
else
	@echo "ptxdist: disabling ecdh"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_ECDH)
endif

ifdef PTXCONF_DROPBEAR_CURVE25519
	@echo "ptxdist: enabling curve25519"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_CURVE25519)
else
	@echo "ptxdist: disabling curve25519"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_CURVE25519)
endif

ifdef PTXCONF_DROPBEAR_DH_GROUP1_SHA1
	@echo "ptxdist: enabling dh group1 sha1"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_DH_GROUP1)
else
	@echo "ptxdist: disabling dh group1 sha1"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_DH_GROUP1)
endif

ifdef PTXCONF_DROPBEAR_PASSWD
	@echo "ptxdist: enabling passwd"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_SVR_PASSWORD_AUTH)
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_CLI_PASSWORD_AUTH)
else
	@echo "ptxdist: disabling passwd"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_SVR_PASSWORD_AUTH)
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_CLI_PASSWORD_AUTH)
endif

ifdef PTXCONF_DROPBEAR_PUBKEY
	@echo "ptxdist: enabling pubkey"
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_SVR_PUBKEY_AUTH)
	@$(call enable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_CLI_PUBKEY_AUTH)
else
	@echo "ptxdist: disabling pubkey"
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_SVR_PUBKEY_AUTH)
	@$(call disable_dropbear_option, $(DROPBEAR_DIR)/localoptions.h,DROPBEAR_CLI_PUBKEY_AUTH)
endif

	@$(call touch)

DROPBEAR_MAKE_OPT	:= all scp
DROPBEAR_INSTALL_OPT	:= install inst_scp

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/dropbear.targetinstall:
	@$(call targetinfo)

	@$(call install_init, dropbear)
	@$(call install_fixup, dropbear,PRIORITY,optional)
	@$(call install_fixup, dropbear,SECTION,base)
	@$(call install_fixup, dropbear,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, dropbear,DESCRIPTION,missing)

ifdef PTXCONF_DROPBEAR_DROPBEAR
	@$(call install_copy, dropbear, 0, 0, 0755, -, \
		/usr/sbin/dropbear)
endif

ifdef PTXCONF_DROPBEAR_DROPBEAR_KEY
	@$(call install_copy, dropbear, 0, 0, 0755, -, \
		/usr/bin/dropbearkey)
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

#	#
#	# busybox init: start script
#	#

ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_DROPBEAR_STARTSCRIPT
	@$(call install_alternative, dropbear, 0, 0, 0755, /etc/init.d/dropbear, n)
	@$(call install_replace, dropbear, /etc/init.d/dropbear, \
		@KEYDIR@, $(PTXCONF_DROPBEAR_KEY_DIR))
#	@$(call install_alternative, dropbear, 0, 0, 0755, /etc/rc.once.d/dropbear, n)
#	@$(call install_replace, dropbear, /etc/rc.once.d/dropbear, \
#		@KEYDIR@, $(PTXCONF_DROPBEAR_KEY_DIR))

ifneq ($(call remove_quotes,$(PTXCONF_DROPBEAR_BBINIT_LINK)),)
	@$(call install_link, dropbear, \
		../init.d/dropbear, \
		/etc/rc.d/$(PTXCONF_DROPBEAR_BBINIT_LINK))
endif
endif
endif

	@$(call install_copy, dropbear, 0, 0, 0755, $(PTXCONF_DROPBEAR_KEY_DIR))
	@$(call install_copy, dropbear, 0, 0, 0400, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/dropbear/dropbear_rsa_host_key, \
		/etc/dropbear/dropbear_rsa_host_key)
	@$(call install_copy, dropbear, 0, 0, 0400, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/dropbear/dropbear_dss_host_key, \
		/etc/dropbear/dropbear_dss_host_key)
	@$(call install_copy, dropbear, 0, 0, 0400, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/dropbear/dropbear_ecdsa_host_key, \
		/etc/dropbear/dropbear_ecdsa_host_key)
	@$(call install_alternative, dropbear, 0, 0, 0600, \
		/etc/dropbear/dropbear.conf)

	@$(call install_finish, dropbear)

	@$(call touch)

# vim: syntax=make
