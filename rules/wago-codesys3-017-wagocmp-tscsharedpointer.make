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
PACKAGES-$(PTXCONF_CDS3_TSCSHAREDPOINTER) += cds3-tscsharedpointer

CDS3_TSCSHAREDPOINTER_VERSION	 := 0.0.1
CDS3_TSCSHAREDPOINTER              := TscSharedPointer
CDS3_TSCSHAREDPOINTER_DIR          := $(BUILDDIR)/$(CDS3_TSCSHAREDPOINTER)
CDS3_TSCSHAREDPOINTER_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCSHAREDPOINTER)
CDS3_TSCSHAREDPOINTER_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCSHAREDPOINTER)
CDS3_TSCSHAREDPOINTER_SRC          := $(CDS3_TSCSHAREDPOINTER_SRC_DIR)/$(CDS3_TSCSHAREDPOINTER)
CDS3_TSCSHAREDPOINTER_BIN          := lib$(CDS3_TSCSHAREDPOINTER).so.$(CDS3_TSCSHAREDPOINTER_VERSION)

CDS3_TSCSHAREDPOINTER_PACKAGE_NAME := cds3-tscsharedpointer_$(CDS3_TSCSHAREDPOINTER_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCSHAREDPOINTER_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsharedpointer.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsharedpointer.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCSHAREDPOINTER_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCSHAREDPOINTER_SRC_DIR)/ $(CDS3_TSCSHAREDPOINTER_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscsharedpointer.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCSHAREDPOINTER_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCSHAREDPOINTER_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCSHAREDPOINTER_MAKE_OPT  := CC=$(CROSS_CC)

CDS3_TSCSHAREDPOINTER_MAKE_ENV += VERSION=$(CDS3_TSCSHAREDPOINTER_VERSION)

ifdef PTXCONF_CDS3_TSCSHAREDPOINTER_TARGET_PFCXXX
	CDS3_TSCSHAREDPOINTER_MAKE_ENV += TRG_PLATFORM=pfc
else
ifdef PTXCONF_CDS3_TSCSHAREDPOINTER_TARGET_PAC
	CDS3_TSCSHAREDPOINTER_MAKE_ENV += TRG_PLATFORM=pac
endif
endif

$(STATEDIR)/cds3-tscsharedpointer.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCSHAREDPOINTER)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCSHAREDPOINTER_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscSharedPointer
$(STATEDIR)/cds3-tscsharedpointer.install:
	@$(call targetinfo)

	@mkdir -p $(TSCSHAREDPOINTER_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCSHAREDPOINTER_DIR)/*.h $(TSCSHAREDPOINTER_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCSHAREDPOINTER_DIR) && tar cvzf $(CDS3_TSCSHAREDPOINTER_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCSHAREDPOINTER_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCSHAREDPOINTER_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCSHAREDPOINTER_PACKAGE_NAME).tgz -C $(TSCSHAREDPOINTER_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsharedpointer.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscsharedpointer)

	@$(call install_fixup,cds3-tscsharedpointer,PRIORITY,optional)
	@$(call install_fixup,cds3-tscsharedpointer,SECTION,base)
	@$(call install_fixup,cds3-tscsharedpointer,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscsharedpointer,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCSHAREDPOINTER_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCSHAREDPOINTER_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCSHAREDPOINTER_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCSHAREDPOINTER_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscsharedpointer, 0, 0, $(CDS3_TSCSHAREDPOINTER_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscsharedpointer, 0, 0, 0750, $(CDS3_TSCSHAREDPOINTER_DIR)/$(CDS3_TSCSHAREDPOINTER_BIN), /usr/lib/$(CDS3_TSCSHAREDPOINTER_BIN))
	@$(call install_link, cds3-tscsharedpointer, ./$(CDS3_TSCSHAREDPOINTER_BIN), /usr/lib/lib$(CDS3_TSCSHAREDPOINTER).so);
	@$(call install_link, cds3-tscsharedpointer, ../$(CDS3_TSCSHAREDPOINTER_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCSHAREDPOINTER).so);
	

endif
	@$(call install_finish,cds3-tscsharedpointer)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCSHAREDPOINTER_PACKAGE_NAME).ipk $(CDS3_TSCSHAREDPOINTER_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsharedpointer.clean:
	rm -rf $(STATEDIR)/cds3-tscsharedpointer.*
	rm -rf $(PKGDIR)/cds3-tscsharedpointer_*
	rm -rf $(CDS3_TSCSHAREDPOINTER_DIR)
