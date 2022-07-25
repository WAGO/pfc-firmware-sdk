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
PACKAGES-$(PTXCONF_CDS3_TSCSYSLOG) += cds3-tscsyslog

CDS3_TSCSYSLOG_VERSION := 0.0.1
CDS3_TSCSYSLOG              := TscLogBackendSyslog
CDS3_TSCSYSLOG_DIR          := $(BUILDDIR)/$(CDS3_TSCSYSLOG)
CDS3_TSCSYSLOG_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCSYSLOG)
CDS3_TSCSYSLOG_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCSYSLOG)
CDS3_TSCSYSLOG_SRC          := $(CDS3_TSCSYSLOG_SRC_DIR)/$(CDS3_TSCSYSLOG)
CDS3_TSCSYSLOG_BIN          := lib$(CDS3_TSCSYSLOG).so.$(CDS3_TSCSYSLOG_VERSION)

CDS3_TSCSYSLOG_PACKAGE_NAME := cds3-tscsyslog_$(CDS3_TSCSYSLOG_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCSYSLOG_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsyslog.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsyslog.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCSYSLOG_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCSYSLOG_SRC_DIR)/ $(CDS3_TSCSYSLOG_DIR)/

#	$(call patchin, CDS3_IODRVDAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsyslog.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCSYSLOG_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCSYSLOG_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCSYSLOG_MAKE_OPT  := CC=$(CROSS_CC)

$(STATEDIR)/cds3-tscsyslog.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCSYSLOG)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCSYSLOG_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscLogBackendSyslog
$(STATEDIR)/cds3-tscsyslog.install:
	@$(call targetinfo)

	@mkdir -p $(TSCSYSLOG_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCSYSLOG_DIR)/*.h $(TSCSYSLOG_SYSROOT_INCLUDES)/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCSYSLOG_DIR) && tar cvzf $(CDS3_TSCSYSLOG_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCSYSLOG_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCSYSLOG_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCSYSLOG_PACKAGE_NAME).tgz -C $(TSCSYSLOG_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsyslog.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tscsyslog)

	@$(call install_fixup,cds3-tscsyslog,PRIORITY,optional)
	@$(call install_fixup,cds3-tscsyslog,SECTION,base)
	@$(call install_fixup,cds3-tscsyslog,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tscsyslog,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(CDS3_TSCSYSLOG_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(CDS3_TSCSYSLOG_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CDS3_TSCSYSLOG_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCSYSLOG_PACKAGE_NAME).ipk  
	@$(call install_archive, cds3-tscsyslog, 0, 0, $(CDS3_TSCSYSLOG_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else

	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_copy, cds3-tscsyslog, 0, 0, 0750, $(CDS3_TSCSYSLOG_DIR)/$(CDS3_TSCSYSLOG_BIN), /usr/lib/$(CDS3_TSCSYSLOG_BIN))
	@$(call install_link, cds3-tscsyslog, ./$(CDS3_TSCSYSLOG_BIN), /usr/lib/lib$(CDS3_TSCSYSLOG).so);
	@$(call install_link, cds3-tscsyslog, ../$(CDS3_TSCSYSLOG_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCSYSLOG).so);
endif
	@$(call install_finish,cds3-tscsyslog)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCSYSLOG_PACKAGE_NAME).ipk $(CDS3_TSCSYSLOG_PLATFORMCONFIGPACKAGEDIR)/
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tscsyslog.clean:
	rm -rf $(STATEDIR)/cds3-tscsyslog.*
	rm -rf $(PKGDIR)/cds3-tscsyslog_*
	rm -rf $(CDS3_TSCSYSLOG_DIR)
