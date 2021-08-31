# -*-makefile-*-
#
# Copyright (C) 2015 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CA_CERTIFICATES) += ca-certificates

#
# Paths and names
#
CA_CERTIFICATES_VERSION		:= NSS_3_60_1_RTM
CA_CERTIFICATES_MD5		:= e754eded722beba13b7ea6573d0f3801
CA_CERTIFICATES			:= ca-certificates-$(CA_CERTIFICATES_VERSION)
CA_CERTIFICATES_SUFFIX		:= txt
CA_CERTIFICATES_URL		:= https://hg.mozilla.org/projects/nss/raw-file/$(CA_CERTIFICATES_VERSION)/lib/ckfw/builtins/certdata.$(CA_CERTIFICATES_SUFFIX)
CA_CERTIFICATES_SOURCE		:= $(SRCDIR)/certdata-$(CA_CERTIFICATES_VERSION).$(CA_CERTIFICATES_SUFFIX)
CA_CERTIFICATES_DIR		:= $(BUILDDIR)/$(CA_CERTIFICATES)
CA_CERTIFICATES_LICENSE		:= MPL-2.0
# Use '=' to delay $(shell ...) calls until this is needed
ifeq ($(PTXCONF_CONFIGFILE_VERSION), "2020.08.0")
	CA_CERTIFICATES_CERTDATA2PEM	 = $(call ptx/in-path, PTXDIST_PATH_SCRIPTS, certdata2pem.py)
	CA_CERTIFICATES_BLACKLIST	 = $(call ptx/get-alternative, config/ca-certificates, blacklist.txt)
else
	CA_CERTIFICATES_CERTDATA2PEM	 = $(shell ptxd_in_path PTXDIST_PATH_SCRIPTS certdata2pem.py && echo "$${ptxd_reply}")
	CA_CERTIFICATES_BLACKLIST	 = $(shell ptxd_get_alternative config/ca-certificates blacklist.txt && echo "$${ptxd_reply}")
endif

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/ca-certificates.extract:
	@$(call targetinfo)
	@rm -rf $(CA_CERTIFICATES_DIR)
	@mkdir -p $(CA_CERTIFICATES_DIR)
	@cp -v $(CA_CERTIFICATES_SOURCE) $(CA_CERTIFICATES_DIR)/certdata.txt
	@cp -v $(CA_CERTIFICATES_BLACKLIST) $(CA_CERTIFICATES_DIR)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

CA_CERTIFICATES_CONF_TOOL	:= NO

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/ca-certificates.compile:
	@$(call targetinfo)
ifeq ($(PTXCONF_CONFIGFILE_VERSION), "2020.08.0")
	@$(call world/execute, CA_CERTIFICATES, $(CA_CERTIFICATES_CERTDATA2PEM))
else
	@cd $(CA_CERTIFICATES_DIR) && $(CA_CERTIFICATES_CERTDATA2PEM)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ca-certificates.install:
	@$(call targetinfo)
ifeq ($(PTXCONF_CONFIGFILE_VERSION), "2020.08.0")	
	@$(call world/execute, CA_CERTIFICATES, \
		install -d -m 0755 $(CA_CERTIFICATES_PKGDIR)/etc/ssl/certs/)
else
	@rm -rf $(CA_CERTIFICATES_PKGDIR)
	@install -d -m 0755 $(CA_CERTIFICATES_PKGDIR)/etc/ssl/certs/
endif
ifdef PTXCONF_CA_CERTIFICATES_BUNDLE
	@for crt in $(CA_CERTIFICATES_DIR)/*.crt; do \
		sed -e '$$a\\' "$${crt}" >> \
			$(CA_CERTIFICATES_PKGDIR)/etc/ssl/certs/ca-certificates.crt; \
	done
endif
ifdef PTXCONF_CA_CERTIFICATES_CERTS
	@install -m 0644 $(CA_CERTIFICATES_DIR)/*.crt \
		$(CA_CERTIFICATES_PKGDIR)/etc/ssl/certs/
ifeq ($(PTXCONF_CONFIGFILE_VERSION), "2020.08.0")			
	@$(call execute, CA_CERTIFICATES, \
		OPENSSL_CONF=$(SYSROOT)/usr/lib/ssl/openssl.cnf SSL_CERT_FILE="" \
		c_rehash $(CA_CERTIFICATES_PKGDIR)/etc/ssl/certs/)
else
		@OPENSSL_CONF=$(SYSROOT)/usr/lib/ssl/openssl.cnf SSL_CERT_FILE="" \
		c_rehash $(CA_CERTIFICATES_PKGDIR)/etc/ssl/certs/
endif
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ca-certificates.targetinstall:
	@$(call targetinfo)

	@$(call install_init, ca-certificates)
	@$(call install_fixup, ca-certificates,PRIORITY,optional)
	@$(call install_fixup, ca-certificates,SECTION,base)
	@$(call install_fixup, ca-certificates,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, ca-certificates,DESCRIPTION,missing)

	@$(call install_tree, ca-certificates, 0, 0, -, /etc/ssl/certs)

# FIXME: following 2 lines are the only difference towards upstream.
#        Are they required and, when yes, better to be put somewhere else (wago-custom-install?)
	mkdir -p $(PTXCONF_SYSROOT_TARGET)/usr/lib/ssl
	@$(call install_link, ca-certificates, ../../../etc/ssl/certs, /usr/lib/ssl/certs)

	@$(call install_finish, ca-certificates)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/ca-certificates.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, CA_CERTIFICATES)

# vim: syntax=make
