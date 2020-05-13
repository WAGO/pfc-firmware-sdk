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
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_MODBUS) += wbm-ng-plugin-modbus

#
# Paths and names
#
WBM_NG_PLUGIN_MODBUS_VERSION        := 1.0.2
WBM_NG_PLUGIN_MODBUS                := wbm-modbus-$(WBM_NG_PLUGIN_MODBUS_VERSION)
WBM_NG_PLUGIN_MODBUS_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_MODBUS)
WBM_NG_PLUGIN_MODBUS_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_MODBUS_URL))
WBM_NG_PLUGIN_MODBUS_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_MODBUS)$(WBM_NG_PLUGIN_MODBUS_SUFFIX)
WBM_NG_PLUGIN_MODBUS_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_MODBUS_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_MODBUS_MD5_FILE))
WBM_NG_PLUGIN_MODBUS_MD5_FILE       := $(WBM_NG_PLUGIN_MODBUS_SOURCE).md5
WBM_NG_PLUGIN_MODBUS_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_MODBUS_URL))
WBM_NG_PLUGIN_MODBUS_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-modbus
WBM_NG_PLUGIN_MODBUS_DIR            := $(WBM_NG_PLUGIN_MODBUS_BUILDROOT_DIR)
WBM_NG_PLUGIN_MODBUS_LICENSE        := unknown
WBM_NG_PLUGIN_MODBUS_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_MODBUS_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-modbus
else
WBM_NG_PLUGIN_MODBUS_TARGET_DIR     := /var/www/wbm/plugins/wbm-modbus
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_MODBUS_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_MODBUS_URL)' '$@' '$(WBM_NG_PLUGIN_MODBUS_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-modbus.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-modbus.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-modbus.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-modbus.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-modbus)
	@$(call install_fixup, wbm-ng-plugin-modbus, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-modbus, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-modbus, AUTHOR,"Marius Hellmeier, WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-modbus, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-modbus, 0, 0, 0755, $(WBM_NG_PLUGIN_MODBUS_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_MODBUS_DIR) && \
	for object in $$( find ./* -print ); do \
		if test -f $$object; then \
			$(call install_copy, wbm-ng-plugin-modbus, 0, 0, 0644, $(WBM_NG_PLUGIN_MODBUS_DIR)/$$object, $(WBM_NG_PLUGIN_MODBUS_TARGET_DIR)/$$object); \
		elif test -d $$object; then \
			$(call install_copy, wbm-ng-plugin-modbus, 0, 0, 0755, $(WBM_NG_PLUGIN_MODBUS_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-modbus)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-modbus.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_MODBUS_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_MODBUS)
	@rm -rf $(WBM_NG_PLUGIN_MODBUS_BUILDROOT_DIR)

# vim: syntax=make
