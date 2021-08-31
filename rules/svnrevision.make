# -*-makefile-*-
#
# Copyright (C) 2013 by WAGO Kontakttechnik GmbH.>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_SVNREVISION) += svnrevision

SVNREVISION_VERSION	:= 1.0

# for binary artifacts
SVNREVISION_PACKAGE_NAME := svnrevision_$(SVNREVISION_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
SVNREVISION_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

# Assert that $1 is not empty, abort if this is the case
assert = $(if $(strip $1),$1,$(error $2))

PHONY += $(STATEDIR)/svnrevision.targetinstall

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

# PSV evaluates /etc/SVNREVISION => abort compilation if its contents cannot be determined.
ETC_SVNREVISION_STRING="$(call assert,$(shell ptxd_get_svn_revision '@$(PTXCONF_PLATFORM)'),"Error: cannot determine SVN revision. Aborting!")"
ETC_SVNREVISION_BUILD_DATE="$(call assert,$(shell date),"Error: cannot determine current date. Aborting!")"
ETC_SVNREVISION_BUILD_DATE_BASIC="$(call assert,$(shell date +%Y%m%d),"Error: cannot determine current basic date. Aborting!")"
ETC_SVNREVISION_BUILD_URL="$(call assert,$(shell ptxd_get_svn_build_url '@$(PTXCONF_PLATFORM)'),"Error: cannot determine SVN URL. Aborting!")"
ETC_SVNREVISION_BUILD_CONFIG="$(call assert,$(shell basename '$(shell readlink $(PTXDIST_WORKSPACE)/selected_ptxconfig)'),"Error: cannot determine config. Aborting!")"

endif

$(STATEDIR)/svnrevision.targetinstall: $(PTXDIST_PTXCONFIG) $(PTXDIST_PLATFORMCONFIG)
	@$(call targetinfo)

	@$(call install_init, svnrevision)

	@$(call install_fixup,svnrevision,PRIORITY,optional)
	@$(call install_fixup,svnrevision,SECTION,base)
	@$(call install_fixup,svnrevision,AUTHOR,"WAGO Kontakttechnik GmbH.")
	@$(call install_fixup,svnrevision,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(SVNREVISION_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(SVNREVISION_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(SVNREVISION_PLATFORMCONFIGPACKAGEDIR)/$(SVNREVISION_PACKAGE_NAME).ipk  
	@$(call install_archive, svnrevision, 0, 0, $(SVNREVISION_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else

	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE	
	@$(call install_alternative, svnrevision, 0, 0, 0644, /etc/SVNREVISION)

	@$(call install_replace, svnrevision, /etc/SVNREVISION, \
		@SVN_REVISION@, $(ETC_SVNREVISION_STRING))

	@$(call install_replace, svnrevision, /etc/SVNREVISION, \
		@SVN_BUILD_DATE@, $(ETC_SVNREVISION_BUILD_DATE))

	@$(call install_replace, svnrevision, /etc/SVNREVISION, \
		@SVN_BUILD_DATE_BASIC@, $(ETC_SVNREVISION_BUILD_DATE_BASIC))

	@$(call install_replace, svnrevision, /etc/SVNREVISION, \
		@SVN_BUILD_URL@, $(ETC_SVNREVISION_BUILD_URL))

	@$(call install_replace, svnrevision, /etc/SVNREVISION, \
		@SVN_BUILD_CONFIG@, $(ETC_SVNREVISION_BUILD_CONFIG))

endif
	@$(call install_finish,svnrevision)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(SVNREVISION_PACKAGE_NAME).ipk $(SVNREVISION_PLATFORMCONFIGPACKAGEDIR)/
endif

	@$(call touch)

# vim: syntax=make
