# -*-makefile-*-
#
# Copyright (C) 2007 by Carsten Schlote <c.schlote@konzeptpark.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_OPENVPN) += openvpn

#
# Paths and names
#
OPENVPN_VERSION		:= 2.4.6
OPENVPN_MD5		:= 3a1f3f63bdaede443b4df49957df9405
OPENVPN			:= openvpn-$(OPENVPN_VERSION)
OPENVPN_SUFFIX		:= tar.xz
OPENVPN_URL		:= http://swupdate.openvpn.org/community/releases/$(OPENVPN).$(OPENVPN_SUFFIX)
OPENVPN_SOURCE		:= $(SRCDIR)/$(OPENVPN).$(OPENVPN_SUFFIX)
OPENVPN_DIR		:= $(BUILDDIR)/$(OPENVPN)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

# these options install files from OPENVPN_DIR
ifneq ($(PTXCONF_OPENVPN_INSTALL_SAMPLE_CONFIGS)$(PTXCONF_OPENVPN_INSTALL_SAMPLE_SCRIPTS),)
OPENVPN_DEVPKG := NO
endif

OPENVPN_CONF_ENV	:= \
	$(CROSS_ENV) \
	IFCONFIG=/usr/sbin/ifconfig \
	IPROUTE=/usr/sbin/ip \
	NETSTAT=/usr/bin/netstat \
	ROUTE=/usr/sbin/route \
	SYSTEMD_ASK_PASSWORD=/usr/bin/systemd-ask-password

#
# autoconf
#
OPENVPN_CONF_TOOL	:= autoconf
OPENVPN_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--$(call ptx/endis, PTXCONF_OPENVPN_LZO)-lzo \
	--disable-lz4 \
	--disable-comp-stub \
	--enable-crypto \
	--enable-ofb-cfb \
	--disable-x509-alt-username \
	--enable-server \
	--disable-plugins \
	--enable-management \
	--disable-pkcs11 \
	--enable-fragment \
	--enable-multihome \
	--enable-port-share \
	--disable-debug \
	--$(call ptx/endis, PTXCONF_OPENVPN_SMALL)-small \
	--disable-iproute2 \
	--enable-def-auth \
	--enable-pf \
	--disable-plugin-auth-pam \
	--enable-plugin-down-root \
	--disable-strict \
	--disable-pedantic \
	--disable-werror \
	--disable-strict-options \
	--disable-selinux \
	--$(call ptx/endis, PTXCONF_OPENVPN_SYSTEMD)-systemd \
	--disable-async-push \
	--with-crypto-library=openssl

OPENVPN_INSTALL_SAMPLE_CONFIG_FILES := \
	client.conf loopback-client loopback-server README server.conf \
	static-home.conf static-office.conf tls-home.conf tls-office.conf \
	xinetd-client-config xinetd-server-config

OPENVPN_INSTALL_SAMPLE_CONFIG_SCRIPTS := \
	firewall.sh home.up office.up openvpn-shutdown.sh openvpn-startup.sh

OPENVPN_INSTALL_SAMPLE_SCRIPTS := bridge-start bridge-stop

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/openvpn.targetinstall:
	@$(call targetinfo)

	@$(call install_init, openvpn)
	@$(call install_fixup, openvpn,PRIORITY,optional)
	@$(call install_fixup, openvpn,SECTION,base)
	@$(call install_fixup, openvpn,AUTHOR,"Carsten Schlote <c.schlote@konzeptpark.de>")
	@$(call install_fixup, openvpn,DESCRIPTION,missing)

ifdef PTXCONF_OPENVPN_INSTALL_SAMPLE_CONFIGS
	@$(foreach file,$(OPENVPN_INSTALL_SAMPLE_CONFIG_FILES), \
		$(call install_copy, openvpn, 0, 0, 0644, \
		$(OPENVPN_DIR)/sample/sample-config-files/$(file), \
		/usr/share/openvpn/sample-config-files/$(file))$(ptx/nl))

	@$(foreach script,$(OPENVPN_INSTALL_SAMPLE_CONFIG_SCRIPTS), \
		$(call install_copy, openvpn, 0, 0, 0755, \
		$(OPENVPN_DIR)/sample/sample-config-files/$(script), \
		/usr/share/openvpn/sample-config-files/$(script))$(ptx/nl))
endif

ifdef PTXCONF_OPENVPN_INSTALL_SAMPLE_SCRIPTS
	@$(foreach script,$(OPENVPN_INSTALL_SAMPLE_SCRIPTS), \
		$(call install_copy, openvpn, 0, 0, 0755, \
		$(OPENVPN_DIR)/sample/sample-scripts/$(script), \
		/usr/share/openvpn/sample-scripts/$(script))$(ptx/nl))
endif

	@$(call install_copy, openvpn, 0, 0, 0755, -, /usr/sbin/openvpn)

	@$(call install_finish, openvpn)

	@$(call touch)

# vim: syntax=make

