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
PACKAGES-$(PTXCONF_CDS3_TSCUSERDB) += cds3-tscuserdb

CDS3_TSCUSERDB_VERSION		:= 1.0.0
CDS3_TSCUSERDB              := TscUserDB
CDS3_TSCUSERDB_DIR          := $(BUILDDIR)/$(CDS3_TSCUSERDB)
CDS3_TSCUSERDB_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCUSERDB)
CDS3_TSCUSERDB_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCUSERDB)
CDS3_TSCUSERDB_SRC          := $(CDS3_TSCUSERDB_SRC_DIR)/$(CDS3_TSCUSERDB)
CDS3_TSCUSERDB_BIN          := lib$(CDS3_TSCUSERDB).so.$(CDS3_TSCUSERDB_VERSION)

CDS3_TSCUSERDB_PACKAGE_NAME := cds3-tscuserdb_$(CDS3_TSCUSERDB_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCUSERDB_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscuserdb.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscuserdb.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCUSERDB_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCUSERDB_SRC_DIR)/ $(CDS3_TSCUSERDB_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tscuserdb.prepare:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCUSERDB_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCUSERDB_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCUSERDB_MAKE_OPT  := CC=$(CROSS_CC)

CDS3_TSCUSERDB_MAKE_ENV += VERSION=$(CDS3_TSCUSERDB_VERSION)

ifdef PTXCONF_CDS3_TSCUSERDB_TARGET_PFCXXX
	CDS3_TSCUSERDB_MAKE_ENV += TRG_PLATFORM=pfc
else
ifdef PTXCONF_CDS3_TSCUSERDB_TARGET_PAC
	CDS3_TSCUSERDB_MAKE_ENV += TRG_PLATFORM=pac
endif
endif

$(STATEDIR)/cds3-tscuserdb.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCUSERDB)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCUSERDB_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscUserDB
$(STATEDIR)/cds3-tscuserdb.install:
	@$(call targetinfo)

	@mkdir -p $(TSCUSERDB_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCUSERDB_DIR)/*.h $(TSCUSERDB_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCUSERDB_DIR) && tar cvzf $(CDS3_TSCUSERDB_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCUSERDB_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCUSERDB_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCUSERDB_PACKAGE_NAME).tgz -C $(TSCUSERDB_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscuserdb.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscuserdb)

	@$(call install_fixup,cds3-tscuserdb,PRIORITY,optional)
	@$(call install_fixup,cds3-tscuserdb,SECTION,base)
	@$(call install_fixup,cds3-tscuserdb,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscuserdb,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCUSERDB_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(CDS3_TSCUSERDB_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCUSERDB_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCUSERDB_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tscuserdb, 0, 0, $(CDS3_TSCUSERDB_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tscuserdb, 0, 0, 0750, $(CDS3_TSCUSERDB_DIR)/$(CDS3_TSCUSERDB_BIN), /usr/lib/$(CDS3_TSCUSERDB_BIN))
	@$(call install_copy, cds3-tscuserdb, 0, 0, 0640, $(CDS3_TSCUSERDB_DIR)/$(CDS3_TSCUSERDB).cfg, $(PTXCONF_CDS3_PLCCONFIGDIR)/$(CDS3_TSCUSERDB).cfg, n)
ifdef PTXCONF_CDS3_SECURE_LOGIN_ENABLE
		@$(call install_replace, cds3-tscuserdb, $(PTXCONF_CDS3_PLCCONFIGDIR)/$(CDS3_TSCUSERDB).cfg, @CDS3_SECURE_LOGIN_ENABLE@, $(PTXCONF_CDS3_SECURE_LOGIN_ENABLE));
endif
	@$(call install_link, cds3-tscuserdb, ./$(CDS3_TSCUSERDB_BIN), /usr/lib/lib$(CDS3_TSCUSERDB).so);
	@$(call install_link, cds3-tscuserdb, ../$(CDS3_TSCUSERDB_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCUSERDB).so);
	

endif
	@$(call install_finish,cds3-tscuserdb)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCUSERDB_PACKAGE_NAME).ipk $(CDS3_TSCUSERDB_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscuserdb.clean:
	rm -rf $(STATEDIR)/cds3-tscuserdb.*
	rm -rf $(PKGDIR)/cds3-tscuserdb_*
	rm -rf $(CDS3_TSCUSERDB_DIR)
