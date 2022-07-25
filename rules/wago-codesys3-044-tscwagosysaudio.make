# -*-makefile-*-
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
PACKAGES-$(PTXCONF_CDS3_TSCWAGOSYSAUDIO) += cds3-tscwagosysaudio

CDS3_TSCWAGOSYSAUDIO_VERSION	 := 1.0.1
CDS3_TSCWAGOSYSAUDIO              := TscWagoSysAudio
CDS3_TSCWAGOSYSAUDIO_DIR          := $(BUILDDIR)/$(CDS3_TSCWAGOSYSAUDIO)
CDS3_TSCWAGOSYSAUDIO_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCWAGOSYSAUDIO)
CDS3_TSCWAGOSYSAUDIO_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCWAGOSYSAUDIO)
CDS3_TSCWAGOSYSAUDIO_SRC          := $(CDS3_TSCWAGOSYSAUDIO_SRC_DIR)/$(CDS3_TSCWAGOSYSAUDIO)
CDS3_TSCWAGOSYSAUDIO_BIN          := lib$(CDS3_TSCWAGOSYSAUDIO).so.$(CDS3_TSCWAGOSYSAUDIO_VERSION)

CDS3_TSCWAGOSYSAUDIO_PACKAGE_NAME := cds3-tscwagosysaudio_$(CDS3_TSCWAGOSYSAUDIO_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCWAGOSYSAUDIO_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagosysaudio.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagosysaudio.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCWAGOSYSAUDIO_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCWAGOSYSAUDIO_SRC_DIR)/ $(CDS3_TSCWAGOSYSAUDIO_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscwagosysaudio.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCWAGOSYSAUDIO_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCWAGOSYSAUDIO_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCWAGOSYSAUDIO_MAKE_OPT  := CC=$(CROSS_CC)

$(STATEDIR)/cds3-tscwagosysaudio.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCWAGOSYSAUDIO)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCWAGOSYSAUDIO_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscWagoSysAudio
$(STATEDIR)/cds3-tscwagosysaudio.install:
	@$(call targetinfo)

	@mkdir -p $(TSCWAGOSYSAUDIO_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCWAGOSYSAUDIO_DIR)/*.h $(TSCWAGOSYSAUDIO_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCWAGOSYSAUDIO_DIR) && tar cvzf $(CDS3_TSCWAGOSYSAUDIO_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCWAGOSYSAUDIO_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCWAGOSYSAUDIO_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCWAGOSYSAUDIO_PACKAGE_NAME).tgz -C $(TSCWAGOSYSAUDIO_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagosysaudio.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscwagosysaudio)

	@$(call install_fixup,cds3-tscwagosysaudio,PRIORITY,optional)
	@$(call install_fixup,cds3-tscwagosysaudio,SECTION,base)
	@$(call install_fixup,cds3-tscwagosysaudio,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscwagosysaudio,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCWAGOSYSAUDIO_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCWAGOSYSAUDIO_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCWAGOSYSAUDIO_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCWAGOSYSAUDIO_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscwagosysaudio, 0, 0, $(CDS3_TSCWAGOSYSAUDIO_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscwagosysaudio, 0, 0, 0750, $(CDS3_TSCWAGOSYSAUDIO_DIR)/$(CDS3_TSCWAGOSYSAUDIO_BIN), /usr/lib/$(CDS3_TSCWAGOSYSAUDIO_BIN))
	@$(call install_link, cds3-tscwagosysaudio, ./$(CDS3_TSCWAGOSYSAUDIO_BIN), /usr/lib/lib$(CDS3_TSCWAGOSYSAUDIO).so);
	@$(call install_link, cds3-tscwagosysaudio, ../$(CDS3_TSCWAGOSYSAUDIO_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCWAGOSYSAUDIO).so);
#	@$(call install_copy, cds3-tscwagosysaudio, 0, 0, 0644, $(CDS3_TSCWAGOSYSAUDIO_DIR)/$(CDS3_TSCWAGOSYSAUDIO).cfg, $(PTXCONF_CDS3_PLCCONFIGDIR)/$(CDS3_TSCWAGOSYSAUDIO).cfg, n)

endif
	@$(call install_finish,cds3-tscwagosysaudio)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCWAGOSYSAUDIO_PACKAGE_NAME).ipk $(CDS3_TSCWAGOSYSAUDIO_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagosysaudio.clean:
	rm -rf $(STATEDIR)/cds3-tscwagosysaudio.*
	rm -rf $(PKGDIR)/cds3-tscwagosysaudio_*
	rm -rf $(CDS3_TSCWAGOSYSAUDIO_DIR)
