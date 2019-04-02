# -*-makefile-*-
#
# Copyright (C) 2017 by <PEn>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBCOMMONHEADER) += libcommonheader

#
# Paths and names
#
LIBCOMMONHEADER_VERSION        := 0.0.1
LIBCOMMONHEADER_MD5            :=
LIBCOMMONHEADER                := common-header
LIBCOMMONHEADER_URL            := file://shared_public/$(LIBCOMMONHEADER)
LIBCOMMONHEADER_BUILDCONFIG    := Release
LIBCOMMONHEADER_SRC_DIR        := $(PTXDIST_WORKSPACE)/shared_public/$(LIBCOMMONHEADER)
LIBCOMMONHEADER_BUILDROOT_DIR  := $(BUILDDIR)/$(LIBCOMMONHEADER)
#LIBCOMMONHEADER_DIR            := $(LIBCOMMONHEADER_BUILDROOT_DIR)/src
LIBCOMMONHEADER_BUILD_DIR      := $(LIBCOMMONHEADER_BUILDROOT_DIR)/bin/$(LIBCOMMONHEADER_BUILDCONFIG)
LIBCOMMONHEADER_LICENSE        := MPL-2.0
LIBCOMMONHEADER_BIN            := 
LIBCOMMONHEADER_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(LIBCOMMONHEADER_BUILDCONFIG) \
BIN_DIR=$(LIBCOMMONHEADER_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build
# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libcommonheader.extract:
	@$(call targetinfo)

	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBCOMMONHEADER_CONF_TOOL	:= NO

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/libcommonheader.compile:
#	@$(call targetinfo)
#	@$(call world/compile, LIBCOMMONHEADER)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libcommonheader.install:
	@$(call targetinfo)
	@mkdir -p $(PTXCONF_SYSROOT_TARGET)/usr/include/wc/
	@install -m 0644 $(LIBCOMMONHEADER_SRC_DIR)/inc/wc/*.h \
		$(PTXCONF_SYSROOT_TARGET)/usr/include/wc
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libcommonheader.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libcommonheader)
	@$(call install_fixup, libcommonheader,PRIORITY,optional)
	@$(call install_fixup, libcommonheader,SECTION,base)
	@$(call install_fixup, libcommonheader,AUTHOR,"<PEn>")
	@$(call install_fixup, libcommonheader,DESCRIPTION,missing)

	@$(call install_finish, libcommonheader)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libcommonheader.clean:
	@$(call targetinfo)
	@rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/wc/
	
	@$(call clean_pkg, LIBCOMMONHEADER)

# vim: syntax=make

