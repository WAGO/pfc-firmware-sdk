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
PACKAGES-$(PTXCONF_CDS3_TSCCOMEXTRA) += cds3-tsccomextra

CDS3_TSCCOMEXTRA_VERSION	 := 0.0.1
CDS3_TSCCOMEXTRA              := TscComExtra
CDS3_TSCCOMEXTRA_DIR          := $(BUILDDIR)/$(CDS3_TSCCOMEXTRA)
CDS3_TSCCOMEXTRA_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCCOMEXTRA)
CDS3_TSCCOMEXTRA_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCCOMEXTRA)
CDS3_TSCCOMEXTRA_SRC          := $(CDS3_TSCCOMEXTRA_SRC_DIR)/$(CDS3_TSCCOMEXTRA)
CDS3_TSCCOMEXTRA_BIN          := lib$(CDS3_TSCCOMEXTRA).so.$(CDS3_TSCCOMEXTRA_VERSION)

CDS3_TSCCOMEXTRA_PACKAGE_NAME := cds3-tsccomextra_$(CDS3_TSCCOMEXTRA_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCCOMEXTRA_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsccomextra.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsccomextra.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCCOMEXTRA_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCCOMEXTRA_SRC_DIR)/ $(CDS3_TSCCOMEXTRA_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tsccomextra.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCCOMEXTRA_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCCOMEXTRA_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCCOMEXTRA_MAKE_OPT  := CC=$(CROSS_CC)

$(STATEDIR)/cds3-tsccomextra.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCCOMEXTRA)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCCOMEXTRA_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscComSwitch
$(STATEDIR)/cds3-tsccomextra.install:
	@$(call targetinfo)

	@mkdir -p $(TSCCOMEXTRA_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCCOMEXTRA_DIR)/*.h $(TSCCOMEXTRA_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCCOMEXTRA_DIR) && tar cvzf $(CDS3_TSCCOMEXTRA_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCCOMEXTRA_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCCOMEXTRA_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCCOMEXTRA_PACKAGE_NAME).tgz -C $(TSCCOMEXTRA_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsccomextra.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tsccomextra)

	@$(call install_fixup,cds3-tsccomextra,PRIORITY,optional)
	@$(call install_fixup,cds3-tsccomextra,SECTION,base)
	@$(call install_fixup,cds3-tsccomextra,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tsccomextra,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCCOMEXTRA_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCCOMEXTRA_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCCOMEXTRA_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCCOMEXTRA_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tsccomextra, 0, 0, $(CDS3_TSCCOMEXTRA_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tsccomextra, 0, 0, 0750, $(CDS3_TSCCOMEXTRA_DIR)/$(CDS3_TSCCOMEXTRA_BIN), /usr/lib/$(CDS3_TSCCOMEXTRA_BIN))
	@$(call install_link, cds3-tsccomextra, ./$(CDS3_TSCCOMEXTRA_BIN), /usr/lib/lib$(CDS3_TSCCOMEXTRA).so);
	@$(call install_link, cds3-tsccomextra, ../$(CDS3_TSCCOMEXTRA_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCCOMEXTRA).so);

endif
	@$(call install_finish,cds3-tsccomextra)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCCOMEXTRA_PACKAGE_NAME).ipk $(CDS3_TSCCOMEXTRA_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsccomextra.clean:
	rm -rf $(STATEDIR)/cds3-tsccomextra.*
	rm -rf $(PKGDIR)/cds3-tsccomextra_*
	rm -rf $(CDS3_TSCCOMEXTRA_DIR)
