# -*-makefile-*-
# $Id: template-make 8509 2008-06-12 12:45:40Z mkl $
#
# Copyright (C) 2010 by WAGO
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_TYPELABEL) += typelabel

#
# Paths and names
#
TYPELABEL_VERSION		:= 1.0.0
TYPELABEL			:= typelabel
TYPELABEL_URL		:= file://$(PTXDIST_WORKSPACE)/wago_intern/$(TYPELABEL)
TYPELABEL_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/$(TYPELABEL)
TYPELABEL_DIR		:= $(BUILDDIR)/$(TYPELABEL)

TYPELABEL_PACKAGE_NAME := $(TYPELABEL)_$(TYPELABEL_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
TYPELABEL_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------
$(STATEDIR)/typelabel.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(TYPELABEL_DIR))
	@mkdir -p $(TYPELABEL_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=".project" --exclude=".cproject"  $(TYPELABEL_SRC) $(BUILDDIR)
	@$(call patchin, TYPELABEL)
	cd $(TYPELABEL_DIR) && sh autogen.sh
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

TYPELABEL_PATH	:= PATH=$(CROSS_PATH)
TYPELABEL_ENV 	:= $(CROSS_ENV)
TYPELABEL_CONF_TOOL	:= autoconf
TYPELABEL_CONF_OPT := $(CROSS_AUTOCONF_USR)

ifneq ($(PTXCONF_TYPELABEL_EEPROM_PATH),)
	TYPELABEL_CONF_OPT += --with-eeprom-path=$(PTXCONF_TYPELABEL_EEPROM_PATH)
endif

ifdef PTXCONF_TYPELABEL_EEPROM_HAS_WP
	TYPELABEL_CONF_OPT += --enable-eeprom-wp=yes
else
	TYPELABEL_CONF_OPT += --enable-eeprom-wp=no
endif
ifdef PTXCONF_TYPELABEL_FAKELABEL
	TYPELABEL_CONF_OPT += --enable-fakelabel=yes
endif

$(STATEDIR)/typelabel.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/prepare, TYPELABEL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/typelabel.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/compile, TYPELABEL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/typelabel.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/install, TYPELABEL)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup headers for later use in configs/@platform@/packages
	@mkdir -p $(TYPELABEL_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PKGDIR)/$(TYPELABEL)/ && \
	tar -czvf $(TYPELABEL).tgz * && \
	mv $(TYPELABEL).tgz $(TYPELABEL_PLATFORMCONFIGPACKAGEDIR)/ 
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Recover header from archive in configs/@platform@/packages
	mkdir -p $(PKGDIR)/$(TYPELABEL)
	tar -xzvf $(TYPELABEL_PLATFORMCONFIGPACKAGEDIR)/$(TYPELABEL).tgz -C $(PKGDIR)/$(TYPELABEL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/typelabel.targetinstall:
	@$(call targetinfo)
	@$(call install_init, typelabel)
	@$(call install_fixup, typelabel,PRIORITY,optional)
	@$(call install_fixup, typelabel,SECTION,base)
	@$(call install_fixup, typelabel,AUTHOR,"WAGO <www.WAGO.com>")
	@$(call install_fixup, typelabel,DESCRIPTION,missing)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	@rm -rf $(TYPELABEL_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	@cd $(TYPELABEL_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(TYPELABEL_PLATFORMCONFIGPACKAGEDIR)/$(TYPELABEL_PACKAGE_NAME).ipk  
	@$(call install_archive, typelabel, 0, 0, $(TYPELABEL_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE	
	@$(call install_copy, typelabel, 0, 0, 0755, -, /usr/lib/libtypelabel.so)

ifdef PTXCONF_TYPELABEL_SETFWNR
	@$(call install_copy, typelabel, 0, 0, 0755, -, /usr/bin/setfwnr)
endif 

ifdef PTXCONF_TYPELABEL_FAKELABEL
	@$(error Typelabel should not be targetinstalled with fakelabel support)
endif 

endif
	@$(call install_finish, typelabel)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(TYPELABEL_PACKAGE_NAME).ipk $(TYPELABEL_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/typelabel.clean:
	@$(call targetinfo)
	@$(call clean_pkg, TYPELABEL)
	@-rm -rf $(TYPELABEL_DIR)

# vim: syntax=make
