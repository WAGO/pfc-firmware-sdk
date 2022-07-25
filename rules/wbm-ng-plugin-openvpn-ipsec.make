# -*-makefile-*-
#
# Copyright (C) 2017 by Marius Hellmeier (marius.hellmeier@wago.com), WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_OPENVPN_IPSEC) += wbm-ng-plugin-openvpn-ipsec

#
# Paths and names
#
WBM_NG_PLUGIN_OPENVPN_IPSEC_VERSION        := 1.1.1
WBM_NG_PLUGIN_OPENVPN_IPSEC                := wbm-openvpn-ipsec-$(WBM_NG_PLUGIN_OPENVPN_IPSEC_VERSION)
WBM_NG_PLUGIN_OPENVPN_IPSEC_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_OPENVPN_IPSEC)
WBM_NG_PLUGIN_OPENVPN_IPSEC_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_OPENVPN_IPSEC_URL))
WBM_NG_PLUGIN_OPENVPN_IPSEC_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_OPENVPN_IPSEC)$(WBM_NG_PLUGIN_OPENVPN_IPSEC_SUFFIX)
WBM_NG_PLUGIN_OPENVPN_IPSEC_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_OPENVPN_IPSEC_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_OPENVPN_IPSEC_MD5_FILE))
WBM_NG_PLUGIN_OPENVPN_IPSEC_MD5_FILE       := $(WBM_NG_PLUGIN_OPENVPN_IPSEC_SOURCE).md5
WBM_NG_PLUGIN_OPENVPN_IPSEC_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_OPENVPN_IPSEC_URL))
WBM_NG_PLUGIN_OPENVPN_IPSEC_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-openvpn-ipsec
WBM_NG_PLUGIN_OPENVPN_IPSEC_DIR            := $(WBM_NG_PLUGIN_OPENVPN_IPSEC_BUILDROOT_DIR)
WBM_NG_PLUGIN_OPENVPN_IPSEC_LICENSE        := unknown
WBM_NG_PLUGIN_OPENVPN_IPSEC_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_OPENVPN_IPSEC_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-openvpn-ipsec
else
WBM_NG_PLUGIN_OPENVPN_IPSEC_TARGET_DIR     := /var/www/wbm/plugins/wbm-openvpn-ipsec
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_OPENVPN_IPSEC_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_OPENVPN_IPSEC_URL)' '$@' '$(WBM_NG_PLUGIN_OPENVPN_IPSEC_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-openvpn-ipsec.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-openvpn-ipsec.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-openvpn-ipsec.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-openvpn-ipsec.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-openvpn-ipsec)
	@$(call install_fixup, wbm-ng-plugin-openvpn-ipsec, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-openvpn-ipsec, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-openvpn-ipsec, AUTHOR,"Marius Hellmeier, WAGO GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-openvpn-ipsec, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-openvpn-ipsec, 0, 0, 0755, $(WBM_NG_PLUGIN_OPENVPN_IPSEC_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_OPENVPN_IPSEC_DIR) && \
	for object in $$( find ./* \( -path './series' -o -path './.ptxdist*' -o -path './.pc*' \) -prune -o -print ); do \
		if test -f $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-openvpn-ipsec, 0, 0, 0644, $(WBM_NG_PLUGIN_OPENVPN_IPSEC_DIR)/$$object, $(WBM_NG_PLUGIN_OPENVPN_IPSEC_TARGET_DIR)/$$object); \
		elif test -d $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-openvpn-ipsec, 0, 0, 0755, $(WBM_NG_PLUGIN_OPENVPN_IPSEC_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-openvpn-ipsec)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-openvpn-ipsec.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_OPENVPN_IPSEC_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_OPENVPN_IPSEC)
	@rm -rf $(WBM_NG_PLUGIN_OPENVPN_IPSEC_BUILDROOT_DIR)

# vim: syntax=make
