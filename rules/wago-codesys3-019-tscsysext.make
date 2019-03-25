# -*-makefile-*-
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
PACKAGES-$(PTXCONF_CDS3_TSCSYSEXT) += cds3-tscsysext

CDS3_TSCSYSEXT_VERSION	 := 0.2.0
CDS3_TSCSYSEXT              := TscSysExt
CDS3_TSCSYSEXT_DIR          := $(BUILDDIR)/$(CDS3_TSCSYSEXT)
CDS3_TSCSYSEXT_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCSYSEXT)
CDS3_TSCSYSEXT_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCSYSEXT)
CDS3_TSCSYSEXT_SRC          := $(CDS3_TSCSYSEXT_SRC_DIR)/$(CDS3_TSCSYSEXT)
CDS3_TSCSYSEXT_BIN          := lib$(CDS3_TSCSYSEXT).so.$(CDS3_TSCSYSEXT_VERSION)

CDS3_TSCSYSEXT_PACKAGE_NAME := cds3-tscsysext_$(CDS3_TSCSYSEXT_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCSYSEXT_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsysext.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsysext.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCSYSEXT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCSYSEXT_SRC_DIR)/ $(CDS3_TSCSYSEXT_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscsysext.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCSYSEXT_PATH      := PATH=$(CROSS_PATH);
CDS3_TSCSYSEXT_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCSYSEXT_MAKE_OPT  := CC=$(CROSS_CC)
CDS3_TSCSYSEXT_MAKE_OPT  += MYCFLAGS='-I$(BUILDDIR)/codesys-3/Platforms/wosl/Sys/include'

$(STATEDIR)/cds3-tscsysext.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCSYSEXT)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCSYSEXT_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscComSwitch
$(STATEDIR)/cds3-tscsysext.install:
	@$(call targetinfo)
	
	@mkdir -p $(TSCSYSEXT_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCSYSEXT_DIR)/*.h $(TSCSYSEXT_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCSYSEXT_DIR) && tar cvzf $(CDS3_TSCSYSEXT_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCSYSEXT_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCSYSEXT_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCSYSEXT_PACKAGE_NAME).tgz -C $(TSCSYSEXT_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsysext.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscsysext)

	@$(call install_fixup,cds3-tscsysext,PRIORITY,optional)
	@$(call install_fixup,cds3-tscsysext,SECTION,base)
	@$(call install_fixup,cds3-tscsysext,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscsysext,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCSYSEXT_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCSYSEXT_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCSYSEXT_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCSYSEXT_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscsysext, 0, 0, $(CDS3_TSCSYSEXT_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscsysext, 0, 0, 0750, $(CDS3_TSCSYSEXT_DIR)/$(CDS3_TSCSYSEXT_BIN), /usr/lib/$(CDS3_TSCSYSEXT_BIN))
	@$(call install_link, cds3-tscsysext, ./$(CDS3_TSCSYSEXT_BIN), /usr/lib/lib$(CDS3_TSCSYSEXT).so);
	@$(call install_link, cds3-tscsysext, ../$(CDS3_TSCSYSEXT_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCSYSEXT).so);

endif
	@$(call install_finish,cds3-tscsysext)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCSYSEXT_PACKAGE_NAME).ipk $(CDS3_TSCSYSEXT_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsysext.clean:
	rm -rf $(STATEDIR)/cds3-tscsysext.*
	rm -rf $(PKGDIR)/cds3-tscsysext_*
	rm -rf $(CDS3_TSCSYSEXT_DIR)
