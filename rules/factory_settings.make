# -*-makefile-*-
#
# Copyright (C) 2020 by WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_FACTORY_SETTINGS) += factory_settings

FACTORY_SETTINGS_VERSION	:= 1.0.0
FACTORY_SETTINGS_MD5            :=
FACTORY_SETTINGS                := factory_settings
FACTORY_SETTINGS_URL            := file://local_src/factory_settings
FACTORY_SETTINGS_LICENSE        := MPLv2

FACTORY_SETTINGS_DIR  := $(BUILDDIR)/$(FACTORY_SETTINGS)-$(FACTORY_SETTINGS_VERSION)
FACTORY_SETTINGS_CONF_TOOL      := NO

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(STATEDIR)/factory_settings.get:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#$(STATEDIR)/factory_settings.extract:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#$(STATEDIR)/factory_settings.prepare:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/factory_settings.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/factory_settings.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/factory_settings.targetinstall:
	@$(call targetinfo)

	@$(call install_init, factory_settings)
	@$(call install_fixup,factory_settings,PRIORITY,optional)
	@$(call install_fixup,factory_settings,SECTION,base)
	@$(call install_fixup,factory_settings,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup,factory_settings,DESCRIPTION,missing)

	# install factory settings
	@$(call install_copy, factory_settings, 0, 0, 750, $(FACTORY_SETTINGS_DIR)/etc/config-tools/settings_factory, /etc/config-tools/settings_factory)

	# install start script
	# since netconfd is needed to store network settings, create_factory_settings should
	# run quite after (S14a)
	@$(call install_copy, factory_settings, 0, 0, 0750, $(FACTORY_SETTINGS_DIR)/etc/init.d/create_factory_settings, /etc/init.d/create_factory_settings)
	@$(call install_link, factory_settings, ../init.d/create_factory_settings, /etc/rc.d/S14a_create_factory_settings)

	# install license file
	@$(call install_copy, factory_settings, 0, 0, 0644, $(FACTORY_SETTINGS_DIR)/LICENSE, /usr/share/licenses/oss/license.factory-settings_$(FACTORY_SETTINGS_VERSION).txt)


	@$(call install_finish,factory_settings)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/factory_settings.clean:
	@$(call targetinfo)
	@$(call clean_pkg, FACTORY_SETTINGS)

# vim: syntax=make
