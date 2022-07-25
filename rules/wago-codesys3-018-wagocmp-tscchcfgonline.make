## -*-makefile-*-
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
PACKAGES-$(PTXCONF_CDS3_TSCCHCFGONLINE) += cds3-tscchcfgonline

CDS3_TSCCHCFGONLINE_VERSION	 := 0.0.1
CDS3_TSCCHCFGONLINE              := TscChCfgOnline
CDS3_TSCCHCFGONLINE_DIR          := $(BUILDDIR)/$(CDS3_TSCCHCFGONLINE)
CDS3_TSCCHCFGONLINE_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCCHCFGONLINE)
CDS3_TSCCHCFGONLINE_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCCHCFGONLINE)
CDS3_TSCCHCFGONLINE_SRC          := $(CDS3_TSCCHCFGONLINE_SRC_DIR)/$(CDS3_TSCCHCFGONLINE)
CDS3_TSCCHCFGONLINE_BIN          := lib$(CDS3_TSCCHCFGONLINE).so.$(CDS3_TSCCHCFGONLINE_VERSION)

CDS3_TSCCHCFGONLINE_PACKAGE_NAME := cds3-tscchcfgonline_$(CDS3_TSCCHCFGONLINE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCCHCFGONLINE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscchcfgonline.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscchcfgonline.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCCHCFGONLINE_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCCHCFGONLINE_SRC_DIR)/ $(CDS3_TSCCHCFGONLINE_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscchcfgonline.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCCHCFGONLINE_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCCHCFGONLINE_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCCHCFGONLINE_MAKE_OPT  := CC=$(CROSS_CC)

CDS3_TSCCHCFGONLINE_MAKE_ENV += VERSION=$(CDS3_TSCCHCFGONLINE_VERSION)

ifdef PTXCONF_CDS3_TSCCHCFGONLINE_TARGET_PFCXXX
	CDS3_TSCCHCFGONLINE_MAKE_ENV += TRG_PLATFORM=pfc
else
ifdef PTXCONF_CDS3_TSCCHCFGONLINE_TARGET_PAC
	CDS3_TSCCHCFGONLINE_MAKE_ENV += TRG_PLATFORM=pac
endif
endif

$(STATEDIR)/cds3-tscchcfgonline.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCCHCFGONLINE)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCCHCFGONLINE_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscChCfgOnline
$(STATEDIR)/cds3-tscchcfgonline.install:
	@$(call targetinfo)

	@mkdir -p $(TSCCHCFGONLINE_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCCHCFGONLINE_DIR)/*.h $(TSCCHCFGONLINE_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCCHCFGONLINE_DIR) && tar cvzf $(CDS3_TSCCHCFGONLINE_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCCHCFGONLINE_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCCHCFGONLINE_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCCHCFGONLINE_PACKAGE_NAME).tgz -C $(TSCCHCFGONLINE_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscchcfgonline.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscchcfgonline)

	@$(call install_fixup,cds3-tscchcfgonline,PRIORITY,optional)
	@$(call install_fixup,cds3-tscchcfgonline,SECTION,base)
	@$(call install_fixup,cds3-tscchcfgonline,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscchcfgonline,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCCHCFGONLINE_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCCHCFGONLINE_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCCHCFGONLINE_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCCHCFGONLINE_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscchcfgonline, 0, 0, $(CDS3_TSCCHCFGONLINE_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscchcfgonline, 0, 0, 0750, $(CDS3_TSCCHCFGONLINE_DIR)/$(CDS3_TSCCHCFGONLINE_BIN), /usr/lib/$(CDS3_TSCCHCFGONLINE_BIN))
	@$(call install_link, cds3-tscchcfgonline, ./$(CDS3_TSCCHCFGONLINE_BIN), /usr/lib/lib$(CDS3_TSCCHCFGONLINE).so);
	@$(call install_link, cds3-tscchcfgonline, ../$(CDS3_TSCCHCFGONLINE_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCCHCFGONLINE).so);

	@$(call install_copy, cds3-tscchcfgonline, 0, 0, 0750, $(CDS3_TSCCHCFGONLINE_DIR)/chplcconfig, /usr/bin/chplcconfig)

endif
	@$(call install_finish,cds3-tscchcfgonline)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCCHCFGONLINE_PACKAGE_NAME).ipk $(CDS3_TSCCHCFGONLINE_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscchcfgonline.clean:
	rm -rf $(STATEDIR)/cds3-tscchcfgonline.*
	rm -rf $(PKGDIR)/cds3-tscchcfgonline_*
	rm -rf $(CDS3_TSCCHCFGONLINE_DIR)
