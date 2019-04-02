## -*-makefile-*-
#
# Copyright (C) 2014 by Wago Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_TSCDPSLIB) += cds3-tscdpslib

CDS3_TSCDPSLIB_VERSION      := 0.0.1
CDS3_TSCDPSLIB              := TscDpsLib
CDS3_TSCDPSLIB_DIR          := $(BUILDDIR)/$(CDS3_TSCDPSLIB)
CDS3_TSCDPSLIB_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCDPSLIB)
CDS3_TSCDPSLIB_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCDPSLIB)
CDS3_TSCDPSLIB_SRC          := $(CDS3_TSCDPSLIB_SRC_DIR)/$(CDS3_TSCDPSLIB)
CDS3_TSCDPSLIB_BIN          := lib$(CDS3_TSCDPSLIB).so.$(CDS3_TSCDPSLIB_VERSION)

CDS3_TSCDPSLIB_PACKAGE_NAME := cds3-tscdpslib_$(CDS3_TSCDPSLIB_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCDPSLIB_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscdpslib.get:
	@$(call targetinfo)
#	make -C $(CDS3_TSCDPSLIB_SRC_DIR) $(CDS3_TSCDPSLIB)Dep.h $(CDS3_TSCDPSLIB)Itf.h
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscdpslib.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCDPSLIB_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCDPSLIB_SRC_DIR)/ $(CDS3_TSCDPSLIB_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscdpslib.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCDPSLIB_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCDPSLIB_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCDPSLIB_MAKE_OPT  := CC=$(CROSS_CC)

#CDS3_IODRVDAL_MAKE_OPT += "DBGMODE=-g3"
#CDS3_IODRVDAL_MAKE_OPT += "OPTIMIZE=-O3"


$(STATEDIR)/cds3-tscdpslib.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCDPSLIB)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCDPSLIB_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscProfibus
$(STATEDIR)/cds3-tscdpslib.install:
	@$(call targetinfo)

	@mkdir -p $(TSCDPSLIB_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCDPSLIB_DIR)/*.h $(TSCDPSLIB_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCDPSLIB_DIR) && tar cvzf $(CDS3_TSCDPSLIB_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCDPSLIB_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCDPSLIB_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCDPSLIB_PACKAGE_NAME).tgz -C $(TSCDPSLIB_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscdpslib.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscdpslib)

	@$(call install_fixup,cds3-tscdpslib,PRIORITY,optional)
	@$(call install_fixup,cds3-tscdpslib,SECTION,base)
	@$(call install_fixup,cds3-tscdpslib,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscdpslib,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCDPSLIB_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCDPSLIB_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCDPSLIB_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCDPSLIB_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscdpslib, 0, 0, $(CDS3_TSCDPSLIB_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscdpslib, 0, 0, 0750, $(CDS3_TSCDPSLIB_DIR)/$(CDS3_TSCDPSLIB_BIN), /usr/lib/$(CDS3_TSCDPSLIB_BIN))
	@$(call install_link, cds3-tscdpslib, ./$(CDS3_TSCDPSLIB_BIN), /usr/lib/lib$(CDS3_TSCDPSLIB).so);
	@$(call install_link, cds3-tscdpslib, ../$(CDS3_TSCDPSLIB_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCDPSLIB).so);

endif
	@$(call install_finish,cds3-tscdpslib)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCDPSLIB_PACKAGE_NAME).ipk $(CDS3_TSCDPSLIB_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscdpslib.clean:
	rm -rf $(STATEDIR)/cds3-tscdpslib.*
	rm -rf $(PKGDIR)/cds3-tscdpslib_*
	rm -rf $(CDS3_TSCDPSLIB_DIR)
