# -*-makefile-*-
#
# Copyright (C) 2012 by <Hans Florian Scholz>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBWIDEOMS) += libwideoms

#
# Paths and names
#

LIBWIDEOMS_VERSION	:= 0.1
LIBWIDEOMS		:= libwideoms
LIBWIDEOMS_URL		:= file://$(PTXDIST_WORKSPACE)/wago_intern/wide/$(LIBWIDEOMS)
LIBWIDEOMS_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/wide/$(LIBWIDEOMS)
LIBWIDEOMS_DIR		:= $(BUILDDIR)/$(LIBWIDEOMS)
LIBWIDEOMS_BUILD_OOT	:= NO
LIBWIDEOMS_LICENSE	:= unknown

LIBWIDEOMS_PACKAGE_NAME := $(LIBWIDEOMS)_$(LIBWIDEOMS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBWIDEOMS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libwideoms.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(LIBWIDEOMS_DIR))	
	@mkdir -p $(LIBWIDEOMS_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=".project" --exclude=".cproject"  $(LIBWIDEOMS_SRC) $(BUILDDIR)
	cd $(LIBWIDEOMS_DIR) && sh autogen.sh
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBWIDEOMS_CONF_TOOL	:= autoconf
LIBWIDEOMS_CONF_OPT	:= $(CROSS_AUTOCONF_USR)

$(STATEDIR)/libwideoms.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/prepare, LIBWIDEOMS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/libwideoms.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=".project" --exclude=".cproject"  $(LIBWIDEOMS_SRC) $(BUILDDIR)
	@$(call world/compile, LIBWIDEOMS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libwideoms.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/install, LIBWIDEOMS)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup headers for later use in configs/@platform@/packages
	cd $(PKGDIR)/$(LIBWIDEOMS)/ && \
	tar -czvf $(LIBWIDEOMS).tgz * && \
	mv $(LIBWIDEOMS).tgz $(LIBWIDEOMS_PLATFORMCONFIGPACKAGEDIR)/ 
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Recover header from archive in configs/@platform@/packages
	mkdir -p $(PKGDIR)/$(LIBWIDEOMS)
	tar -xzvf $(LIBWIDEOMS_PLATFORMCONFIGPACKAGEDIR)/$(LIBWIDEOMS).tgz -C $(PKGDIR)/$(LIBWIDEOMS)
endif
	@$(call touch) 


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libwideoms.targetinstall:
	@$(call targetinfo)
	@$(call install_init, libwideoms)
	@$(call install_fixup, libwideoms, PRIORITY, optional)
	@$(call install_fixup, libwideoms, SECTION, base)
	@$(call install_fixup, libwideoms, AUTHOR, "<Hans Florian Scholz>")
	@$(call install_fixup, libwideoms, DESCRIPTION, missing)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(LIBWIDEOMS_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(LIBWIDEOMS_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(LIBWIDEOMS_PLATFORMCONFIGPACKAGEDIR)/$(LIBWIDEOMS_PACKAGE_NAME).ipk  
	@$(call install_archive, libwideoms, 0, 0, $(LIBWIDEOMS_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE	
	@$(call install_copy, libwideoms, 0, 0, 0644, $(LIBWIDEOMS_PKGDIR)/usr/lib/wide/libwideoms.so, /usr/lib/wide/libwideoms.so)	
endif
	@$(call install_finish, libwideoms)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(LIBWIDEOMS_PACKAGE_NAME).ipk $(LIBWIDEOMS_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libwideoms.clean:
	@$(call targetinfo)

	@$(call clean_pkg, LIBWIDEOMS)
	@-rm -rf $(LIBWIDEOMS_DIR)


# vim: syntax=make
