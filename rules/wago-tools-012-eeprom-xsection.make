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
EEPROM_XSECTION_DIR	:= $(BUILDDIR)/$(EEPROM_XSECTION)
EEPROM_XSECTION_LICENSE	:= unknown
EEPROM_XSECTION_MAKE_OPT:= CROSS=$(COMPILER_PREFIX) VPATH=$(EEPROM_XSECTION_SRC)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/eeprom-xsection.prepare:
	@$(call targetinfo)
	mkdir -p "$(EEPROM_XSECTION_DIR)"
	rsync -a --exclude=".*" --exclude="*.o" $(EEPROM_XSECTION_SRC)/ $(EEPROM_XSECTION_DIR)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

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

	@$(call install_copy, eeprom-xsection, 0, 0, 0755, $(EEPROM_XSECTION_DIR)/eeprom-xsection, /usr/sbin/eeprom-xsection)

	@$(call install_finish, eeprom-xsection)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

# vim: syntax=make
