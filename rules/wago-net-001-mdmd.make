# -*-makefile-*-
#
# Copyright (C) 2013 by <cm>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_MDMD) += mdmd



#
# Paths and names
#
MDMD_VERSION 				:= 0.5.0
MDMD_MD5				:=
MDMD					:= mdmd
MDMD_URL     				:= file://$(PTXDIST_WORKSPACE)/local_src/$(MDMD)
MDMD_BUILDCONFIG			:= Release
MDMD_SRC_DIR				:= $(PTXDIST_WORKSPACE)/local_src/$(MDMD)
MDMD_BUILDROOT_DIR			:= $(BUILDDIR)/$(MDMD)
MDMD_DIR     				:= $(MDMD_BUILDROOT_DIR)/src
MDMD_BUILD_DIR				:= $(MDMD_BUILDROOT_DIR)/bin/$(MDMD_BUILDCONFIG)
MDMD_LICENSE				:= unknown
MDMD_BIN				:= $(MDMD)
MDMD_CONF_TOOL				:= NO
MDMD_MAKE_ENV				:= $(CROSS_ENV) \
BUILDCONFIG=$(MDMD_BUILDCONFIG) \
BIN_DIR=$(MDMD_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build
    
MDMD_PACKAGE_NAME			:= $(MDMD)_$(MDMD_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
MDMD_PLATFORMCONFIGPACKAGEDIR		:= $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------
# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(MDMD_SRC_DIR):
	@echo "Error: $@: directory not found!" >&2; \
         exit 2

$(STATEDIR)/mdmd.extract: | $(MDMD_SRC_DIR)
	@$(call targetinfo)
	@mkdir -p $(MDMD_BUILDROOT_DIR)
	@if [ ! -L $(MDMD_DIR) ]; then \
		ln -s $(MDMD_SRC_DIR) $(MDMD_DIR); \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/mdmd.prepare:
	@$(call targetinfo)
	@$(call world/prepare, MDMD)
	@$(call xslt_compile, $(MDMD_DIR)/mdmd_diag.xml)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/mdmd.compile:
#	@$(call targetinfo)
#	@$(call world/compile, MDMD)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/mdmd.install:
#	@$(call targetinfo)
#	@$(call world/install, MDMD)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/mdmd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, mdmd)
	@$(call install_fixup, mdmd,PRIORITY,optional)
	@$(call install_fixup, mdmd,SECTION,base)
	@$(call install_fixup, mdmd,AUTHOR,"WAGO")
	@$(call install_fixup, mdmd,DESCRIPTION,missing)

	@$(call install_copy, mdmd, 0, 0, 0755, -, /usr/bin/mdmd)
	@$(call install_copy, mdmd, 0, 0, 0755, -, /usr/bin/mdm_cuse)
	@$(call install_copy, mdmd, 0, 0, 0644, -, /etc/dbus-1/system.d/mdmd.conf)
	@$(call install_copy, mdmd, 0, 0, 0640, -, /etc/specific/mdmd_init.conf)
	@$(call install_copy, mdmd, 0, 0, 0644, -, /lib/udev/rules.d/76-tty-modem.rules)

	@$(call install_finish, mdmd)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/mdmd.clean:
	@$(call xslt_clean, $(MDMD_DIR)/mdmd_diag.xml)
	@$(call targetinfo)
	@if [ -d $(MDMD_DIR) ]; then \
		$(MDMD_MAKE_ENV) $(MDMD_PATH) $(MAKE) $(MFLAGS) -C $(MDMD_DIR) clean; \
	fi
	@$(call clean_pkg, MDMD)
	@rm -rf $(MDMD_BUILDROOT_DIR)

# vim: syntax=make
