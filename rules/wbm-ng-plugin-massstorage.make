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
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_MASSSTORAGE) += wbm-ng-plugin-massstorage

#
# Paths and names
#
WBM_NG_PLUGIN_MASSSTORAGE_VERSION        := 1.0.2
WBM_NG_PLUGIN_MASSSTORAGE                := wbm-massstorage-$(WBM_NG_PLUGIN_MASSSTORAGE_VERSION)
WBM_NG_PLUGIN_MASSSTORAGE_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_MASSSTORAGE)
WBM_NG_PLUGIN_MASSSTORAGE_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_MASSSTORAGE_URL))
WBM_NG_PLUGIN_MASSSTORAGE_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_MASSSTORAGE)$(WBM_NG_PLUGIN_MASSSTORAGE_SUFFIX)
WBM_NG_PLUGIN_MASSSTORAGE_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_MASSSTORAGE_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_MASSSTORAGE_MD5_FILE))
WBM_NG_PLUGIN_MASSSTORAGE_MD5_FILE       := $(WBM_NG_PLUGIN_MASSSTORAGE_SOURCE).md5
WBM_NG_PLUGIN_MASSSTORAGE_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_MASSSTORAGE_URL))
WBM_NG_PLUGIN_MASSSTORAGE_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-massstorage
WBM_NG_PLUGIN_MASSSTORAGE_DIR            := $(WBM_NG_PLUGIN_MASSSTORAGE_BUILDROOT_DIR)
WBM_NG_PLUGIN_MASSSTORAGE_LICENSE        := unknown
WBM_NG_PLUGIN_MASSSTORAGE_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_MASSSTORAGE_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-massstorage
else
WBM_NG_PLUGIN_MASSSTORAGE_TARGET_DIR     := /var/www/wbm/plugins/wbm-massstorage
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_MASSSTORAGE_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_MASSSTORAGE_URL)' '$@' '$(WBM_NG_PLUGIN_MASSSTORAGE_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-massstorage.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-massstorage.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-massstorage.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-massstorage.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-massstorage)
	@$(call install_fixup, wbm-ng-plugin-massstorage, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-massstorage, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-massstorage, AUTHOR,"Stefanie Meihoefer, WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-massstorage, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-massstorage, 0, 0, 0755, $(WBM_NG_PLUGIN_MASSSTORAGE_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_MASSSTORAGE_DIR) && \
	for object in $$( find ./* -print ); do \
		if test -f $$object; then \
			$(call install_copy, wbm-ng-plugin-massstorage, 0, 0, 0644, $(WBM_NG_PLUGIN_MASSSTORAGE_DIR)/$$object, $(WBM_NG_PLUGIN_MASSSTORAGE_TARGET_DIR)/$$object); \
		elif test -d $$object; then \
			$(call install_copy, wbm-ng-plugin-massstorage, 0, 0, 0755, $(WBM_NG_PLUGIN_MASSSTORAGE_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-massstorage)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-massstorage.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_MASSSTORAGE_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_MASSSTORAGE)
	@rm -rf $(WBM_NG_PLUGIN_MASSSTORAGE_BUILDROOT_DIR)

# vim: syntax=make
