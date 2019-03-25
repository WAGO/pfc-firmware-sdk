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
PACKAGES-$(PTXCONF_CDS3_TSCCOMSWITCH) += cds3-tsccomswitch

CDS3_TSCCOMSWITCH_VERSION	 := 0.0.4
CDS3_TSCCOMSWITCH              := TscComSwitch
CDS3_TSCCOMSWITCH_DIR          := $(BUILDDIR)/$(CDS3_TSCCOMSWITCH)
CDS3_TSCCOMSWITCH_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCCOMSWITCH)
CDS3_TSCCOMSWITCH_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCCOMSWITCH)
CDS3_TSCCOMSWITCH_SRC          := $(CDS3_TSCCOMSWITCH_SRC_DIR)/$(CDS3_TSCCOMSWITCH)
CDS3_TSCCOMSWITCH_BIN          := lib$(CDS3_TSCCOMSWITCH).so.$(CDS3_TSCCOMSWITCH_VERSION)

CDS3_TSCCOMSWITCH_PACKAGE_NAME := cds3-tsccomswitch_$(CDS3_TSCCOMSWITCH_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCCOMSWITCH_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsccomswitch.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsccomswitch.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCCOMSWITCH_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCCOMSWITCH_SRC_DIR)/ $(CDS3_TSCCOMSWITCH_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tsccomswitch.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCCOMSWITCH_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCCOMSWITCH_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCCOMSWITCH_MAKE_OPT  := CC=$(CROSS_CC)

$(STATEDIR)/cds3-tsccomswitch.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCCOMSWITCH)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCCOMSWITCH_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscComSwitch
$(STATEDIR)/cds3-tsccomswitch.install:
	@$(call targetinfo)

	@mkdir -p $(TSCCOMSWITCH_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCCOMSWITCH_DIR)/*.h $(TSCCOMSWITCH_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCCOMSWITCH_DIR) && tar cvzf $(CDS3_TSCCOMSWITCH_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCCOMSWITCH_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	tar -xzvf $(CDS3_TSCCOMSWITCH_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCCOMSWITCH_PACKAGE_NAME).tgz -C $(TSCCOMSWITCH_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsccomswitch.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tsccomswitch)

	@$(call install_fixup,cds3-tsccomswitch,PRIORITY,optional)
	@$(call install_fixup,cds3-tsccomswitch,SECTION,base)
	@$(call install_fixup,cds3-tsccomswitch,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tsccomswitch,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCCOMSWITCH_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCCOMSWITCH_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCCOMSWITCH_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCCOMSWITCH_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tsccomswitch, 0, 0, $(CDS3_TSCCOMSWITCH_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tsccomswitch, 0, 0, 0750, $(CDS3_TSCCOMSWITCH_DIR)/$(CDS3_TSCCOMSWITCH_BIN), /usr/lib/$(CDS3_TSCCOMSWITCH_BIN))
	@$(call install_link, cds3-tsccomswitch, ./$(CDS3_TSCCOMSWITCH_BIN), /usr/lib/lib$(CDS3_TSCCOMSWITCH).so);
	@$(call install_link, cds3-tsccomswitch, ../$(CDS3_TSCCOMSWITCH_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCCOMSWITCH).so);

endif
	@$(call install_finish,cds3-tsccomswitch)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCCOMSWITCH_PACKAGE_NAME).ipk $(CDS3_TSCCOMSWITCH_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsccomswitch.clean:
	rm -rf $(STATEDIR)/cds3-tsccomswitch.*
	rm -rf $(PKGDIR)/cds3-tsccomswitch_*
	rm -rf $(CDS3_TSCCOMSWITCH_DIR)
