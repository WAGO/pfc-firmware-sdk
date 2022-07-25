# -*-makefile-*-
#
# Copyright (C) 2017 by Stefanie Meihöfer (stefanie.meihoefer@wago.com), WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WBM_NG_PLUGIN_SERIAL_INTERFACE) += wbm-ng-plugin-serial-interface

#
# Paths and names
#
WBM_NG_PLUGIN_SERIAL_INTERFACE_VERSION        := 1.1.0
WBM_NG_PLUGIN_SERIAL_INTERFACE                := wbm-serial-interface-$(WBM_NG_PLUGIN_SERIAL_INTERFACE_VERSION)
WBM_NG_PLUGIN_SERIAL_INTERFACE_URL            := $(call jfrog_template_to_url, WBM_NG_PLUGIN_SERIAL_INTERFACE)
WBM_NG_PLUGIN_SERIAL_INTERFACE_SUFFIX         := $(suffix $(WBM_NG_PLUGIN_SERIAL_INTERFACE_URL))
WBM_NG_PLUGIN_SERIAL_INTERFACE_SOURCE         := $(SRCDIR)/$(WBM_NG_PLUGIN_SERIAL_INTERFACE)$(WBM_NG_PLUGIN_SERIAL_INTERFACE_SUFFIX)
WBM_NG_PLUGIN_SERIAL_INTERFACE_MD5             = $(shell [ -f $(WBM_NG_PLUGIN_SERIAL_INTERFACE_MD5_FILE) ] && cat $(WBM_NG_PLUGIN_SERIAL_INTERFACE_MD5_FILE))
WBM_NG_PLUGIN_SERIAL_INTERFACE_MD5_FILE       := $(WBM_NG_PLUGIN_SERIAL_INTERFACE_SOURCE).md5
WBM_NG_PLUGIN_SERIAL_INTERFACE_ARTIFACT        = $(call jfrog_get_filename,$(WBM_NG_PLUGIN_SERIAL_INTERFACE_URL))
WBM_NG_PLUGIN_SERIAL_INTERFACE_BUILDROOT_DIR  := $(BUILDDIR)/wbm-ng-plugin-serial-interface
WBM_NG_PLUGIN_SERIAL_INTERFACE_DIR            := $(WBM_NG_PLUGIN_SERIAL_INTERFACE_BUILDROOT_DIR)
WBM_NG_PLUGIN_SERIAL_INTERFACE_LICENSE        := unknown
WBM_NG_PLUGIN_SERIAL_INTERFACE_MAKE_ENV       :=
ifeq ($(PTXCONF_WBM),y)
WBM_NG_PLUGIN_SERIAL_INTERFACE_TARGET_DIR     := /var/www/wbm-ng/plugins/wbm-serial-interface
else
WBM_NG_PLUGIN_SERIAL_INTERFACE_TARGET_DIR     := /var/www/wbm/plugins/wbm-serial-interface
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(WBM_NG_PLUGIN_SERIAL_INTERFACE_SOURCE):
	@$(call targetinfo)
	${PTXDIST_WORKSPACE}/scripts/wago/artifactory.sh fetch \
        '$(WBM_NG_PLUGIN_SERIAL_INTERFACE_URL)' '$@' '$(WBM_NG_PLUGIN_SERIAL_INTERFACE_MD5_FILE)'

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-serial-interface.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-serial-interface.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-serial-interface.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-serial-interface.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wbm-ng-plugin-serial-interface)
	@$(call install_fixup, wbm-ng-plugin-serial-interface, PRIORITY, optional)
	@$(call install_fixup, wbm-ng-plugin-serial-interface, SECTION, base)
	@$(call install_fixup, wbm-ng-plugin-serial-interface, AUTHOR,"Stefanie Meihoefer, WAGO GmbH \& Co. KG")
	@$(call install_fixup, wbm-ng-plugin-serial-interface, DESCRIPTION, missing)

	# create target directory itself
	@$(call install_copy, wbm-ng-plugin-serial-interface, 0, 0, 0755, $(WBM_NG_PLUGIN_SERIAL_INTERFACE_TARGET_DIR))

	# loop over all files and subdirectories (deep)
	@cd $(WBM_NG_PLUGIN_SERIAL_INTERFACE_DIR) && \
	for object in $$( find ./* \( -path './series' -o -path './.ptxdist*' -o -path './.pc*' \) -prune -o -print ); do \
		if test -f $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-serial-interface, 0, 0, 0644, $(WBM_NG_PLUGIN_SERIAL_INTERFACE_DIR)/$$object, $(WBM_NG_PLUGIN_SERIAL_INTERFACE_TARGET_DIR)/$$object); \
		elif test -d $$object -a ! -h $$object; then \
			$(call install_copy, wbm-ng-plugin-serial-interface, 0, 0, 0755, $(WBM_NG_PLUGIN_SERIAL_INTERFACE_TARGET_DIR)/$$object); \
		fi; \
	done;


	@$(call install_finish, wbm-ng-plugin-serial-interface)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wbm-ng-plugin-serial-interface.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)$(WBM_NG_PLUGIN_SERIAL_INTERFACE_TARGET_DIR)
	@$(call clean_pkg, WBM_NG_PLUGIN_SERIAL_INTERFACE)
	@rm -rf $(WBM_NG_PLUGIN_SERIAL_INTERFACE_BUILDROOT_DIR)

# vim: syntax=make
