# -*-makefile-*-
# $Id$
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
PACKAGES-$(PTXCONF_CDS3_TSCIODRVCC100) += cds3-tsciodrvcc100

CDS3_TSCIODRVCC100_VERSION   := 1.0.5
CDS3_TSCIODRVCC100           := TscIoDrvCC100
CDS3_TSCIODRVCC100_DIR       := $(BUILDDIR)/$(CDS3_TSCIODRVCC100)
CDS3_TSCIODRVCC100_URL       := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCIODRVCC100)
CDS3_TSCIODRVCC100_SRC_DIR   := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCIODRVCC100)
CDS3_TSCIODRVCC100_SRC       := $(CDS3_TSCIODRVCC100_SRC_DIR)/$(CDS3_TSCIODRVCC100)
CDS3_TSCIODRVCC100_BIN       := lib$(CDS3_TSCIODRVCC100).so.$(CDS3_TSCIODRVCC100_VERSION)

CDS3_TSCIODRVCC100_PACKAGE_NAME := cds3-tsciodrvcc100_$(CDS3_TSCIODRVCC100_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCIODRVCC100_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciodrvcc100.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciodrvcc100.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCIODRVCC100_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCIODRVCC100_SRC_DIR)/ $(CDS3_TSCIODRVCC100_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tsciodrvcc100.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCIODRVCC100_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCIODRVCC100_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCIODRVCC100_MAKE_OPT  := CC=$(CROSS_CC)

$(STATEDIR)/cds3-tsciodrvcc100.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCIODRVCC100)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCIODRVCC100_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscIoDrvCC100
$(STATEDIR)/cds3-tsciodrvcc100.install:
	@$(call targetinfo)

	@mkdir -p $(TSCIODRVCC100_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCIODRVCC100_DIR)/*.h $(TSCIODRVCC100_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCIODRVCC100_DIR) && tar cvzf $(CDS3_TSCIODRVCC100_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCIODRVCC100_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCIODRVCC100_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCIODRVCC100_PACKAGE_NAME).tgz -C $(TSCIODRVCC100_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciodrvcc100.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tsciodrvcc100)

	@$(call install_fixup,cds3-tsciodrvcc100,PRIORITY,optional)
	@$(call install_fixup,cds3-tsciodrvcc100,SECTION,base)
	@$(call install_fixup,cds3-tsciodrvcc100,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tsciodrvcc100,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCIODRVCC100_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCIODRVCC100_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCIODRVCC100_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCIODRVCC100_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tsciodrvcc100, 0, 0, $(CDS3_TSCIODRVCC100_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tsciodrvcc100, 0, 0, 0750, $(CDS3_TSCIODRVCC100_DIR)/$(CDS3_TSCIODRVCC100_BIN), /usr/lib/$(CDS3_TSCIODRVCC100_BIN))
	@$(call install_link, cds3-tsciodrvcc100, ./$(CDS3_TSCIODRVCC100_BIN), /usr/lib/lib$(CDS3_TSCIODRVCC100).so);
	@$(call install_link, cds3-tsciodrvcc100, ../$(CDS3_TSCIODRVCC100_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCIODRVCC100).so);
#	@$(call install_copy, cds3-tsciodrvcc100, 0, 0, 0644, $(CDS3_TSCIODRVCC100_DIR)/$(CDS3_TSCIODRVCC100).cfg, $(PTXCONF_CDS3_PLCCONFIGDIR)/$(CDS3_TSCIODRVCC100).cfg, n)

endif
	@$(call install_finish,cds3-tsciodrvcc100)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCIODRVCC100_PACKAGE_NAME).ipk $(CDS3_TSCIODRVCC100_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciodrvcc100.clean:
	rm -rf $(STATEDIR)/cds3-tsciodrvcc100.*
	rm -rf $(PKGDIR)/cds3-tsciodrvcc100_*
	rm -rf $(CDS3_TSCIODRVCC100_DIR)
