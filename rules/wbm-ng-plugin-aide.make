# -*-makefile-*-
#
# Copyright (C) 2017 by Stefanie Meihöfer (stefanie.meihoefer@wago.com), WAGO Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_AIDE) += wbm-ng-plugin-aide

#
# Paths and names
#
WBM_NG_PLUGIN_AIDE_VERSION        := 1.0.2
WBM_NG_PLUGIN_AIDE                := wbm-aide-$(WBM_NG_PLUGIN_AIDE_VERSION)
WBM_NG_PLUGIN_AIDE_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_AIDE)
WBM_NG_PLUGIN_AIDE_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_AIDE_URL))
WBM_NG_PLUGIN_AIDE_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_AIDE)$(WBM_NG_PLUGIN_AIDE_SUFFIX)
WBM_NG_PLUGIN_AIDE_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_AIDE_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_AIDE_MD5_FILE))
WBM_NG_PLUGIN_AIDE_MD5_FILE       := $(WBM_NG_PLUGIN_AIDE_SOURCE).md5
WBM_NG_PLUGIN_AIDE_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_AIDE_URL))
WBM_NG_PLUGIN_AIDE_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-aide
WBM_NG_PLUGIN_AIDE_DIR            := $(WBM_NG_PLUGIN_AIDE_BUILDROOT_DIR)
WBM_NG_PLUGIN_AIDE_LICENSE        := unknown
WBM_NG_PLUGIN_AIDE_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_AIDE_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-aide
else
WBM_NG_PLUGIN_AIDE_TARGET_DIR     := /var/www/wbm/plugins/wbm-aide
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_AIDE_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_AIDE_URL)' '$@' '$(WBM_NG_PLUGIN_AIDE_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-aide.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-aide.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-aide.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-aide.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-aide)
	@$(call install_fixup, wbm-ng-plugin-aide, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-aide, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-aide, AUTHOR,"Stefanie Meihoefer, WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-aide, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-aide, 0, 0, 0755, $(WBM_NG_PLUGIN_AIDE_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_AIDE_DIR) && \
	for object in $$( find ./* -print ); do \
		if test -f $$object; then \
			$(call install_copy, wbm-ng-plugin-aide, 0, 0, 0644, $(WBM_NG_PLUGIN_AIDE_DIR)/$$object, $(WBM_NG_PLUGIN_AIDE_TARGET_DIR)/$$object); \
		elif test -d $$object; then \
			$(call install_copy, wbm-ng-plugin-aide, 0, 0, 0755, $(WBM_NG_PLUGIN_AIDE_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-aide)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-aide.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_AIDE_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_AIDE)
	@rm -rf $(WBM_NG_PLUGIN_AIDE_BUILDROOT_DIR)

# vim: syntax=make
