# -*-makefile-*-
#
# Copyright (C) 2017 by <FMa>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBDALIMASTER) += libdalimaster

#
# Paths and names
#
LIBDALIMASTER_VERSION		      := 0.0.1
LIBDALIMASTER_MD5		          :=
LIBDALIMASTER			            := libdalimaster
LIBDALIMASTER_URL		          := file://$(PTXDIST_WORKSPACE)/wago_intern/device/src/dali/master-stack
LIBDALIMASTER_BUILDCONFIG    	:= Release
LIBDALIMASTER_SRC_DIR        	:= $(PTXDIST_WORKSPACE)/wago_intern/device/src/dali/master-stack
LIBDALIMASTER_BUILDROOT_DIR  	:= $(BUILDDIR)/$(LIBDALIMASTER)
LIBDALIMASTER_DIR		          := $(LIBDALIMASTER_BUILDROOT_DIR)/src
LIBDALIMASTER_BUILD_DIR      	:= $(LIBDALIMASTER_BUILDROOT_DIR)/bin/$(LIBDALIMASTER_BUILDCONFIG)
LIBDALIMASTER_LICENSE	       	:= unknown
LIBDALIMASTER_BIN            	:= $(LIBDALIMASTER).so.$(LIBDALIMASTER_VERSION)
LIBDALIMASTER_CONF_TOOL      	:= NO
LIBDALIMASTER_MAKE_ENV       	:= $(CROSS_ENV) \
BUILDCONFIG=$(LIBDALIMASTER_BUILDCONFIG) \
BIN_DIR=$(LIBDALIMASTER_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build
# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libdalimaster.extract:
	@$(call targetinfo)
	mkdir -p $(LIBDALIMASTER_BUILDROOT_DIR)
	@if [ ! -L $(LIBDALIMASTER_DIR) ]; then \
		ln -s $(LIBDALIMASTER_SRC_DIR) $(LIBDALIMASTER_DIR); \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


#$(STATEDIR)/libdalimaster.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, LIBDALIMASTER)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/libdalimaster.compile:
#	@$(call targetinfo)
#	@$(call world/compile, LIBDALIMASTER)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdalimaster.install:
	@$(call targetinfo)
	@$(call world/install, LIBDALIMASTER)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdalimaster.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libdalimaster)
	@$(call install_fixup, libdalimaster,PRIORITY,optional)
	@$(call install_fixup, libdalimaster,SECTION,base)
	@$(call install_fixup, libdalimaster,AUTHOR,"<FMa>")
	@$(call install_fixup, libdalimaster,DESCRIPTION,missing)


	@$(call install_lib, libdalimaster, 0, 0, 0644, libdalimaster)

	@$(call install_finish, libdalimaster)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------
$(STATEDIR)/libdalimaster.clean:
	@$(call targetinfo)
	@if [ -d $(LIBDALIMASTER_DIR) ]; then \
		$(LIBDALIMASTER_MAKE_ENV) $(LIBDALIMASTER_PATH) $(MAKE) $(MFLAGS) -C $(LIBDALIMASTER_DIR) clean; \
	fi
	@$(call clean_pkg, LIBDALIMASTER)
	@rm -rf $(LIBDALIMASTER_BUILDROOT_DIR)

# vim: syntax=make
