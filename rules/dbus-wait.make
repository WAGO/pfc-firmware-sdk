# -*-makefile-*-
#
# Copyright (C) 2019 by <>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_DBUS_WAIT) += dbus-wait

#
# Paths and names
#
DBUS_WAIT_VERSION	:= 1.0.0
DBUS_WAIT		:= dbus-wait
DBUS_WAIT_URL		:= file://$(PTXDIST_WORKSPACE)/local_src/dbus-wait
DBUS_WAIT_LICENSE	:= unknown

DBUS_WAIT_BUILDCONFIG  := Debug

DBUS_WAIT_SRC_DIR := $(PTXDIST_WORKSPACE)/local_src/dbus-wait
DBUS_WAIT_BUILDROOT_DIR  := $(BUILDDIR)/$(DBUS_WAIT)-$(DBUS_WAIT_VERSION)
DBUS_WAIT_DIR            := $(DBUS_WAIT_BUILDROOT_DIR)/src
DBUS_WAIT_BUILD_DIR      := $(DBUS_WAIT_BUILDROOT_DIR)/bin/$(DBUS_WAIT_BUILDCONFIG)
DBUS_WAIT_SCRIPT_DIR     := $(DBUS_WAIT_DIR)/scripts
#DBUS_WAIT_BIN := $(DBUS_WAIT).elf.$(DBUS_WAIT_VERSION)
DBUS_WAIT_CONF_TOOL := NO
DBUS_WAIT_MAKE_ENV       := $(CROSS_ENV) \
                            BUILDCONFIG=$(DBUS_WAIT_BUILDCONFIG) \
                            BIN_DIR=$(DBUS_WAIT_BUILD_DIR) \
                            SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

DBUS_WAIT_PACKAGE_NAME := $(DBUS_WAIT)_$(DBUS_WAIT_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)

# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(DBUS_WAIT_SRC_DIR):
	@echo "Error: $@: directory not found!" >&2; \
	exit 2


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------
$(STATEDIR)/dbus-wait.extract: | $(DBUS_WAIT_SRC_DIR)
	@$(call targetinfo)
	mkdir -p $(DBUS_WAIT_BUILDROOT_DIR)
	@if [ ! -L $(DBUS_WAIT_DIR) ]; then \
	ln -s $(DBUS_WAIT_SRC_DIR) $(DBUS_WAIT_DIR); \
	fi
	@$(call touch)
# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#DBUS_WAIT_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
#DBUS_WAIT_CONF_TOOL	:= autoconf
#DBUS_WAIT_CONF_OPT	:= $(CROSS_AUTOCONF_USR)

#$(STATEDIR)/dbus-wait.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, DBUS_WAIT)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/dbus-wait.compile:
#	@$(call targetinfo)
#	@$(call world/compile, DBUS_WAIT)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/dbus-wait.install:
#	@$(call targetinfo)
#	@$(call world/install, DBUS_WAIT)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/dbus-wait.targetinstall:
	@$(call targetinfo)

	@$(call install_init, dbus-wait)
	@$(call install_fixup, dbus-wait, PRIORITY, optional)
	@$(call install_fixup, dbus-wait, SECTION, base)
	@$(call install_fixup, dbus-wait, AUTHOR, "<kozec>")
	@$(call install_fixup, dbus-wait, DESCRIPTION, "Simple executable that waits until service with name passed as argument is available and exits with exit code 0. If anything fails, exits with code 1.")

	$(call install_copy, dbus-wait, 0, 0, 0755, $(DBUS_WAIT_BUILD_DIR)/dbus-wait.elf, /usr/bin/dbus-wait)

	@$(call install_finish, dbus-wait)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/dbus-wait.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, DBUS_WAIT)

# vim: syntax=make
