## -*-makefile-*-
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
PACKAGES-$(PTXCONF_CDS3_TSCFUSE) += cds3-tscfuse

CDS3_TSCFUSE_VERSION	  := 1.0.0
CDS3_TSCFUSE              := TscFuse
CDS3_TSCFUSE_DIR          := $(BUILDDIR)/$(CDS3_TSCFUSE)
CDS3_TSCFUSE_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCFUSE)
CDS3_TSCFUSE_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCFUSE)
CDS3_TSCFUSE_SRC          := $(CDS3_TSCFUSE_SRC_DIR)/$(CDS3_TSCFUSE)
CDS3_TSCFUSE_BIN          := lib$(CDS3_TSCFUSE).so.$(CDS3_TSCFUSE_VERSION)

CDS3_TSCFUSE_PACKAGE_NAME := cds3-tscfuse_$(CDS3_TSCFUSE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCFUSE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscfuse.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscfuse.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCFUSE_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCFUSE_SRC_DIR)/ $(CDS3_TSCFUSE_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscfuse.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCFUSE_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCFUSE_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCFUSE_MAKE_OPT  := CC=$(CROSS_CC)

CDS3_TSCFUSE_MAKE_ENV += VERSION=$(CDS3_TSCFUSE_VERSION)

$(STATEDIR)/cds3-tscfuse.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCFUSE)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCFUSE_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscFuse
$(STATEDIR)/cds3-tscfuse.install:
	@$(call targetinfo)

	@mkdir -p $(TSCFUSE_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCFUSE_DIR)/*.h $(TSCFUSE_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCFUSE_DIR) && tar cvzf $(CDS3_TSCFUSE_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCFUSE_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCFUSE_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCFUSE_PACKAGE_NAME).tgz -C $(TSCFUSE_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscfuse.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscfuse)

	@$(call install_fixup,cds3-tscfuse,PRIORITY,optional)
	@$(call install_fixup,cds3-tscfuse,SECTION,base)
	@$(call install_fixup,cds3-tscfuse,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscfuse,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCFUSE_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCFUSE_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCFUSE_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCFUSE_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscfuse, 0, 0, $(CDS3_TSCFUSE_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscfuse, 0, 0, 0750, $(CDS3_TSCFUSE_DIR)/$(CDS3_TSCFUSE_BIN), /usr/lib/$(CDS3_TSCFUSE_BIN))
	@$(call install_link, cds3-tscfuse, ./$(CDS3_TSCFUSE_BIN), /usr/lib/lib$(CDS3_TSCFUSE).so);
	@$(call install_link, cds3-tscfuse, ../$(CDS3_TSCFUSE_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCFUSE).so);

endif
	@$(call install_finish,cds3-tscfuse)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCFUSE_PACKAGE_NAME).ipk $(CDS3_TSCFUSE_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscfuse.clean:
	@$(call xslt_clean, $(CDS3_TSCFUSE_DIR)/xml/codesys3.xml)
	rm -rf $(STATEDIR)/cds3-tscfuse.*
	rm -rf $(PKGDIR)/cds3-tscfuse_*
	rm -rf $(CDS3_TSCFUSE_DIR)
