# -*-makefile-*-
#
# Copyright (C) 2020 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PRINT_LOG) += print_log

#
# Paths and names
#
PRINT_LOG_VERSION        := 1.0.0
PRINT_LOG                := print_log
PRINT_LOG_MD5            :=
PRINT_LOG_BUILDCONFIG    := Release
PRINT_LOG_SRC_DIR        := $(PTXDIST_WORKSPACE)/local_src/config-tools/$(PRINT_LOG)
PRINT_LOG_BUILDROOT_DIR  := $(BUILDDIR)/$(PRINT_LOG)
PRINT_LOG_DIR            := $(PRINT_LOG_BUILDROOT_DIR)/src
PRINT_LOG_BUILD_DIR      := $(PRINT_LOG_BUILDROOT_DIR)/bin/$(PRINT_LOG_BUILDCONFIG)
PRINT_LOG_LICENSE        := WAGO
PRINT_LOG_CONF_TOOL      := NO
PRINT_LOG_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PRINT_LOG_BUILDCONFIG) \
BIN_DIR=$(PRINT_LOG_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(PRINT_LOG_SRC_DIR):
	@echo "Error: $@: directory not found!" >&2; exit 2

$(STATEDIR)/print_log.extract:  | $(PRINT_LOG_SRC_DIR) 
	@$(call targetinfo)
	@mkdir -p $(PRINT_LOG_BUILDROOT_DIR)
	@if [ ! -L $(PRINT_LOG_DIR) ]; then \
	  ln -s $(PRINT_LOG_SRC_DIR) $(PRINT_LOG_DIR); \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/print_log.prepare:
	@$(call targetinfo)
	@$(call world/prepare, PRINT_LOG)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/print_log.compile:
	@$(call targetinfo)
	@$(call world/compile, PRINT_LOG)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/print_log.install:
	@$(call targetinfo)

	@$(call world/install, PRINT_LOG)


	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/print_log.targetinstall:
	@$(call targetinfo)

	@$(call install_init, print_log)
	@$(call install_fixup, print_log,PRIORITY,optional)
	@$(call install_fixup, print_log,SECTION,base)
	@$(call install_fixup, print_log,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, print_log,DESCRIPTION,missing)

	@$(call install_copy, print_log, 0, 0, 0755, $(PRINT_LOG_BUILD_DIR)/print_log.elf, /etc/config-tools/print_log) 
	@$(call install_copy, print_log, 0, 0, 0444, $(PRINT_LOG_DIR)/etc/sudoers.d/print_log, /etc/sudoers.d/print_log)

	@$(call install_finish, print_log)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/print_log.clean:
	@$(call targetinfo)
	@if [ -d $(PRINT_LOG_DIR) ]; then \
		$(PRINT_LOG_MAKE_ENV) $(PRINT_LOG_PATH) $(MAKE) $(MFLAGS) -C $(PRINT_LOG_DIR) clean; \
	fi
	@$(call clean_pkg, PRINT_LOG)
	@rm -rf $(PRINT_LOG_BUILDROOT_DIR)

# vim: syntax=make
