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
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_RUNTIME_SERVICES) += wbm-ng-plugin-runtime-services

#
# Paths and names
#
WBM_NG_PLUGIN_RUNTIME_SERVICES_VERSION        := 1.2.0
WBM_NG_PLUGIN_RUNTIME_SERVICES                := wbm-runtime-services-$(WBM_NG_PLUGIN_RUNTIME_SERVICES_VERSION)
WBM_NG_PLUGIN_RUNTIME_SERVICES_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_RUNTIME_SERVICES)
WBM_NG_PLUGIN_RUNTIME_SERVICES_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_RUNTIME_SERVICES_URL))
WBM_NG_PLUGIN_RUNTIME_SERVICES_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_RUNTIME_SERVICES)$(WBM_NG_PLUGIN_RUNTIME_SERVICES_SUFFIX)
WBM_NG_PLUGIN_RUNTIME_SERVICES_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_RUNTIME_SERVICES_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_RUNTIME_SERVICES_MD5_FILE))
WBM_NG_PLUGIN_RUNTIME_SERVICES_MD5_FILE       := $(WBM_NG_PLUGIN_RUNTIME_SERVICES_SOURCE).md5
WBM_NG_PLUGIN_RUNTIME_SERVICES_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_RUNTIME_SERVICES_URL))
WBM_NG_PLUGIN_RUNTIME_SERVICES_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-runtime-services
WBM_NG_PLUGIN_RUNTIME_SERVICES_DIR            := $(WBM_NG_PLUGIN_RUNTIME_SERVICES_BUILDROOT_DIR)
WBM_NG_PLUGIN_RUNTIME_SERVICES_LICENSE        := unknown
WBM_NG_PLUGIN_RUNTIME_SERVICES_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_RUNTIME_SERVICES_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-runtime-services
else
WBM_NG_PLUGIN_RUNTIME_SERVICES_TARGET_DIR     := /var/www/wbm/plugins/wbm-runtime-services
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_RUNTIME_SERVICES_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_RUNTIME_SERVICES_URL)' '$@' '$(WBM_NG_PLUGIN_RUNTIME_SERVICES_MD5_FILE)'


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-runtime-services.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-runtime-services.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-runtime-services.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-runtime-services.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-runtime-services)
	@$(call install_fixup, wbm-ng-plugin-runtime-services, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-runtime-services, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-runtime-services, AUTHOR,"Marius Hellmeier, WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-runtime-services, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-runtime-services, 0, 0, 0755, $(WBM_NG_PLUGIN_RUNTIME_SERVICES_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_RUNTIME_SERVICES_DIR) && \
	for object in $$( find ./* \( -path './series' -o -path './.ptxdist*' -o -path './.pc*' \) -prune -o -print ); do \
		if test -f $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-runtime-services, 0, 0, 0644, $(WBM_NG_PLUGIN_RUNTIME_SERVICES_DIR)/$$object, $(WBM_NG_PLUGIN_RUNTIME_SERVICES_TARGET_DIR)/$$object); \
		elif test -d $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-runtime-services, 0, 0, 0755, $(WBM_NG_PLUGIN_RUNTIME_SERVICES_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-runtime-services)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-runtime-services.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_RUNTIME_SERVICES_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_RUNTIME_SERVICES)
	@rm -rf $(WBM_NG_PLUGIN_RUNTIME_SERVICES_BUILDROOT_DIR)

# vim: syntax=make
