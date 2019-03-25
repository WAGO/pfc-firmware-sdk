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
PACKAGES-$(PTXCONF_CDS3_TSCASYNCMGR) += cds3-tscasyncmgr

CDS3_TSCASYNCMGR_VERSION	 := 0.0.1
CDS3_TSCASYNCMGR              := TscAsyncMgr
CDS3_TSCASYNCMGR_DIR          := $(BUILDDIR)/$(CDS3_TSCASYNCMGR)
CDS3_TSCASYNCMGR_URL          := file:///wago_intern/codesys3-Component/$(CDS3_TSCASYNCMGR)
CDS3_TSCASYNCMGR_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCASYNCMGR)
CDS3_TSCASYNCMGR_SRC          := $(CDS3_TSCASYNCMGR_SRC_DIR)/$(CDS3_TSCASYNCMGR)
CDS3_TSCASYNCMGR_BIN          := lib$(CDS3_TSCASYNCMGR).so.$(CDS3_TSCASYNCMGR_VERSION)

CDS3_TSCASYNCMGR_PACKAGE_NAME := cds3-tscasyncmgr_$(CDS3_TSCASYNCMGR_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCASYNCMGR_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscasyncmgr.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscasyncmgr.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCASYNCMGR_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCASYNCMGR_SRC_DIR)/ $(CDS3_TSCASYNCMGR_DIR)/

endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscasyncmgr.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCASYNCMGR_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCASYNCMGR_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCASYNCMGR_MAKE_OPT  := CC=$(CROSS_CC)

$(STATEDIR)/cds3-tscasyncmgr.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCASYNCMGR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCASYNCMGR_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscAsyncMgr
$(STATEDIR)/cds3-tscasyncmgr.install:
	@$(call targetinfo)

	@mkdir -p $(TSCASYNCMGR_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@cp $(CDS3_TSCASYNCMGR_DIR)/*.h $(TSCASYNCMGR_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCASYNCMGR_DIR) && tar cvzf $(CDS3_TSCASYNCMGR_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCASYNCMGR_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCASYNCMGR_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCASYNCMGR_PACKAGE_NAME).tgz -C $(TSCASYNCMGR_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscasyncmgr.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscasyncmgr)

	@$(call install_fixup,cds3-tscasyncmgr,PRIORITY,optional)
	@$(call install_fixup,cds3-tscasyncmgr,SECTION,base)
	@$(call install_fixup,cds3-tscasyncmgr,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscasyncmgr,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCASYNCMGR_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCASYNCMGR_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCASYNCMGR_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCASYNCMGR_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscasyncmgr, 0, 0, $(CDS3_TSCASYNCMGR_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscasyncmgr, 0, 0, 0750, $(CDS3_TSCASYNCMGR_DIR)/$(CDS3_TSCASYNCMGR_BIN), /usr/lib/$(CDS3_TSCASYNCMGR_BIN))
	@$(call install_link, cds3-tscasyncmgr, ./$(CDS3_TSCASYNCMGR_BIN), /usr/lib/lib$(CDS3_TSCASYNCMGR).so);
	@$(call install_link, cds3-tscasyncmgr, ../$(CDS3_TSCASYNCMGR_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCASYNCMGR).so);

endif
	@$(call install_finish,cds3-tscasyncmgr)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCASYNCMGR_PACKAGE_NAME).ipk $(CDS3_TSCASYNCMGR_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscasyncmgr.clean:
	rm -rf $(STATEDIR)/cds3-tscasyncmgr.*
	rm -rf $(PKGDIR)/cds3-tscasyncmgr_*
	rm -rf $(CDS3_TSCASYNCMGR_DIR)
