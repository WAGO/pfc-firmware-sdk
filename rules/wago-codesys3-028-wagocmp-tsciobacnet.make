## -*-makefile-*-
# $Id: wago-codesys3-024-tsciobacnet.make 7242 2014-06-24 15:44:38Z joerg.bolte@wago.com $
#
# Copyright (C) 2016 by Wago Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_TSCIOBACNET) += cds3-tsciobacnet

CDS3_TSCIOBACNET_VERSION    := 0.0.1
CDS3_TSCIOBACNET            := TscIoBacnet
CDS3_TSCIOBACNET_NAME       := TscIoBacnet
CDS3_TSCIOBACNET_DIR        := $(BUILDDIR)/$(CDS3_TSCIOBACNET)
CDS3_TSCIOBACNET_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCIOBACNET)
CDS3_TSCIOBACNET_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCIOBACNET)
CDS3_TSCIOBACNET_SRC        := $(CDS3_TSCIOBACNET_SRC_DIR)/$(CDS3_TSCIOBACNET_NAME)
CDS3_TSCIOBACNET_BIN        := lib$(CDS3_TSCIOBACNET_NAME).so.$(CDS3_TSCIOBACNET_VERSION)

CDS3_TSCIOBACNET_PACKAGE_NAME := cds3-tsciobacnet_$(CDS3_TSCIOBACNET_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCIOBACNET_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages


# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciobacnet.get:
	@$(call targetinfo)
#JB???	make -C $(CDS3_TSCIOBACNET_SRC_DIR) m4headers
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciobacnet.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCIOBACNET_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCIOBACNET_SRC_DIR)/ $(CDS3_TSCIOBACNET_DIR)/

#	$(call patchin, CDS3_TSCIOBACNET)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tsciobacnet.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCIOBACNET_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCIOBACNET_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCIOBACNET_MAKE_OPT  := CC=$(CROSS_CC)

CDS3_TSCIOBACNET_MAKE_OPT += "DBGMODE=-g3"
CDS3_TSCIOBACNET_MAKE_OPT += "OPTIMIZE=-O2"


$(STATEDIR)/cds3-tsciobacnet.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCIOBACNET)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciobacnet.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciobacnet.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tsciobacnet)

	@$(call install_fixup,cds3-tsciobacnet,PRIORITY,optional)
	@$(call install_fixup,cds3-tsciobacnet,SECTION,base)
	@$(call install_fixup,cds3-tsciobacnet,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tsciobacnet,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCIOBACNET_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCIOBACNET_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCIOBACNET_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCIOBACNET_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tsciobacnet, 0, 0, $(CDS3_TSCIOBACNET_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tsciobacnet, 0, 0, 0755, $(CDS3_TSCIOBACNET_DIR)/$(CDS3_TSCIOBACNET_BIN), /usr/lib/$(CDS3_TSCIOBACNET_BIN))
	@$(call install_link, cds3-tsciobacnet, $(CDS3_TSCIOBACNET_BIN), /usr/lib/lib$(CDS3_TSCIOBACNET_NAME).so);
	@$(call install_link, cds3-tsciobacnet, ../$(CDS3_TSCIOBACNET_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCIOBACNET_NAME).so);

endif
	@$(call install_finish,cds3-tsciobacnet)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCIOBACNET_PACKAGE_NAME).ipk $(CDS3_TSCIOBACNET_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciobacnet.clean:
	rm -rf $(STATEDIR)/cds3-tsciobacnet.*
	rm -rf $(PKGDIR)/cds3-tsciobacnet_*
	rm -rf $(CDS3_TSCIOBACNET_DIR)
