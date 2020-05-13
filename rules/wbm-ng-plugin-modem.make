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
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_MODEM) += wbm-ng-plugin-modem

#
# Paths and names
#
WBM_NG_PLUGIN_MODEM_VERSION        := 1.2.3
WBM_NG_PLUGIN_MODEM                := wbm-modem-$(WBM_NG_PLUGIN_MODEM_VERSION)
WBM_NG_PLUGIN_MODEM_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_MODEM)
WBM_NG_PLUGIN_MODEM_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_MODEM_URL))
WBM_NG_PLUGIN_MODEM_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_MODEM)$(WBM_NG_PLUGIN_MODEM_SUFFIX)
WBM_NG_PLUGIN_MODEM_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_MODEM_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_MODEM_MD5_FILE))
WBM_NG_PLUGIN_MODEM_MD5_FILE       := $(WBM_NG_PLUGIN_MODEM_SOURCE).md5
WBM_NG_PLUGIN_MODEM_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_MODEM_URL))
WBM_NG_PLUGIN_MODEM_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-modem
WBM_NG_PLUGIN_MODEM_DIR            := $(WBM_NG_PLUGIN_MODEM_BUILDROOT_DIR)
WBM_NG_PLUGIN_MODEM_LICENSE        := unknown
WBM_NG_PLUGIN_MODEM_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_MODEM_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-modem
else
WBM_NG_PLUGIN_MODEM_TARGET_DIR     := /var/www/wbm/plugins/wbm-modem
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_MODEM_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_MODEM_URL)' '$@' '$(WBM_NG_PLUGIN_MODEM_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-modem.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-modem.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-modem.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-modem.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-modem)
	@$(call install_fixup, wbm-ng-plugin-modem, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-modem, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-modem, AUTHOR,"Marius Hellmeier, WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-modem, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-modem, 0, 0, 0755, $(WBM_NG_PLUGIN_MODEM_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_MODEM_DIR) && \
	for object in $$( find ./* -print ); do \
		if test -f $$object; then \
			$(call install_copy, wbm-ng-plugin-modem, 0, 0, 0644, $(WBM_NG_PLUGIN_MODEM_DIR)/$$object, $(WBM_NG_PLUGIN_MODEM_TARGET_DIR)/$$object); \
		elif test -d $$object; then \
			$(call install_copy, wbm-ng-plugin-modem, 0, 0, 0755, $(WBM_NG_PLUGIN_MODEM_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-modem)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-modem.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_MODEM_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_MODEM)
	@rm -rf $(WBM_NG_PLUGIN_MODEM_BUILDROOT_DIR)

# vim: syntax=make
