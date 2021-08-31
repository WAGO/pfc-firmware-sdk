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
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_USER) += wbm-ng-plugin-user

#
# Paths and names
#
WBM_NG_PLUGIN_USER_VERSION        := 1.2.2
WBM_NG_PLUGIN_USER                := wbm-user-$(WBM_NG_PLUGIN_USER_VERSION)
WBM_NG_PLUGIN_USER_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_USER)
WBM_NG_PLUGIN_USER_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_USER_URL))
WBM_NG_PLUGIN_USER_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_USER)$(WBM_NG_PLUGIN_USER_SUFFIX)
WBM_NG_PLUGIN_USER_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_USER_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_USER_MD5_FILE))
WBM_NG_PLUGIN_USER_MD5_FILE       := $(WBM_NG_PLUGIN_USER_SOURCE).md5
WBM_NG_PLUGIN_USER_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_USER_URL))
WBM_NG_PLUGIN_USER_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-user
WBM_NG_PLUGIN_USER_DIR            := $(WBM_NG_PLUGIN_USER_BUILDROOT_DIR)
WBM_NG_PLUGIN_USER_LICENSE        := unknown
WBM_NG_PLUGIN_USER_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_USER_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-user
else
WBM_NG_PLUGIN_USER_TARGET_DIR     := /var/www/wbm/plugins/wbm-user
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_USER_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_USER_URL)' '$@' '$(WBM_NG_PLUGIN_USER_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-user.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-user.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-user.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-user.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-user)
	@$(call install_fixup, wbm-ng-plugin-user, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-user, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-user, AUTHOR,"Marius Hellmeier, WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-user, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-user, 0, 0, 0755, $(WBM_NG_PLUGIN_USER_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_USER_DIR) && \
	for object in $$( find ./* \( -path './series' -o -path './.ptxdist*' -o -path './.pc*' \) -prune -o -print ); do \
		if test -f $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-user, 0, 0, 0644, $(WBM_NG_PLUGIN_USER_DIR)/$$object, $(WBM_NG_PLUGIN_USER_TARGET_DIR)/$$object); \
		elif test -d $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-user, 0, 0, 0755, $(WBM_NG_PLUGIN_USER_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-user)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-user.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_USER_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_USER)
	@rm -rf $(WBM_NG_PLUGIN_USER_BUILDROOT_DIR)

# vim: syntax=make
