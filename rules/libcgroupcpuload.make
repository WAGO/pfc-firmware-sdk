# -*-makefile-*-
#
# Copyright (C) 2015 by <jso@wago.com> JSo
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBCGROUPCPULOAD) += libcgroupcpuload

#
# Paths and names
#
LIBCGROUPCPULOAD_VERSION        := 0.0.1
LIBCGROUPCPULOAD_MD5            :=
LIBCGROUPCPULOAD                := libcgroupcpuload
LIBCGROUPCPULOAD_SUFFIX         :=
LIBCGROUPCPULOAD_URL            :=
LIBCGROUPCPULOAD_BUILDCONFIG    := Release
LIBCGROUPCPULOAD_SRC_DIR        := $(PTXDIST_WORKSPACE)/local_src/$(LIBCGROUPCPULOAD)
LIBCGROUPCPULOAD_BUILDROOT_DIR  := $(BUILDDIR)/$(LIBCGROUPCPULOAD)
LIBCGROUPCPULOAD_DIR            := $(LIBCGROUPCPULOAD_BUILDROOT_DIR)/src
LIBCGROUPCPULOAD_BUILD_DIR      := $(LIBCGROUPCPULOAD_BUILDROOT_DIR)/bin/$(LIBCGROUPCPULOAD_BUILDCONFIG)
LIBCGROUPCPULOAD_LICENSE        := unknown
LIBCGROUPCPULOAD_BIN            := $(LIBCGROUPCPULOAD).so.$(LIBCGROUPCPULOAD_VERSION)
LIBCGROUPCPULOAD_CONF_TOOL      := NO
LIBCGROUPCPULOAD_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(LIBCGROUPCPULOAD_BUILDCONFIG) \
BIN_DIR=$(LIBCGROUPCPULOAD_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(LIBCGROUPCPULOAD_SRC_DIR):
	$(error "Error: $(LIBCGROUPCPULOAD_SRC_DIR): directory not found!")

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libcgroupcpuload.extract: | $(LIBCGROUPCPULOAD_SRC_DIR)
	@$(call targetinfo)
	@mkdir -p $(LIBCGROUPCPULOAD_BUILDROOT_DIR)
	@if [ ! -L $(LIBCGROUPCPULOAD_DIR) ]; then \
	  ln -s $(LIBCGROUPCPULOAD_SRC_DIR) $(LIBCGROUPCPULOAD_DIR); \
	fi
	@$(call touch)


# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(LIBCGROUPCPULOAD_SOURCE):
#	@$(call targetinfo)
#	@$(call get, LIBCGROUPCPULOAD)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#$(STATEDIR)/libcgroupcpuload.prepare:
#	@$(call targetinfo)
#	@$(call clean, $(LIBCGROUPCPULOAD_DIR)/config.cache)
#	cd $(LIBCGROUPCPULOAD_DIR) && \
#		$(LIBCGROUPCPULOAD_PATH) $(LIBCGROUPCPULOAD_ENV) \
#		./configure $(LIBCGROUPCPULOAD_CONF_OPT)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/libcgroupcpuload.compile:
#	@$(call targetinfo)
#	@$(call world/compile, LIBCGROUPCPULOAD)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/libcgroupcpuload.install:
#	@$(call targetinfo)
# @$(call world/install, LIBCGROUPCPULOAD)
#	cp $(LIBCGROUPCPULOAD_DIR)/cgroupcpuload.h $(PTXDIST_SYSROOT_TARGET)/usr/include
#	cp $(LIBCGROUPCPULOAD_DIR)/libcgroupcpuload.so $(PTXDIST_SYSROOT_TARGET)/usr/lib/
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libcgroupcpuload.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libcgroupcpuload)
	@$(call install_fixup, libcgroupcpuload,PRIORITY,optional)
	@$(call install_fixup, libcgroupcpuload,SECTION,base)
	@$(call install_fixup, libcgroupcpuload,AUTHOR,"<jso@wago.com> JSo")
	@$(call install_fixup, libcgroupcpuload,DESCRIPTION,missing)

# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish

	@$(call install_lib, libcgroupcpuload, 0, 0, 0644, libcgroupcpuload)

#	@$(call install_copy, libcgroupcpuload, 0, 0, 0755, $(LIBCGROUPCPULOAD_BUILD_DIR)/$(LIBCGROUPCPULOAD).so, /usr/lib/$(LIBCGROUPCPULOAD_FILE))
#	@$(call install_link, libcgroupcpuload, $(LIBCGROUPCPULOAD_FILE), /usr/lib/$(LIBCGROUPCPULOAD).so.0)
#	@$(call install_link, libcgroupcpuload, $(LIBCGROUPCPULOAD_FILE), /usr/lib/$(LIBCGROUPCPULOAD).so)

	@$(call install_finish, libcgroupcpuload)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libcgroupcpuload.clean:
	@$(call targetinfo)
	@if [ -d $(LIBCGROUPCPULOAD_DIR) ]; then \
	$(LIBCGROUPCPULOAD_MAKE_ENV) $(LIBCGROUPCPULOAD_PATH) $(MAKE) $(MFLAGS) -C $(LIBCGROUPCPULOAD_DIR) clean; \
	fi
	@$(call clean_pkg, LIBCGROUPCPULOAD)
	@rm -rf $(LIBCGROUPCPULOAD_BUILDROOT_DIR)
# vim: syntax=make
