# -*-makefile-*-
#
# Copyright (C) 2017 by Henrik Lampe (henrik.lampe@wago.com), WAGO Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LICENSES) += licenses

#
# Paths and names
#
LICENSES                := licenses
LICENSES_SRC_DIR				:= /usr/share/licenses
LICENSES_TARGET_DIR			:= /var/www/wbm/licenses
LICENSES_DIR						:= $(BUILDDIR)/$(LICENSES)
LICENSES_VERSION        := 1.0
ifeq ($(PTXCONF_CONFIGFILE_VERSION),"2020.08.0")
LICENSES_PATH						:= $(call ptx/get-alternative, projectroot, $(LICENSES_SRC_DIR)/oss)
else
LICENSES_PATH						:= $(call ptx/get_alternative, projectroot, $(LICENSES_SRC_DIR)/oss)
endif
ifdef PTXCONF_PFC_200_G2
LICENSES_PATH						:= $(LICENSES_PATH)_g2
endif
LICENSES_PACKAGE_NAME 	:= $(LICENSES)_$(LICENSES_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LICENSES_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

LICENSES_LICENSE        := unknown
LICENSES_MAKE_ENV       :=

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# use ptxdist default

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/licenses.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/licenses.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/licenses.install:
	@$(call targetinfo)
	
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#		# BSP mode: install by extracting tgz file
	@mkdir -p $(LICENSES_DIR) && \
		tar xvzf $(LICENSES_PLATFORMCONFIGPACKAGEDIR)/$(LICENSES_PACKAGE_NAME).tgz -C $(LICENSES_DIR)
else
#		normal mode, call "make install" 
	
	@# WBM can only show UTF-8 format, so test and convert licenses
	@$(PTXDIST_WORKSPACE)/wago_intern/tools/create-license-infos/convert_dynamically.sh $(LICENSES_PATH)
	@# license files need a lot of space, so package them and copy to build-target
	@$(PTXDIST_WORKSPACE)/wago_intern/tools/create-license-infos/pack_licenses.sh $(LICENSES_PATH) $(LICENSES_DIR)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#		# save install directory to tgz for BSP mode
	@mkdir -p $(LICENSES_PLATFORMCONFIGPACKAGEDIR)
	@cd $(LICENSES_DIR) && tar cvzf $(LICENSES_PLATFORMCONFIGPACKAGEDIR)/$(LICENSES_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/licenses.targetinstall:
	@$(call targetinfo)

	@$(call install_init, licenses)
	@$(call install_fixup, licenses, PRIORITY, optional)
	@$(call install_fixup, licenses, SECTION, base)
	@$(call install_fixup, licenses, AUTHOR,"Henrik Lampe, WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, licenses, DESCRIPTION, missing)

	
	@# install license files from build-target to plattform.../root/
	@$(call install_tree, licenses, 0, 0, $(LICENSES_DIR), $(LICENSES_SRC_DIR)/oss/)
	@# install general licenses
	@$(call install_alternative, licenses, 0, 0, 0644, $(LICENSES_SRC_DIR)/oss.txt)
	@$(call install_alternative, licenses, 0, 0, 0644, $(LICENSES_SRC_DIR)/wago.txt)
	@$(call install_alternative_tree, licenses, 0, 0, $(LICENSES_SRC_DIR)/oss/generic/)
	@# install link from destination place
	@$(call install_link, licenses, $(LICENSES_SRC_DIR), $(LICENSES_TARGET_DIR))

	@$(call install_finish, licenses)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

# use ptxdist default

# vim: syntax=make
