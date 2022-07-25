## -*-makefile-*-
# $Id: wago-codesys3-015-tsckbusfs.make 7242 2014-06-24 15:44:38Z timur.celik@wago.com $
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
PACKAGES-$(PTXCONF_CDS3_TSCKBUSFS) += cds3-tsckbusfs

CDS3_TSCKBUSFS_VERSION    := 0.0.2
CDS3_TSCKBUSFS            := TscKbusfs
CDS3_TSCKBUSFS_DIR        := $(BUILDDIR)/$(CDS3_TSCKBUSFS)
CDS3_TSCKBUSFS_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCKBUSFS)
CDS3_TSCKBUSFS_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCKBUSFS)
CDS3_TSCKBUSFS_SRC        := $(CDS3_TSCKBUSFS_SRC_DIR)/$(CDS3_TSCKBUSFS)
CDS3_TSCKBUSFS_BIN        := lib$(CDS3_TSCKBUSFS).so.$(CDS3_TSCKBUSFS_VERSION)

CDS3_TSCKBUSFS_PACKAGE_NAME := cds3-tsckbusfs_$(CDS3_TSCKBUSFS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCKBUSFS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_TSCKBUSFS_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_TSCKBUSFS_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsckbusfs.get:
	@$(call targetinfo)
	make -C $(CDS3_TSCKBUSFS_SRC_DIR) $(CDS3_TSCKBUSFS)Dep.h $(CDS3_TSCKBUSFS)Itf.h
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsckbusfs.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCKBUSFS_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCKBUSFS_SRC_DIR)/ $(CDS3_TSCKBUSFS_DIR)/

#	$(call patchin, CDS3_TSCKBUSFS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tsckbusfs.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCKBUSFS_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCKBUSFS_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCKBUSFS_MAKE_OPT  := CC=$(CROSS_CC)

#CDS3_TSCKBUSFS_MAKE_OPT += "DBGMODE=-g3"
#CDS3_TSCKBUSFS_MAKE_OPT += "OPTIMIZE=-O2"


$(STATEDIR)/cds3-tsckbusfs.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCKBUSFS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsckbusfs.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsckbusfs.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tsckbusfs)

	@$(call install_fixup,cds3-tsckbusfs,PRIORITY,optional)
	@$(call install_fixup,cds3-tsckbusfs,SECTION,base)
	@$(call install_fixup,cds3-tsckbusfs,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tsckbusfs,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCKBUSFS_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCKBUSFS_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCKBUSFS_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCKBUSFS_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tsckbusfs, 0, 0, $(CDS3_TSCKBUSFS_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tsckbusfs, 0, 0, 0750, $(CDS3_TSCKBUSFS_DIR)/$(CDS3_TSCKBUSFS_BIN), /usr/lib/$(CDS3_TSCKBUSFS_BIN))
	@$(call install_link, cds3-tsckbusfs, $(CDS3_TSCKBUSFS_BIN), /usr/lib/lib$(CDS3_TSCKBUSFS).so);
	@$(call install_link, cds3-tsckbusfs, ../$(CDS3_TSCKBUSFS_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCKBUSFS).so);

endif
	@$(call install_finish,cds3-tsckbusfs)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCKBUSFS_PACKAGE_NAME).ipk $(CDS3_TSCKBUSFS_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsckbusfs.clean:
	rm -rf $(STATEDIR)/cds3-tsckbusfs.*
	rm -rf $(PKGDIR)/cds3-tsckbusfs_*
	rm -rf $(CDS3_TSCKBUSFS_DIR)
