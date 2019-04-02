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
PACKAGES-$(PTXCONF_CDS3_IODRVSERIAL) += cds3-iodrvserial

CDS3_IODRVSERIAL_VERSION	 := 0.0.1
CDS3_IODRVSERIAL              := IoDrvSerial
CDS3_IODRVSERIAL_DIR          := $(BUILDDIR)/$(CDS3_IODRVSERIAL)
CDS3_IODRVSERIAL_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVSERIAL)
CDS3_IODRVSERIAL_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVSERIAL)
CDS3_IODRVSERIAL_SRC          := $(CDS3_IODRVSERIAL_SRC_DIR)/$(CDS3_IODRVSERIAL)
CDS3_IODRVSERIAL_BIN          := lib$(CDS3_IODRVSERIAL).so.$(CDS3_IODRVSERIAL_VERSION)

CDS3_IODRVSERIAL_PACKAGE_NAME := cds3-iodrvserial_$(CDS3_IODRVSERIAL_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_IODRVSERIAL_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvserial.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvserial.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_IODRVSERIAL_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_IODRVSERIAL_SRC_DIR)/ $(CDS3_IODRVSERIAL_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-iodrvserial.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_IODRVSERIAL_PATH      := PATH=$(CROSS_PATH)
CDS3_IODRVSERIAL_MAKE_ENV  := $(CROSS_ENV)
CDS3_IODRVSERIAL_MAKE_OPT  := CC=$(CROSS_CC)

$(STATEDIR)/cds3-iodrvserial.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_IODRVSERIAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

IODRVSERIAL_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscComSwitch
$(STATEDIR)/cds3-iodrvserial.install:
	@$(call targetinfo)

	@mkdir -p $(IODRVSERIAL_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_IODRVSERIAL_DIR)/*.h $(IODRVSERIAL_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_IODRVSERIAL_DIR) && tar cvzf $(CDS3_IODRVSERIAL_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVSERIAL_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_IODRVSERIAL_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVSERIAL_PACKAGE_NAME).tgz -C $(IODRVSERIAL_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvserial.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-iodrvserial)

	@$(call install_fixup,cds3-iodrvserial,PRIORITY,optional)
	@$(call install_fixup,cds3-iodrvserial,SECTION,base)
	@$(call install_fixup,cds3-iodrvserial,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-iodrvserial,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_IODRVSERIAL_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_IODRVSERIAL_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_IODRVSERIAL_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVSERIAL_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-iodrvserial, 0, 0, $(CDS3_IODRVSERIAL_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-iodrvserial, 0, 0, 0750, $(CDS3_IODRVSERIAL_DIR)/$(CDS3_IODRVSERIAL_BIN), /usr/lib/$(CDS3_IODRVSERIAL_BIN))
	@$(call install_link, cds3-iodrvserial, ./$(CDS3_IODRVSERIAL_BIN), /usr/lib/lib$(CDS3_IODRVSERIAL).so);
	@$(call install_link, cds3-iodrvserial, ../$(CDS3_IODRVSERIAL_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_IODRVSERIAL).so);

endif
	@$(call install_finish,cds3-iodrvserial)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_IODRVSERIAL_PACKAGE_NAME).ipk $(CDS3_IODRVSERIAL_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvserial.clean:
	rm -rf $(STATEDIR)/cds3-iodrvserial.*
	rm -rf $(PKGDIR)/cds3-iodrvserial_*
	rm -rf $(CDS3_IODRVSERIAL_DIR)
