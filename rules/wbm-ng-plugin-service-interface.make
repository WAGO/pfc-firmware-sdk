# -*-makefile-*-
#
# Copyright (C) 2020 by Patrick Enns (patrick.enns@wago.com), WAGO Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_SERVICE_INTERFACE) += wbm-ng-plugin-service-interface

#
# Paths and names
#
WBM_NG_PLUGIN_SERVICE_INTERFACE_VERSION        := 1.0.4
WBM_NG_PLUGIN_SERVICE_INTERFACE                := wbm-service-interface-$(WBM_NG_PLUGIN_SERVICE_INTERFACE_VERSION)
WBM_NG_PLUGIN_SERVICE_INTERFACE_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_SERVICE_INTERFACE)
WBM_NG_PLUGIN_SERVICE_INTERFACE_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_SERVICE_INTERFACE_URL))
WBM_NG_PLUGIN_SERVICE_INTERFACE_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_SERVICE_INTERFACE)$(WBM_NG_PLUGIN_SERVICE_INTERFACE_SUFFIX)
WBM_NG_PLUGIN_SERVICE_INTERFACE_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_SERVICE_INTERFACE_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_SERVICE_INTERFACE_MD5_FILE))
WBM_NG_PLUGIN_SERVICE_INTERFACE_MD5_FILE       := $(WBM_NG_PLUGIN_SERVICE_INTERFACE_SOURCE).md5
WBM_NG_PLUGIN_SERVICE_INTERFACE_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_SERVICE_INTERFACE_URL))
WBM_NG_PLUGIN_SERVICE_INTERFACE_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-service-interface
WBM_NG_PLUGIN_SERVICE_INTERFACE_DIR            := $(WBM_NG_PLUGIN_SERVICE_INTERFACE_BUILDROOT_DIR)
WBM_NG_PLUGIN_SERVICE_INTERFACE_LICENSE        := unknown
WBM_NG_PLUGIN_SERVICE_INTERFACE_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_SERVICE_INTERFACE_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-service-interface
else
WBM_NG_PLUGIN_SERVICE_INTERFACE_TARGET_DIR     := /var/www/wbm/plugins/wbm-service-interface
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_SERVICE_INTERFACE_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_SERVICE_INTERFACE_URL)' '$@' '$(WBM_NG_PLUGIN_SERVICE_INTERFACE_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-service-interface.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-service-interface.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-service-interface.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-service-interface.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-service-interface)
	@$(call install_fixup, wbm-ng-plugin-service-interface, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-service-interface, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-service-interface, AUTHOR, "Patrick Enns, WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-service-interface, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-service-interface, 0, 0, 0755, $(WBM_NG_PLUGIN_SERVICE_INTERFACE_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_SERVICE_INTERFACE_DIR) && \
	for object in $$( find ./* -print ); do \
		if test -f $$object; then \
			$(call install_copy, wbm-ng-plugin-service-interface, 0, 0, 0644, $(WBM_NG_PLUGIN_SERVICE_INTERFACE_DIR)/$$object, $(WBM_NG_PLUGIN_SERVICE_INTERFACE_TARGET_DIR)/$$object); \
		elif test -d $$object; then \
			$(call install_copy, wbm-ng-plugin-service-interface, 0, 0, 0755, $(WBM_NG_PLUGIN_SERVICE_INTERFACE_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-service-interface)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-service-interface.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_SERVICE_INTERFACE_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_SERVICE_INTERFACE)
	@rm -rf $(WBM_NG_PLUGIN_SERVICE_INTERFACE_BUILDROOT_DIR)

# vim: syntax=make
