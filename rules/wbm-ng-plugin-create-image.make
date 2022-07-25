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
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_CREATE_IMAGE) += wbm-ng-plugin-create-image

#
# Paths and names
#
WBM_NG_PLUGIN_CREATE_IMAGE_VERSION        := 1.2.1
WBM_NG_PLUGIN_CREATE_IMAGE                := wbm-create-image-$(WBM_NG_PLUGIN_CREATE_IMAGE_VERSION)
WBM_NG_PLUGIN_CREATE_IMAGE_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_CREATE_IMAGE)
WBM_NG_PLUGIN_CREATE_IMAGE_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_CREATE_IMAGE_URL))
WBM_NG_PLUGIN_CREATE_IMAGE_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_CREATE_IMAGE)$(WBM_NG_PLUGIN_CREATE_IMAGE_SUFFIX)
WBM_NG_PLUGIN_CREATE_IMAGE_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_CREATE_IMAGE_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_CREATE_IMAGE_MD5_FILE))
WBM_NG_PLUGIN_CREATE_IMAGE_MD5_FILE       := $(WBM_NG_PLUGIN_CREATE_IMAGE_SOURCE).md5
WBM_NG_PLUGIN_CREATE_IMAGE_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_CREATE_IMAGE_URL))
WBM_NG_PLUGIN_CREATE_IMAGE_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-create-image
WBM_NG_PLUGIN_CREATE_IMAGE_DIR            := $(WBM_NG_PLUGIN_CREATE_IMAGE_BUILDROOT_DIR)
WBM_NG_PLUGIN_CREATE_IMAGE_LICENSE        := unknown
WBM_NG_PLUGIN_CREATE_IMAGE_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_CREATE_IMAGE_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-create-image
else
WBM_NG_PLUGIN_CREATE_IMAGE_TARGET_DIR     := /var/www/wbm/plugins/wbm-create-image
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_CREATE_IMAGE_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_CREATE_IMAGE_URL)' '$@' '$(WBM_NG_PLUGIN_CREATE_IMAGE_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-create-image.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-create-image.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-create-image.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-create-image.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-create-image)
	@$(call install_fixup, wbm-ng-plugin-create-image, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-create-image, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-create-image, AUTHOR,"Marius Hellmeier, WAGO GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-create-image, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-create-image, 0, 0, 0755, $(WBM_NG_PLUGIN_CREATE_IMAGE_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_CREATE_IMAGE_DIR) && \
	for object in $$( find ./* \( -path './series' -o -path './.ptxdist*' -o -path './.pc*' \) -prune -o -print ); do \
		if test -f $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-create-image, 0, 0, 0644, $(WBM_NG_PLUGIN_CREATE_IMAGE_DIR)/$$object, $(WBM_NG_PLUGIN_CREATE_IMAGE_TARGET_DIR)/$$object); \
		elif test -d $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-create-image, 0, 0, 0755, $(WBM_NG_PLUGIN_CREATE_IMAGE_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-create-image)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-create-image.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_CREATE_IMAGE_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_CREATE_IMAGE)
	@rm -rf $(WBM_NG_PLUGIN_CREATE_IMAGE_BUILDROOT_DIR)

# vim: syntax=make
