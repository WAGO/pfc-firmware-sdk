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
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_CERTIFICATE_UPLOADS) += wbm-ng-plugin-certificate-uploads

#
# Paths and names
#
WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_VERSION        := 1.1.0
WBM_NG_PLUGIN_CERTIFICATE_UPLOADS                := wbm-certificate-uploads-$(WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_VERSION)
WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_CERTIFICATE_UPLOADS)
WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_URL))
WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_CERTIFICATE_UPLOADS)$(WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_SUFFIX)
WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_MD5_FILE))
WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_MD5_FILE       := $(WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_SOURCE).md5
WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_URL))
WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-certificate-uploads
WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_DIR            := $(WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_BUILDROOT_DIR)
WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_LICENSE        := unknown
WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-certificate-uploads
else
WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_TARGET_DIR     := /var/www/wbm/plugins/wbm-certificate-uploads
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_URL)' '$@' '$(WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-certificate-uploads.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-certificate-uploads.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-certificate-uploads.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-certificate-uploads.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-certificate-uploads)
	@$(call install_fixup, wbm-ng-plugin-certificate-uploads, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-certificate-uploads, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-certificate-uploads, AUTHOR,"Marius Hellmeier, WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-certificate-uploads, DESCRIPTION, missing)


	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-certificate-uploads, 0, 0, 0755, $(WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_DIR) && \
	for object in $$( find ./* \( -path './series' -o -path './.ptxdist*' -o -path './.pc*' \) -prune -o -print ); do \
		if test -f $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-certificate-uploads, 0, 0, 0644, $(WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_DIR)/$$object, $(WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_TARGET_DIR)/$$object); \
		elif test -d $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-certificate-uploads, 0, 0, 0755, $(WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-certificate-uploads)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-certificate-uploads.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_CERTIFICATE_UPLOADS)
	@rm -rf $(WBM_NG_PLUGIN_CERTIFICATE_UPLOADS_BUILDROOT_DIR)

# vim: syntax=make
