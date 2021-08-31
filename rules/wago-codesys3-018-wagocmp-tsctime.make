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
PACKAGES-$(PTXCONF_CDS3_TSCTIME) += cds3-tsctime

CDS3_TSCTIME_VERSION	 := 0.0.4
CDS3_TSCTIME              := TscTime
CDS3_TSCTIME_DIR          := $(BUILDDIR)/$(CDS3_TSCTIME)
CDS3_TSCTIME_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCTIME)
CDS3_TSCTIME_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCTIME)
CDS3_TSCTIME_SRC          := $(CDS3_TSCTIME_SRC_DIR)/$(CDS3_TSCTIME)
CDS3_TSCTIME_BIN          := lib$(CDS3_TSCTIME).so.$(CDS3_TSCTIME_VERSION)

CDS3_TSCTIME_PACKAGE_NAME := cds3-tsctime_$(CDS3_TSCTIME_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCTIME_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsctime.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsctime.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCTIME_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCTIME_SRC_DIR)/ $(CDS3_TSCTIME_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tsctime.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCTIME_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCTIME_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCTIME_MAKE_OPT  := CC=$(CROSS_CC)

CDS3_TSCTIME_MAKE_ENV += VERSION=$(CDS3_TSCTIME_VERSION)

ifdef PTXCONF_CDS3_TSCTIME_TARGET_PFCXXX
	CDS3_TSCTIME_MAKE_ENV += TRG_PLATFORM=pfc
else
ifdef PTXCONF_CDS3_TSCTIME_TARGET_PAC
	CDS3_TSCTIME_MAKE_ENV += TRG_PLATFORM=pac
else
ifdef  PTXCONF_CDS3_TSCTIME_TARGET_VTPCTP
	CDS3_TSCTIME_MAKE_ENV += TRG_PLATFORM=pfc
endif
ifdef PTXCONF_CDS3_TSCTIME_TARGET_CONTROLLER100
	CDS3_TSCTIME_MAKE_ENV += TRG_PLATFORM=pfc
endif
endif
endif

$(STATEDIR)/cds3-tsctime.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCTIME)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsctime.install:
	@$(call targetinfo)

	@mkdir -p $(CDS3_TSCTIME_PKGDIR)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@cd $(CDS3_TSCTIME_DIR) && \
	for header in *.h include/*.h; do \
		install -D $$header $(CDS3_TSCTIME_PKGDIR)/usr/include/TscTime/$$(basename $$header); \
	done; 

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCTIME_PKGDIR) && tar cvzf $(CDS3_TSCTIME_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCTIME_PACKAGE_NAME).tgz *
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCTIME_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCTIME_PACKAGE_NAME).tgz -C $(CDS3_TSCTIME_PKGDIR)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsctime.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tsctime)

	@$(call install_fixup,cds3-tsctime,PRIORITY,optional)
	@$(call install_fixup,cds3-tsctime,SECTION,base)
	@$(call install_fixup,cds3-tsctime,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tsctime,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCTIME_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCTIME_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCTIME_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCTIME_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tsctime, 0, 0, $(CDS3_TSCTIME_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tsctime, 0, 0, 0750, $(CDS3_TSCTIME_DIR)/$(CDS3_TSCTIME_BIN), /usr/lib/$(CDS3_TSCTIME_BIN))
	@$(call install_link, cds3-tsctime, ./$(CDS3_TSCTIME_BIN), /usr/lib/lib$(CDS3_TSCTIME).so);
	@$(call install_link, cds3-tsctime, ../$(CDS3_TSCTIME_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCTIME).so);
	@$(call install_copy, cds3-tsctime, 0, 0, 0750, $(CDS3_TSCTIME_DIR)/lib/libwagotime.so, /usr/lib/libwagotime.so)
	

endif
	@$(call install_finish,cds3-tsctime)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCTIME_PACKAGE_NAME).ipk $(CDS3_TSCTIME_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsctime.clean:
	rm -rf $(STATEDIR)/cds3-tsctime.*
	rm -rf $(PKGDIR)/cds3-tsctime_*
	rm -rf $(CDS3_TSCTIME_DIR)
