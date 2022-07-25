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
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_FIREWALL) += wbm-ng-plugin-firewall

#
# Paths and names
#
WBM_NG_PLUGIN_FIREWALL_VERSION        := 1.6.3
WBM_NG_PLUGIN_FIREWALL                := wbm-firewall-$(WBM_NG_PLUGIN_FIREWALL_VERSION)
WBM_NG_PLUGIN_FIREWALL_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_FIREWALL)
WBM_NG_PLUGIN_FIREWALL_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_FIREWALL_URL))
WBM_NG_PLUGIN_FIREWALL_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_FIREWALL)$(WBM_NG_PLUGIN_FIREWALL_SUFFIX)
WBM_NG_PLUGIN_FIREWALL_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_FIREWALL_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_FIREWALL_MD5_FILE))
WBM_NG_PLUGIN_FIREWALL_MD5_FILE       := $(WBM_NG_PLUGIN_FIREWALL_SOURCE).md5
WBM_NG_PLUGIN_FIREWALL_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_FIREWALL_URL))
WBM_NG_PLUGIN_FIREWALL_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-firewall
WBM_NG_PLUGIN_FIREWALL_DIR            := $(WBM_NG_PLUGIN_FIREWALL_BUILDROOT_DIR)
WBM_NG_PLUGIN_FIREWALL_LICENSE        := unknown
WBM_NG_PLUGIN_FIREWALL_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_FIREWALL_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-firewall
else
WBM_NG_PLUGIN_FIREWALL_TARGET_DIR     := /var/www/wbm/plugins/wbm-firewall
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_FIREWALL_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_FIREWALL_URL)' '$@' '$(WBM_NG_PLUGIN_FIREWALL_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-firewall.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-firewall.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-firewall.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-firewall.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-firewall)
	@$(call install_fixup, wbm-ng-plugin-firewall, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-firewall, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-firewall, AUTHOR,"Marius Hellmeier, WAGO GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-firewall, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-firewall, 0, 0, 0755, $(WBM_NG_PLUGIN_FIREWALL_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_FIREWALL_DIR) && \
	for object in $$( find ./* \( -path './series' -o -path './.ptxdist*' -o -path './.pc*' \) -prune -o -print ); do \
		if test -f $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-firewall, 0, 0, 0644, $(WBM_NG_PLUGIN_FIREWALL_DIR)/$$object, $(WBM_NG_PLUGIN_FIREWALL_TARGET_DIR)/$$object); \
		elif test -d $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-firewall, 0, 0, 0755, $(WBM_NG_PLUGIN_FIREWALL_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-firewall)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-firewall.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_FIREWALL_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_FIREWALL)
	@rm -rf $(WBM_NG_PLUGIN_FIREWALL_BUILDROOT_DIR)

# vim: syntax=make
