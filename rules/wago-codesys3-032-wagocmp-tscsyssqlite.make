## -*-makefile-*-
# $Id$
#
# Copyright (C) 2018 by WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_TSCSYSSQLITE) += cds3-tscsyssqlite

CDS3_TSCSYSSQLITE_VERSION	 	:= 1.0.1
CDS3_TSCSYSSQLITE              := TscSysSqlite
#CDS3_TSCSYSSQLITE_DIR          := $(BUILDDIR)/$(CDS3_TSCSYSSQLITE)
CDS3_TSCSYSSQLITE_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCSYSSQLITE)
CDS3_TSCSYSSQLITE_BUILDCONFIG  := Release
CDS3_TSCSYSSQLITE_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCSYSSQLITE)

CDS3_TSCSYSSQLITE_BUILDROOT_DIR  := $(BUILDDIR)/$(CDS3_TSCSYSSQLITE)
CDS3_TSCSYSSQLITE_DIR            := $(CDS3_TSCSYSSQLITE_BUILDROOT_DIR)
CDS3_TSCSYSSQLITE_BUILD_DIR      := $(CDS3_TSCSYSSQLITE_BUILDROOT_DIR)/bin/$(CDS3_TSCSYSSQLITE_BUILDCONFIG)

#CDS3_TSCSYSSQLITE_SRC          := $(CDS3_TSCSYSSQLITE_SRC_DIR)/$(CDS3_TSCSYSSQLITE)
CDS3_TSCSYSSQLITE_BIN          := lib$(CDS3_TSCSYSSQLITE).so.$(CDS3_TSCSYSSQLITE_VERSION)
#CDS3_TSCSYSSQLITE_BIN          := libtscsys_sqlite.so

CDS3_TSCSYSSQLITE_PACKAGE_NAME := cds3-tscsyssqlite_$(CDS3_TSCSYSSQLITE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCSYSSQLITE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

CDS3_TSCSYSSQLITE_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(CDS3_TSCSYSSQLITE_BUILDCONFIG) \
BIN_DIR=$(CDS3_TSCSYSSQLITE_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build \
VERSION=$(CDS3_TSCSYSSQLITE_VERSION)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsyssqlite.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsyssqlite.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCSYSSQLITE_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCSYSSQLITE_SRC_DIR)/ $(CDS3_TSCSYSSQLITE_BUILDROOT_DIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscsyssqlite.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
#CDS3_TSCSYSSQLITE_PATH      := PATH=$(CROSS_PATH)
#CDS3_TSCSYSSQLITE_MAKE_ENV  := $(CROSS_ENV)
#CDS3_TSCSYSSQLITE_MAKE_OPT  := CC=$(CROSS_CC)

#CDS3_TSCSYSSQLITE_MAKE_ENV += VERSION=$(CDS3_TSCSYSSQLITE_VERSION)

$(STATEDIR)/cds3-tscsyssqlite.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCSYSSQLITE)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsyssqlite.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsyssqlite.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscsyssqlite)

	@$(call install_fixup,cds3-tscsyssqlite,PRIORITY,optional)
	@$(call install_fixup,cds3-tscsyssqlite,SECTION,base)
	@$(call install_fixup,cds3-tscsyssqlite,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscsyssqlite,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCSYSSQLITE_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCSYSSQLITE_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCSYSSQLITE_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCSYSSQLITE_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscsyssqlite, 0, 0, $(CDS3_TSCSYSSQLITE_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_copy, cds3-tscsyssqlite, 0, 0, 0750, $(CDS3_TSCSYSSQLITE_BUILD_DIR)/lib$(CDS3_TSCSYSSQLITE).so, /usr/lib/$(CDS3_TSCSYSSQLITE_BIN))
	@$(call install_link, cds3-tscsyssqlite, ../$(CDS3_TSCSYSSQLITE_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCSYSSQLITE).so);
	@$(call install_link, cds3-tscsyssqlite, ./$(CDS3_TSCSYSSQLITE_BIN), /usr/lib/lib$(CDS3_TSCSYSSQLITE).so);
	@$(call install_link, cds3-tscsyssqlite, ./$(CDS3_TSCSYSSQLITE_BIN), /usr/lib/lib$(CDS3_TSCSYSSQLITE).so.0);


endif
	@$(call install_finish,cds3-tscsyssqlite)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCSYSSQLITE_PACKAGE_NAME).ipk $(CDS3_TSCSYSSQLITE_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsyssqlite.clean:
	rm -rf $(STATEDIR)/cds3-tscsyssqlite.*
	rm -rf $(PKGDIR)/cds3-tscsyssqlite_*
	rm -rf $(CDS3_TSCSYSSQLITE_BUILDROOT_DIR)
