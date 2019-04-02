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
PACKAGES-$(PTXCONF_CDS3_IODRVDAL) += cds3-iodrvdal

CDS3_IODRVDAL_VERSION    := 0.0.1
CDS3_IODRVDAL            := IoDrvDal
CDS3_IODRVDAL_DIR        := $(BUILDDIR)/$(CDS3_IODRVDAL)
CDS3_IODRVDAL_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVDAL)
CDS3_IODRVDAL_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVDAL)
CDS3_IODRVDAL_SRC        := $(CDS3_IODRVDAL_SRC_DIR)/$(CDS3_IODRVDAL)
CDS3_IODRVDAL_BIN        := lib$(CDS3_IODRVDAL).so.$(CDS3_IODRVDAL_VERSION)

CDS3_IODRVDAL_PACKAGE_NAME := cds3-iodrvdal_$(CDS3_IODRVDAL_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_IODRVDAL_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdal.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdal.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_IODRVDAL_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_IODRVDAL_SRC_DIR)/ $(CDS3_IODRVDAL_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-iodrvdal.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_IODRVDAL_PATH      := PATH=$(CROSS_PATH)
CDS3_IODRVDAL_MAKE_ENV  := $(CROSS_ENV)
CDS3_IODRVDAL_MAKE_OPT  := CC=$(CROSS_CC)

#CDS3_IODRVDAL_MAKE_OPT += "DBGMODE=-g3"
#CDS3_IODRVDAL_MAKE_OPT += "OPTIMIZE=-O3"


$(STATEDIR)/cds3-iodrvdal.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdal.install:
	@$(call targetinfo)
	
	@mkdir -p $(CDS3_IODRVDAL_PKGDIR)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@cd $(CDS3_IODRVDAL_DIR) && \
	for header in *.h; do \
		install -D $$header $(CDS3_IODRVDAL_PKGDIR)/usr/include/IoDrvDal/$$header; \
	done; \
	install -D $(CDS3_IODRVDAL_DIR)/$(CDS3_IODRVDAL_BIN) $(CDS3_IODRVDAL_PKGDIR)/usr/lib/libIoDrvDal.so
   
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_IODRVDAL_PKGDIR) && tar cvzf $(CDS3_IODRVDAL_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVDAL_PACKAGE_NAME).tgz *
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install headers and/or libraries from archive
	@tar xzvf $(CDS3_IODRVDAL_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVDAL_PACKAGE_NAME).tgz -C $(CDS3_IODRVDAL_PKGDIR)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdal.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-iodrvdal)

	@$(call install_fixup,cds3-iodrvdal,PRIORITY,optional)
	@$(call install_fixup,cds3-iodrvdal,SECTION,base)
	@$(call install_fixup,cds3-iodrvdal,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-iodrvdal,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_IODRVDAL_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_IODRVDAL_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_IODRVDAL_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVDAL_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-iodrvdal, 0, 0, $(CDS3_IODRVDAL_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-iodrvdal, 0, 0, 0750, $(CDS3_IODRVDAL_DIR)/$(CDS3_IODRVDAL_BIN), /usr/lib/$(CDS3_IODRVDAL_BIN))
	@$(call install_link, cds3-iodrvdal, ./$(CDS3_IODRVDAL_BIN), /usr/lib/lib$(CDS3_IODRVDAL).so);
	@$(call install_link, cds3-iodrvdal, ../$(CDS3_IODRVDAL_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_IODRVDAL).so);
	@$(call install_copy, cds3-iodrvdal, 0, 0, 0640, $(CDS3_IODRVDAL_DIR)/$(CDS3_IODRVDAL).cfg, $(PTXCONF_CDS3_PLCCONFIGDIR)/$(CDS3_IODRVDAL).cfg, n)

endif
	@$(call install_finish,cds3-iodrvdal)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_IODRVDAL_PACKAGE_NAME).ipk $(CDS3_IODRVDAL_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvdal.clean:
	@-cd $(CDS3_IODRVDAL_DIR) && \
		$(CDS3_IODRVDAL_ENV) $(CDS3_IODRVDAL_PATH) $(MAKE) clean
	@$(call clean_pkg, CDS3_IODRVDAL)

