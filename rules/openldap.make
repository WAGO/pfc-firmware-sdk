# -*-makefile-*-
#
# Copyright (C) 2012 by Bernhard Walle <bernhard@bwalle.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_OPENLDAP) += openldap

#
# Paths and names
#
OPENLDAP_VERSION	:= 2.4.31
OPENLDAP_LIBVERSION	:= 2.4
OPENLDAP_MD5		:= 804c6cb5698db30b75ad0ff1c25baefd
OPENLDAP		:= openldap-$(OPENLDAP_VERSION)
OPENLDAP_SUFFIX		:= tgz
OPENLDAP_URL		:= ftp://ftp.openldap.org/pub/OpenLDAP/openldap-release/$(OPENLDAP).$(OPENLDAP_SUFFIX)
OPENLDAP_SOURCE		:= $(SRCDIR)/$(OPENLDAP).$(OPENLDAP_SUFFIX)
OPENLDAP_DIR		:= $(BUILDDIR)/$(OPENLDAP)
# http://www.openldap.org/software/release/license.html
OPENLDAP_LICENSE	:= OpenLDAP Public License

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

OPENLDAP_TLS_CONF_OPT-$(PTXCONF_OPENLDAP_TLS_GNUTLS)	:= gnutls
OPENLDAP_TLS_CONF_OPT-$(PTXCONF_OPENLDAP_TLS_OPENSSL)	:= openssl
OPENLDAP_TLS_CONF_OPT-$(PTXCONF_OPENLDAP_TLS_DISABLED)	:= no

#
# autoconf
#
OPENLDAP_CONF_TOOL	:= autoconf
OPENLDAP_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--$(call ptx/endis, PTXCONF_GLOBAL_IPV6)-ipv6 \
	--enable-syslog \
	--enable-local \
	--enable-proctitle \
	--enable-dynamic \
	\
	--$(call ptx/endis, PTXCONF_OPENLDAP_SLAPD)-slapd \
	--disable-dynacl \
	--disable-aci \
	--enable-cleartext \
	--enable-crypt \
	--disable-lmpasswd \
	--disable-spasswd \
	--disable-modules \
	--enable-rewrite \
	--disable-rlookups \
	--disable-slapi \
	--disable-slp \
	--disable-wrappers \
	\
	--enable-hdb=no \
	--enable-dnssrv=no \
	--enable-bdb=no \
	--enable-ldap=yes \
	--enable-mdb=yes \
	--enable-meta=no \
	--enable-monitor=yes \
	--enable-ndb=no \
	--enable-null=yes \
	--enable-passwd=yes \
	--enable-perl=no \
	--enable-relay=yes \
	--enable-shell=no \
	--enable-sock=no \
	--enable-sql=no \
	\
	--enable-overlays=no \
	\
	--without-cyrus-sasl \
	--without-fetch \
	--with-yielding_select=yes \
	--$(call ptx/wwo, PTXCONF_OPENLDAP_THREADS)-threads \
	--with-tls=$(OPENLDAP_TLS_CONF_OPT-y)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/openldap.targetinstall:
	@$(call targetinfo)

	@$(call install_init, openldap)
	@$(call install_fixup, openldap,PRIORITY,optional)
	@$(call install_fixup, openldap,SECTION,base)
	@$(call install_fixup, openldap,AUTHOR,"Bernhard Walle <bernhard@bwalle.de>")
	@$(call install_fixup, openldap,DESCRIPTION,missing)

#	slapd

ifdef PTXCONF_OPENLDAP_SLAPD
	@$(call install_copy, openldap, 0, 0, 0755, -, /usr/libexec/slapd)
	@$(call install_link, openldap, ../libexec/slapd, /usr/bin/slapacl)
	@$(call install_link, openldap, ../libexec/slapd, /usr/bin/slapadd)
	@$(call install_link, openldap, ../libexec/slapd, /usr/bin/slapauth)
	@$(call install_link, openldap, ../libexec/slapd, /usr/bin/slapcat)
	@$(call install_link, openldap, ../libexec/slapd, /usr/bin/slapdn)
	@$(call install_link, openldap, ../libexec/slapd, /usr/bin/slapindex)
	@$(call install_link, openldap, ../libexec/slapd, /usr/bin/slappasswd)
	@$(call install_link, openldap, ../libexec/slapd, /usr/bin/slapschema)
	@$(call install_link, openldap, ../libexec/slapd, /usr/bin/slaptest)
endif

#	libraries

	@$(call install_lib, openldap, 0, 0, 0644, liblber-$(OPENLDAP_LIBVERSION))
	@$(call install_lib, openldap, 0, 0, 0644, libldap-$(OPENLDAP_LIBVERSION))
ifdef PTXCONF_OPENLDAP_THREADS
	@$(call install_lib, openldap, 0, 0, 0644, libldap_r-$(OPENLDAP_LIBVERSION))
endif

#	tools

ifdef PTXCONF_OPENLDAP_INSTALL_UTILITIES
	@$(call install_link, openldap, ldapmodify, /usr/bin/ldapadd)
	@$(call install_copy, openldap, 0, 0, 0755, -, /usr/bin/ldapcompare)
	@$(call install_copy, openldap, 0, 0, 0755, -, /usr/bin/ldapdelete)
	@$(call install_copy, openldap, 0, 0, 0755, -, /usr/bin/ldapexop)
	@$(call install_copy, openldap, 0, 0, 0755, -, /usr/bin/ldapmodify)
	@$(call install_copy, openldap, 0, 0, 0755, -, /usr/bin/ldapmodrdn)
	@$(call install_copy, openldap, 0, 0, 0755, -, /usr/bin/ldappasswd)
	@$(call install_copy, openldap, 0, 0, 0755, -, /usr/bin/ldapsearch)
	@$(call install_copy, openldap, 0, 0, 0755, -, /usr/bin/ldapurl)
	@$(call install_copy, openldap, 0, 0, 0755, -, /usr/bin/ldapwhoami)
endif

#	configuration files

	@$(call install_tree, openldap, 0, 0, -, /etc/openldap)
	@$(call install_alternative, openldap, 0, 0, 0644, /etc/openldap/ldap.conf)

	@$(call install_finish, openldap)

	@$(call touch)

# vim: syntax=make

