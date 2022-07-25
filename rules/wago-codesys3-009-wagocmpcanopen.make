## -*-makefile-*-
# $Id: wago-codesys3-009-wagocmpcanopen.make 65677 2022-03-11 11:28:13Z falk.werner@wago.com $
#
# Copyright (C) 2014 by WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_CMPCANCONFIG) += cds3-cmpcanconfig

CDS3_CMPCANCONFIG_VERSION      := 0.0.1
CDS3_CMPCANCONFIG              := CmpCanConfig
CDS3_CMPCANCONFIG_DIR          := $(BUILDDIR)/$(CDS3_CMPCANCONFIG)
CDS3_CMPCANCONFIG_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_CMPCANCONFIG)
CDS3_CMPCANCONFIG_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_CMPCANCONFIG)
CDS3_CMPCANCONFIG_SRC          := $(CDS3_CMPCANCONFIG_SRC_DIR)/$(CDS3_CMPCANCONFIG)
CDS3_CMPCANCONFIG_BIN          := lib$(CDS3_CMPCANCONFIG).so.$(CDS3_CMPCANCONFIG_VERSION)

CDS3_CMPCANCONFIG_PACKAGE_NAME := cds3-cmpcanconfig_$(CDS3_CMPCANCONFIG_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_CMPCANCONFIG_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-cmpcanconfig.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-cmpcanconfig.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_CMPCANCONFIG_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_CMPCANCONFIG_SRC_DIR)/ $(CDS3_CMPCANCONFIG_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-cmpcanconfig.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_CMPCANCONFIG_PATH      := PATH=$(CROSS_PATH)
CDS3_CMPCANCONFIG_MAKE_ENV  := $(CROSS_ENV)
CDS3_CMPCANCONFIG_MAKE_OPT  := CC=$(CROSS_CC)

#CDS3_IODRVDAL_MAKE_OPT += "DBGMODE=-g3"
#CDS3_IODRVDAL_MAKE_OPT += "OPTIMIZE=-O3"


$(STATEDIR)/cds3-cmpcanconfig.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_CMPCANCONFIG)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

CMPCANCONFIG_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/CmpKbusAsyncCom
$(STATEDIR)/cds3-cmpcanconfig.install:
	@$(call targetinfo)

	@mkdir -p $(CMPCANCONFIG_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_CMPCANCONFIG_DIR)/*.h $(CMPCANCONFIG_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_CMPCANCONFIG_DIR) && tar cvzf $(CDS3_CMPCANCONFIG_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_CMPCANCONFIG_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_CMPCANCONFIG_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_CMPCANCONFIG_PACKAGE_NAME).tgz -C $(CMPCANCONFIG_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-cmpcanconfig.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-cmpcanconfig)

	@$(call install_fixup,cds3-cmpcanconfig,PRIORITY,optional)
	@$(call install_fixup,cds3-cmpcanconfig,SECTION,base)
	@$(call install_fixup,cds3-cmpcanconfig,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-cmpcanconfig,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_CMPCANCONFIG_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_CMPCANCONFIG_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_CMPCANCONFIG_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_CMPCANCONFIG_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-cmpcanconfig, 0, 0, $(CDS3_CMPCANCONFIG_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-cmpcanconfig, 0, 0, 0750, $(CDS3_CMPCANCONFIG_DIR)/$(CDS3_CMPCANCONFIG_BIN), /usr/lib/$(CDS3_CMPCANCONFIG_BIN))
	@$(call install_link, cds3-cmpcanconfig, ./$(CDS3_CMPCANCONFIG_BIN), /usr/lib/lib$(CDS3_CMPCANCONFIG).so);
	@$(call install_link, cds3-cmpcanconfig, ../$(CDS3_CMPCANCONFIG_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_CMPCANCONFIG).so);

endif
	@$(call install_finish,cds3-cmpcanconfig)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_CMPCANCONFIG_PACKAGE_NAME).ipk $(CDS3_CMPCANCONFIG_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-cmpcanconfig.clean:
	rm -rf $(STATEDIR)/cds3-cmpcanconfig.*
	rm -rf $(PKGDIR)/cds3-cmpcanconfig_*
	rm -rf $(CDS3_CMPCANCONFIG_DIR)
