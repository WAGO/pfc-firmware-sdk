# -*-makefile-*-
#
# Copyright (C) 2012 by <Andrej.Gantvorg@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CT_BOARD_SPECIFIC_EXTENSIONS) += ct-board-specific-extensions

#
# Paths and names
#
CT_BOARD_SPECIFIC_EXTENSIONS_VERSION	:= 1.0.0
CT_BOARD_SPECIFIC_EXTENSIONS		:= ct-board-specific-extensions-$(CT_BOARD_SPECIFIC_EXTENSIONS_VERSION)
CT_BOARD_SPECIFIC_EXTENSIONS_SUFFIX	:= 
CT_BOARD_SPECIFIC_EXTENSIONS_URL		:= /$(CT_BOARD_SPECIFIC_EXTENSIONS).$(CT_BOARD_SPECIFIC_EXTENSIONS_SUFFIX)
CT_BOARD_SPECIFIC_EXTENSIONS_SOURCE	:= 
CT_BOARD_SPECIFIC_EXTENSIONS_DIR		:= $(BUILDDIR)/$(CT_BOARD_SPECIFIC_EXTENSIONS)
CT_BOARD_SPECIFIC_EXTENSIONS_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(CT_BOARD_SPECIFIC_EXTENSIONS_SOURCE):
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/ct-board-specific-extensions.extract:
	@$(call targetinfo)
	@$(call touch)

$(STATEDIR)/ct-board-specific-extensions.extract.post:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#CT_BOARD_SPECIFIC_EXTENSIONS_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
# CT_BOARD_SPECIFIC_EXTENSIONS_CONF_TOOL	:= autoconf
#CT_BOARD_SPECIFIC_EXTENSIONS_CONF_OPT	:= $(CROSS_AUTOCONF_USR)

$(STATEDIR)/ct-board-specific-extensions.prepare:
	@$(call targetinfo)
#	@$(call clean, $(CT_BOARD_SPECIFIC_EXTENSIONS_DIR)/config.cache)
#	cd $(CT_BOARD_SPECIFIC_EXTENSIONS_DIR) && \
#		$(CT_BOARD_SPECIFIC_EXTENSIONS_PATH) $(CT_BOARD_SPECIFIC_EXTENSIONS_ENV) \
#		./configure $(CT_BOARD_SPECIFIC_EXTENSIONS_CONF_OPT)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/ct-board-specific-extensions.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ct-board-specific-extensions.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

EXTENSION_SCRIPT_SUFFIXES := defines commons copy_data.sh copy_data_emmc_extensions.sh sd_check_size.sh save_partition.sh \
                             restore_partition.sh switch_boot_partition.sh get_inactive_system_partition.sh config_rs232.sh \
                             config_default_gateway.sh config_ethernet.sh get_boot_project_location.sh \
                             gen_net_if_eth_settings.sh mv88e6321_write_atu.sh config_network_switch_eeprom.sh

$(STATEDIR)/ct-board-specific-extensions.targetinstall:
	@$(call targetinfo)

	@$(call install_init, ct-board-specific-extensions)
	@$(call install_fixup, ct-board-specific-extensions,PRIORITY,optional)
	@$(call install_fixup, ct-board-specific-extensions,SECTION,base)
	@$(call install_fixup, ct-board-specific-extensions,AUTHOR,"<Andrej.Gantvorg@wago.com>")
	@$(call install_fixup, ct-board-specific-extensions,DESCRIPTION,missing)

	@$(call install_copy, ct-board-specific-extensions, 0, 0, 0755, /etc/config-tools);

	@$(call install_alternative, ct-board-specific-extensions, 0, 0, 0644, /etc/config-tools/board_specific_defines)
	@$(call install_replace, ct-board-specific-extensions, /etc/config-tools/board_specific_defines, @CT_EXTENSION_PREFIX@, $(PTXCONF_CT_EXTENSION_PREFIX))

	@for suffix in $(EXTENSION_SCRIPT_SUFFIXES); do \
	    $(call install_copy, ct-board-specific-extensions, 0, 0, 0755, \
                $(PTXDIST_PLATFORMCONFIGDIR)/projectroot/usr/sbin/$(PTXCONF_CT_EXTENSION_PREFIX)_$$suffix, /usr/sbin/$(PTXCONF_CT_EXTENSION_PREFIX)_$$suffix); \
    done

ifdef PTXCONF_CT_FEATURE_UPDATE_BOOTLOADER_ON_TARGET
	@$(call install_copy, ct-board-specific-extensions, 0, 0, 0755, \
            $(PTXDIST_PLATFORMCONFIGDIR)/projectroot/usr/sbin/$(PTXCONF_CT_EXTENSION_PREFIX)_update_bootloader.sh, \
            /usr/sbin/$(PTXCONF_CT_EXTENSION_PREFIX)_update_bootloader.sh)

endif

	@$(call install_finish, ct-board-specific-extensions)
 
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/ct-board-specific-extensions.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, CT_BOARD_SPECIFIC_EXTENSIONS)

# vim: syntax=make
