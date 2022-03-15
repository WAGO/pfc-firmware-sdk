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
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_OPCUA) += wbm-ng-plugin-opcua

#
# Paths and names
#
WBM_NG_PLUGIN_OPCUA_VERSION        := 2.4.0
WBM_NG_PLUGIN_OPCUA                := wbm-opcua-$(WBM_NG_PLUGIN_OPCUA_VERSION)
WBM_NG_PLUGIN_OPCUA_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_OPCUA)
WBM_NG_PLUGIN_OPCUA_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_OPCUA_URL))
WBM_NG_PLUGIN_OPCUA_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_OPCUA)$(WBM_NG_PLUGIN_OPCUA_SUFFIX)
WBM_NG_PLUGIN_OPCUA_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_OPCUA_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_OPCUA_MD5_FILE))
WBM_NG_PLUGIN_OPCUA_MD5_FILE       := $(WBM_NG_PLUGIN_OPCUA_SOURCE).md5
WBM_NG_PLUGIN_OPCUA_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_OPCUA_URL))
WBM_NG_PLUGIN_OPCUA_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-opcua
WBM_NG_PLUGIN_OPCUA_DIR            := $(WBM_NG_PLUGIN_OPCUA_BUILDROOT_DIR)
WBM_NG_PLUGIN_OPCUA_LICENSE        := unknown
WBM_NG_PLUGIN_OPCUA_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_OPCUA_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-opcua
else
WBM_NG_PLUGIN_OPCUA_TARGET_DIR     := /var/www/wbm/plugins/wbm-opcua
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_OPCUA_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_OPCUA_URL)' '$@' '$(WBM_NG_PLUGIN_OPCUA_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-opcua.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-opcua.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-opcua.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-opcua.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-opcua)
	@$(call install_fixup, wbm-ng-plugin-opcua, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-opcua, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-opcua, AUTHOR,"Stefanie Meihoefer, WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-opcua, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-opcua, 0, 0, 0755, $(WBM_NG_PLUGIN_OPCUA_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_OPCUA_DIR) && \
	for object in $$( find ./* \( -path './series' -o -path './.ptxdist*' -o -path './.pc*' \) -prune -o -print ); do \
		if test -f $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-opcua, 0, 0, 0644, $(WBM_NG_PLUGIN_OPCUA_DIR)/$$object, $(WBM_NG_PLUGIN_OPCUA_TARGET_DIR)/$$object); \
		elif test -d $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-opcua, 0, 0, 0755, $(WBM_NG_PLUGIN_OPCUA_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-opcua)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-opcua.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_OPCUA_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_OPCUA)
	@rm -rf $(WBM_NG_PLUGIN_OPCUA_BUILDROOT_DIR)

# vim: syntax=make
