## -*-makefile-*-
# $Id$
#
# Copyright (C) 2013 by Wago Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_IODRVDALDUMMY) += cds3-iodrvdaldummy

CDS3_IODRVDALDUMMY_VERSION    := 0.0.1
CDS3_IODRVDALDUMMY            := IoDrvDalDummy
CDS3_IODRVDALDUMMY_DIR        := $(BUILDDIR)/$(CDS3_IODRVDALDUMMY)
CDS3_IODRVDALDUMMY_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVDALDUMMY)
CDS3_IODRVDALDUMMY_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVDALDUMMY)
CDS3_IODRVDALDUMMY_SRC        := $(CDS3_IODRVDALDUMMY_SRC_DIR)/$(CDS3_IODRVDALDUMMY)
CDS3_IODRVDALDUMMY_BIN        := lib$(CDS3_IODRVDALDUMMY).so.$(CDS3_IODRVDALDUMMY_VERSION)

CDS3_IODRVDALDUMMY_PACKAGE_NAME := cds3-iodrvdaldummy_$(CDS3_IODRVDALDUMMY_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_IODRVDALDUMMY_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDALDUMMY_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDALDUMMY_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdaldummy.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdaldummy.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_IODRVDALDUMMY_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_IODRVDALDUMMY_SRC_DIR)/ $(CDS3_IODRVDALDUMMY_DIR)/

#	$(call patchin, CDS3_IODRVDALDUMMY)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-iodrvdaldummy.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_IODRVDALDUMMY_PATH      := PATH=$(CROSS_PATH)
CDS3_IODRVDALDUMMY_MAKE_ENV  := $(CROSS_ENV)

#CDS3_IODRVDALDUMMY_MAKE_OPT += "DBGMODE=-g3"
#CDS3_IODRVDALDUMMY_MAKE_OPT += "OPTIMIZE=-O3"


$(STATEDIR)/cds3-iodrvdaldummy.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_IODRVDALDUMMY)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdaldummy.install:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdaldummy.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-iodrvdaldummy)

	@$(call install_fixup,cds3-iodrvdaldummy,PRIORITY,optional)
	@$(call install_fixup,cds3-iodrvdaldummy,SECTION,base)
	@$(call install_fixup,cds3-iodrvdaldummy,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-iodrvdaldummy,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_IODRVDALDUMMY_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_IODRVDALDUMMY_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_IODRVDALDUMMY_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVDALDUMMY_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-iodrvdaldummy, 0, 0, $(CDS3_IODRVDALDUMMY_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-iodrvdaldummy, 0, 0, 0750, $(CDS3_IODRVDALDUMMY_DIR)/$(CDS3_IODRVDALDUMMY_BIN), /usr/lib/$(CDS3_IODRVDALDUMMY_BIN))
	@$(call install_link, cds3-iodrvdaldummy, ../$(CDS3_IODRVDALDUMMY_BIN), /usr/lib/IoDrvDal/lib$(CDS3_IODRVDALDUMMY).so);

endif
	@$(call install_finish,cds3-iodrvdaldummy)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_IODRVDALDUMMY_PACKAGE_NAME).ipk $(CDS3_IODRVDALDUMMY_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdaldummy.clean:
	rm -rf $(STATEDIR)/cds3-iodrvdaldummy.*
	rm -rf $(PKGDIR)/cds3-iodrvdaldummy_*
	rm -rf $(CDS3_IODRVDALDUMMY_DIR)
