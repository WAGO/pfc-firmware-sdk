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
PACKAGES-$(PTXCONF_CDS3_TSCWEBCALENDER) += cds3-tscwebcalender

CDS3_TSCWEBCALENDER_VERSION	 := 0.0.1
CDS3_TSCWEBCALENDER              := TscWebCalender
CDS3_TSCWEBCALENDER_DIR          := $(BUILDDIR)/$(CDS3_TSCWEBCALENDER)
CDS3_TSCWEBCALENDER_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCWEBCALENDER)
CDS3_TSCWEBCALENDER_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCWEBCALENDER)
CDS3_TSCWEBCALENDER_SRC          := $(CDS3_TSCWEBCALENDER_SRC_DIR)/$(CDS3_TSCWEBCALENDER)
CDS3_TSCWEBCALENDER_BIN          := lib$(CDS3_TSCWEBCALENDER).so.$(CDS3_TSCWEBCALENDER_VERSION)

CDS3_TSCWEBCALENDER_PACKAGE_NAME := cds3-TscWebCalender_$(CDS3_TSCWEBCALENDER_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCWEBCALENDER_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwebcalender.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwebcalender.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCWEBCALENDER_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCWEBCALENDER_SRC_DIR)/ $(CDS3_TSCWEBCALENDER_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscwebcalender.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCWEBCALENDER_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCWEBCALENDER_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCWEBCALENDER_MAKE_OPT  := CC=$(CROSS_CC)

CDS3_TSCWEBCALENDER_MAKE_ENV += VERSION=$(CDS3_TSCWEBCALENDER_VERSION)

ifdef PTXCONF_CDS3_TSCWEBCALENDER_TARGET_PFCXXX
	CDS3_TSCWEBCALENDER_MAKE_ENV += TRG_PLATFORM=pfc
else
ifdef PTXCONF_CDS3_TSCWEBCALENDER_TARGET_PAC
	CDS3_TSCWEBCALENDER_MAKE_ENV += TRG_PLATFORM=pac
endif
endif

$(STATEDIR)/cds3-tscwebcalender.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCWEBCALENDER)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCWEBCALENDER_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscWebCalender
$(STATEDIR)/cds3-tscwebcalender.install:
	@$(call targetinfo)

	@mkdir -p $(TSCWEBCALENDER_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCWEBCALENDER_DIR)/*.h $(TSCWEBCALENDER_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCWEBCALENDER_DIR) && tar cvzf $(CDS3_TSCWEBCALENDER_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCWEBCALENDER_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCWEBCALENDER_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCWEBCALENDER_PACKAGE_NAME).tgz -C $(TSCWEBCALENDER_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwebcalender.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscwebcalender)

	@$(call install_fixup,cds3-tscwebcalender,PRIORITY,optional)
	@$(call install_fixup,cds3-tscwebcalender,SECTION,base)
	@$(call install_fixup,cds3-tscwebcalender,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscwebcalender,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCWEBCALENDER_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCWEBCALENDER_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCWEBCALENDER_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCWEBCALENDER_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscwebcalender, 0, 0, $(CDS3_TSCWEBCALENDER_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscwebcalender, 0, 0, 0750, $(CDS3_TSCWEBCALENDER_DIR)/$(CDS3_TSCWEBCALENDER_BIN), /usr/lib/$(CDS3_TSCWEBCALENDER_BIN))
	@$(call install_link, cds3-tscwebcalender, ./$(CDS3_TSCWEBCALENDER_BIN), /usr/lib/lib$(CDS3_TSCWEBCALENDER).so);
	@$(call install_link, cds3-tscwebcalender, ../$(CDS3_TSCWEBCALENDER_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCWEBCALENDER).so);
	

endif
	@$(call install_finish,cds3-tscwebcalender)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCWEBCALENDER_PACKAGE_NAME).ipk $(CDS3_TSCWEBCALENDER_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscwebcalender.clean:
	rm -rf $(STATEDIR)/cds3-tscwebcalender.*
	rm -rf $(PKGDIR)/cds3-tscwebcalender_*
	rm -rf $(CDS3_TSCWEBCALENDER_DIR)
