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
PACKAGES-$(PTXCONF_LIBDALIDRIVER) += libdalidriver

#
# Paths and names
#
LIBDALIDRIVER_VERSION		      := 0.0.1
LIBDALIDRIVER_MD5		          :=
LIBDALIDRIVER			            := libdalidriver
LIBDALIDRIVER_URL		          := file://$(PTXDIST_WORKSPACE)/wago_intern/device/src/dali/dalifecdriver
LIBDALIDRIVER_BUILDCONFIG    	:= Release
LIBDALIDRIVER_SRC_DIR        	:= $(PTXDIST_WORKSPACE)/wago_intern/device/src/dali/dalifecdriver
LIBDALIDRIVER_BUILDROOT_DIR  	:= $(BUILDDIR)/$(LIBDALIDRIVER)
LIBDALIDRIVER_DIR		          := $(LIBDALIDRIVER_BUILDROOT_DIR)/src
LIBDALIDRIVER_BUILD_DIR      	:= $(LIBDALIDRIVER_BUILDROOT_DIR)/bin/$(LIBDALIDRIVER_BUILDCONFIG)
LIBDALIDRIVER_LICENSE	       	:= unknown
LIBDALIDRIVER_BIN            	:= $(LIBDALIDRIVER).so.$(LIBDALIDRIVER_VERSION)
LIBDALIDRIVER_CONF_TOOL      	:= NO
LIBDALIDRIVER_MAKE_ENV       	:= $(CROSS_ENV) \
BUILDCONFIG=$(LIBDALIDRIVER_BUILDCONFIG) \
BIN_DIR=$(LIBDALIDRIVER_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build
# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libdalidriver.extract:
	@$(call targetinfo)
	mkdir -p $(LIBDALIDRIVER_BUILDROOT_DIR)
	@if [ ! -L $(LIBDALIDRIVER_DIR) ]; then \
		ln -s $(LIBDALIDRIVER_SRC_DIR) $(LIBDALIDRIVER_DIR); \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


#$(STATEDIR)/libdalidriver.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, LIBDALIDRIVER)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/libdalidriver.compile:
#	@$(call targetinfo)
#	@$(call world/compile, LIBDALIDRIVER)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdalidriver.install:
	@$(call targetinfo)
	@$(call world/install, LIBDALIDRIVER)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdalidriver.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libdalidriver)
	@$(call install_fixup, libdalidriver,PRIORITY,optional)
	@$(call install_fixup, libdalidriver,SECTION,base)
	@$(call install_fixup, libdalidriver,AUTHOR,"<FMa>")
	@$(call install_fixup, libdalidriver,DESCRIPTION,missing)


	@$(call install_lib, libdalidriver, 0, 0, 0644, libdalidriver)

	@$(call install_finish, libdalidriver)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------
$(STATEDIR)/libdalidriver.clean:
	@$(call targetinfo)
	@if [ -d $(LIBDALIDRIVER_DIR) ]; then \
		$(LIBDALIDRIVER_MAKE_ENV) $(LIBDALIDRIVER_PATH) $(MAKE) $(MFLAGS) -C $(LIBDALIDRIVER_DIR) clean; \
	fi
	@$(call clean_pkg, LIBDALIDRIVER)
	@rm -rf $(LIBDALIDRIVER_BUILDROOT_DIR)

# vim: syntax=make
