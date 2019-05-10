## -*-makefile-*-
# $Id: wago-codesys3-015-tscmodbus.make 7242 2014-06-24 15:44:38Z timur.celik@wago.com $
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
PACKAGES-$(PTXCONF_CDS3_TSCMODBUS) += cds3-tscmodbus

CDS3_TSCMODBUS_VERSION    := 0.0.2
CDS3_TSCMODBUS            := TscModbus
CDS3_TSCMODBUS_NAME       := WagoModbus
CDS3_TSCMODBUS_DIR        := $(BUILDDIR)/$(CDS3_TSCMODBUS)
CDS3_TSCMODBUS_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCMODBUS)
CDS3_TSCMODBUS_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCMODBUS)
CDS3_TSCMODBUS_SRC        := $(CDS3_TSCMODBUS_SRC_DIR)/$(CDS3_TSCMODBUS_NAME)
CDS3_TSCMODBUS_BIN        := lib$(CDS3_TSCMODBUS_NAME).so.$(CDS3_TSCMODBUS_VERSION)

CDS3_TSCMODBUS_PACKAGE_NAME := cds3-tscmodbus_$(CDS3_TSCMODBUS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCMODBUS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_TSCMODBUS_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_TSCMODBUS_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscmodbus.get:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	make -C $(CDS3_TSCMODBUS_SRC_DIR) m4headers
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscmodbus.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCMODBUS_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCMODBUS_SRC_DIR)/ $(CDS3_TSCMODBUS_DIR)/

#	$(call patchin, CDS3_TSCMODBUS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscmodbus.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCMODBUS_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCMODBUS_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCMODBUS_MAKE_OPT  := CC=$(CROSS_CC)

#CDS3_TSCMODBUS_MAKE_OPT += "DBGMODE=-g3"
#CDS3_TSCMODBUS_MAKE_OPT += "OPTIMIZE=-O2"


$(STATEDIR)/cds3-tscmodbus.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCMODBUS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscmodbus.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscmodbus.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscmodbus)

	@$(call install_fixup,cds3-tscmodbus,PRIORITY,optional)
	@$(call install_fixup,cds3-tscmodbus,SECTION,base)
	@$(call install_fixup,cds3-tscmodbus,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscmodbus,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCMODBUS_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCMODBUS_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCMODBUS_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCMODBUS_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscmodbus, 0, 0, $(CDS3_TSCMODBUS_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscmodbus, 0, 0, 0750, $(CDS3_TSCMODBUS_DIR)/$(CDS3_TSCMODBUS_BIN), /usr/lib/$(CDS3_TSCMODBUS_BIN))
	@$(call install_link, cds3-tscmodbus, $(CDS3_TSCMODBUS_BIN), /usr/lib/lib$(CDS3_TSCMODBUS_NAME).so);
	@$(call install_link, cds3-tscmodbus, ../$(CDS3_TSCMODBUS_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCMODBUS_NAME).so);

endif
	@$(call install_finish,cds3-tscmodbus)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCMODBUS_PACKAGE_NAME).ipk $(CDS3_TSCMODBUS_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscmodbus.clean:
	rm -rf $(STATEDIR)/cds3-tscmodbus.*
	rm -rf $(PKGDIR)/cds3-tscmodbus_*
	rm -rf $(CDS3_TSCMODBUS_DIR)
