# -*-makefile-*-
#
# Copyright (C) 2014 by <AGa>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WAGO_AUTOMOUNT) += wago-automount

WAGO_AUTOMOUNT_VERSION  := 1.0.0
WAGO_AUTOMOUNT_SRCDIR   := $(PTXDIST_WORKSPACE)/local_src/wago-automount
WAGO_AUTOMOUNT_DIR      := $(BUILDDIR)/wago-automount

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-automount.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-automount.extract:
	@$(call targetinfo)
	@cp -r $(WAGO_AUTOMOUNT_SRCDIR) $(BUILDDIR)/
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-automount.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-automount.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-automount.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-automount.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wago-automount)
	@$(call install_fixup,wago-automount,PRIORITY,optional)
	@$(call install_fixup,wago-automount,SECTION,base)
	@$(call install_fixup,wago-automount,AUTHOR,"<AGa>")
	@$(call install_fixup,wago-automount,DESCRIPTION,missing)

	@$(call install_copy, wago-automount, 0, 0, 0755, $(WAGO_AUTOMOUNT_DIR)/wago-automount, /usr/local/bin/wago-automount)
	@$(call install_copy, wago-automount, 0, 0, 0644, $(WAGO_AUTOMOUNT_DIR)/90-wago-automount.rules , /lib/udev/rules.d/90-wago-automount.rules)

	@$(call install_finish,wago-automount)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/wago-automount.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, WAGO_AUTOMOUNT)

# vim: syntax=make
