# -*-makefile-*-
#
# Copyright (C) 2017 by Marius Hellmeier (marius.hellmeier@wago.com), WAGO Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_SECURITY) += wbm-ng-plugin-security

#
# Paths and names
#
WBM_NG_PLUGIN_SECURITY_VERSION        := 1.3.0
WBM_NG_PLUGIN_SECURITY                := wbm-security-$(WBM_NG_PLUGIN_SECURITY_VERSION)
WBM_NG_PLUGIN_SECURITY_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_SECURITY)
WBM_NG_PLUGIN_SECURITY_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_SECURITY_URL))
WBM_NG_PLUGIN_SECURITY_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_SECURITY)$(WBM_NG_PLUGIN_SECURITY_SUFFIX)
WBM_NG_PLUGIN_SECURITY_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_SECURITY_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_SECURITY_MD5_FILE))
WBM_NG_PLUGIN_SECURITY_MD5_FILE       := $(WBM_NG_PLUGIN_SECURITY_SOURCE).md5
WBM_NG_PLUGIN_SECURITY_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_SECURITY_URL))
WBM_NG_PLUGIN_SECURITY_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-security
WBM_NG_PLUGIN_SECURITY_DIR            := $(WBM_NG_PLUGIN_SECURITY_BUILDROOT_DIR)
WBM_NG_PLUGIN_SECURITY_LICENSE        := unknown
WBM_NG_PLUGIN_SECURITY_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_SECURITY_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-security
else
WBM_NG_PLUGIN_SECURITY_TARGET_DIR     := /var/www/wbm/plugins/wbm-security
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_SECURITY_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_SECURITY_URL)' '$@' '$(WBM_NG_PLUGIN_SECURITY_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-security.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-security.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-security.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-security.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-security)
	@$(call install_fixup, wbm-ng-plugin-security, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-security, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-security, AUTHOR,"Marius Hellmeier, WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-security, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-security, 0, 0, 0755, $(WBM_NG_PLUGIN_SECURITY_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_SECURITY_DIR) && \
	for object in $$( find ./* \( -path './series' -o -path './.ptxdist*' -o -path './.pc*' \) -prune -o -print ); do \
		if test -f $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-security, 0, 0, 0644, $(WBM_NG_PLUGIN_SECURITY_DIR)/$$object, $(WBM_NG_PLUGIN_SECURITY_TARGET_DIR)/$$object); \
		elif test -d $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-security, 0, 0, 0755, $(WBM_NG_PLUGIN_SECURITY_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-security)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-security.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_SECURITY_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_SECURITY)
	@rm -rf $(WBM_NG_PLUGIN_SECURITY_BUILDROOT_DIR)

# vim: syntax=make
