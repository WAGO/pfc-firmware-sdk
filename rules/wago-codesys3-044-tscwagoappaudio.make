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
PACKAGES-$(PTXCONF_CDS3_TSCWAGOAPPAUDIO) += cds3-tscwagoappaudio

CDS3_TSCWAGOAPPAUDIO_VERSION	 := 1.0.1
CDS3_TSCWAGOAPPAUDIO              := TscWagoAppAudio
CDS3_TSCWAGOAPPAUDIO_DIR          := $(BUILDDIR)/$(CDS3_TSCWAGOAPPAUDIO)
CDS3_TSCWAGOAPPAUDIO_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCWAGOAPPAUDIO)
CDS3_TSCWAGOAPPAUDIO_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCWAGOAPPAUDIO)
CDS3_TSCWAGOAPPAUDIO_SRC          := $(CDS3_TSCWAGOAPPAUDIO_SRC_DIR)/$(CDS3_TSCWAGOAPPAUDIO)
CDS3_TSCWAGOAPPAUDIO_BIN          := lib$(CDS3_TSCWAGOAPPAUDIO).so.$(CDS3_TSCWAGOAPPAUDIO_VERSION)

CDS3_TSCWAGOAPPAUDIO_PACKAGE_NAME := cds3-tscwagoappaudio_$(CDS3_TSCWAGOAPPAUDIO_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCWAGOAPPAUDIO_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagoappaudio.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagoappaudio.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCWAGOAPPAUDIO_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCWAGOAPPAUDIO_SRC_DIR)/ $(CDS3_TSCWAGOAPPAUDIO_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscwagoappaudio.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCWAGOAPPAUDIO_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCWAGOAPPAUDIO_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCWAGOAPPAUDIO_MAKE_OPT  := CC=$(CROSS_CC)

$(STATEDIR)/cds3-tscwagoappaudio.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCWAGOAPPAUDIO)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCWAGOAPPAUDIO_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscWagoAppAudio
$(STATEDIR)/cds3-tscwagoappaudio.install:
	@$(call targetinfo)

	@mkdir -p $(TSCWAGOAPPAUDIO_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCWAGOAPPAUDIO_DIR)/*.h $(TSCWAGOAPPAUDIO_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCWAGOAPPAUDIO_DIR) && tar cvzf $(CDS3_TSCWAGOAPPAUDIO_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCWAGOAPPAUDIO_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCWAGOAPPAUDIO_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCWAGOAPPAUDIO_PACKAGE_NAME).tgz -C $(TSCWAGOAPPAUDIO_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagoappaudio.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscwagoappaudio)

	@$(call install_fixup,cds3-tscwagoappaudio,PRIORITY,optional)
	@$(call install_fixup,cds3-tscwagoappaudio,SECTION,base)
	@$(call install_fixup,cds3-tscwagoappaudio,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscwagoappaudio,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCWAGOAPPAUDIO_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCWAGOAPPAUDIO_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCWAGOAPPAUDIO_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCWAGOAPPAUDIO_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscwagoappaudio, 0, 0, $(CDS3_TSCWAGOAPPAUDIO_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscwagoappaudio, 0, 0, 0750, $(CDS3_TSCWAGOAPPAUDIO_DIR)/$(CDS3_TSCWAGOAPPAUDIO_BIN), /usr/lib/$(CDS3_TSCWAGOAPPAUDIO_BIN))
	@$(call install_link, cds3-tscwagoappaudio, ./$(CDS3_TSCWAGOAPPAUDIO_BIN), /usr/lib/lib$(CDS3_TSCWAGOAPPAUDIO).so);
	@$(call install_link, cds3-tscwagoappaudio, ../$(CDS3_TSCWAGOAPPAUDIO_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCWAGOAPPAUDIO).so);
#	@$(call install_copy, cds3-tscwagoappaudio, 0, 0, 0644, $(CDS3_TSCWAGOAPPAUDIO_DIR)/$(CDS3_TSCWAGOAPPAUDIO).cfg, $(PTXCONF_CDS3_PLCCONFIGDIR)/$(CDS3_TSCWAGOAPPAUDIO).cfg, n)

endif
	@$(call install_finish,cds3-tscwagoappaudio)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCWAGOAPPAUDIO_PACKAGE_NAME).ipk $(CDS3_TSCWAGOAPPAUDIO_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagoappaudio.clean:
	rm -rf $(STATEDIR)/cds3-tscwagoappaudio.*
	rm -rf $(PKGDIR)/cds3-tscwagoappaudio_*
	rm -rf $(CDS3_TSCWAGOAPPAUDIO_DIR)
