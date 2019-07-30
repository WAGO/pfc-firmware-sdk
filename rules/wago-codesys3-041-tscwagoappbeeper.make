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
PACKAGES-$(PTXCONF_CDS3_TSCWAGOAPPBEEPER) += cds3-tscwagoappbeeper

CDS3_TSCWAGOAPPBEEPER_VERSION	 := 1.0.2
CDS3_TSCWAGOAPPBEEPER              := TscWagoAppBeeper
CDS3_TSCWAGOAPPBEEPER_DIR          := $(BUILDDIR)/$(CDS3_TSCWAGOAPPBEEPER)
CDS3_TSCWAGOAPPBEEPER_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCWAGOAPPBEEPER)
CDS3_TSCWAGOAPPBEEPER_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCWAGOAPPBEEPER)
CDS3_TSCWAGOAPPBEEPER_SRC          := $(CDS3_TSCWAGOAPPBEEPER_SRC_DIR)/$(CDS3_TSCWAGOAPPBEEPER)
CDS3_TSCWAGOAPPBEEPER_BIN          := lib$(CDS3_TSCWAGOAPPBEEPER).so.$(CDS3_TSCWAGOAPPBEEPER_VERSION)

CDS3_TSCWAGOAPPBEEPER_PACKAGE_NAME := cds3-tscwagoappbeeper_$(CDS3_TSCWAGOAPPBEEPER_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCWAGOAPPBEEPER_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagoappbeeper.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagoappbeeper.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCWAGOAPPBEEPER_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCWAGOAPPBEEPER_SRC_DIR)/ $(CDS3_TSCWAGOAPPBEEPER_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscwagoappbeeper.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCWAGOAPPBEEPER_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCWAGOAPPBEEPER_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCWAGOAPPBEEPER_MAKE_OPT  := CC=$(CROSS_CC)

$(STATEDIR)/cds3-tscwagoappbeeper.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCWAGOAPPBEEPER)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCWAGOAPPBEEPER_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscWagoAppBeeper
$(STATEDIR)/cds3-tscwagoappbeeper.install:
	@$(call targetinfo)

	@mkdir -p $(TSCWAGOAPPBEEPER_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCWAGOAPPBEEPER_DIR)/*.h $(TSCWAGOAPPBEEPER_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCWAGOAPPBEEPER_DIR) && tar cvzf $(CDS3_TSCWAGOAPPBEEPER_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCWAGOAPPBEEPER_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCWAGOAPPBEEPER_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCWAGOAPPBEEPER_PACKAGE_NAME).tgz -C $(TSCWAGOAPPBEEPER_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagoappbeeper.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscwagoappbeeper)

	@$(call install_fixup,cds3-tscwagoappbeeper,PRIORITY,optional)
	@$(call install_fixup,cds3-tscwagoappbeeper,SECTION,base)
	@$(call install_fixup,cds3-tscwagoappbeeper,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscwagoappbeeper,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCWAGOAPPBEEPER_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCWAGOAPPBEEPER_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCWAGOAPPBEEPER_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCWAGOAPPBEEPER_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscwagoappbeeper, 0, 0, $(CDS3_TSCWAGOAPPBEEPER_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscwagoappbeeper, 0, 0, 0750, $(CDS3_TSCWAGOAPPBEEPER_DIR)/$(CDS3_TSCWAGOAPPBEEPER_BIN), /usr/lib/$(CDS3_TSCWAGOAPPBEEPER_BIN))
	@$(call install_link, cds3-tscwagoappbeeper, ./$(CDS3_TSCWAGOAPPBEEPER_BIN), /usr/lib/lib$(CDS3_TSCWAGOAPPBEEPER).so);
	@$(call install_link, cds3-tscwagoappbeeper, ../$(CDS3_TSCWAGOAPPBEEPER_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCWAGOAPPBEEPER).so);
#	@$(call install_copy, cds3-tscwagoappbeeper, 0, 0, 0644, $(CDS3_TSCWAGOAPPBEEPER_DIR)/$(CDS3_TSCWAGOAPPBEEPER).cfg, $(PTXCONF_CDS3_PLCCONFIGDIR)/$(CDS3_TSCWAGOAPPBEEPER).cfg, n)

endif
	@$(call install_finish,cds3-tscwagoappbeeper)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCWAGOAPPBEEPER_PACKAGE_NAME).ipk $(CDS3_TSCWAGOAPPBEEPER_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagoappbeeper.clean:
	rm -rf $(STATEDIR)/cds3-tscwagoappbeeper.*
	rm -rf $(PKGDIR)/cds3-tscwagoappbeeper_*
	rm -rf $(CDS3_TSCWAGOAPPBEEPER_DIR)
