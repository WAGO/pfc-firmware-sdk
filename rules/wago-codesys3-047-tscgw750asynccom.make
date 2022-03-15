## -*-makefile-*-
# $Id$
#
# Copyright (C) 2013 by Wago GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_TSCGW750ASYNCCOM) += cds3-tscgw750asynccom

CDS3_TSCGW750ASYNCCOM_VERSION      	:= 0.0.1
CDS3_TSCGW750ASYNCCOM_MD5				:=
CDS3_TSCGW750ASYNCCOM              	:= TscGW750AsyncCom
CDS3_TSCGW750ASYNCCOM_BUILDCONFIG 	:= Debug
CDS3_TSCGW750ASYNCCOM_DIR          	:= $(BUILDDIR)/$(CDS3_TSCGW750ASYNCCOM)
CDS3_TSCGW750ASYNCCOM_BUILDROOT_DIR  	:= $(BUILDDIR)/$(CDS3_TSCGW750ASYNCCOM)
CDS3_TSCGW750ASYNCCOM_URL          	:= file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCGW750ASYNCCOM)
CDS3_TSCGW750ASYNCCOM_SRC_DIR      	:= $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCGW750ASYNCCOM)
CDS3_TSCGW750ASYNCCOM_SRC          	:= $(CDS3_TSCGW750ASYNCCOM_SRC_DIR)/$(CDS3_TSCGW750ASYNCCOM)
CDS3_TSCGW750ASYNCCOM_BIN          	:= lib$(CDS3_TSCGW750ASYNCCOM).so.$(CDS3_TSCGW750ASYNCCOM_VERSION)


CDS3_TSCGW750ASYNCCOM_BUILD_DIR      := $(CDS3_TSCGW750ASYNCCOM_BUILDROOT_DIR)/bin/$(CDS3_TSCGW750ASYNCCOM_BUILDCONFIG)
CDS3_TSCGW750ASYNCCOM_LICENSE        := WAGO
CDS3_TSCGW750ASYNCCOM_CONF_TOOL      := NO
CDS3_TSCGW750ASYNCCOM_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(CDS3_TSCGW750ASYNCCOM_BUILDCONFIG) \
BIN_DIR=$(CDS3_TSCGW750ASYNCCOM_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

CDS3_TSCGW750ASYNCCOM_PACKAGE_NAME := cds3-tscgw750asynccom_$(CDS3_TSCGW750ASYNCCOM_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCGW750ASYNCCOM_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscgw750asynccom.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscgw750asynccom.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCGW750ASYNCCOM_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCGW750ASYNCCOM_SRC_DIR)/ $(CDS3_TSCGW750ASYNCCOM_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscgw750asynccom.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscgw750asynccom.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#	@$(call xslt_compile, $(CDS3_TSCGW750ASYNCCOM_DIR)/xml/codesys3.xml)
	$(call world/compile, CDS3_TSCGW750ASYNCCOM)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscgw750asynccom.install:

	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(CDS3_TSCGW750ASYNCCOM_PKGDIR) && \
	  tar xvzf $(CDS3_TSCGW750ASYNCCOM_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCGW750ASYNCCOM_PACKAGE_NAME).tgz -C $(CDS3_TSCGW750ASYNCCOM_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, CDS3_TSCGW750ASYNCCOM)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(CDS3_TSCGW750ASYNCCOM_PLATFORMCONFIGPACKAGEDIR)
	@cd $(CDS3_TSCGW750ASYNCCOM_PKGDIR) && tar cvzf $(CDS3_TSCGW750ASYNCCOM_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCGW750ASYNCCOM_PACKAGE_NAME).tgz *
endif
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscgw750asynccom.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscgw750asynccom)

	@$(call install_fixup,cds3-tscgw750asynccom,PRIORITY,optional)
	@$(call install_fixup,cds3-tscgw750asynccom,SECTION,base)
	@$(call install_fixup,cds3-tscgw750asynccom,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscgw750asynccom,DESCRIPTION,missing)

	@$(call install_lib, cds3-tscgw750asynccom, 0, 0, 0644, libTscGW750AsyncCom)
	@$(call install_link, cds3-tscgw750asynccom, ../$(CDS3_TSCGW750ASYNCCOM_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCGW750ASYNCCOM).so);


	@$(call install_finish,cds3-tscgw750asynccom)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscgw750asynccom.clean:
#	@$(call xslt_clean, $(CDS3_TSCGW750ASYNCCOM_DIR)/xml/codesys3.xml)
	rm -rf $(STATEDIR)/cds3-tscgw750asynccom.*
	rm -rf $(PKGDIR)/cds3-tscgw750asynccom_*
	rm -rf $(CDS3_TSCGW750ASYNCCOM_DIR)
