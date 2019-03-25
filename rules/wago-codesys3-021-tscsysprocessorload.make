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
PACKAGES-$(PTXCONF_CDS3_TSCSYSPROCESSORLOAD) += cds3-tscsysprocessorload

CDS3_TSCSYSPROCESSORLOAD_VERSION	 := 0.0.1
CDS3_TSCSYSPROCESSORLOAD              := TscSysProcessorLoad
CDS3_TSCSYSPROCESSORLOAD_DIR          := $(BUILDDIR)/$(CDS3_TSCSYSPROCESSORLOAD)
CDS3_TSCSYSPROCESSORLOAD_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCSYSPROCESSORLOAD)
CDS3_TSCSYSPROCESSORLOAD_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCSYSPROCESSORLOAD)
CDS3_TSCSYSPROCESSORLOAD_SRC          := $(CDS3_TSCSYSPROCESSORLOAD_SRC_DIR)/$(CDS3_TSCSYSPROCESSORLOAD)
CDS3_TSCSYSPROCESSORLOAD_BIN          := lib$(CDS3_TSCSYSPROCESSORLOAD).so.$(CDS3_TSCSYSPROCESSORLOAD_VERSION)

CDS3_TSCSYSPROCESSORLOAD_PACKAGE_NAME := cds3-tscsysprocessorload_$(CDS3_TSCSYSPROCESSORLOAD_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCSYSPROCESSORLOAD_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsysprocessorload.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsysprocessorload.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCSYSPROCESSORLOAD_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCSYSPROCESSORLOAD_SRC_DIR)/ $(CDS3_TSCSYSPROCESSORLOAD_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscsysprocessorload.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCSYSPROCESSORLOAD_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCSYSPROCESSORLOAD_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCSYSPROCESSORLOAD_MAKE_OPT  := CC=$(CROSS_CC)

$(STATEDIR)/cds3-tscsysprocessorload.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCSYSPROCESSORLOAD)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCSYSPROCESSORLOAD_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscComSwitch
$(STATEDIR)/cds3-tscsysprocessorload.install:
	@$(call targetinfo)

	@mkdir -p $(TSCSYSPROCESSORLOAD_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCSYSPROCESSORLOAD_DIR)/*.h $(TSCSYSPROCESSORLOAD_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCSYSPROCESSORLOAD_DIR) && tar cvzf $(CDS3_TSCSYSPROCESSORLOAD_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCSYSPROCESSORLOAD_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCSYSPROCESSORLOAD_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCSYSPROCESSORLOAD_PACKAGE_NAME).tgz -C $(TSCSYSPROCESSORLOAD_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsysprocessorload.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscsysprocessorload)

	@$(call install_fixup,cds3-tscsysprocessorload,PRIORITY,optional)
	@$(call install_fixup,cds3-tscsysprocessorload,SECTION,base)
	@$(call install_fixup,cds3-tscsysprocessorload,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscsysprocessorload,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCSYSPROCESSORLOAD_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCSYSPROCESSORLOAD_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCSYSPROCESSORLOAD_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCSYSPROCESSORLOAD_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscsysprocessorload, 0, 0, $(CDS3_TSCSYSPROCESSORLOAD_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscsysprocessorload, 0, 0, 0750, $(CDS3_TSCSYSPROCESSORLOAD_DIR)/$(CDS3_TSCSYSPROCESSORLOAD_BIN), /usr/lib/$(CDS3_TSCSYSPROCESSORLOAD_BIN))
	@$(call install_link, cds3-tscsysprocessorload, ./$(CDS3_TSCSYSPROCESSORLOAD_BIN), /usr/lib/lib$(CDS3_TSCSYSPROCESSORLOAD).so);
	@$(call install_link, cds3-tscsysprocessorload, ../$(CDS3_TSCSYSPROCESSORLOAD_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCSYSPROCESSORLOAD).so);
	@$(call install_copy, cds3-tscsysprocessorload, 0, 0, 0644, $(CDS3_TSCSYSPROCESSORLOAD_DIR)/$(CDS3_TSCSYSPROCESSORLOAD).cfg, $(PTXCONF_CDS3_PLCCONFIGDIR)/$(CDS3_TSCSYSPROCESSORLOAD).cfg, n)

endif
	@$(call install_finish,cds3-tscsysprocessorload)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCSYSPROCESSORLOAD_PACKAGE_NAME).ipk $(CDS3_TSCSYSPROCESSORLOAD_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsysprocessorload.clean:
	rm -rf $(STATEDIR)/cds3-tscsysprocessorload.*
	rm -rf $(PKGDIR)/cds3-tscsysprocessorload_*
	rm -rf $(CDS3_TSCSYSPROCESSORLOAD_DIR)
