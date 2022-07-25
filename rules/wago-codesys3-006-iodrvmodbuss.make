## -*-makefile-*-
# $Id: wago-codesys3-005-iodrvmodbuss.make 7242 2013-11-06 10:29:38Z timur.celik@wago.com $
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
PACKAGES-$(PTXCONF_CDS3_IODRVMODBUSS) += cds3-iodrvmodbuss

CDS3_IODRVMODBUSS_VERSION    := 0.3.3
CDS3_IODRVMODBUSS            := IoDrvModbusS
CDS3_IODRVMODBUSS_DIR        := $(BUILDDIR)/$(CDS3_IODRVMODBUSS)
CDS3_IODRVMODBUSS_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVMODBUSS)
CDS3_IODRVMODBUSS_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVMODBUSS)
CDS3_IODRVMODBUSS_SRC        := $(CDS3_IODRVMODBUSS_SRC_DIR)/$(CDS3_IODRVMODBUSS)
CDS3_IODRVMODBUSS_BIN        := lib$(CDS3_IODRVMODBUSS).so.$(CDS3_IODRVMODBUSS_VERSION)
LIBMBS_CONF_DIR              := $(PTXDIST_WORKSPACE)/wago_intern/device/modbus/slave/libmbs/config

CDS3_IODRVMODBUSS_PACKAGE_NAME := cds3-iodrvmodbuss_$(CDS3_IODRVMODBUSS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_IODRVMODBUSS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVMODBUSS_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVMODBUSS_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvmodbuss.get:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	make -C $(CDS3_IODRVMODBUSS_SRC_DIR) $(CDS3_IODRVMODBUSS)Dep.h $(CDS3_IODRVMODBUSS)Itf.h
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvmodbuss.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_IODRVMODBUSS_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_IODRVMODBUSS_SRC_DIR)/ $(CDS3_IODRVMODBUSS_DIR)/

#	$(call patchin, CDS3_IODRVMODBUSS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-iodrvmodbuss.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@echo "Copy diagnostic configuration file into dir: $(CDS3_IODRVMODBUSS_DIR)..."
	$(NOPRINT)cp -d $(LIBMBS_CONF_DIR)/mbs_diag.xml $(CDS3_IODRVMODBUSS_DIR)   
	@echo "Create diagnostic header file for libmbs in sysroot-target/usr/include/diagnostic directory by using of XSLT-script"
	$(NOPRINT)$(call xslt_compile, $(CDS3_IODRVMODBUSS_DIR)/mbs_diag.xml)
	$(NOPRINT)@echo
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_IODRVMODBUSS_PATH      := PATH=$(CROSS_PATH)
CDS3_IODRVMODBUSS_MAKE_ENV  := $(CROSS_ENV)
CDS3_IODRVMODBUSS_MAKE_OPT  := CC=$(CROSS_CC)

#CDS3_IODRVMODBUSS_MAKE_OPT += "DBGMODE=-g3"
#CDS3_IODRVMODBUSS_MAKE_OPT += "OPTIMIZE=-O3"


$(STATEDIR)/cds3-iodrvmodbuss.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_IODRVMODBUSS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvmodbuss.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvmodbuss.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-iodrvmodbuss)

	@$(call install_fixup,cds3-iodrvmodbuss,PRIORITY,optional)
	@$(call install_fixup,cds3-iodrvmodbuss,SECTION,base)
	@$(call install_fixup,cds3-iodrvmodbuss,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-iodrvmodbuss,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_IODRVMODBUSS_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_IODRVMODBUSS_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_IODRVMODBUSS_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVMODBUSS_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-iodrvmodbuss, 0, 0, $(CDS3_IODRVMODBUSS_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-iodrvmodbuss, 0, 0, 0750, $(CDS3_IODRVMODBUSS_DIR)/$(CDS3_IODRVMODBUSS_BIN), /usr/lib/$(CDS3_IODRVMODBUSS_BIN))
	@$(call install_link, cds3-iodrvmodbuss, $(CDS3_IODRVMODBUSS_BIN), /usr/lib/lib$(CDS3_IODRVMODBUSS).so);
	@$(call install_link, cds3-iodrvmodbuss, ../$(CDS3_IODRVMODBUSS_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_IODRVMODBUSS).so);
endif
	@$(call install_finish,cds3-iodrvmodbuss)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_IODRVMODBUSS_PACKAGE_NAME).ipk $(CDS3_IODRVMODBUSS_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvmodbuss.clean:
	$(NOPRINT)$(call xslt_clean, $(CDS3_IODRVMODBUSS_DIR)/mbs_diag.xml)
	rm -rf $(STATEDIR)/cds3-iodrvmodbuss.*
	rm -rf $(PKGDIR)/cds3-iodrvmodbuss_*
	rm -rf $(CDS3_IODRVMODBUSS_DIR)
