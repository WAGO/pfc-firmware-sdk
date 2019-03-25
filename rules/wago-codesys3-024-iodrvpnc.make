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
PACKAGES-$(PTXCONF_CDS3_IODRVPNC) += cds3-iodrvpnc

CDS3_IODRVPNC_VERSION    := 0.0.1
CDS3_IODRVPNC            := IoDrvPnc
CDS3_IODRVPNC_DIR        := $(BUILDDIR)/$(CDS3_IODRVPNC)
CDS3_IODRVPNC_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVPNC)
CDS3_IODRVPNC_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVPNC)
CDS3_IODRVPNC_SRC        := $(CDS3_IODRVPNC_SRC_DIR)/$(CDS3_IODRVPNC)
CDS3_IODRVPNC_BIN        := lib$(CDS3_IODRVPNC).so.$(CDS3_IODRVPNC_VERSION)

CDS3_IODRVPNC_PACKAGE_NAME := cds3-iodrvpnc_$(CDS3_IODRVPNC_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_IODRVPNC_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVPNC_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVPNC_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvpnc.get:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	make -C $(CDS3_IODRVPNC_SRC_DIR) $(CDS3_IODRVPNC)Dep.h $(CDS3_IODRVPNC)Itf.h
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvpnc.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_IODRVPNC_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_IODRVPNC_SRC_DIR)/ $(CDS3_IODRVPNC_DIR)/

#	$(call patchin, CDS3_IODRVPNC)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-iodrvpnc.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_IODRVPNC_PATH      := PATH=$(CROSS_PATH)
CDS3_IODRVPNC_MAKE_ENV  := $(CROSS_ENV)
CDS3_IODRVPNC_MAKE_OPT  := CC=$(CROSS_CC)

#CDS3_IODRVPNC_MAKE_OPT += "DBGMODE=-g3"
#CDS3_IODRVPNC_MAKE_OPT += "OPTIMIZE=-O2"


$(STATEDIR)/cds3-iodrvpnc.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_IODRVPNC)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvpnc.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvpnc.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-iodrvpnc)

	@$(call install_fixup,cds3-iodrvpnc,PRIORITY,optional)
	@$(call install_fixup,cds3-iodrvpnc,SECTION,base)
	@$(call install_fixup,cds3-iodrvpnc,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-iodrvpnc,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_IODRVPNC_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_IODRVPNC_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_IODRVPNC_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVPNC_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-iodrvpnc, 0, 0, $(CDS3_IODRVPNC_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-iodrvpnc, 0, 0, 0755, $(CDS3_IODRVPNC_DIR)/$(CDS3_IODRVPNC_BIN), /usr/lib/$(CDS3_IODRVPNC_BIN))
	@$(call install_link, cds3-iodrvpnc, $(CDS3_IODRVPNC_BIN), /usr/lib/lib$(CDS3_IODRVPNC).so);
	@$(call install_link, cds3-iodrvpnc, ../$(CDS3_IODRVPNC_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_IODRVPNC).so);

endif
	@$(call install_finish,cds3-iodrvpnc)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_IODRVPNC_PACKAGE_NAME).ipk $(CDS3_IODRVPNC_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvpnc.clean:
	rm -rf $(STATEDIR)/cds3-iodrvpnc.*
	rm -rf $(PKGDIR)/cds3-iodrvpnc_*
	rm -rf $(CDS3_IODRVPNC_DIR)
