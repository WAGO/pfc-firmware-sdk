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
PACKAGES-$(PTXCONF_CDS3_SYSUIO) += cds3-sysuio

CDS3_SYSUIO_VERSION    := 0.0.1
CDS3_SYSUIO            := SysUio
CDS3_SYSUIO_DIR        := $(BUILDDIR)/$(CDS3_SYSUIO)
CDS3_SYSUIO_URL        := file://wago_intern/codesys3-Component/$(CDS3_SYSUIO)
CDS3_SYSUIO_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_SYSUIO)
CDS3_SYSUIO_SRC        := $(CDS3_SYSUIO_SRC_DIR)/$(CDS3_SYSUIO)
CDS3_SYSUIO_BIN        := lib$(CDS3_SYSUIO).so.$(CDS3_SYSUIO_VERSION)

CDS3_SYSUIO_PACKAGE_NAME := cds3-sysuio_$(CDS3_SYSUIO_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_SYSUIO_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_SYSUIO_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_SYSUIO_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-sysuio.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-sysuio.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_SYSUIO_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_SYSUIO_SRC_DIR)/ $(CDS3_SYSUIO_DIR)/

#	$(call patchin, CDS3_SYSUIO)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-sysuio.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_SYSUIO_PATH      := PATH=$(CROSS_PATH)
CDS3_SYSUIO_MAKE_ENV  := $(CROSS_ENV)

#CDS3_SYSUIO_MAKE_OPT += "DBGMODE=-g3"
#CDS3_SYSUIO_MAKE_OPT += "OPTIMIZE=-O3"


$(STATEDIR)/cds3-sysuio.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_SYSUIO)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-sysuio.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-sysuio.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-sysuio)

	@$(call install_fixup,cds3-sysuio,PRIORITY,optional)
	@$(call install_fixup,cds3-sysuio,SECTION,base)
	@$(call install_fixup,cds3-sysuio,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-sysuio,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_SYSUIO_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_SYSUIO_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_SYSUIO_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_SYSUIO_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-sysuio, 0, 0, $(CDS3_SYSUIO_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-sysuio, 0, 0, 0750, $(CDS3_SYSUIO_DIR)/$(CDS3_SYSUIO_BIN), /usr/lib/$(CDS3_SYSUIO_BIN))
	@$(call install_link, cds3-sysuio, $(CDS3_SYSUIO_BIN), /usr/lib/lib$(CDS3_SYSUIO).so);

endif
	@$(call install_finish,cds3-sysuio)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_SYSUIO_PACKAGE_NAME).ipk $(CDS3_SYSUIO_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-sysuio.clean:
	rm -rf $(STATEDIR)/cds3-sysuio.*
	rm -rf $(PKGDIR)/cds3-sysuio_*
	rm -rf $(CDS3_SYSUIO_DIR)
