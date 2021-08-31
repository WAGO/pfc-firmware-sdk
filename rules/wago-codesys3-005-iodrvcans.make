## -*-makefile-*-
# $Id: wago-codesys3-005-iodrvcans.make 54507 2020-12-14 15:09:28Z karsten.nuenthel@wago.com $
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
PACKAGES-$(PTXCONF_CDS3_IODRVCANS) += cds3-iodrvcans

CDS3_IODRVCANS_VERSION    := 0.0.2
CDS3_IODRVCANS            := IoDrvCanS
CDS3_IODRVCANS_DIR        := $(BUILDDIR)/$(CDS3_IODRVCANS)
CDS3_IODRVCANS_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVCANS)
CDS3_IODRVCANS_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVCANS)
CDS3_IODRVCANS_SRC        := $(CDS3_IODRVCANS_SRC_DIR)/$(CDS3_IODRVCANS)
CDS3_IODRVCANS_BIN        := lib$(CDS3_IODRVCANS).so.$(CDS3_IODRVCANS_VERSION)

CDS3_IODRVCANS_PACKAGE_NAME := cds3-iodrvcans_$(CDS3_IODRVCANS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_IODRVCANS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVCANS_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVCANS_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvcans.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvcans.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_IODRVCANS_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_IODRVCANS_SRC_DIR)/ $(CDS3_IODRVCANS_DIR)/

#	$(call patchin, CDS3_IODRVCANS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-iodrvcans.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_IODRVCANS_PATH      := PATH=$(CROSS_PATH)
CDS3_IODRVCANS_MAKE_ENV  := $(CROSS_ENV)

#CDS3_IODRVCANS_MAKE_OPT += "DBGMODE=-g3"
#CDS3_IODRVCANS_MAKE_OPT += "OPTIMIZE=-O3"


$(STATEDIR)/cds3-iodrvcans.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_IODRVCANS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvcans.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvcans.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-iodrvcans)

	@$(call install_fixup,cds3-iodrvcans,PRIORITY,optional)
	@$(call install_fixup,cds3-iodrvcans,SECTION,base)
	@$(call install_fixup,cds3-iodrvcans,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-iodrvcans,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_IODRVCANS_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_IODRVCANS_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_IODRVCANS_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVCANS_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-iodrvcans, 0, 0, $(CDS3_IODRVCANS_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-iodrvcans, 0, 0, 0750, $(CDS3_IODRVCANS_DIR)/$(CDS3_IODRVCANS_BIN), /usr/lib/$(CDS3_IODRVCANS_BIN))
	@$(call install_link, cds3-iodrvcans, ../$(CDS3_IODRVCANS_BIN), /usr/lib/IoDrvDal/lib$(CDS3_IODRVCANS).so);

endif
	@$(call install_finish,cds3-iodrvcans)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_IODRVCANS_PACKAGE_NAME).ipk $(CDS3_IODRVCANS_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvcans.clean:
	rm -rf $(STATEDIR)/cds3-iodrvcans.*
	rm -rf $(PKGDIR)/cds3-iodrvcans_*
	rm -rf $(CDS3_IODRVCANS_DIR)
