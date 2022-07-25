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
PACKAGES-$(PTXCONF_CDS3_TSCIECPRIOCHECK) += cds3-tsciecpriocheck

CDS3_TSCIECPRIOCHECK_VERSION	 := 0.0.1
CDS3_TSCIECPRIOCHECK              := TscIecPrioCheck
CDS3_TSCIECPRIOCHECK_DIR          := $(BUILDDIR)/$(CDS3_TSCIECPRIOCHECK)
CDS3_TSCIECPRIOCHECK_URL          := file:///wago_intern/codesys3-Component/$(CDS3_TSCIECPRIOCHECK)
CDS3_TSCIECPRIOCHECK_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCIECPRIOCHECK)
CDS3_TSCIECPRIOCHECK_SRC          := $(CDS3_TSCIECPRIOCHECK_SRC_DIR)/$(CDS3_TSCIECPRIOCHECK)
CDS3_TSCIECPRIOCHECK_BIN          := lib$(CDS3_TSCIECPRIOCHECK).so.$(CDS3_TSCIECPRIOCHECK_VERSION)

CDS3_TSCIECPRIOCHECK_PACKAGE_NAME := cds3-tsciecpriocheck_$(CDS3_TSCIECPRIOCHECK_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCIECPRIOCHECK_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciecpriocheck.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciecpriocheck.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCIECPRIOCHECK_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCIECPRIOCHECK_SRC_DIR)/ $(CDS3_TSCIECPRIOCHECK_DIR)/

endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tsciecpriocheck.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCIECPRIOCHECK_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCIECPRIOCHECK_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCIECPRIOCHECK_MAKE_OPT  := CC=$(CROSS_CC)

$(STATEDIR)/cds3-tsciecpriocheck.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCIECPRIOCHECK)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCIECPRIOCHECK_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscIecPrioCheck
$(STATEDIR)/cds3-tsciecpriocheck.install:
	@$(call targetinfo)

	@mkdir -p $(TSCIECPRIOCHECK_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@cp $(CDS3_TSCIECPRIOCHECK_DIR)/*.h $(TSCIECPRIOCHECK_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCIECPRIOCHECK_DIR) && tar cvzf $(CDS3_TSCIECPRIOCHECK_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCIECPRIOCHECK_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCIECPRIOCHECK_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCIECPRIOCHECK_PACKAGE_NAME).tgz -C $(TSCIECPRIOCHECK_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciecpriocheck.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tsciecpriocheck)

	@$(call install_fixup,cds3-tsciecpriocheck,PRIORITY,optional)
	@$(call install_fixup,cds3-tsciecpriocheck,SECTION,base)
	@$(call install_fixup,cds3-tsciecpriocheck,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tsciecpriocheck,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCIECPRIOCHECK_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCIECPRIOCHECK_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCIECPRIOCHECK_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCIECPRIOCHECK_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tsciecpriocheck, 0, 0, $(CDS3_TSCIECPRIOCHECK_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tsciecpriocheck, 0, 0, 0750, $(CDS3_TSCIECPRIOCHECK_DIR)/$(CDS3_TSCIECPRIOCHECK_BIN), /usr/lib/$(CDS3_TSCIECPRIOCHECK_BIN))
	@$(call install_link, cds3-tsciecpriocheck, ./$(CDS3_TSCIECPRIOCHECK_BIN), /usr/lib/lib$(CDS3_TSCIECPRIOCHECK).so);
	@$(call install_link, cds3-tsciecpriocheck, ../$(CDS3_TSCIECPRIOCHECK_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCIECPRIOCHECK).so);

endif
	@$(call install_finish,cds3-tsciecpriocheck)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCIECPRIOCHECK_PACKAGE_NAME).ipk $(CDS3_TSCIECPRIOCHECK_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsciecpriocheck.clean:
	rm -rf $(STATEDIR)/cds3-tsciecpriocheck.*
	rm -rf $(PKGDIR)/cds3-tsciecpriocheck_*
	rm -rf $(CDS3_TSCIECPRIOCHECK_DIR)
