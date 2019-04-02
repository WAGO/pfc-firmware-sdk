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
PACKAGES-$(PTXCONF_CDS3_TSCRETAINUIO) += cds3-tscretainuio

CDS3_TSCRETAINUIO_VERSION	 := 0.0.1
CDS3_TSCRETAINUIO              := TscRetainUIO
CDS3_TSCRETAINUIO_DIR          := $(BUILDDIR)/$(CDS3_TSCRETAINUIO)
CDS3_TSCRETAINUIO_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCRETAINUIO)
CDS3_TSCRETAINUIO_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCRETAINUIO)
CDS3_TSCRETAINUIO_SRC          := $(CDS3_TSCRETAINUIO_SRC_DIR)/$(CDS3_TSCRETAINUIO)
CDS3_TSCRETAINUIO_BIN          := lib$(CDS3_TSCRETAINUIO).so.$(CDS3_TSCRETAINUIO_VERSION)
CDS3_TSCRETAINUIO_PKGDIR       := $(PKGDIR)/$(CDS3_TSCRETAINUIO)

CDS3_TSCRETAINUIO_PACKAGE_NAME := cds3-tscretainuio_$(CDS3_TSCRETAINUIO_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCRETAINUIO_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscretainuio.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscretainuio.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCRETAINUIO_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCRETAINUIO_SRC_DIR)/ $(CDS3_TSCRETAINUIO_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscretainuio.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCRETAINUIO_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCRETAINUIO_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCRETAINUIO_MAKE_OPT  := CC=$(CROSS_CC)

ifdef PTXCONF_CDS3_TSCRETAINUIO_TARGET_PFCXXX
	CDS3_TSCRETAINUIO_MAKE_ENV += TRG_PLATFORM=pfc
else
ifdef PTXCONF_CDS3_TSCRETAINUIO_TARGET_PAC
	CDS3_TSCRETAINUIO_MAKE_ENV += TRG_PLATFORM=pac
else
ifdef PTXCONF_CDS3_TSCRETAINUIO_TARGET_VTPCTP
	CDS3_TSCRETAINUIO_MAKE_ENV += TRG_PLATFORM=pfc
endif
endif
endif

$(STATEDIR)/cds3-tscretainuio.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCRETAINUIO)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscretainuio.install:
	@$(call targetinfo)

	@mkdir -p $(CDS3_TSCRETAINUIO_PKGDIR)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@cd $(CDS3_TSCRETAINUIO_DIR) && \
	for header in *.h include/*.h; do \
		install -D $$header $(CDS3_TSCRETAINUIO_PKGDIR)/usr/include/TscRetainUIO/$$(basename $$header); \
	done; 

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCRETAINUIO_PKGDIR) && tar cvzf $(CDS3_TSCRETAINUIO_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCRETAINUIO_PACKAGE_NAME).tgz *
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar xzvf $(CDS3_TSCRETAINUIO_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCRETAINUIO_PACKAGE_NAME).tgz -C $(CDS3_TSCRETAINUIO_PKGDIR)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscretainuio.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscretainuio)

	@$(call install_fixup,cds3-tscretainuio,PRIORITY,optional)
	@$(call install_fixup,cds3-tscretainuio,SECTION,base)
	@$(call install_fixup,cds3-tscretainuio,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscretainuio,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCRETAINUIO_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCRETAINUIO_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCRETAINUIO_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCRETAINUIO_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscretainuio, 0, 0, $(CDS3_TSCRETAINUIO_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscretainuio, 0, 0, 0750, $(CDS3_TSCRETAINUIO_DIR)/$(CDS3_TSCRETAINUIO_BIN), /usr/lib/$(CDS3_TSCRETAINUIO_BIN))
	@$(call install_link, cds3-tscretainuio, ./$(CDS3_TSCRETAINUIO_BIN), /usr/lib/lib$(CDS3_TSCRETAINUIO).so);
	@$(call install_link, cds3-tscretainuio, ../$(CDS3_TSCRETAINUIO_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCRETAINUIO).so);
	@$(call install_copy, cds3-tscretainuio, 0, 0, 0750, $(CDS3_TSCRETAINUIO_DIR)/lib/libwagoretain_uio.so, /usr/lib/libwagoretain_uio.so)
	

endif
	@$(call install_finish,cds3-tscretainuio)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCRETAINUIO_PACKAGE_NAME).ipk $(CDS3_TSCRETAINUIO_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscretainuio.clean:
	@$(call targetinfo)
	@-cd $(CDS3_TSCRETAINUIO_DIR) && \
		$(CDS3_TSCRETAINUIO_ENV) $(CDS3_TSCRETAINUIO_PATH) $(MAKE) clean
	@$(call clean_pkg, CDS3_TSCRETAINUIO)

