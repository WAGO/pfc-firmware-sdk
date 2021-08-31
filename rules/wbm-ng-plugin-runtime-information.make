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
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_RUNTIME_INFORMATION) += wbm-ng-plugin-runtime-information

#
# Paths and names
#
WBM_NG_PLUGIN_RUNTIME_INFORMATION_VERSION        := 1.2.0
WBM_NG_PLUGIN_RUNTIME_INFORMATION                := wbm-runtime-information-$(WBM_NG_PLUGIN_RUNTIME_INFORMATION_VERSION)
WBM_NG_PLUGIN_RUNTIME_INFORMATION_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_RUNTIME_INFORMATION)
WBM_NG_PLUGIN_RUNTIME_INFORMATION_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_RUNTIME_INFORMATION_URL))
WBM_NG_PLUGIN_RUNTIME_INFORMATION_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_RUNTIME_INFORMATION)$(WBM_NG_PLUGIN_RUNTIME_INFORMATION_SUFFIX)
WBM_NG_PLUGIN_RUNTIME_INFORMATION_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_RUNTIME_INFORMATION_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_RUNTIME_INFORMATION_MD5_FILE))
WBM_NG_PLUGIN_RUNTIME_INFORMATION_MD5_FILE       := $(WBM_NG_PLUGIN_RUNTIME_INFORMATION_SOURCE).md5
WBM_NG_PLUGIN_RUNTIME_INFORMATION_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_RUNTIME_INFORMATION_URL))
WBM_NG_PLUGIN_RUNTIME_INFORMATION_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-runtime-information
WBM_NG_PLUGIN_RUNTIME_INFORMATION_DIR            := $(WBM_NG_PLUGIN_RUNTIME_INFORMATION_BUILDROOT_DIR)
WBM_NG_PLUGIN_RUNTIME_INFORMATION_LICENSE        := unknown
WBM_NG_PLUGIN_RUNTIME_INFORMATION_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_RUNTIME_INFORMATION_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-runtime-information
else
WBM_NG_PLUGIN_RUNTIME_INFORMATION_TARGET_DIR     := /var/www/wbm/plugins/wbm-runtime-information
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_RUNTIME_INFORMATION_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_RUNTIME_INFORMATION_URL)' '$@' '$(WBM_NG_PLUGIN_RUNTIME_INFORMATION_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-runtime-information.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-runtime-information.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-runtime-information.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-runtime-information.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-runtime-information)
	@$(call install_fixup, wbm-ng-plugin-runtime-information, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-runtime-information, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-runtime-information, AUTHOR,"Marius Hellmeier, WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-runtime-information, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-runtime-information, 0, 0, 0755, $(WBM_NG_PLUGIN_RUNTIME_INFORMATION_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_RUNTIME_INFORMATION_DIR) && \
	for object in $$( find ./* \( -path './series' -o -path './.ptxdist*' -o -path './.pc*' \) -prune -o -print ); do \
		if test -f $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-runtime-information, 0, 0, 0644, $(WBM_NG_PLUGIN_RUNTIME_INFORMATION_DIR)/$$object, $(WBM_NG_PLUGIN_RUNTIME_INFORMATION_TARGET_DIR)/$$object); \
		elif test -d $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-runtime-information, 0, 0, 0755, $(WBM_NG_PLUGIN_RUNTIME_INFORMATION_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-runtime-information)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-runtime-information.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_RUNTIME_INFORMATION_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_RUNTIME_INFORMATION)
	@rm -rf $(WBM_NG_PLUGIN_RUNTIME_INFORMATION_BUILDROOT_DIR)

# vim: syntax=make
