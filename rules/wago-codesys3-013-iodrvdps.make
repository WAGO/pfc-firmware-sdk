## -*-makefile-*-
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
PACKAGES-$(PTXCONF_CDS3_IODRVDPS) += cds3-iodrvdps

CDS3_IODRVDPS_VERSION    := 0.0.1
CDS3_IODRVDPS            := IoDrvDps
CDS3_IODRVDPS_DIR        := $(BUILDDIR)/$(CDS3_IODRVDPS)
CDS3_IODRVDPS_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVDPS)
CDS3_IODRVDPS_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVDPS)
CDS3_IODRVDPS_SRC        := $(CDS3_IODRVDPS_SRC_DIR)/$(CDS3_IODRVDPS)
CDS3_IODRVDPS_BIN        := lib$(CDS3_IODRVDPS).so.$(CDS3_IODRVDPS_VERSION)

CDS3_IODRVDPS_PACKAGE_NAME := cds3-iodrvdps_$(CDS3_IODRVDPS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_IODRVDPS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDPS_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDPS_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdps.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdps.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_IODRVDPS_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_IODRVDPS_SRC_DIR)/ $(CDS3_IODRVDPS_DIR)/

#	$(call patchin, CDS3_IODRVDPS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-iodrvdps.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_IODRVDPS_PATH      := PATH=$(CROSS_PATH)
CDS3_IODRVDPS_MAKE_ENV  := $(CROSS_ENV)

CDS3_IODRVDPS_MAKE_OPT += "DBGMODE=-g3"
CDS3_IODRVDPS_MAKE_OPT += "OPTIMIZE=-O0"


$(STATEDIR)/cds3-iodrvdps.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_IODRVDPS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdps.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdps.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-iodrvdps)

	@$(call install_fixup,cds3-iodrvdps,PRIORITY,optional)
	@$(call install_fixup,cds3-iodrvdps,SECTION,base)
	@$(call install_fixup,cds3-iodrvdps,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-iodrvdps,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_IODRVDPS_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_IODRVDPS_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_IODRVDPS_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVDPS_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-iodrvdps, 0, 0, $(CDS3_IODRVDPS_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-iodrvdps, 0, 0, 0750, $(CDS3_IODRVDPS_DIR)/$(CDS3_IODRVDPS_BIN), /usr/lib/$(CDS3_IODRVDPS_BIN))
	@$(call install_link, cds3-iodrvdps, ../$(CDS3_IODRVDPS_BIN), /usr/lib/IoDrvDal/lib$(CDS3_IODRVDPS).so);

endif
	@$(call install_finish,cds3-iodrvdps)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_IODRVDPS_PACKAGE_NAME).ipk $(CDS3_IODRVDPS_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdps.clean:
	rm -rf $(STATEDIR)/cds3-iodrvdps.*
	rm -rf $(PKGDIR)/cds3-iodrvdps_*
	rm -rf $(CDS3_IODRVDPS_DIR)
