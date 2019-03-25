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
PACKAGES-$(PTXCONF_EEPROM_XSECTION) += eeprom-xsection

#
# Paths and names
#
EEPROM_XSECTION_VERSION	:= 0.0.5
EEPROM_XSECTION		:= eeprom-xsection-$(EEPROM_XSECTION_VERSION)
EEPROM_XSECTION_SRC	:= $(SRCDIR)/eeprom-xsection
EEPROM_XSECTION_DIR		:= $(BUILDDIR)/$(EEPROM_XSECTION)
EEPROM_XSECTION_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/eeprom-xsection.get:
		@$(call targetinfo)
		@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/eeprom-xsection.extract:
	@$(call targetinfo)	
	@mkdir -p $(EEPROM_XSECTION_DIR)
	@rsync -a --exclude=".*" --exclude=objs/ --exclude="*.d" --exclude="*.o" $(EEPROM_XSECTION_SRC)/ $(EEPROM_XSECTION_DIR)
	@$(call touch)


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#EEPROM_XSECTION_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
# EEPROM_XSECTION_CONF_TOOL	:= autoconf
#EEPROM_XSECTION_CONF_OPT	:= $(CROSS_AUTOCONF_USR)

$(STATEDIR)/eeprom-xsection.prepare:
	@$(call targetinfo)
#	@$(call clean, $(EEPROM_XSECTION_DIR)/config.cache)
#	cd $(EEPROM_XSECTION_DIR) && \
#		$(EEPROM_XSECTION_PATH) $(EEPROM_XSECTION_ENV) \
#		./configure $(EEPROM_XSECTION_CONF_OPT)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/eeprom-xsection.compile:
	@$(call targetinfo)
	cd $(EEPROM_XSECTION_DIR) && $(EEPROM_XSECTION_ENV) $(EEPROM_XSECTION_PATH) \
		_HW_PLATFORM_XSECTION=$(PTXCONF_PLATFORM) $(MAKE) CROSS_COMPILE=$(PTXCONF_COMPILER_PREFIX)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/eeprom-xsection.install:
	@$(call targetinfo)
#ifeq ($(PTXCONF_PLATFORM), vtp-ctp)
	@cp $(EEPROM_XSECTION_DIR)/src/pac_boot_table.h $(PTXDIST_SYSROOT_TARGET)/usr/include
#endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/eeprom-xsection.targetinstall:
	@$(call targetinfo)

	@$(call install_init, eeprom-xsection)
	@$(call install_fixup, eeprom-xsection,PRIORITY,optional)
	@$(call install_fixup, eeprom-xsection,SECTION,base)
	@$(call install_fixup, eeprom-xsection,AUTHOR,"<Andrej.Gantvorg@wago.com>")
	@$(call install_fixup, eeprom-xsection,DESCRIPTION,missing)

	@$(call install_copy, eeprom-xsection, 0, 0, 0755, $(EEPROM_XSECTION_DIR)/bin/eeprom-xsection, /usr/sbin/eeprom-xsection)
	
	@$(call install_finish, eeprom-xsection)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/eeprom-xsection.clean:
	@$(call targetinfo)
	@$(call clean_pkg, EEPROM_XSECTION)

# vim: syntax=make
