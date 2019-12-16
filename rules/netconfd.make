# -*-makefile-*-
#
# Copyright (C) 2019 by Wago
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_NETCONFD) += netconfd

#
# Paths and names
#
NETCONFD_VERSION	:= 0.0.7
NETCONFD_MD5		:=
NETCONFD		:= netconfd
NETCONFD_URL		:= file://local_src/netconfd
NETCONFD_LICENSE	:= unknown



NETCONFD_BUILDCONFIG  := Debug

NETCONFD_SRC_DIR := $(PTXDIST_WORKSPACE)/local_src/netconfd
NETCONFD_BUILDROOT_DIR  := $(BUILDDIR)/$(NETCONFD)-$(NETCONFD_VERSION)
NETCONFD_DIR            := $(NETCONFD_BUILDROOT_DIR)/src
NETCONFD_BUILD_DIR      := $(NETCONFD_BUILDROOT_DIR)/bin/$(NETCONFD_BUILDCONFIG)
#NETCONFD_BIN := $(NETCONFD).elf.$(NETCONFD_VERSION)
NETCONFD_CONF_TOOL := NO
NETCONFD_MAKE_ENV       := $(CROSS_ENV) \
                            BUILDCONFIG=$(NETCONFD_BUILDCONFIG) \
                            BIN_DIR=$(NETCONFD_BUILD_DIR) \
                            SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

NETCONFD_PACKAGE_NAME := $(NETCONFD)_$(NETCONFD_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)


# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(NETCONFD_SRC_DIR):
	@echo "Error: $@: directory not found!" >&2; \
	exit 2


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/netconfd.extract: | $(NETCONFD_SRC_DIR)
	@$(call targetinfo)
	@echo "create link ($(NETCONFD_DIR) -> $(NETCONFD_SRC_DIR))"
	@mkdir -p $(NETCONFD_BUILDROOT_DIR)
	@if [ ! -L $(NETCONFD_DIR) ]; then \
		ln -s $(NETCONFD_SRC_DIR) $(NETCONFD_DIR); \
	fi
	@$(call touch)


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#$(STATEDIR)/netconfd.prepare:
#  @$(call targetinfo)
#  @$(call world/prepare, NETCONFD)
#  @$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/netconfd.compile:
#	@$(call targetinfo)
#	@$(call world/compile, NETCONFD)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/netconfd.install:
#	@$(call targetinfo)
#	@$(call world/install, NETCONFD)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/netconfd.targetinstall: $(STATEDIR)/config-tools.targetinstall
	@$(call targetinfo)

	@$(call install_init, netconfd)
	@$(call install_fixup, netconfd,PRIORITY,optional)
	@$(call install_fixup, netconfd,SECTION,base)
	@$(call install_fixup, netconfd,AUTHOR,"Wago")
	@$(call install_fixup, netconfd,DESCRIPTION,missing)

	@$(call install_copy, netconfd, 0, 0, 0755, $(NETCONFD_BUILD_DIR)/netconfd.elf, /usr/bin/netconfd)
	@$(call install_copy, netconfd, 0, 0, 0755, $(NETCONFD_BUILD_DIR)/bridgeconfig_dsa.elf, /usr/bin/bc2dsa)

	# install legacy wrapper and override old config tools
	@$(call install_copy, netconfd, 0, 0, 0755,$(NETCONFD_DIR)/root/etc/config-tools/network_config, /etc/config-tools/network_config)
	@$(call install_link, netconfd, network_config, /etc/config-tools/get_dsa_mode)
	@$(call install_link, netconfd, network_config, /etc/config-tools/set_dsa_mode_c)

	@$(call install_copy, netconfd, 0, 0, 0755, $(NETCONFD_DIR)/root/etc/config-tools/backup-restore/backup_netconfd, /etc/config-tools/backup-restore/backup_netconfd)
	
	@$(call install_copy, netconfd, 0, 0, 0750, $(NETCONFD_DIR)/root/etc/init.d/netconfd, /etc/init.d/netconfd)
	@$(call install_link, netconfd, ../init.d/netconfd, /etc/rc.d/S13_netconfd)

	# stuff from wago-confg-000-common.make, to be removed in the end!
	@$(call install_copy, netconfd, 0, 0, 0750, $(NETCONFD_DIR)/root/etc/config-tools/set_dsa_mode, /etc/config-tools/set_dsa_mode)
	@$(call install_copy, netconfd, 0, 0, 0750, $(NETCONFD_DIR)/root/etc/config-tools/config_interfaces, /etc/config-tools/config_interfaces)
	@$(call install_copy, netconfd, 0, 0, 0750, $(NETCONFD_DIR)/root/etc/config-tools/config_wwan, /etc/config-tools/config_wwan)

	# stuff from initmethod-bbinit.make, to be removed in the end!
	@$(call install_copy, netconfd, 0, 0, 0750, $(NETCONFD_DIR)/root/etc/init.d/networking, /etc/init.d/networking)
	@$(call install_copy, netconfd, 0, 0, 0750, $(NETCONFD_DIR)/root/etc/init.d/networking-finish, /etc/init.d/networking-finish)

	# stuff from wago-custom-000-install.make, to be removed in the end!
	@$(call install_copy, netconfd, 0, 0, 0750, $(NETCONFD_DIR)/root/usr/sbin/settings_backup_lib, /usr/sbin/settings_backup_lib)

	# stuff from busybox.make, to be removed in the end!
	@$(call install_copy, netconfd, 0, 0, 0750, $(NETCONFD_DIR)/root/etc/udhcpc.script, /etc/udhcpc.script)

	# stuff from bootpc.make, to be removed in the end!
	@$(call install_copy, netconfd, 0, 0, 0750, $(NETCONFD_DIR)/root/sbin/bootpc-startup.default, /sbin/bootpc-startup)

	@$(call install_finish, netconfd)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/netconfd.clean:
	@$(call targetinfo)
	@-if [ -d $(NETCONFD_DIR) ]; then \
		$(NETCONFD_MAKE_ENV) $(NETCONFD_PATH) $(MAKE) $(MFLAGS) -C $(NETCONFD_DIR) clean; \
	fi
	@$(call clean_pkg, NETCONFD)
	@rm -rf $(NETCONFD_BUILDROOT_DIR)

# vim: syntax=make
