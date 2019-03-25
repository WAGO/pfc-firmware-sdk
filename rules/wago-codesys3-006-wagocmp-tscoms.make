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
PACKAGES-$(PTXCONF_CDS3_TSCOMS) += cds3-tscoms

CDS3_TSCOMS_VERSION	 := 0.0.1
CDS3_TSCOMS              := TscOms
CDS3_TSCOMS_DIR          := $(BUILDDIR)/$(CDS3_TSCOMS)
CDS3_TSCOMS_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCOMS)
CDS3_TSCOMS_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCOMS)
CDS3_TSCOMS_SRC          := $(CDS3_TSCOMS_SRC_DIR)/$(CDS3_TSCOMS)
CDS3_TSCOMS_BIN          := lib$(CDS3_TSCOMS).so.$(CDS3_TSCOMS_VERSION)

CDS3_TSCOMS_PACKAGE_NAME := cds3-tscoms_$(CDS3_TSCOMS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCOMS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscoms.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscoms.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCOMS_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCOMS_SRC_DIR)/ $(CDS3_TSCOMS_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscoms.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCOMS_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCOMS_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCOMS_MAKE_OPT  := CC=$(CROSS_CC)

#CDS3_IODRVDAL_MAKE_OPT += "DBGMODE=-g3"
#CDS3_IODRVDAL_MAKE_OPT += "OPTIMIZE=-O3"


$(STATEDIR)/cds3-tscoms.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCOMS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCOMS_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscOms
$(STATEDIR)/cds3-tscoms.install:
	@$(call targetinfo)
	
	@mkdir -p $(TSCOMS_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCOMS_DIR)/*.h $(TSCOMS_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCOMS_DIR) && tar cvzf $(CDS3_TSCOMS_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCOMS_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCOMS_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCOMS_PACKAGE_NAME).tgz -C $(PTXCONF_SYSROOT_TARGET)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscoms.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscoms)

	@$(call install_fixup,cds3-tscoms,PRIORITY,optional)
	@$(call install_fixup,cds3-tscoms,SECTION,base)
	@$(call install_fixup,cds3-tscoms,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscoms,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCOMS_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCOMS_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCOMS_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCOMS_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscoms, 0, 0, $(CDS3_TSCOMS_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscoms, 0, 0, 0750, $(CDS3_TSCOMS_DIR)/$(CDS3_TSCOMS_BIN), /usr/lib/$(CDS3_TSCOMS_BIN))
	@$(call install_link, cds3-tscoms, ./$(CDS3_TSCOMS_BIN), /usr/lib/lib$(CDS3_TSCOMS).so);
	@$(call install_link, cds3-tscoms, ../$(CDS3_TSCOMS_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCOMS).so);

endif
	@$(call install_finish,cds3-tscoms)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCOMS_PACKAGE_NAME).ipk $(CDS3_TSCOMS_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscoms.clean:
	rm -rf $(STATEDIR)/cds3-tscoms.*
	rm -rf $(PKGDIR)/cds3-tscoms_*
	rm -rf $(CDS3_TSCOMS_DIR)
