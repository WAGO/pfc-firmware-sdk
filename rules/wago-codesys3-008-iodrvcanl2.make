## -*-makefile-*-
# $Id: wago-codesys3-008-iodrvcanl2.make 65677 2022-03-11 11:28:13Z falk.werner@wago.com $
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
PACKAGES-$(PTXCONF_CDS3_IODRVCANL2) += cds3-iodrvcanl2

CDS3_IODRVCANL2_VERSION    := 0.0.1
CDS3_IODRVCANL2            := IoDrvCanL2
CDS3_IODRVCANL2_DIR        := $(BUILDDIR)/$(CDS3_IODRVCANL2)
CDS3_IODRVCANL2_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVCANL2)
CDS3_IODRVCANL2_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVCANL2)
CDS3_IODRVCANL2_SRC        := $(CDS3_IODRVCANL2_SRC_DIR)/$(CDS3_IODRVCANL2)
CDS3_IODRVCANL2_BIN        := lib$(CDS3_IODRVCANL2).so.$(CDS3_IODRVCANL2_VERSION)

CDS3_IODRVCANL2_PACKAGE_NAME := cds3-iodrvcanl2_$(CDS3_IODRVCANL2_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_IODRVCANL2_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVCANL2_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVCANL2_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvcanl2.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvcanl2.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_IODRVCANL2_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_IODRVCANL2_SRC_DIR)/ $(CDS3_IODRVCANL2_DIR)/

#	$(call patchin, CDS3_IODRVCANL2)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-iodrvcanl2.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_IODRVCANL2_PATH      := PATH=$(CROSS_PATH)
CDS3_IODRVCANL2_MAKE_ENV  := $(CROSS_ENV)

#CDS3_IODRVCANL2_MAKE_OPT += "DBGMODE=-g3"
#CDS3_IODRVCANL2_MAKE_OPT += "OPTIMIZE=-O3"


$(STATEDIR)/cds3-iodrvcanl2.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_IODRVCANL2)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvcanl2.install:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvcanl2.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-iodrvcanl2)

	@$(call install_fixup,cds3-iodrvcanl2,PRIORITY,optional)
	@$(call install_fixup,cds3-iodrvcanl2,SECTION,base)
	@$(call install_fixup,cds3-iodrvcanl2,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-iodrvcanl2,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_IODRVCANL2_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_IODRVCANL2_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_IODRVCANL2_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVCANL2_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-iodrvcanl2, 0, 0, $(CDS3_IODRVCANL2_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-iodrvcanl2, 0, 0, 0750, $(CDS3_IODRVCANL2_DIR)/$(CDS3_IODRVCANL2_BIN), /usr/lib/$(CDS3_IODRVCANL2_BIN))
	@$(call install_link, cds3-iodrvcanl2, ../$(CDS3_IODRVCANL2_BIN), /usr/lib/IoDrvDal/lib$(CDS3_IODRVCANL2).so);

endif
	@$(call install_finish,cds3-iodrvcanl2)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_IODRVCANL2_PACKAGE_NAME).ipk $(CDS3_IODRVCANL2_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvcanl2.clean:
	rm -rf $(STATEDIR)/cds3-iodrvcanl2.*
	rm -rf $(PKGDIR)/cds3-iodrvcanl2_*
	rm -rf $(CDS3_IODRVCANL2_DIR)
