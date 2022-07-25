## -*-makefile-*-
# $Id: wago-codesys3-004-iodrvcanm.make 65677 2022-03-11 11:28:13Z falk.werner@wago.com $
#
# Copyright (C) 2013 by WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_IODRVCANM) += cds3-iodrvcanm

CDS3_IODRVCANM_VERSION    := 0.0.1
CDS3_IODRVCANM            := IoDrvCanM
CDS3_IODRVCANM_DIR        := $(BUILDDIR)/$(CDS3_IODRVCANM)
CDS3_IODRVCANM_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVCANM)
CDS3_IODRVCANM_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVCANM)
CDS3_IODRVCANM_SRC        := $(CDS3_IODRVCANM_SRC_DIR)/$(CDS3_IODRVCANM)
CDS3_IODRVCANM_BIN        := lib$(CDS3_IODRVCANM).so.$(CDS3_IODRVCANM_VERSION)

CDS3_IODRVCANM_PACKAGE_NAME := cds3-iodrvcanm_$(CDS3_IODRVCANM_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_IODRVCANM_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVCANM_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVCANM_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvcanm.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvcanm.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_IODRVCANM_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_IODRVCANM_SRC_DIR)/ $(CDS3_IODRVCANM_DIR)/

#	$(call patchin, CDS3_IODRVCAN)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-iodrvcanm.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_IODRVCANM_PATH      := PATH=$(CROSS_PATH)
CDS3_IODRVCANM_MAKE_ENV  := $(CROSS_ENV)

#CDS3_IODRVCANM_MAKE_OPT += "DBGMODE=-g3"
#CDS3_IODRVCANM_MAKE_OPT += "OPTIMIZE=-O3"


$(STATEDIR)/cds3-iodrvcanm.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_IODRVCANM)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvcanm.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvcanm.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-iodrvcanm)

	@$(call install_fixup,cds3-iodrvcanm,PRIORITY,optional)
	@$(call install_fixup,cds3-iodrvcanm,SECTION,base)
	@$(call install_fixup,cds3-iodrvcanm,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-iodrvcanm,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_IODRVCANM_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_IODRVCANM_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_IODRVCANM_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVCANM_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-iodrvcanm, 0, 0, $(CDS3_IODRVCANM_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-iodrvcanm, 0, 0, 0750, $(CDS3_IODRVCANM_DIR)/$(CDS3_IODRVCANM_BIN), /usr/lib/$(CDS3_IODRVCANM_BIN))
	@$(call install_link, cds3-iodrvcanm, ../$(CDS3_IODRVCANM_BIN), /usr/lib/IoDrvDal/lib$(CDS3_IODRVCANM).so);

endif
	@$(call install_finish,cds3-iodrvcanm)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_IODRVCANM_PACKAGE_NAME).ipk $(CDS3_IODRVCANM_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvcanm.clean:
	rm -rf $(STATEDIR)/cds3-iodrvcanm.*
	rm -rf $(PKGDIR)/cds3-iodrvcanm_*
	rm -rf $(CDS3_IODRVCANM_DIR)
