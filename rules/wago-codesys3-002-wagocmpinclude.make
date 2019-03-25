## -*-makefile-*-
# $Id$
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
PACKAGES-$(PTXCONF_CDS3_INCLUDES) += cds3-includes

CDS3_INCLUDES_VERSION	 := 0.0.1
CDS3_INCLUDES              := cds3-includes
CDS3_INCLUDES_DIR          := $(BUILDDIR)/$(CDS3_INCLUDES)
CDS3_INCLUDES_RTS_NAME     := codesys-3
CDS3_INCLUDES_RTS_DIR      := $(BUILDDIR)/$(CDS3_INCLUDES_RTS_NAME)
CDS3_INCLUDES_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_INCLUDES)
CDS3_INCLUDES_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/Include
CDS3_INCLUDES_RTS_SRC_DIR  := $(PTXDIST_WORKSPACE)/wago_intern/plc/codesys/$(CDS3_INCLUDES_RTS_NAME)/
CDS3_INCLUDES_SRC          := $(CDS3_INCLUDES_SRC_DIR)/$(CDS3_INCLUDES)
CDS3_INCLUDES_RTS_SRC      := $(CDS3_INCLUDES_RTS_SRC_DIR)/
CDS3_INCLUDES_BIN          := lib$(CDS3_INCLUDES).so.$(CDS3_INCLUDES_VERSION)

CDS3_INCLUDES_PACKAGE_NAME := cds3-includes_$(CDS3_INCLUDES_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_INCLUDES_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-includes.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-includes.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_INCLUDES_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	-mkdir $(CDS3_INCLUDES_RTS_DIR)
	rsync -a --exclude=Documentation \
		--exclude=Templates/ \
		--exclude=RtsConfigurator/ \
		--exclude=Placeholder \
		--exclude=BuildUtils/ \
		--exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_INCLUDES_RTS_SRC_DIR) $(CDS3_INCLUDES_RTS_DIR)

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_INCLUDES_SRC_DIR)/ $(CDS3_INCLUDES_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-includes.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
$(STATEDIR)/cds3-includes.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-includes.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	-mkdir -p $(PTXCONF_SYSROOT_TARGET)/usr/include/codesys3
	rsync -a --include='*.h' \
                 -f 'hide,! */' \
                $(CODESYS3_DIR)/Components/ $(PTXCONF_SYSROOT_TARGET)/usr/include/codesys3/
	rsync -a --include='*.h' \
                 -f 'hide,! */' \
                $(CODESYS3_DIR)/Platforms/Linux/ $(PTXCONF_SYSROOT_TARGET)/usr/include/codesys3/
	@cp $(CDS3_INCLUDES_DIR)/*.h $(PTXCONF_SYSROOT_TARGET)/usr/include/codesys3
	cp $(CDS3_INCLUDES_DIR)/TscInterfaces/*.h  $(PTXCONF_SYSROOT_TARGET)/usr/include/codesys3
	$(AR) -r $(PTXCONF_SYSROOT_TARGET)/usr/lib/libcds3-includes.a
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE

endif
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-includes.targetinstall:
	@$(call targetinfo)

# Packet contains no artifcats.

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-includes.clean:
