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
PACKAGES-$(PTXCONF_CDS3_TSCEXTTASK) += cds3-tscexttask

CDS3_TSCEXTTASK_VERSION	 := 0.0.1
CDS3_TSCEXTTASK              := TscExtTask
CDS3_TSCEXTTASK_DIR          := $(BUILDDIR)/$(CDS3_TSCEXTTASK)
CDS3_TSCEXTTASK_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCEXTTASK)
CDS3_TSCEXTTASK_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCEXTTASK)
CDS3_TSCEXTTASK_SRC          := $(CDS3_TSCEXTTASK_SRC_DIR)/$(CDS3_TSCEXTTASK)
CDS3_TSCEXTTASK_BIN          := lib$(CDS3_TSCEXTTASK).so.$(CDS3_TSCEXTTASK_VERSION)

CDS3_TSCEXTTASK_PACKAGE_NAME := cds3-tscexttask_$(CDS3_TSCEXTTASK_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCEXTTASK_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscexttask.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscexttask.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCEXTTASK_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCEXTTASK_SRC_DIR)/ $(CDS3_TSCEXTTASK_DIR)/

endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscexttask.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCEXTTASK_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCEXTTASK_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCEXTTASK_MAKE_OPT  := CC=$(CROSS_CC)

$(STATEDIR)/cds3-tscexttask.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCEXTTASK)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCEXTTASK_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscExtTask
$(STATEDIR)/cds3-tscexttask.install:
	@$(call targetinfo)

	@mkdir -p $(TSCEXTTASK_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@cp $(CDS3_TSCEXTTASK_DIR)/*.h $(TSCEXTTASK_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCEXTTASK_DIR) && tar cvzf $(CDS3_TSCEXTTASK_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCEXTTASK_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCEXTTASK_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCEXTTASK_PACKAGE_NAME).tgz -C $(TSCEXTTASK_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscexttask.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscexttask)

	@$(call install_fixup,cds3-tscexttask,PRIORITY,optional)
	@$(call install_fixup,cds3-tscexttask,SECTION,base)
	@$(call install_fixup,cds3-tscexttask,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscexttask,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCEXTTASK_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCEXTTASK_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCEXTTASK_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCEXTTASK_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscexttask, 0, 0, $(CDS3_TSCEXTTASK_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscexttask, 0, 0, 0750, $(CDS3_TSCEXTTASK_DIR)/$(CDS3_TSCEXTTASK_BIN), /usr/lib/$(CDS3_TSCEXTTASK_BIN))
	@$(call install_link, cds3-tscexttask, ./$(CDS3_TSCEXTTASK_BIN), /usr/lib/lib$(CDS3_TSCEXTTASK).so);
	@$(call install_link, cds3-tscexttask, ../$(CDS3_TSCEXTTASK_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCEXTTASK).so);

endif
	@$(call install_finish,cds3-tscexttask)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCEXTTASK_PACKAGE_NAME).ipk $(CDS3_TSCEXTTASK_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscexttask.clean:
	rm -rf $(STATEDIR)/cds3-tscexttask.*
	rm -rf $(PKGDIR)/cds3-tscexttask_*
	rm -rf $(CDS3_TSCEXTTASK_DIR)
