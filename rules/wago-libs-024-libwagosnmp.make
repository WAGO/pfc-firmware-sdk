# -*-makefile-*-
#
# Copyright (C) 2015 by <HFS>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBWAGOSNMP) += libwagosnmp

#
# Paths and names
#
LIBWAGOSNMP_VERSION        := 0.0.2
LIBWAGOSNMP_MD5            :=
LIBWAGOSNMP                := libwagosnmp
LIBWAGOSNMP_URL            := file://local_src/$(LIBWAGOSNMP)
LIBWAGOSNMP_BUILDCONFIG    := Release
LIBWAGOSNMP_SRC_DIR        := $(PTXDIST_WORKSPACE)/local_src/$(LIBWAGOSNMP)
LIBWAGOSNMP_BUILDROOT_DIR  := $(BUILDDIR)/$(LIBWAGOSNMP)
LIBWAGOSNMP_DIR            := $(LIBWAGOSNMP_BUILDROOT_DIR)/src
LIBWAGOSNMP_BUILD_DIR      := $(LIBWAGOSNMP_BUILDROOT_DIR)/bin/$(LIBWAGOSNMP_BUILDCONFIG)
LIBWAGOSNMP_LICENSE        := unknown
LIBWAGOSNMP_BIN            := $(LIBWAGOSNMP).so.$(LIBWAGOSNMP_VERSION)
LIBWAGOSNMP_CONF_TOOL      := NO
LIBWAGOSNMP_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(LIBWAGOSNMP_BUILDCONFIG) \
BIN_DIR=$(LIBWAGOSNMP_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build
# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(LIBWAGOSNMP_SRC_DIR):
	$(error "Error: $(LIBWAGOSNMP_SRC_DIR): directory not found!")

$(STATEDIR)/libwagosnmp.extract: | $(LIBWAGOSNMP_SRC_DIR)
	@$(call targetinfo)
	@mkdir -p $(LIBWAGOSNMP_BUILDROOT_DIR)
	@if [ ! -L $(LIBWAGOSNMP_DIR) ]; then \
	  ln -s $(LIBWAGOSNMP_SRC_DIR) $(LIBWAGOSNMP_DIR); \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#$(STATEDIR)/libwagosnmp.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, LIBWAGOSNMP)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/libwagosnmp.compile:
#	@$(call targetinfo)
#	@$(call world/compile, LIBWAGOSNMP)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libwagosnmp.install:
	@$(call targetinfo)
	@$(call world/install, LIBWAGOSNMP)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libwagosnmp.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libwagosnmp)
	@$(call install_fixup, libwagosnmp,PRIORITY,optional)
	@$(call install_fixup, libwagosnmp,SECTION,base)
	@$(call install_fixup, libwagosnmp,AUTHOR,"<HFS>")
	@$(call install_fixup, libwagosnmp,DESCRIPTION,missing)

	@$(call install_lib, libwagosnmp, 0, 0, 0644, libwagosnmp)
	@$(call install_alternative, libwagosnmp, 0, 0, 0644, /etc/snmp/snmpd_wagoagent.conf)

	@$(call install_finish, libwagosnmp)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libwagosnmp.clean:
	@$(call targetinfo)
	@if [ -d $(LIBWAGOSNMP_DIR) ]; then \
		$(LIBWAGOSNMP_MAKE_ENV) $(LIBWAGOSNMP_PATH) $(MAKE) $(MFLAGS) -C $(LIBWAGOSNMP_DIR) clean; \
	fi
	@$(call clean_pkg, LIBWAGOSNMP)
	@rm -rf $(LIBWAGOSNMP_BUILDROOT_DIR)

# vim: syntax=make

