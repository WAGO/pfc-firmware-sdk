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
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_MODEM_NG) += wbm-ng-plugin-modem-ng

#
# Paths and names
#
WBM_NG_PLUGIN_MODEM_NG_VERSION        := 1.1.1
WBM_NG_PLUGIN_MODEM_NG                := wbm-modem-ng-$(WBM_NG_PLUGIN_MODEM_NG_VERSION)
WBM_NG_PLUGIN_MODEM_NG_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_MODEM_NG)
WBM_NG_PLUGIN_MODEM_NG_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_MODEM_NG_URL))
WBM_NG_PLUGIN_MODEM_NG_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_MODEM_NG)$(WBM_NG_PLUGIN_MODEM_NG_SUFFIX)
WBM_NG_PLUGIN_MODEM_NG_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_MODEM_NG_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_MODEM_NG_MD5_FILE))
WBM_NG_PLUGIN_MODEM_NG_MD5_FILE       := $(WBM_NG_PLUGIN_MODEM_NG_SOURCE).md5
WBM_NG_PLUGIN_MODEM_NG_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_MODEM_NG_URL))
WBM_NG_PLUGIN_MODEM_NG_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-modem-ng
WBM_NG_PLUGIN_MODEM_NG_DIR            := $(WBM_NG_PLUGIN_MODEM_NG_BUILDROOT_DIR)
WBM_NG_PLUGIN_MODEM_NG_LICENSE        := unknown
WBM_NG_PLUGIN_MODEM_NG_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_MODEM_NG_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-modem-ng
else
WBM_NG_PLUGIN_MODEM_NG_TARGET_DIR     := /var/www/wbm/plugins/wbm-modem-ng
endif


# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_MODEM_NG_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_MODEM_NG_URL)' '$@' '$(WBM_NG_PLUGIN_MODEM_NG_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-modem-ng.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-modem-ng.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-modem-ng.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-modem-ng.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-modem-ng)
	@$(call install_fixup, wbm-ng-plugin-modem-ng, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-modem-ng, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-modem-ng, AUTHOR,"Holger Kelch, WAGO GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-modem-ng, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-modem-ng, 0, 0, 0755, $(WBM_NG_PLUGIN_MODEM_NG_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_MODEM_NG_DIR) && \
	for object in $$( find ./* \( -path './series' -o -path './.ptxdist*' -o -path './.pc*' \) -prune -o -print ); do \
		if test -f $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-modem-ng, 0, 0, 0644, $(WBM_NG_PLUGIN_MODEM_NG_DIR)/$$object, $(WBM_NG_PLUGIN_MODEM_NG_TARGET_DIR)/$$object); \
		elif test -d $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-modem-ng, 0, 0, 0755, $(WBM_NG_PLUGIN_MODEM_NG_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-modem-ng)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-modem-ng.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_MODEM_NG_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_MODEM_NG)
	@rm -rf $(WBM_NG_PLUGIN_MODEM_NG_BUILDROOT_DIR)

# vim: syntax=make
