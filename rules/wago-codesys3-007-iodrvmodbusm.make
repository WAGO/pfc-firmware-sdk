## -*-makefile-*-
# $Id: wago-codesys3-005-iodrvmodbusm.make 7242 2013-11-06 10:29:38Z timur.celik@wago.com $
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
PACKAGES-$(PTXCONF_CDS3_IODRVMODBUSM) += cds3-iodrvmodbusm

CDS3_IODRVMODBUSM_VERSION    := 0.1.1
CDS3_IODRVMODBUSM            := IoDrvModbusM
CDS3_IODRVMODBUSM_DIR        := $(BUILDDIR)/$(CDS3_IODRVMODBUSM)
CDS3_IODRVMODBUSM_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVMODBUSM)
CDS3_IODRVMODBUSM_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVMODBUSM)
CDS3_IODRVMODBUSM_SRC        := $(CDS3_IODRVMODBUSM_SRC_DIR)/$(CDS3_IODRVMODBUSM)
CDS3_IODRVMODBUSM_BIN        := lib$(CDS3_IODRVMODBUSM).so.$(CDS3_IODRVMODBUSM_VERSION)

CDS3_IODRVMODBUSM_PACKAGE_NAME := cds3-iodrvmodbusm_$(CDS3_IODRVMODBUSM_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_IODRVMODBUSM_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVMODBUSM_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVMODBUSM_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvmodbusm.get:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	make -C $(CDS3_IODRVMODBUSM_SRC_DIR) $(CDS3_IODRVMODBUSM)Dep.h $(CDS3_IODRVMODBUSM)Itf.h
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvmodbusm.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_IODRVMODBUSM_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_IODRVMODBUSM_SRC_DIR)/ $(CDS3_IODRVMODBUSM_DIR)/

#	$(call patchin, CDS3_IODRVMODBUSM)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-iodrvmodbusm.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_IODRVMODBUSM_PATH      := PATH=$(CROSS_PATH)
CDS3_IODRVMODBUSM_MAKE_ENV  := $(CROSS_ENV)
CDS3_IODRVMODBUSM_MAKE_OPT  := CC=$(CROSS_CC)

#CDS3_IODRVMODBUSM_MAKE_OPT += "DBGMODE=-g3"
#CDS3_IODRVMODBUSM_MAKE_OPT += "OPTIMIZE=-O2"


$(STATEDIR)/cds3-iodrvmodbusm.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_IODRVMODBUSM)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvmodbusm.install:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvmodbusm.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-iodrvmodbusm)

	@$(call install_fixup,cds3-iodrvmodbusm,PRIORITY,optional)
	@$(call install_fixup,cds3-iodrvmodbusm,SECTION,base)
	@$(call install_fixup,cds3-iodrvmodbusm,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-iodrvmodbusm,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_IODRVMODBUSM_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_IODRVMODBUSM_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_IODRVMODBUSM_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVMODBUSM_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-iodrvmodbusm, 0, 0, $(CDS3_IODRVMODBUSM_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-iodrvmodbusm, 0, 0, 0750, $(CDS3_IODRVMODBUSM_DIR)/$(CDS3_IODRVMODBUSM_BIN), /usr/lib/$(CDS3_IODRVMODBUSM_BIN))
	@$(call install_link, cds3-iodrvmodbusm, $(CDS3_IODRVMODBUSM_BIN), /usr/lib/lib$(CDS3_IODRVMODBUSM).so);
	@$(call install_link, cds3-iodrvmodbusm, ../$(CDS3_IODRVMODBUSM_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_IODRVMODBUSM).so);

endif
	@$(call install_finish,cds3-iodrvmodbusm)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_IODRVMODBUSM_PACKAGE_NAME).ipk $(CDS3_IODRVMODBUSM_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvmodbusm.clean:
	rm -rf $(STATEDIR)/cds3-iodrvmodbusm.*
	rm -rf $(PKGDIR)/cds3-iodrvmodbusm_*
	rm -rf $(CDS3_IODRVMODBUSM_DIR)
