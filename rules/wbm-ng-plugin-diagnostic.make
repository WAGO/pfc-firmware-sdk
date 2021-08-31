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
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_DIAGNOSTIC) += wbm-ng-plugin-diagnostic

#
# Paths and names
#
WBM_NG_PLUGIN_DIAGNOSTIC_VERSION        := 1.2.0
WBM_NG_PLUGIN_DIAGNOSTIC                := wbm-diagnostic-$(WBM_NG_PLUGIN_DIAGNOSTIC_VERSION)
WBM_NG_PLUGIN_DIAGNOSTIC_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_DIAGNOSTIC)
WBM_NG_PLUGIN_DIAGNOSTIC_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_DIAGNOSTIC_URL))
WBM_NG_PLUGIN_DIAGNOSTIC_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_DIAGNOSTIC)$(WBM_NG_PLUGIN_DIAGNOSTIC_SUFFIX)
WBM_NG_PLUGIN_DIAGNOSTIC_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_DIAGNOSTIC_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_DIAGNOSTIC_MD5_FILE))
WBM_NG_PLUGIN_DIAGNOSTIC_MD5_FILE       := $(WBM_NG_PLUGIN_DIAGNOSTIC_SOURCE).md5
WBM_NG_PLUGIN_DIAGNOSTIC_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_DIAGNOSTIC_URL))
WBM_NG_PLUGIN_DIAGNOSTIC_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-diagnostic
WBM_NG_PLUGIN_DIAGNOSTIC_DIR            := $(WBM_NG_PLUGIN_DIAGNOSTIC_BUILDROOT_DIR)
WBM_NG_PLUGIN_DIAGNOSTIC_LICENSE        := unknown
WBM_NG_PLUGIN_DIAGNOSTIC_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_DIAGNOSTIC_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-diagnostic
else
WBM_NG_PLUGIN_DIAGNOSTIC_TARGET_DIR     := /var/www/wbm/plugins/wbm-diagnostic
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_DIAGNOSTIC_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_DIAGNOSTIC_URL)' '$@' '$(WBM_NG_PLUGIN_DIAGNOSTIC_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-diagnostic.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-diagnostic.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-diagnostic.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-diagnostic.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-diagnostic)
	@$(call install_fixup, wbm-ng-plugin-diagnostic, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-diagnostic, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-diagnostic, AUTHOR,"Marius Hellmeier, WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-diagnostic, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-diagnostic, 0, 0, 0755, $(WBM_NG_PLUGIN_DIAGNOSTIC_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_DIAGNOSTIC_DIR) && \
	for object in $$( find ./* \( -path './series' -o -path './.ptxdist*' -o -path './.pc*' \) -prune -o -print ); do \
		if test -f $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-diagnostic, 0, 0, 0644, $(WBM_NG_PLUGIN_DIAGNOSTIC_DIR)/$$object, $(WBM_NG_PLUGIN_DIAGNOSTIC_TARGET_DIR)/$$object); \
		elif test -d $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-diagnostic, 0, 0, 0755, $(WBM_NG_PLUGIN_DIAGNOSTIC_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-diagnostic)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-diagnostic.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_DIAGNOSTIC_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_DIAGNOSTIC)
	@rm -rf $(WBM_NG_PLUGIN_DIAGNOSTIC_BUILDROOT_DIR)

# vim: syntax=make
