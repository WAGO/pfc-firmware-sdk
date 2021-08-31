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
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_RUNTIME_CONFIGURATION) += wbm-ng-plugin-runtime-configuration

#
# Paths and names
#
WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_VERSION        := 1.1.0
WBM_NG_PLUGIN_RUNTIME_CONFIGURATION                := wbm-runtime-configuration-$(WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_VERSION)
WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_RUNTIME_CONFIGURATION)
WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_URL))
WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_RUNTIME_CONFIGURATION)$(WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_SUFFIX)
WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_MD5_FILE))
WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_MD5_FILE       := $(WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_SOURCE).md5
WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_URL))
WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-runtime-configuration
WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_DIR            := $(WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_BUILDROOT_DIR)
WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_LICENSE        := unknown
WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-runtime-configuration
else
WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_TARGET_DIR     := /var/www/wbm/plugins/wbm-runtime-configuration
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_URL)' '$@' '$(WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-runtime-configuration.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-runtime-configuration.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-runtime-configuration.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-runtime-configuration.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-runtime-configuration)
	@$(call install_fixup, wbm-ng-plugin-runtime-configuration, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-runtime-configuration, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-runtime-configuration, AUTHOR,"Marius Hellmeier, WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-runtime-configuration, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-runtime-configuration, 0, 0, 0755, $(WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_DIR) && \
	for object in $$( find ./* \( -path './series' -o -path './.ptxdist*' -o -path './.pc*' \) -prune -o -print ); do \
		if test -f $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-runtime-configuration, 0, 0, 0644, $(WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_DIR)/$$object, $(WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_TARGET_DIR)/$$object); \
		elif test -d $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-runtime-configuration, 0, 0, 0755, $(WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-runtime-configuration)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-runtime-configuration.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_RUNTIME_CONFIGURATION)
	@rm -rf $(WBM_NG_PLUGIN_RUNTIME_CONFIGURATION_BUILDROOT_DIR)

# vim: syntax=make
