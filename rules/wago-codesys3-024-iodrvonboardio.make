## -*-makefile-*-
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
PACKAGES-$(PTXCONF_CDS3_IODRVONBOARDIO) += cds3-iodrvonboardio

CDS3_IODRVONBOARDIO_VERSION    := 0.0.1
CDS3_IODRVONBOARDIO            := IoDrvOnboardIO
CDS3_IODRVONBOARDIO_DIR        := $(BUILDDIR)/$(CDS3_IODRVONBOARDIO)
CDS3_IODRVONBOARDIO_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVONBOARDIO)
CDS3_IODRVONBOARDIO_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVONBOARDIO)
CDS3_IODRVONBOARDIO_SRC        := $(CDS3_IODRVONBOARDIO_SRC_DIR)/$(CDS3_IODRVONBOARDIO)
CDS3_IODRVONBOARDIO_BIN        := lib$(CDS3_IODRVONBOARDIO).so.$(CDS3_IODRVONBOARDIO_VERSION)

CDS3_IODRVONBOARDIO_PACKAGE_NAME := cds3-iodrvonboardio_$(CDS3_IODRVONBOARDIO_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_IODRVONBOARDIO_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVONBOARDIO_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVONBOARDIO_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvonboardio.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvonboardio.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_IODRVONBOARDIO_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_IODRVONBOARDIO_SRC_DIR)/ $(CDS3_IODRVONBOARDIO_DIR)/

#	$(call patchin, CDS3_IODRVONBOARDIO)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-iodrvonboardio.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_IODRVONBOARDIO_PATH      := PATH=$(CROSS_PATH)
CDS3_IODRVONBOARDIO_MAKE_ENV  := $(CROSS_ENV)

CDS3_IODRVONBOARDIO_MAKE_OPT += "DBGMODE=-g3"
CDS3_IODRVONBOARDIO_MAKE_OPT += "OPTIMIZE=-O0"


$(STATEDIR)/cds3-iodrvonboardio.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_IODRVONBOARDIO)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvonboardio.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvonboardio.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-iodrvonboardio)

	@$(call install_fixup,cds3-iodrvonboardio,PRIORITY,optional)
	@$(call install_fixup,cds3-iodrvonboardio,SECTION,base)
	@$(call install_fixup,cds3-iodrvonboardio,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-iodrvonboardio,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_IODRVONBOARDIO_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_IODRVONBOARDIO_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_IODRVONBOARDIO_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVONBOARDIO_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-iodrvonboardio, 0, 0, $(CDS3_IODRVONBOARDIO_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-iodrvonboardio, 0, 0, 0755, $(CDS3_IODRVONBOARDIO_DIR)/$(CDS3_IODRVONBOARDIO_BIN), /usr/lib/$(CDS3_IODRVONBOARDIO_BIN))
	@$(call install_link, cds3-iodrvonboardio, ../$(CDS3_IODRVONBOARDIO_BIN), /usr/lib/IoDrvDal/lib$(CDS3_IODRVONBOARDIO).so);

endif
	@$(call install_finish,cds3-iodrvonboardio)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_IODRVONBOARDIO_PACKAGE_NAME).ipk $(CDS3_IODRVONBOARDIO_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvonboardio.clean:
	rm -rf $(STATEDIR)/cds3-iodrvonboardio.*
	rm -rf $(PKGDIR)/cds3-iodrvonboardio_*
	rm -rf $(CDS3_IODRVONBOARDIO_DIR)
