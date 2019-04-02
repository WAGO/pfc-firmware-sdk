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
PACKAGES-$(PTXCONF_LIBDALICOMM) += libdalicomm

#
# Paths and names
#
LIBDALICOMM_VERSION		      := 0.0.1
LIBDALICOMM_MD5		          :=
LIBDALICOMM			            := libdalicomm
LIBDALICOMM_URL		          := file://$(PTXDIST_WORKSPACE)/wago_intern/device/src/dali/communicationhandler
LIBDALICOMM_BUILDCONFIG    	:= Release
LIBDALICOMM_SRC_DIR        	:= $(PTXDIST_WORKSPACE)/wago_intern/device/src/dali/communicationhandler
LIBDALICOMM_BUILDROOT_DIR  	:= $(BUILDDIR)/$(LIBDALICOMM)
LIBDALICOMM_DIR		          := $(LIBDALICOMM_BUILDROOT_DIR)/src
LIBDALICOMM_BUILD_DIR      	:= $(LIBDALICOMM_BUILDROOT_DIR)/bin/$(LIBDALICOMM_BUILDCONFIG)
LIBDALICOMM_LICENSE	       	:= unknown
LIBDALICOMM_BIN            	:= $(LIBDALICOMM).so.$(LIBDALICOMM_VERSION)
LIBDALICOMM_CONF_TOOL      	:= NO
LIBDALICOMM_MAKE_ENV       	:= $(CROSS_ENV) \
BUILDCONFIG=$(LIBDALICOMM_BUILDCONFIG) \
BIN_DIR=$(LIBDALICOMM_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build
# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libdalicomm.extract:
	@$(call targetinfo)
	mkdir -p $(LIBDALICOMM_BUILDROOT_DIR)
	@if [ ! -L $(LIBDALICOMM_DIR) ]; then \
		ln -s $(LIBDALICOMM_SRC_DIR) $(LIBDALICOMM_DIR); \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


#$(STATEDIR)/libdalicomm.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, LIBDALICOMM)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/libdalicomm.compile:
#	@$(call targetinfo)
#	@$(call world/compile, LIBDALICOMM)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdalicomm.install:
	@$(call targetinfo)
	@$(call world/install, LIBDALICOMM)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdalicomm.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libdalicomm)
	@$(call install_fixup, libdalicomm,PRIORITY,optional)
	@$(call install_fixup, libdalicomm,SECTION,base)
	@$(call install_fixup, libdalicomm,AUTHOR,"<FMa>")
	@$(call install_fixup, libdalicomm,DESCRIPTION,missing)


	@$(call install_lib, libdalicomm, 0, 0, 0644, libdalicomm)

	@$(call install_finish, libdalicomm)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------
$(STATEDIR)/libdalicomm.clean:
	@$(call targetinfo)
	@if [ -d $(LIBDALICOMM_DIR) ]; then \
		$(LIBDALICOMM_MAKE_ENV) $(LIBDALICOMM_PATH) $(MAKE) $(MFLAGS) -C $(LIBDALICOMM_DIR) clean; \
	fi
	@$(call clean_pkg, LIBDALICOMM)
	@rm -rf $(LIBDALICOMM_BUILDROOT_DIR)

# vim: syntax=make
