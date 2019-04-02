## -*-makefile-*-
# $Id: wago-codesys3-015-cmpopcua.make 7242 2014-06-24 15:44:38Z timur.celik@wago.com $
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
PACKAGES-$(PTXCONF_CDS3_CMPOPCUA) += cds3-cmpopcua

CDS3_CMPOPCUA_VERSION    := 0.1.0
CDS3_CMPOPCUA            := CmpOPCUA
CDS3_CMPOPCUA_NAME       := CmpOPCUA
CDS3_CMPOPCUA_DIR        := $(BUILDDIR)/$(CDS3_CMPOPCUA)
CDS3_CMPOPCUA_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_CMPOPCUA)
CDS3_CMPOPCUA_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_CMPOPCUA)
CDS3_CMPOPCUA_SRC        := $(CDS3_CMPOPCUA_SRC_DIR)/$(CDS3_CMPOPCUA_NAME)
CDS3_CMPOPCUA_BIN        := lib$(CDS3_CMPOPCUA_NAME).so.$(CDS3_CMPOPCUA_VERSION)

CDS3_CMPOPCUA_PACKAGE_NAME := cds3-cmpopcua_$(CDS3_CMPOPCUA_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_CMPOPCUA_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_CMPOPCUA_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_CMPOPCUA_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-cmpopcua.get:
	@$(call targetinfo)
#	make -C $(CDS3_CMPOPCUA_SRC_DIR) m4headers
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-cmpopcua.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_CMPOPCUA_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_CMPOPCUA_SRC_DIR)/ $(CDS3_CMPOPCUA_DIR)/

#	$(call patchin, CDS3_CMPOPCUA)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-cmpopcua.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_CMPOPCUA_PATH      := PATH=$(CROSS_PATH)
CDS3_CMPOPCUA_MAKE_ENV  := $(CROSS_ENV)
CDS3_CMPOPCUA_MAKE_OPT  := CC=$(CROSS_CC)

#CDS3_CMPOPCUA_MAKE_OPT += "DBGMODE=-g3"
#CDS3_CMPOPCUA_MAKE_OPT += "OPTIMIZE=-O2"


$(STATEDIR)/cds3-cmpopcua.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_CMPOPCUA)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-cmpopcua.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-cmpopcua.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-cmpopcua)

	@$(call install_fixup,cds3-cmpopcua,PRIORITY,optional)
	@$(call install_fixup,cds3-cmpopcua,SECTION,base)
	@$(call install_fixup,cds3-cmpopcua,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-cmpopcua,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_CMPOPCUA_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_CMPOPCUA_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_CMPOPCUA_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_CMPOPCUA_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-cmpopcua, 0, 0, $(CDS3_CMPOPCUA_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-cmpopcua, 0, 0, 0750, $(CDS3_CMPOPCUA_DIR)/$(CDS3_CMPOPCUA_BIN), /usr/lib/$(CDS3_CMPOPCUA_BIN), n)
	@$(call install_link, cds3-cmpopcua, $(CDS3_CMPOPCUA_BIN), /usr/lib/lib$(CDS3_CMPOPCUA_NAME).so);
	@$(call install_link, cds3-cmpopcua, ../$(CDS3_CMPOPCUA_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_CMPOPCUA_NAME).so);

endif
	@$(call install_finish,cds3-cmpopcua)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_CMPOPCUA_PACKAGE_NAME).ipk $(CDS3_CMPOPCUA_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-cmpopcua.clean:
	rm -rf $(STATEDIR)/cds3-cmpopcua.*
	rm -rf $(PKGDIR)/cds3-cmpopcua_*
	rm -rf $(CDS3_CMPOPCUA_DIR)
