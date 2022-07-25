# -*-makefile-*-
#
# Copyright (C) 2022 by Marius Hellmeier (marius.hellmeier@wago.com), WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_BOOT_MODE) += wbm-ng-plugin-boot-mode

#
# Paths and names
#
WBM_NG_PLUGIN_BOOT_MODE_VERSION        := 1.0.0
WBM_NG_PLUGIN_BOOT_MODE                := wbm-boot-mode-$(WBM_NG_PLUGIN_BOOT_MODE_VERSION)
WBM_NG_PLUGIN_BOOT_MODE_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_BOOT_MODE)
WBM_NG_PLUGIN_BOOT_MODE_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_BOOT_MODE_URL))
WBM_NG_PLUGIN_BOOT_MODE_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_BOOT_MODE)$(WBM_NG_PLUGIN_BOOT_MODE_SUFFIX)
WBM_NG_PLUGIN_BOOT_MODE_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_BOOT_MODE_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_BOOT_MODE_MD5_FILE))
WBM_NG_PLUGIN_BOOT_MODE_MD5_FILE       := $(WBM_NG_PLUGIN_BOOT_MODE_SOURCE).md5
WBM_NG_PLUGIN_BOOT_MODE_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_BOOT_MODE_URL))
WBM_NG_PLUGIN_BOOT_MODE_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-boot-mode
WBM_NG_PLUGIN_BOOT_MODE_DIR            := $(WBM_NG_PLUGIN_BOOT_MODE_BUILDROOT_DIR)
WBM_NG_PLUGIN_BOOT_MODE_LICENSE        := unknown
WBM_NG_PLUGIN_BOOT_MODE_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_BOOT_MODE_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-boot-mode
else
WBM_NG_PLUGIN_BOOT_MODE_TARGET_DIR     := /var/www/wbm/plugins/wbm-boot-mode
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_BOOT_MODE_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_BOOT_MODE_URL)' '$@' '$(WBM_NG_PLUGIN_BOOT_MODE_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-boot-mode.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-boot-mode.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-boot-mode.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-boot-mode.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-boot-mode)
	@$(call install_fixup, wbm-ng-plugin-boot-mode, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-boot-mode, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-boot-mode, AUTHOR,"Marius Hellmeier, WAGO GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-boot-mode, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-boot-mode, 0, 0, 0755, $(WBM_NG_PLUGIN_BOOT_MODE_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_BOOT_MODE_DIR) && \
	for object in $$( find ./* \( -path './series' -o -path './.ptxdist*' -o -path './.pc*' \) -prune -o -print ); do \
		if test -f $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-boot-mode, 0, 0, 0644, $(WBM_NG_PLUGIN_BOOT_MODE_DIR)/$$object, $(WBM_NG_PLUGIN_BOOT_MODE_TARGET_DIR)/$$object); \
		elif test -d $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-boot-mode, 0, 0, 0755, $(WBM_NG_PLUGIN_BOOT_MODE_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-boot-mode)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-boot-mode.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_BOOT_MODE_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_BOOT_MODE)
	@rm -rf $(WBM_NG_PLUGIN_BOOT_MODE_BUILDROOT_DIR)

# vim: syntax=make
