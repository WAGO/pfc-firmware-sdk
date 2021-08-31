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
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_BACNET) += wbm-ng-plugin-bacnet

#
# Paths and names
#
WBM_NG_PLUGIN_BACNET_VERSION        := 1.4.1
WBM_NG_PLUGIN_BACNET                := wbm-bacnet-$(WBM_NG_PLUGIN_BACNET_VERSION)
WBM_NG_PLUGIN_BACNET_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_BACNET)
WBM_NG_PLUGIN_BACNET_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_BACNET_URL))
WBM_NG_PLUGIN_BACNET_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_BACNET)$(WBM_NG_PLUGIN_BACNET_SUFFIX)
WBM_NG_PLUGIN_BACNET_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_BACNET_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_BACNET_MD5_FILE))
WBM_NG_PLUGIN_BACNET_MD5_FILE       := $(WBM_NG_PLUGIN_BACNET_SOURCE).md5
WBM_NG_PLUGIN_BACNET_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_BACNET_URL))
WBM_NG_PLUGIN_BACNET_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-bacnet
WBM_NG_PLUGIN_BACNET_DIR            := $(WBM_NG_PLUGIN_BACNET_BUILDROOT_DIR)
WBM_NG_PLUGIN_BACNET_LICENSE        := unknown
WBM_NG_PLUGIN_BACNET_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_BACNET_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-bacnet
else
WBM_NG_PLUGIN_BACNET_TARGET_DIR     := /var/www/wbm/plugins/wbm-bacnet
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_BACNET_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_BACNET_URL)' '$@' '$(WBM_NG_PLUGIN_BACNET_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-bacnet.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-bacnet.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-bacnet.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-bacnet.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-bacnet)
	@$(call install_fixup, wbm-ng-plugin-bacnet, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-bacnet, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-bacnet, AUTHOR,"Nico Baade, WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-bacnet, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-bacnet, 0, 0, 0755, $(WBM_NG_PLUGIN_BACNET_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_BACNET_DIR) && \
	for object in $$( find ./* \( -path './series' -o -path './.ptxdist*' -o -path './.pc*' \) -prune -o -print ); do \
		if test -f $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-bacnet, 0, 0, 0644, $(WBM_NG_PLUGIN_BACNET_DIR)/$$object, $(WBM_NG_PLUGIN_BACNET_TARGET_DIR)/$$object); \
		elif test -d $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-bacnet, 0, 0, 0755, $(WBM_NG_PLUGIN_BACNET_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-bacnet)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-bacnet.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_BACNET_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_BACNET)
	@rm -rf $(WBM_NG_PLUGIN_BACNET_BUILDROOT_DIR)

# vim: syntax=make
