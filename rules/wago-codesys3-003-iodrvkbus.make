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
PACKAGES-$(PTXCONF_CDS3_IODRVKBUS) += cds3-iodrvkbus

CDS3_IODRVKBUS_VERSION    := 0.0.1
CDS3_IODRVKBUS            := IoDrvKbus
CDS3_IODRVKBUS_DIR        := $(BUILDDIR)/$(CDS3_IODRVKBUS)
CDS3_IODRVKBUS_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVKBUS)
CDS3_IODRVKBUS_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVKBUS)
CDS3_IODRVKBUS_SRC        := $(CDS3_IODRVKBUS_SRC_DIR)/$(CDS3_IODRVKBUS)
CDS3_IODRVKBUS_BIN        := lib$(CDS3_IODRVKBUS).so.$(CDS3_IODRVKBUS_VERSION)

CDS3_IODRVKBUS_PACKAGE_NAME := cds3-iodrvkbus_$(CDS3_IODRVKBUS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_IODRVKBUS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVKBUS_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVKBUS_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvkbus.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvkbus.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_IODRVKBUS_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_IODRVKBUS_SRC_DIR)/ $(CDS3_IODRVKBUS_DIR)/

#	$(call patchin, CDS3_IODRVKBUS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-iodrvkbus.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_IODRVKBUS_PATH      := PATH=$(CROSS_PATH)
CDS3_IODRVKBUS_MAKE_ENV  := $(CROSS_ENV)

#CDS3_IODRVKBUS_MAKE_OPT += "DBGMODE=-g3"
#CDS3_IODRVKBUS_MAKE_OPT += "OPTIMIZE=-O3"


$(STATEDIR)/cds3-iodrvkbus.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_IODRVKBUS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvkbus.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvkbus.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-iodrvkbus)

	@$(call install_fixup,cds3-iodrvkbus,PRIORITY,optional)
	@$(call install_fixup,cds3-iodrvkbus,SECTION,base)
	@$(call install_fixup,cds3-iodrvkbus,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-iodrvkbus,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_IODRVKBUS_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_IODRVKBUS_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_IODRVKBUS_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVKBUS_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-iodrvkbus, 0, 0, $(CDS3_IODRVKBUS_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-iodrvkbus, 0, 0, 0750, $(CDS3_IODRVKBUS_DIR)/$(CDS3_IODRVKBUS_BIN), /usr/lib/$(CDS3_IODRVKBUS_BIN))
	@$(call install_link, cds3-iodrvkbus, ../$(CDS3_IODRVKBUS_BIN), /usr/lib/IoDrvDal/lib$(CDS3_IODRVKBUS).so);

endif
	@$(call install_finish,cds3-iodrvkbus)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_IODRVKBUS_PACKAGE_NAME).ipk $(CDS3_IODRVKBUS_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvkbus.clean:
	rm -rf $(STATEDIR)/cds3-iodrvkbus.*
	rm -rf $(PKGDIR)/cds3-iodrvkbus_*
	rm -rf $(CDS3_IODRVKBUS_DIR)
