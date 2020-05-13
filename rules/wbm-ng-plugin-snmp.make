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
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_SNMP) += wbm-ng-plugin-snmp

#
# Paths and names
#
WBM_NG_PLUGIN_SNMP_VERSION        := 1.0.0
WBM_NG_PLUGIN_SNMP                := wbm-snmp-$(WBM_NG_PLUGIN_SNMP_VERSION)
WBM_NG_PLUGIN_SNMP_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_SNMP)
WBM_NG_PLUGIN_SNMP_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_SNMP_URL))
WBM_NG_PLUGIN_SNMP_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_SNMP)$(WBM_NG_PLUGIN_SNMP_SUFFIX)
WBM_NG_PLUGIN_SNMP_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_SNMP_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_SNMP_MD5_FILE))
WBM_NG_PLUGIN_SNMP_MD5_FILE       := $(WBM_NG_PLUGIN_SNMP_SOURCE).md5
WBM_NG_PLUGIN_SNMP_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_SNMP_URL))
WBM_NG_PLUGIN_SNMP_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-snmp
WBM_NG_PLUGIN_SNMP_DIR            := $(WBM_NG_PLUGIN_SNMP_BUILDROOT_DIR)
WBM_NG_PLUGIN_SNMP_LICENSE        := unknown
WBM_NG_PLUGIN_SNMP_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_SNMP_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-snmp
else
WBM_NG_PLUGIN_SNMP_TARGET_DIR     := /var/www/wbm/plugins/wbm-snmp
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_SNMP_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_SNMP_URL)' '$@' '$(WBM_NG_PLUGIN_SNMP_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-snmp.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-snmp.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-snmp.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-snmp.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-snmp)
	@$(call install_fixup, wbm-ng-plugin-snmp, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-snmp, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-snmp, AUTHOR,"Stefanie Meihoefer, WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-snmp, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-snmp, 0, 0, 0755, $(WBM_NG_PLUGIN_SNMP_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_SNMP_DIR) && \
	for object in $$( find ./* -print ); do \
		if test -f $$object; then \
			$(call install_copy, wbm-ng-plugin-snmp, 0, 0, 0644, $(WBM_NG_PLUGIN_SNMP_DIR)/$$object, $(WBM_NG_PLUGIN_SNMP_TARGET_DIR)/$$object); \
		elif test -d $$object; then \
			$(call install_copy, wbm-ng-plugin-snmp, 0, 0, 0755, $(WBM_NG_PLUGIN_SNMP_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-snmp)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-snmp.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_SNMP_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_SNMP)
	@rm -rf $(WBM_NG_PLUGIN_SNMP_BUILDROOT_DIR)

# vim: syntax=make
