# -*-makefile-*-
#
# Copyright (C) 2022 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_TSCUSERDB) += cds3-tscuserdb

#
# Paths and names
#
CDS3_TSCUSERDB_VERSION        := 0.0.1
CDS3_TSCUSERDB_MD5            :=
CDS3_TSCUSERDB                := cds3-tscuserdb
CDS3_TSCUSERDB_BUILDCONFIG    := Debug
CDS3_TSCUSERDB_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCUSERDB)
CDS3_TSCUSERDB_BUILDROOT_DIR  := $(BUILDDIR)/$(CDS3_TSCUSERDB)
CDS3_TSCUSERDB_DIR            := $(CDS3_TSCUSERDB_BUILDROOT_DIR)/src
CDS3_TSCUSERDB_BUILD_DIR      := $(CDS3_TSCUSERDB_BUILDROOT_DIR)/bin/$(CDS3_TSCUSERDB_BUILDCONFIG)
CDS3_TSCUSERDB_BIN            := libTscUserDB.so.$(CDS3_TSCUSERDB_VERSION)
CDS3_TSCUSERDB_LICENSE        := WAGO
CDS3_TSCUSERDB_CONF_TOOL      := NO
CDS3_TSCUSERDB_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(CDS3_TSCUSERDB_BUILDCONFIG) \
BIN_DIR=$(CDS3_TSCUSERDB_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

CDS3_TSCUSERDB_PACKAGE_NAME             := $(CDS3_TSCUSERDB)_$(CDS3_TSCUSERDB_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCUSERDB_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscuserdb.extract:
	@$(call targetinfo)
	@mkdir -p $(CDS3_TSCUSERDB_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(CDS3_TSCUSERDB_DIR) ]; then \
	  ln -s $(CDS3_TSCUSERDB_SRC_DIR) $(CDS3_TSCUSERDB_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract.post
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/cds3-tscuserdb.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscuserdb.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, CDS3_TSCUSERDB)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscuserdb.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, CDS3_TSCUSERDB)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscuserdb.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(CDS3_TSCUSERDB_PKGDIR) && \
	  tar xvzf $(CDS3_TSCUSERDB_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCUSERDB_PACKAGE_NAME).tgz -C $(CDS3_TSCUSERDB_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, CDS3_TSCUSERDB)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(CDS3_TSCUSERDB_PLATFORMCONFIGPACKAGEDIR)
	@cd $(CDS3_TSCUSERDB_PKGDIR) && tar cvzf $(CDS3_TSCUSERDB_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCUSERDB_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscuserdb.targetinstall:
	@$(call targetinfo)

	@$(call install_init, cds3-tscuserdb)
	@$(call install_fixup, cds3-tscuserdb,PRIORITY,optional)
	@$(call install_fixup, cds3-tscuserdb,SECTION,base)
	@$(call install_fixup, cds3-tscuserdb,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, cds3-tscuserdb,DESCRIPTION,missing)

	@$(call install_lib, cds3-tscuserdb, 0, 0, 0640, libTscUserDB)
	@$(call install_link, cds3-tscuserdb, ../$(CDS3_TSCUSERDB_BIN), /usr/lib/cds3-custom-components/libTscUserDB.so)
	# Linux-PAM service files for CODESYS
	@$(call install_copy, cds3-tscuserdb, 0, 0, 644, -, /etc/pam.d/rts-admin)
	@$(call install_copy, cds3-tscuserdb, 0, 0, 644, -, /etc/pam.d/rts-devel)
	@$(call install_copy, cds3-tscuserdb, 0, 0, 644, -, /etc/pam.d/rts-service)
	@$(call install_copy, cds3-tscuserdb, 0, 0, 644, -, /etc/pam.d/rts-user)
	# Linux-PAM configuration files for CODESYS
	@$(call install_copy, cds3-tscuserdb, 0, 0, 0644, -, /etc/security/rts-admin.conf)
	@$(call install_copy, cds3-tscuserdb, 0, 0, 0644, -, /etc/security/rts-devel.conf)
	@$(call install_copy, cds3-tscuserdb, 0, 0, 0644, -, /etc/security/rts-service.conf)
	@$(call install_copy, cds3-tscuserdb, 0, 0, 0644, -, /etc/security/rts-user.conf)

	@$(call install_finish, cds3-tscuserdb)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscuserdb.clean:
	@$(call targetinfo)
	@if [ -d $(CDS3_TSCUSERDB_DIR) ]; then \
		$(CDS3_TSCUSERDB_MAKE_ENV) $(CDS3_TSCUSERDB_PATH) $(MAKE) $(MFLAGS) -C $(CDS3_TSCUSERDB_DIR) clean; \
	fi
	@$(call clean_pkg, CDS3_TSCUSERDB)
	@rm -rf $(CDS3_TSCUSERDB_BUILDROOT_DIR)

# vim: syntax=make
