## -*-makefile-*-
# $Id: wago-codesys3-025-iodrvbacnet.make 7242 2014-06-24 15:44:38Z joerg.bolte@wago.com $
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
PACKAGES-$(PTXCONF_CDS3_IODRVBACNET) += cds3-iodrvbacnet

CDS3_IODRVBACNET_VERSION    := 0.0.1
CDS3_IODRVBACNET            := IoDrvBacnet
CDS3_IODRVBACNET_NAME       := IoDrvBacnet
CDS3_IODRVBACNET_DIR        := $(BUILDDIR)/$(CDS3_IODRVBACNET)
CDS3_IODRVBACNET_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVBACNET)
CDS3_IODRVBACNET_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVBACNET)
CDS3_IODRVBACNET_SRC        := $(CDS3_IODRVBACNET_SRC_DIR)/$(CDS3_IODRVBACNET_NAME)
CDS3_IODRVBACNET_BIN        := lib$(CDS3_IODRVBACNET_NAME).so.$(CDS3_IODRVBACNET_VERSION)

CDS3_IODRVBACNET_PACKAGE_NAME := cds3-iodrvbacnet_$(CDS3_IODRVBACNET_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_IODRVBACNET_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages


# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvbacnet.get:
	@$(call targetinfo)
#JB???	make -C $(CDS3_IODRVBACNET_SRC_DIR) m4headers
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvbacnet.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_IODRVBACNET_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_IODRVBACNET_SRC_DIR)/ $(CDS3_IODRVBACNET_DIR)/

#	$(call patchin, CDS3_IODRVBACNET)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-iodrvbacnet.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_IODRVBACNET_PATH      := PATH=$(CROSS_PATH)
CDS3_IODRVBACNET_MAKE_ENV  := $(CROSS_ENV)
CDS3_IODRVBACNET_MAKE_OPT  := CC=$(CROSS_CC)

CDS3_IODRVBACNET_MAKE_OPT += "DBGMODE=-ggdb"
CDS3_IODRVBACNET_MAKE_OPT += "OPTIMIZE=-O0"


$(STATEDIR)/cds3-iodrvbacnet.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_IODRVBACNET)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvbacnet.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvbacnet.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-iodrvbacnet)

	@$(call install_fixup,cds3-iodrvbacnet,PRIORITY,optional)
	@$(call install_fixup,cds3-iodrvbacnet,SECTION,base)
	@$(call install_fixup,cds3-iodrvbacnet,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-iodrvbacnet,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_IODRVBACNET_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_IODRVBACNET_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_IODRVBACNET_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVBACNET_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-iodrvbacnet, 0, 0, $(CDS3_IODRVBACNET_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-iodrvbacnet, 0, 0, 0755, $(CDS3_IODRVBACNET_DIR)/$(CDS3_IODRVBACNET_BIN), /usr/lib/$(CDS3_IODRVBACNET_BIN))
	@$(call install_link, cds3-iodrvbacnet, ../$(CDS3_IODRVBACNET_BIN), /usr/lib/IoDrvDal/lib$(CDS3_IODRVBACNET).so);

#	@$(call install_link, cds3-iodrvbacnet, $(CDS3_IODRVBACNET_BIN), /usr/lib/lib$(CDS3_IODRVBACNET_NAME).so);
#	@$(call install_link, cds3-iodrvbacnet, ../$(CDS3_IODRVBACNET_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_IODRVBACNET_NAME).so);

endif
	@$(call install_finish,cds3-iodrvbacnet)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_IODRVBACNET_PACKAGE_NAME).ipk $(CDS3_IODRVBACNET_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvbacnet.clean:
	rm -rf $(STATEDIR)/cds3-iodrvbacnet.*
	rm -rf $(PKGDIR)/cds3-iodrvbacnet_*
	rm -rf $(CDS3_IODRVBACNET_DIR)
