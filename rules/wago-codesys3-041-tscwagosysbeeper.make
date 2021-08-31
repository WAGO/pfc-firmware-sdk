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
PACKAGES-$(PTXCONF_CDS3_TSCWAGOSYSBEEPER) += cds3-tscwagosysbeeper

CDS3_TSCWAGOSYSBEEPER_VERSION	:= 1.0.2
CDS3_TSCWAGOSYSBEEPER           := TscWagoSysBeeper
CDS3_TSCWAGOSYSBEEPER_DIR       := $(BUILDDIR)/$(CDS3_TSCWAGOSYSBEEPER)
CDS3_TSCWAGOSYSBEEPER_URL       := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCWAGOSYSBEEPER)
CDS3_TSCWAGOSYSBEEPER_SRC_DIR   := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCWAGOSYSBEEPER)
CDS3_TSCWAGOSYSBEEPER_SRC       := $(CDS3_TSCWAGOSYSBEEPER_SRC_DIR)/$(CDS3_TSCWAGOSYSBEEPER)
CDS3_TSCWAGOSYSBEEPER_BIN       := lib$(CDS3_TSCWAGOSYSBEEPER).so.$(CDS3_TSCWAGOSYSBEEPER_VERSION)

CDS3_TSCWAGOSYSBEEPER_PACKAGE_NAME := cds3-tscwagosysbeeper_$(CDS3_TSCWAGOSYSBEEPER_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCWAGOSYSBEEPER_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagosysbeeper.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagosysbeeper.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCWAGOSYSBEEPER_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCWAGOSYSBEEPER_SRC_DIR)/ $(CDS3_TSCWAGOSYSBEEPER_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscwagosysbeeper.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCWAGOSYSBEEPER_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCWAGOSYSBEEPER_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCWAGOSYSBEEPER_MAKE_OPT  := CC=$(CROSS_CC)

$(STATEDIR)/cds3-tscwagosysbeeper.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCWAGOSYSBEEPER)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCWAGOSYSBEEPER_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscWagoSysBeeper
$(STATEDIR)/cds3-tscwagosysbeeper.install:
	@$(call targetinfo)

	@mkdir -p $(TSCWAGOSYSBEEPER_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCWAGOSYSBEEPER_DIR)/*.h $(TSCWAGOSYSBEEPER_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCWAGOSYSBEEPER_DIR) && tar cvzf $(CDS3_TSCWAGOSYSBEEPER_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCWAGOSYSBEEPER_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCWAGOSYSBEEPER_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCWAGOSYSBEEPER_PACKAGE_NAME).tgz -C $(TSCWAGOSYSBEEPER_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagosysbeeper.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscwagosysbeeper)

	@$(call install_fixup,cds3-tscwagosysbeeper,PRIORITY,optional)
	@$(call install_fixup,cds3-tscwagosysbeeper,SECTION,base)
	@$(call install_fixup,cds3-tscwagosysbeeper,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscwagosysbeeper,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCWAGOSYSBEEPER_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCWAGOSYSBEEPER_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCWAGOSYSBEEPER_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCWAGOSYSBEEPER_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscwagosysbeeper, 0, 0, $(CDS3_TSCWAGOSYSBEEPER_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscwagosysbeeper, 0, 0, 0750, $(CDS3_TSCWAGOSYSBEEPER_DIR)/$(CDS3_TSCWAGOSYSBEEPER_BIN), /usr/lib/$(CDS3_TSCWAGOSYSBEEPER_BIN))
	@$(call install_link, cds3-tscwagosysbeeper, ./$(CDS3_TSCWAGOSYSBEEPER_BIN), /usr/lib/lib$(CDS3_TSCWAGOSYSBEEPER).so);
	@$(call install_link, cds3-tscwagosysbeeper, ../$(CDS3_TSCWAGOSYSBEEPER_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCWAGOSYSBEEPER).so);
#	@$(call install_copy, cds3-tscwagosysbeeper, 0, 0, 0644, $(CDS3_TSCWAGOSYSBEEPER_DIR)/$(CDS3_TSCWAGOSYSBEEPER).cfg, $(PTXCONF_CDS3_PLCCONFIGDIR)/$(CDS3_TSCWAGOSYSBEEPER).cfg, n)

endif
	@$(call install_finish,cds3-tscwagosysbeeper)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCWAGOSYSBEEPER_PACKAGE_NAME).ipk $(CDS3_TSCWAGOSYSBEEPER_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagosysbeeper.clean:
	rm -rf $(STATEDIR)/cds3-tscwagosysbeeper.*
	rm -rf $(PKGDIR)/cds3-tscwagosysbeeper_*
	rm -rf $(CDS3_TSCWAGOSYSBEEPER_DIR)
