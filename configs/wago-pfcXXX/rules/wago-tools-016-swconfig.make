# -*-makefile-*-
#
# Copyright (C) 2013 by Heinrich Toews <heinrich.toews@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_SWCONFIG) += swconfig

#
# Paths and names
#
SWCONFIG_VERSION	:= 0
SWCONFIG		:= swconfig
SWCONFIG_URL		:= file://$(PTXDIST_WORKSPACE)/local_src/$(SWCONFIG)
SWCONFIG_DIR		:= $(BUILDDIR)/$(SWCONFIG)
SWCONFIG_BUILD_OOT	:= NO
SWCONFIG_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/swconfig.extract: $(STATEDIR)/autogen-tools

$(STATEDIR)/swconfig.extract:
	@$(call targetinfo)
	@$(call clean, $(SWCONFIG_DIR))
	@$(call extract, SWCONFIG)
	cd $(SWCONFIG_DIR) && sh autogen.sh
	@$(call patchin, SWCONFIG)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

SWCONFIG_CONF_ENV	:= $(CROSS_ENV) CFLAGS="$(CROSS_CFLAGS) -I$(PTXCONF_SYSROOT_TARGET)/usr/include/libnl3 -I$(KERNEL_HEADERS_DIR)/include"
SWCONFIG_PATH		:= PATH=$(CROSS_PATH)

#
# autoconf
#
SWCONFIG_CONF_TOOL	:= autoconf
SWCONFIG_CONF_OPT	:= $(CROSS_AUTOCONF_USR)

$(STATEDIR)/swconfig.prepare:
	@$(call targetinfo)
	@$(call clean, $(SWCONFIG_DIR)/config.cache)
	cd $(SWCONFIG_DIR) && \
		$(SWCONFIG_PATH) $(SWCONFIG_CONF_ENV) \
		./configure $(SWCONFIG_CONF_OPT)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/swconfig.compile:
	@$(call targetinfo)
	cd $(SWCONFIG_DIR) && $(SWCONFIG_ENV) $(MAKE) $(PARALLELMFLAGS)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/swconfig.install:
#	@$(call targetinfo)
#	@$(call world/install, SWCONFIG)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/swconfig.targetinstall:
	@$(call targetinfo)

	@$(call install_init, swconfig)
	@$(call install_fixup, swconfig, PRIORITY, optional)
	@$(call install_fixup, swconfig, SECTION, base)
	@$(call install_fixup, swconfig, AUTHOR, "Heinrich Toews <heinrich.toews@wago.com>")
	@$(call install_fixup, swconfig, DESCRIPTION, missing)

	@$(call install_copy, swconfig, 0, 0, 0755, -, /usr/bin/swconfig);
	@$(call install_lib, swconfig, 0, 0, 0644, libswconfig)

	@$(call install_finish, swconfig)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/swconfig.clean:
	@$(call targetinfo)
	@make clean -C $(SWCONFIG_DIR)
	@$(call clean_pkg, SWCONFIG)

# vim: syntax=make
