# -*-makefile-*-
#
# Copyright (C) 2019 WAGO Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_CLOUDCONNECTIVITY) += wbm-ng-plugin-cloudconnectivity

#
# Paths and names
#
WBM_NG_PLUGIN_CLOUDCONNECTIVITY_VERSION        := 0.4.10
WBM_NG_PLUGIN_CLOUDCONNECTIVITY                := wbm-cloud-connectivity-$(WBM_NG_PLUGIN_CLOUDCONNECTIVITY_VERSION)
WBM_NG_PLUGIN_CLOUDCONNECTIVITY_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_CLOUDCONNECTIVITY)
WBM_NG_PLUGIN_CLOUDCONNECTIVITY_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_CLOUDCONNECTIVITY_URL))
WBM_NG_PLUGIN_CLOUDCONNECTIVITY_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_CLOUDCONNECTIVITY)$(WBM_NG_PLUGIN_CLOUDCONNECTIVITY_SUFFIX)
WBM_NG_PLUGIN_CLOUDCONNECTIVITY_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_CLOUDCONNECTIVITY_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_CLOUDCONNECTIVITY_MD5_FILE))
WBM_NG_PLUGIN_CLOUDCONNECTIVITY_MD5_FILE       := $(WBM_NG_PLUGIN_CLOUDCONNECTIVITY_SOURCE).md5
WBM_NG_PLUGIN_CLOUDCONNECTIVITY_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_CLOUDCONNECTIVITY_URL))
WBM_NG_PLUGIN_CLOUDCONNECTIVITY_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-cloudconnectivity
WBM_NG_PLUGIN_CLOUDCONNECTIVITY_DIR            := $(WBM_NG_PLUGIN_CLOUDCONNECTIVITY_BUILDROOT_DIR)
WBM_NG_PLUGIN_CLOUDCONNECTIVITY_LICENSE        := unknown
WBM_NG_PLUGIN_CLOUDCONNECTIVITY_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_CLOUDCONNECTIVITY_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-cloud-connectivity
else
WBM_NG_PLUGIN_CLOUDCONNECTIVITY_TARGET_DIR     := /var/www/wbm/plugins/wbm-cloud-connectivity
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_CLOUDCONNECTIVITY_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
       '$(WBM_NG_PLUGIN_CLOUDCONNECTIVITY_URL)' '$@' '$(WBM_NG_PLUGIN_CLOUDCONNECTIVITY_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-cloudconnectivity.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-cloudconnectivity.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-cloudconnectivity.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-cloudconnectivity.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-cloudconnectivity)
	@$(call install_fixup, wbm-ng-plugin-cloudconnectivity, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-cloudconnectivity, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-cloudconnectivity, AUTHOR,"WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-cloudconnectivity, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-cloudconnectivity, 0, 0, 0755, $(WBM_NG_PLUGIN_CLOUDCONNECTIVITY_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_CLOUDCONNECTIVITY_DIR) && \
	for object in $$( find ./* -print ); do \
		if test -f $$object; then \
			$(call install_copy, wbm-ng-plugin-cloudconnectivity, 0, 0, 0644, $(WBM_NG_PLUGIN_CLOUDCONNECTIVITY_DIR)/$$object, $(WBM_NG_PLUGIN_CLOUDCONNECTIVITY_TARGET_DIR)/$$object); \
		elif test -d $$object; then \
			$(call install_copy, wbm-ng-plugin-cloudconnectivity, 0, 0, 0755, $(WBM_NG_PLUGIN_CLOUDCONNECTIVITY_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-cloudconnectivity)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-cloudconnectivity.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_CLOUDCONNECTIVITY_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_CLOUDCONNECTIVITY)
	@rm -rf $(WBM_NG_PLUGIN_CLOUDCONNECTIVITY_BUILDROOT_DIR)

# vim: syntax=make
