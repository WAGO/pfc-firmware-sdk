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
PACKAGES-$(PTXCONF_CDS3_TSCLED) += cds3-tscled

CDS3_TSCLED_VERSION	 := 0.0.3
CDS3_TSCLED              := TscLED
CDS3_TSCLED_DIR          := $(BUILDDIR)/$(CDS3_TSCLED)
CDS3_TSCLED_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCLED)
CDS3_TSCLED_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCLED)
CDS3_TSCLED_SRC          := $(CDS3_TSCLED_SRC_DIR)/$(CDS3_TSCLED)
CDS3_TSCLED_BIN          := lib$(CDS3_TSCLED).so.$(CDS3_TSCLED_VERSION)

CDS3_TSCLED_PACKAGE_NAME := cds3-tscled_$(CDS3_TSCLED_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCLED_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscled.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscled.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCLED_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCLED_SRC_DIR)/ $(CDS3_TSCLED_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscled.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCLED_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCLED_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCLED_MAKE_OPT  := CC=$(CROSS_CC)

CDS3_TSCLED_MAKE_ENV += VERSION=$(CDS3_TSCLED_VERSION)

$(STATEDIR)/cds3-tscled.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call xslt_compile, $(CDS3_TSCLED_DIR)/xml/codesys3.xml)
	$(call world/compile, CDS3_TSCLED)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCLED_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscLED
$(STATEDIR)/cds3-tscled.install:
	@$(call targetinfo)

	@mkdir -p $(TSCLED_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCLED_DIR)/*.h $(TSCLED_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCLED_DIR) && tar cvzf $(CDS3_TSCLED_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCLED_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	tar -xzvf $(CDS3_TSCLED_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCLED_PACKAGE_NAME).tgz -C $(TSCLED_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscled.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscled)

	@$(call install_fixup,cds3-tscled,PRIORITY,optional)
	@$(call install_fixup,cds3-tscled,SECTION,base)
	@$(call install_fixup,cds3-tscled,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscled,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCLED_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCLED_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCLED_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCLED_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscled, 0, 0, $(CDS3_TSCLED_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscled, 0, 0, 0750, $(CDS3_TSCLED_DIR)/$(CDS3_TSCLED_BIN), /usr/lib/$(CDS3_TSCLED_BIN))
	@$(call install_link, cds3-tscled, ./$(CDS3_TSCLED_BIN), /usr/lib/lib$(CDS3_TSCLED).so);
	@$(call install_link, cds3-tscled, ../$(CDS3_TSCLED_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCLED).so);

endif
	@$(call install_finish,cds3-tscled)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCLED_PACKAGE_NAME).ipk $(CDS3_TSCLED_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscled.clean:
	@$(call xslt_clean, $(CDS3_TSCLED_DIR)/xml/codesys3.xml)
	rm -rf $(STATEDIR)/cds3-tscled.*
	rm -rf $(PKGDIR)/cds3-tscled_*
	rm -rf $(CDS3_TSCLED_DIR)
