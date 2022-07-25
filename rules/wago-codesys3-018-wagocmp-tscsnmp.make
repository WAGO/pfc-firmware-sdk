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
PACKAGES-$(PTXCONF_CDS3_TSCSNMP) += cds3-tscsnmp

CDS3_TSCSNMP_VERSION	 := 0.0.1
CDS3_TSCSNMP              := TscSnmp
CDS3_TSCSNMP_DIR          := $(BUILDDIR)/$(CDS3_TSCSNMP)
CDS3_TSCSNMP_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCSNMP)
CDS3_TSCSNMP_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCSNMP)
CDS3_TSCSNMP_SRC          := $(CDS3_TSCSNMP_SRC_DIR)/$(CDS3_TSCSNMP)
CDS3_TSCSNMP_BIN          := lib$(CDS3_TSCSNMP).so.$(CDS3_TSCSNMP_VERSION)

CDS3_TSCSNMP_PACKAGE_NAME := cds3-tscsnmp_$(CDS3_TSCSNMP_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCSNMP_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsnmp.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsnmp.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCSNMP_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCSNMP_SRC_DIR)/ $(CDS3_TSCSNMP_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscsnmp.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCSNMP_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCSNMP_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCSNMP_MAKE_OPT  := CC=$(CROSS_CC)

CDS3_TSCSNMP_MAKE_ENV += VERSION=$(CDS3_TSCSNMP_VERSION)

$(STATEDIR)/cds3-tscsnmp.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCSNMP)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCSNMP_SYSROOT_INCLUDES=$(PTXCONF_SYSROOT_TARGET)/usr/include/TscSnmp
$(STATEDIR)/cds3-tscsnmp.install:
	@$(call targetinfo)

	@mkdir -p $(TSCSNMP_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCSNMP_DIR)/*.h $(TSCSNMP_SYSROOT_INCLUDES)/
	
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCSNMP_DIR) && tar cvzf $(CDS3_TSCSNMP_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCSNMP_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCSNMP_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCSNMP_PACKAGE_NAME).tgz -C $(TSCSNMP_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsnmp.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscsnmp)

	@$(call install_fixup,cds3-tscsnmp,PRIORITY,optional)
	@$(call install_fixup,cds3-tscsnmp,SECTION,base)
	@$(call install_fixup,cds3-tscsnmp,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscsnmp,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCSNMP_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCSNMP_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCSNMP_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCSNMP_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscsnmp, 0, 0, $(CDS3_TSCSNMP_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscsnmp, 0, 0, 0750, $(CDS3_TSCSNMP_DIR)/$(CDS3_TSCSNMP_BIN), /usr/lib/$(CDS3_TSCSNMP_BIN))
	@$(call install_link, cds3-tscsnmp, ./$(CDS3_TSCSNMP_BIN), /usr/lib/lib$(CDS3_TSCSNMP).so);
	@$(call install_link, cds3-tscsnmp, ../$(CDS3_TSCSNMP_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCSNMP).so);
	

endif
	@$(call install_finish,cds3-tscsnmp)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCSNMP_PACKAGE_NAME).ipk $(CDS3_TSCSNMP_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsnmp.clean:
	rm -rf $(STATEDIR)/cds3-tscsnmp.*
	rm -rf $(PKGDIR)/cds3-tscsnmp_*
	rm -rf $(CDS3_TSCSNMP_DIR)
