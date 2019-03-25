## -*-makefile-*-
# $Id$
#
# Copyright (C) 2014 by Wago Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_CMPKBUSASYNCCOM) += cds3-cmpkbusasynccom

CDS3_CMPKBUSASYNCCOM_VERSION	 := 0.0.1
CDS3_CMPKBUSASYNCCOM              := CmpKbusAsyncCom
CDS3_CMPKBUSASYNCCOM_DIR          := $(BUILDDIR)/$(CDS3_CMPKBUSASYNCCOM)
CDS3_CMPKBUSASYNCCOM_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_CMPKBUSASYNCCOM)
CDS3_CMPKBUSASYNCCOM_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_CMPKBUSASYNCCOM)
CDS3_CMPKBUSASYNCCOM_SRC          := $(CDS3_CMPKBUSASYNCCOM_SRC_DIR)/$(CDS3_CMPKBUSASYNCCOM)
CDS3_CMPKBUSASYNCCOM_BIN          := lib$(CDS3_CMPKBUSASYNCCOM).so.$(CDS3_CMPKBUSASYNCCOM_VERSION)

CDS3_CMPKBUSASYNCCOM_PACKAGE_NAME := cds3-cmpkbusasynccom_$(CDS3_CMPKBUSASYNCCOM_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_CMPKBUSASYNCCOM_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-cmpkbusasynccom.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-cmpkbusasynccom.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_CMPKBUSASYNCCOM_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_CMPKBUSASYNCCOM_SRC_DIR)/ $(CDS3_CMPKBUSASYNCCOM_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-cmpkbusasynccom.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_CMPKBUSASYNCCOM_PATH      := PATH=$(CROSS_PATH)
CDS3_CMPKBUSASYNCCOM_MAKE_ENV  := $(CROSS_ENV)
CDS3_CMPKBUSASYNCCOM_MAKE_OPT  := CC=$(CROSS_CC)

#CDS3_IODRVDAL_MAKE_OPT += "DBGMODE=-g3"
#CDS3_IODRVDAL_MAKE_OPT += "OPTIMIZE=-O3"


$(STATEDIR)/cds3-cmpkbusasynccom.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_CMPKBUSASYNCCOM)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

CMPKBUSASYNCCOM_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/CmpKbusAsyncCom
$(STATEDIR)/cds3-cmpkbusasynccom.install:
	@$(call targetinfo)

	@mkdir -p $(CMPKBUSASYNCCOM_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_CMPKBUSASYNCCOM_DIR)/*.h $(CMPKBUSASYNCCOM_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_CMPKBUSASYNCCOM_DIR) && tar cvzf $(CDS3_CMPKBUSASYNCCOM_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_CMPKBUSASYNCCOM_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_CMPKBUSASYNCCOM_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_CMPKBUSASYNCCOM_PACKAGE_NAME).tgz -C $(CMPKBUSASYNCCOM_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-cmpkbusasynccom.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-cmpkbusasynccom)

	@$(call install_fixup,cds3-cmpkbusasynccom,PRIORITY,optional)
	@$(call install_fixup,cds3-cmpkbusasynccom,SECTION,base)
	@$(call install_fixup,cds3-cmpkbusasynccom,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-cmpkbusasynccom,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_CMPKBUSASYNCCOM_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_CMPKBUSASYNCCOM_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_CMPKBUSASYNCCOM_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_CMPKBUSASYNCCOM_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-cmpkbusasynccom, 0, 0, $(CDS3_CMPKBUSASYNCCOM_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-cmpkbusasynccom, 0, 0, 0750, $(CDS3_CMPKBUSASYNCCOM_DIR)/$(CDS3_CMPKBUSASYNCCOM_BIN), /usr/lib/$(CDS3_CMPKBUSASYNCCOM_BIN))
	@$(call install_link, cds3-cmpkbusasynccom, ./$(CDS3_CMPKBUSASYNCCOM_BIN), /usr/lib/lib$(CDS3_CMPKBUSASYNCCOM).so);
	@$(call install_link, cds3-cmpkbusasynccom, ../$(CDS3_CMPKBUSASYNCCOM_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_CMPKBUSASYNCCOM).so);

endif
	@$(call install_finish,cds3-cmpkbusasynccom)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_CMPKBUSASYNCCOM_PACKAGE_NAME).ipk $(CDS3_CMPKBUSASYNCCOM_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-cmpkbusasynccom.clean:
	rm -rf $(STATEDIR)/cds3-cmpkbusasynccom.*
	rm -rf $(PKGDIR)/cds3-cmpkbusasynccom_*
	rm -rf $(CDS3_CMPKBUSASYNCCOM_DIR)
