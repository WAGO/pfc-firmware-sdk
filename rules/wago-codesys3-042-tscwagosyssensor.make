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
PACKAGES-$(PTXCONF_CDS3_TSCWAGOSYSSENSOR) += cds3-tscwagosyssensor

CDS3_TSCWAGOSYSSENSOR_VERSION	 := 1.0.3
CDS3_TSCWAGOSYSSENSOR              := TscWagoSysSensor
CDS3_TSCWAGOSYSSENSOR_DIR          := $(BUILDDIR)/$(CDS3_TSCWAGOSYSSENSOR)
CDS3_TSCWAGOSYSSENSOR_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCWAGOSYSSENSOR)
CDS3_TSCWAGOSYSSENSOR_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCWAGOSYSSENSOR)
CDS3_TSCWAGOSYSSENSOR_SRC          := $(CDS3_TSCWAGOSYSSENSOR_SRC_DIR)/$(CDS3_TSCWAGOSYSSENSOR)
CDS3_TSCWAGOSYSSENSOR_BIN          := lib$(CDS3_TSCWAGOSYSSENSOR).so.$(CDS3_TSCWAGOSYSSENSOR_VERSION)

CDS3_TSCWAGOSYSSENSOR_PACKAGE_NAME := cds3-tscwagosyssensor_$(CDS3_TSCWAGOSYSSENSOR_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCWAGOSYSSENSOR_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagosyssensor.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagosyssensor.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCWAGOSYSSENSOR_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCWAGOSYSSENSOR_SRC_DIR)/ $(CDS3_TSCWAGOSYSSENSOR_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscwagosyssensor.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCWAGOSYSSENSOR_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCWAGOSYSSENSOR_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCWAGOSYSSENSOR_MAKE_OPT  := CC=$(CROSS_CC)

$(STATEDIR)/cds3-tscwagosyssensor.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCWAGOSYSSENSOR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCWAGOSYSSENSOR_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscWagoSysSensor
$(STATEDIR)/cds3-tscwagosyssensor.install:
	@$(call targetinfo)

	@mkdir -p $(TSCWAGOSYSSENSOR_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCWAGOSYSSENSOR_DIR)/*.h $(TSCWAGOSYSSENSOR_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCWAGOSYSSENSOR_DIR) && tar cvzf $(CDS3_TSCWAGOSYSSENSOR_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCWAGOSYSSENSOR_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCWAGOSYSSENSOR_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCWAGOSYSSENSOR_PACKAGE_NAME).tgz -C $(TSCWAGOSYSSENSOR_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagosyssensor.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscwagosyssensor)

	@$(call install_fixup,cds3-tscwagosyssensor,PRIORITY,optional)
	@$(call install_fixup,cds3-tscwagosyssensor,SECTION,base)
	@$(call install_fixup,cds3-tscwagosyssensor,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscwagosyssensor,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCWAGOSYSSENSOR_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCWAGOSYSSENSOR_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCWAGOSYSSENSOR_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCWAGOSYSSENSOR_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscwagosyssensor, 0, 0, $(CDS3_TSCWAGOSYSSENSOR_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscwagosyssensor, 0, 0, 0750, $(CDS3_TSCWAGOSYSSENSOR_DIR)/$(CDS3_TSCWAGOSYSSENSOR_BIN), /usr/lib/$(CDS3_TSCWAGOSYSSENSOR_BIN))
	@$(call install_link, cds3-tscwagosyssensor, ./$(CDS3_TSCWAGOSYSSENSOR_BIN), /usr/lib/lib$(CDS3_TSCWAGOSYSSENSOR).so);
	@$(call install_link, cds3-tscwagosyssensor, ../$(CDS3_TSCWAGOSYSSENSOR_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCWAGOSYSSENSOR).so);
#	@$(call install_copy, cds3-tscwagosyssensor, 0, 0, 0644, $(CDS3_TSCWAGOSYSSENSOR_DIR)/$(CDS3_TSCWAGOSYSSENSOR).cfg, $(PTXCONF_CDS3_PLCCONFIGDIR)/$(CDS3_TSCWAGOSYSSENSOR).cfg, n)

endif
	@$(call install_finish,cds3-tscwagosyssensor)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCWAGOSYSSENSOR_PACKAGE_NAME).ipk $(CDS3_TSCWAGOSYSSENSOR_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwagosyssensor.clean:
	rm -rf $(STATEDIR)/cds3-tscwagosyssensor.*
	rm -rf $(PKGDIR)/cds3-tscwagosyssensor_*
	rm -rf $(CDS3_TSCWAGOSYSSENSOR_DIR)
