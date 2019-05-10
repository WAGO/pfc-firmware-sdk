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

MDMD_VERSION := 0.5.0

#
# Paths and names
#
MDMD	     := mdmd
MDMD_URL     := file://$(PTXDIST_WORKSPACE)/local_src/$(MDMD)
MDMD_DIR     := $(BUILDDIR)/$(MDMD)

MDMD_SRC     := $(PTXDIST_WORKSPACE)/local_src/$(MDMD)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#$(STATEDIR)/mdmd.extract:
#	@$(call targetinfo, $@)
#	@$(call clean, $(MDMD_DIR))
#	cp -rd $(MDMD_SOURCE) $(MDMD_DIR)
#	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#MDMD_PATH	:= PATH=$(CROSS_PATH)
MDMD_CONF_TOOL	:= NO
#MDMD_MAKE_ENV	:= $(CROSS_ENV) PKG_CONFIG_PATH=$(PTXDIST_SYSROOT_TARGET)/usr/lib/pkg-config
MDMD_MAKE_ENV	:= $(CROSS_ENV)
MDMD_MAKE_VARS := MDMD_VERSION=$(MDMD_VERSION)

$(STATEDIR)/mdmd.prepare:
	@$(call targetinfo)
	@echo "Create mdmd diagnostic header file in sysroot-target/usr/include/diagnostic directory by using of XSLT-script"
	$(NOPRINT)$(call xslt_compile, $(MDMD_DIR)/mdmd_diag.xml)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/mdmd.compile:
	@$(call targetinfo, $@)
	@cd $(MDMD_DIR) && \
		$(MDMD_MAKE_ENV) \
		$(MAKE)	CROSS_COMPILE=$(COMPILER_PREFIX) SYSROOT=$(PTXDIST_SYSROOT_TARGET) $(MDMD_MAKE_VARS)
	@$(call touch, $@)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/mdmd.install:
	@$(call targetinfo, $@)
	@cd $(MDMD_DIR) && \
		$(MDMD_MAKE_ENV) \
		$(MAKE)	CROSS_COMPILE=$(COMPILER_PREFIX) SYSROOT=$(PTXDIST_SYSROOT_TARGET)
	@$(call touch, $@)
	@$(call targetinfo)
	@$(call world/install, MDMD)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/mdmd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, mdmd)
	@$(call install_fixup, mdmd,PRIORITY,optional)
	@$(call install_fixup, mdmd,SECTION,base)
	@$(call install_fixup, mdmd,AUTHOR,"Torben Hohn <torbenh@linutronix.de>)
	@$(call install_fixup, mdmd,DESCRIPTION,missing)

	@$(call install_copy, mdmd, 0, 0, 0755, -, /usr/bin/mdmd)
	@$(call install_copy, mdmd, 0, 0, 0755, -, /usr/bin/mdm_cuse)
	@$(call install_copy, mdmd, 0, 0, 0755, -, /etc/dbus-1/system.d/mdmd.conf)
	@$(call install_copy, mdmd, 0, 0, 0740, -, /etc/specific/mdmd_init.conf)
	@$(call install_copy, mdmd, 0, 0, 0644, -, /lib/udev/rules.d/76-tty-modem.rules)

	@$(call install_finish, mdmd)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/mdmd.clean:
	$(NOPRINT)$(call xslt_clean, $(MDMD_DIR)/mdmd_diag.xml)
	@$(call targetinfo)
	@-cd $(MDMD_DIR) && \
		$(MDMD_MAKE_ENV) $(MDMD_PATH) $(MAKE) clean
	@$(call clean_pkg, MDMD)

# vim: syntax=make
