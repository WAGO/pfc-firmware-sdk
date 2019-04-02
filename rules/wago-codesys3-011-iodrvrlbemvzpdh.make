## -*-makefile-*-
# $Id: wago-codesys3-005-iodrvrlbemvzpdh.make 7242 2013-11-06 10:29:38Z timur.celik@wago.com $
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
PACKAGES-$(PTXCONF_CDS3_IODRVRLBEMVZPDH) += cds3-iodrvrlbemvzpdh

CDS3_IODRVRLBEMVZPDH_VERSION    := 0.0.1
CDS3_IODRVRLBEMVZPDH            := IoDrvRlbEmvZpdh
CDS3_IODRVRLBEMVZPDH_DIR        := $(BUILDDIR)/$(CDS3_IODRVRLBEMVZPDH)
CDS3_IODRVRLBEMVZPDH_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVRLBEMVZPDH)
CDS3_IODRVRLBEMVZPDH_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_IODRVRLBEMVZPDH)
CDS3_IODRVRLBEMVZPDH_SRC        := $(CDS3_IODRVRLBEMVZPDH_SRC_DIR)/$(CDS3_IODRVRLBEMVZPDH)
CDS3_IODRVRLBEMVZPDH_BIN        := lib$(CDS3_IODRVRLBEMVZPDH).so.$(CDS3_IODRVRLBEMVZPDH_VERSION)

CDS3_IODRVRLBEMVZPDH_PACKAGE_NAME := cds3-iodrvrlbemvzpdh_$(CDS3_IODRVRLBEMVZPDH_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_IODRVRLBEMVZPDH_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVRLBEMVZPDH_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVRLBEMVZPDH_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvrlbemvzpdh.get:
	@$(call targetinfo)
	make -C $(CDS3_IODRVRLBEMVZPDH_SRC_DIR) $(CDS3_IODRVRLBEMVZPDH)Dep.h
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvrlbemvzpdh.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_IODRVRLBEMVZPDH_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_IODRVRLBEMVZPDH_SRC_DIR)/ $(CDS3_IODRVRLBEMVZPDH_DIR)/

#	$(call patchin, CDS3_IODRVRLBEMVZPDH)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-iodrvrlbemvzpdh.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_IODRVRLBEMVZPDH_PATH      := PATH=$(CROSS_PATH)
CDS3_IODRVRLBEMVZPDH_MAKE_ENV  := $(CROSS_ENV)
CDS3_IODRVRLBEMVZPDH_MAKE_OPT  := CC=$(CROSS_CC)

#CDS3_IODRVRLBEMVZPDH_MAKE_OPT += "DBGMODE=-g3"
#CDS3_IODRVRLBEMVZPDH_MAKE_OPT += "OPTIMIZE=-O3"


$(STATEDIR)/cds3-iodrvrlbemvzpdh.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_IODRVRLBEMVZPDH)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvrlbemvzpdh.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvrlbemvzpdh.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-iodrvrlbemvzpdh)

	@$(call install_fixup,cds3-iodrvrlbemvzpdh,PRIORITY,optional)
	@$(call install_fixup,cds3-iodrvrlbemvzpdh,SECTION,base)
	@$(call install_fixup,cds3-iodrvrlbemvzpdh,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-iodrvrlbemvzpdh,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_IODRVRLBEMVZPDH_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_IODRVRLBEMVZPDH_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_IODRVRLBEMVZPDH_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_IODRVRLBEMVZPDH_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-iodrvrlbemvzpdh, 0, 0, $(CDS3_IODRVRLBEMVZPDH_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-iodrvrlbemvzpdh, 0, 0, 0750, $(CDS3_IODRVRLBEMVZPDH_DIR)/$(CDS3_IODRVRLBEMVZPDH_BIN), /usr/lib/$(CDS3_IODRVRLBEMVZPDH_BIN))
	@$(call install_link, cds3-iodrvrlbemvzpdh, $(CDS3_IODRVRLBEMVZPDH_BIN), /usr/lib/lib$(CDS3_IODRVRLBEMVZPDH).so);
	@$(call install_link, cds3-iodrvrlbemvzpdh, ../$(CDS3_IODRVRLBEMVZPDH_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_IODRVRLBEMVZPDH).so);

endif
	@$(call install_finish,cds3-iodrvrlbemvzpdh)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_IODRVRLBEMVZPDH_PACKAGE_NAME).ipk $(CDS3_IODRVRLBEMVZPDH_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-iodrvrlbemvzpdh.clean:
	rm -rf $(STATEDIR)/cds3-iodrvrlbemvzpdh.*
	rm -rf $(PKGDIR)/cds3-iodrvrlbemvzpdh_*
	rm -rf $(CDS3_IODRVRLBEMVZPDH_DIR)
