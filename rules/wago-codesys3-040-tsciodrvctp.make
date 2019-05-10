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
PACKAGES-$(PTXCONF_CDS3_TSCIODRVCTP) += cds3-tsciodrvctp

CDS3_TSCIODRVCTP_VERSION := 1.0.3
CDS3_TSCIODRVCTP         := TscIoDrvCTP
CDS3_TSCIODRVCTP_DIR     := $(BUILDDIR)/$(CDS3_TSCIODRVCTP)
CDS3_TSCIODRVCTP_URL     := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCIODRVCTP)
CDS3_TSCIODRVCTP_SRC_DIR := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCIODRVCTP)
CDS3_TSCIODRVCTP_SRC     := $(CDS3_TSCIODRVCTP_SRC_DIR)/$(CDS3_TSCIODRVCTP)
CDS3_TSCIODRVCTP_BIN     := lib$(CDS3_TSCIODRVCTP).so.$(CDS3_TSCIODRVCTP_VERSION)

CDS3_TSCIODRVCTP_PACKAGE_NAME := cds3-tsciodrvctp_$(CDS3_TSCIODRVCTP_VERSION)_$(PTXCONF_ARCH_STRING)
CDS3_TSCIODRVCTP_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_TSCIODRVCTP_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_TSCIODRVCTP_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciodrvctp.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciodrvctp.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCIODRVCTP_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCIODRVCTP_SRC_DIR)/ $(CDS3_TSCIODRVCTP_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tsciodrvctp.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCIODRVCTP_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCIODRVCTP_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCIODRVCTP_MAKE_OPT  := CC=$(CROSS_CC)

ifdef PTXCONF_CDS3_TSCIODRVCTP_TARGET_PFCXXX
	CDS3_TSCIODRVCTP_MAKE_ENV += TRG_PLATFORM=pfc
else
ifdef PTXCONF_CDS3_TSCIODRVCTP_TARGET_PAC
	CDS3_TSCIODRVCTP_MAKE_ENV += TRG_PLATFORM=pac
endif
endif

$(STATEDIR)/cds3-tsciodrvctp.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#	@$(call xslt_compile, $(CDS3_TSCIODRVCTP_DIR)/xml/codesys3.xml)
	$(call world/compile, CDS3_TSCIODRVCTP)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciodrvctp.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE

endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	tar -xzvf $(CDS3_TSCIODRVCTP_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCIODRVCTP_PACKAGE_NAME).tgz -C $(PTXCONF_SYSROOT_TARGET)
	tar -xzvf $(CDS3_TSCIODRVCTP_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCIODRVCTP_PACKAGE_NAME).tgz -C $(PKGDIR)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciodrvctp.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tsciodrvctp)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCIODRVCTP_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCIODRVCTP_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCIODRVCTP_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCIODRVCTP_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tsciodrvctp, 0, 0, $(CDS3_TSCIODRVCTP_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_fixup,cds3-tsciodrvctp,PRIORITY,optional)
	@$(call install_fixup,cds3-tsciodrvctp,SECTION,base)
	@$(call install_fixup,cds3-tsciodrvctp,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tsciodrvctp,DESCRIPTION,missing)


	@$(call install_copy, cds3-tsciodrvctp, 0, 0, 0750, $(CDS3_TSCIODRVCTP_DIR)/$(CDS3_TSCIODRVCTP_BIN), /usr/lib/$(CDS3_TSCIODRVCTP_BIN))
	@$(call install_link, cds3-tsciodrvctp, ./$(CDS3_TSCIODRVCTP_BIN), /usr/lib/lib$(CDS3_TSCIODRVCTP).so);
	@$(call install_link, cds3-tsciodrvctp, ../$(CDS3_TSCIODRVCTP_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCIODRVCTP).so);


endif
	@$(call install_finish,cds3-tsciodrvctp)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCIODRVCTP_PACKAGE_NAME).ipk $(CDS3_TSCIODRVCTP_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciodrvctp.clean:
#	@$(call xslt_clean, $(CDS3_TSCIODRVCTP_DIR)/xml/codesys3.xml)
	rm -rf $(STATEDIR)/cds3-tsciodrvctp.*
	rm -rf $(PKGDIR)/cds3-tsciodrvctp_*
	rm -rf $(CDS3_TSCIODRVCTP_DIR)
