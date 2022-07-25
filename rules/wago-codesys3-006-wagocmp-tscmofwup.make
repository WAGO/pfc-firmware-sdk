## -*-makefile-*-
# $Rev: 66127 $
#
# Copyright (C) 2021 by WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
LAZY_PACKAGES-$(PTXCONF_CDS3_TSCMOFWUP) += cds3-tscmofwup

CDS3_TSCMOFWUP_VERSION      := 0.0.2
CDS3_TSCMOFWUP              := TscMoFwUp
CDS3_TSCMOFWUP_DIR          := $(BUILDDIR)/$(CDS3_TSCMOFWUP)
CDS3_TSCMOFWUP_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCMOFWUP)
CDS3_TSCMOFWUP_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCMOFWUP)
CDS3_TSCMOFWUP_SRC          := $(CDS3_TSCMOFWUP_SRC_DIR)/$(CDS3_TSCMOFWUP)
CDS3_TSCMOFWUP_BIN          := lib$(CDS3_TSCMOFWUP).so.$(CDS3_TSCMOFWUP_VERSION)

CDS3_TSCMOFWUP_PACKAGE_NAME := cds3-tscmofwup_$(CDS3_TSCMOFWUP_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCMOFWUP_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscmofwup.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscmofwup.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCMOFWUP_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCMOFWUP_SRC_DIR)/ $(CDS3_TSCMOFWUP_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscmofwup.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCMOFWUP_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCMOFWUP_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCMOFWUP_MAKE_OPT  := CC=$(CROSS_CC)

$(STATEDIR)/cds3-tscmofwup.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCMOFWUP)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCMOFWUP_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscMoFwUp
$(STATEDIR)/cds3-tscmofwup.install:
	@$(call targetinfo)
	
	@mkdir -p $(TSCMOFWUP_SYSROOT_INCLUDES)
	
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCMOFWUP_DIR)/*.h $(TSCMOFWUP_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCMOFWUP_DIR) && tar cvzf $(CDS3_TSCMOFWUP_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCMOFWUP_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCMOFWUP_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCMOFWUP_PACKAGE_NAME).tgz -C $(PTXCONF_SYSROOT_TARGET)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscmofwup.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscmofwup)

	@$(call install_fixup,cds3-tscmofwup,PRIORITY,optional)
	@$(call install_fixup,cds3-tscmofwup,SECTION,base)
	@$(call install_fixup,cds3-tscmofwup,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscmofwup,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCMOFWUP_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCMOFWUP_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCMOFWUP_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCMOFWUP_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscmofwup, 0, 0, $(CDS3_TSCMOFWUP_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscmofwup, 0, 0, 0750, $(CDS3_TSCMOFWUP_DIR)/$(CDS3_TSCMOFWUP_BIN), /usr/lib/$(CDS3_TSCMOFWUP_BIN))
	@$(call install_link, cds3-tscmofwup, ./$(CDS3_TSCMOFWUP_BIN), /usr/lib/lib$(CDS3_TSCMOFWUP).so);
	@$(call install_link, cds3-tscmofwup, ../$(CDS3_TSCMOFWUP_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCMOFWUP).so);

endif
	@$(call install_finish,cds3-tscmofwup)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCMOFWUP_PACKAGE_NAME).ipk $(CDS3_TSCMOFWUP_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscmofwup.clean:
	rm -rf $(STATEDIR)/cds3-tscmofwup.*
	rm -rf $(PKGDIR)/cds3-tscmofwup_*
	rm -rf $(CDS3_TSCMOFWUP_DIR)
