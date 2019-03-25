# -*-makefile-*-
#
# Copyright (C) 2017 by <PEn>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CHECKSYSTEM) += checksystem

#
# Paths and names
#
CHECKSYSTEM_VERSION        := 0.0.1
CHECKSYSTEM                := check-system
CHECKSYSTEM_SRC_DIR        := wago_intern/utils/$(CHECKSYSTEM)
CHECKSYSTEM_DIR            := $(BUILDDIR)/$(CHECKSYSTEM)

CHECKSYSTEM_PACKAGE_NAME := checksystem_$(CHECKSYSTEM_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CHECKSYSTEM_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/checksystem.extract:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@cp -r $(CHECKSYSTEM_SRC_DIR) $(BUILDDIR)/
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

CHECKSYSTEM_CONF_TOOL	:= NO

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/checksystem.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/checksystem.install:
	@$(call targetinfo)
	@$(call touch)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Recreate package directory from a preserved archive (see else branch)
	@mkdir -p $(CHECKSYSTEM_PKGDIR)
	@tar xzvf $(CHECKSYSTEM_PLATFORMCONFIGPACKAGEDIR)/$(CHECKSYSTEM_PACKAGE_NAME).tgz -C $(CHECKSYSTEM_PKGDIR)
  
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call world/install, CHECKSYSTEM)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CHECKSYSTEM_PKGDIR) && tar cvzf $(CHECKSYSTEM_PLATFORMCONFIGPACKAGEDIR)/$(CHECKSYSTEM_PACKAGE_NAME).tgz *
endif

endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/checksystem.targetinstall:
	@$(call targetinfo)
	@$(call install_init, checksystem)
	@$(call install_fixup, checksystem,PRIORITY,optional)
	@$(call install_fixup, checksystem,SECTION,base)
	@$(call install_fixup, checksystem,AUTHOR,"<PEn>")
	@$(call install_fixup, checksystem,DESCRIPTION,missing)

	@$(call install_copy, checksystem, 0, 0, 0770, /etc/$(CHECKSYSTEM))
	@$(call install_copy, checksystem, 0, 0, 0770, -, /etc/$(CHECKSYSTEM)/configuration)
	@$(call install_copy, checksystem, 0, 0, 0750, -, /etc/$(CHECKSYSTEM)/check_system.sh)
	@$(call install_copy, checksystem, 0, 0, 0770, -, /etc/$(CHECKSYSTEM)/custom_test_example.sh)
	@$(call install_copy, checksystem, 0, 0, 0770, -, /etc/$(CHECKSYSTEM)/README)
	@$(call install_copy, checksystem, 0, 0, 0770, -, /etc/init.d/logbootevent)
	@$(call install_link, checksystem, ../init.d/logbootevent, /etc/rc.d/S70_logbootevent)

	@$(call install_finish, checksystem)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/checksystem.clean:
	@$(call targetinfo)
	@$(call world/clean, CHECKSYSTEM)
	@$(call clean_pkg, CHECKSYSTEM)
	@rm -rf $(BUILDDIR)/$(CHECKSYSTEM)

# vim: syntax=make

