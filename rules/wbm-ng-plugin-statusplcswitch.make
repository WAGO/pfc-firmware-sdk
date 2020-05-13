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
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_STATUSPLCSWITCH) += wbm-ng-plugin-statusplcswitch

#
# Paths and names
#
WBM_NG_PLUGIN_STATUSPLCSWITCH_VERSION        := 1.0.2
WBM_NG_PLUGIN_STATUSPLCSWITCH                := wbm-statusplcswitch-$(WBM_NG_PLUGIN_STATUSPLCSWITCH_VERSION)
WBM_NG_PLUGIN_STATUSPLCSWITCH_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_STATUSPLCSWITCH)
WBM_NG_PLUGIN_STATUSPLCSWITCH_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_STATUSPLCSWITCH_URL))
WBM_NG_PLUGIN_STATUSPLCSWITCH_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_STATUSPLCSWITCH)$(WBM_NG_PLUGIN_STATUSPLCSWITCH_SUFFIX)
WBM_NG_PLUGIN_STATUSPLCSWITCH_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_STATUSPLCSWITCH_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_STATUSPLCSWITCH_MD5_FILE))
WBM_NG_PLUGIN_STATUSPLCSWITCH_MD5_FILE       := $(WBM_NG_PLUGIN_STATUSPLCSWITCH_SOURCE).md5
WBM_NG_PLUGIN_STATUSPLCSWITCH_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_STATUSPLCSWITCH_URL))
WBM_NG_PLUGIN_STATUSPLCSWITCH_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-statusplcswitch
WBM_NG_PLUGIN_STATUSPLCSWITCH_DIR            := $(WBM_NG_PLUGIN_STATUSPLCSWITCH_BUILDROOT_DIR)
WBM_NG_PLUGIN_STATUSPLCSWITCH_LICENSE        := unknown
WBM_NG_PLUGIN_STATUSPLCSWITCH_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_STATUSPLCSWITCH_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-statusplcswitch
else
WBM_NG_PLUGIN_STATUSPLCSWITCH_TARGET_DIR     := /var/www/wbm/plugins/wbm-statusplcswitch
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_STATUSPLCSWITCH_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_STATUSPLCSWITCH_URL)' '$@' '$(WBM_NG_PLUGIN_STATUSPLCSWITCH_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-statusplcswitch.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-statusplcswitch.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-statusplcswitch.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-statusplcswitch.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-statusplcswitch)
	@$(call install_fixup, wbm-ng-plugin-statusplcswitch, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-statusplcswitch, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-statusplcswitch, AUTHOR,"Marius Hellmeier, WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-statusplcswitch, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-statusplcswitch, 0, 0, 0755, $(WBM_NG_PLUGIN_STATUSPLCSWITCH_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_STATUSPLCSWITCH_DIR) && \
	for object in $$( find ./* -print ); do \
		if test -f $$object; then \
			$(call install_copy, wbm-ng-plugin-statusplcswitch, 0, 0, 0644, $(WBM_NG_PLUGIN_STATUSPLCSWITCH_DIR)/$$object, $(WBM_NG_PLUGIN_STATUSPLCSWITCH_TARGET_DIR)/$$object); \
		elif test -d $$object; then \
			$(call install_copy, wbm-ng-plugin-statusplcswitch, 0, 0, 0755, $(WBM_NG_PLUGIN_STATUSPLCSWITCH_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-statusplcswitch)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-statusplcswitch.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_STATUSPLCSWITCH_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_STATUSPLCSWITCH)
	@rm -rf $(WBM_NG_PLUGIN_STATUSPLCSWITCH_BUILDROOT_DIR)

# vim: syntax=make
