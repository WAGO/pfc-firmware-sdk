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
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_STATUSLED) += wbm-ng-plugin-statusled

#
# Paths and names
#
WBM_NG_PLUGIN_STATUSLED_VERSION        := 1.1.0
WBM_NG_PLUGIN_STATUSLED                := wbm-statusled-$(WBM_NG_PLUGIN_STATUSLED_VERSION)
WBM_NG_PLUGIN_STATUSLED_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_STATUSLED)
WBM_NG_PLUGIN_STATUSLED_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_STATUSLED_URL))
WBM_NG_PLUGIN_STATUSLED_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_STATUSLED)$(WBM_NG_PLUGIN_STATUSLED_SUFFIX)
WBM_NG_PLUGIN_STATUSLED_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_STATUSLED_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_STATUSLED_MD5_FILE))
WBM_NG_PLUGIN_STATUSLED_MD5_FILE       := $(WBM_NG_PLUGIN_STATUSLED_SOURCE).md5
WBM_NG_PLUGIN_STATUSLED_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_STATUSLED_URL))
WBM_NG_PLUGIN_STATUSLED_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-statusled
WBM_NG_PLUGIN_STATUSLED_DIR            := $(WBM_NG_PLUGIN_STATUSLED_BUILDROOT_DIR)
WBM_NG_PLUGIN_STATUSLED_LICENSE        := unknown
WBM_NG_PLUGIN_STATUSLED_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_STATUSLED_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-statusled
else
WBM_NG_PLUGIN_STATUSLED_TARGET_DIR     := /var/www/wbm/plugins/wbm-statusled
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_STATUSLED_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_STATUSLED_URL)' '$@' '$(WBM_NG_PLUGIN_STATUSLED_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-statusled.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-statusled.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-statusled.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-statusled.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-statusled)
	@$(call install_fixup, wbm-ng-plugin-statusled, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-statusled, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-statusled, AUTHOR,"Marius Hellmeier, WAGO GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-statusled, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-statusled, 0, 0, 0755, $(WBM_NG_PLUGIN_STATUSLED_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_STATUSLED_DIR) && \
	for object in $$( find ./* \( -path './series' -o -path './.ptxdist*' -o -path './.pc*' \) -prune -o -print ); do \
		if test -f $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-statusled, 0, 0, 0644, $(WBM_NG_PLUGIN_STATUSLED_DIR)/$$object, $(WBM_NG_PLUGIN_STATUSLED_TARGET_DIR)/$$object); \
		elif test -d $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-statusled, 0, 0, 0755, $(WBM_NG_PLUGIN_STATUSLED_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-statusled)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-statusled.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_STATUSLED_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_STATUSLED)
	@rm -rf $(WBM_NG_PLUGIN_STATUSLED_BUILDROOT_DIR)

# vim: syntax=make
