# -*-makefile-*-
#
# Copyright (C) 2012 by <HFS>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CONFIGTOOLLIB) += configtoollib

#
# Paths and names
#
ifdef PTXCONF_CONFIGTOOLLIB_TRUNK
CONFIGTOOLLIB_VERSION	:= trunk
else
CONFIGTOOLLIB_VERSION	:= 1.0.0
endif
CONFIGTOOLLIB_NAME := configtoollib
CONFIGTOOLLIB		:= $(CONFIGTOOLLIB_NAME)-$(CONFIGTOOLLIB_VERSION)
CONFIGTOOLLIB_URL		:= file://$(PTXDIST_WORKSPACE)/wago_intern/wide/$(CONFIGTOOLLIB)
CONFIGTOOLLIB_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/wide/$(CONFIGTOOLLIB)
CONFIGTOOLLIB_DIR		:= $(BUILDDIR)/$(CONFIGTOOLLIB)
CONFIGTOOLLIB_BUILD_OOT	:= NO
CONFIGTOOLLIB_LICENSE	:= unknown

CONFIGTOOLLIB_PACKAGE_NAME := $(CONFIGTOOLLIB_NAME)_$(CONFIGTOOLLIB_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CONFIGTOOLLIB_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/configtoollib.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(CONFIGTOOLLIB_DIR))
	#@$(call extract, CONFIGTOOLLIB)
	@mkdir -p $(CONFIGTOOLLIB_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@rsync -a --exclude=".project" --exclude=".cproject"  $(CONFIGTOOLLIB_SRC) $(BUILDDIR)
	cd $(CONFIGTOOLLIB_DIR) && sh autogen.sh
endif
	@$(call patchin, CONFIGTOOLLIB)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#CONFIGTOOLLIB_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
CONFIGTOOLLIB_CONF_TOOL	:= autoconf
CONFIGTOOLLIB_CONF_OPT	:= $(CROSS_AUTOCONF_USR) --enable-debug=yes

$(STATEDIR)/configtoollib.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, CONFIGTOOLLIB)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/configtoollib.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=".project" --exclude=".cproject"  $(CONFIGTOOLLIB_SRC) $(BUILDDIR)
	@$(call world/compile, CONFIGTOOLLIB)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/configtoollib.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/install, CONFIGTOOLLIB)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup headers for later use in configs/@platform@/packages
	cd $(PKGDIR)/$(CONFIGTOOLLIB)/ && \
	tar -czvf $(CONFIGTOOLLIB).tgz * && \
	mv $(CONFIGTOOLLIB).tgz $(CONFIGTOOLLIB_PLATFORMCONFIGPACKAGEDIR)/ 
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Recover header from archive in configs/@platform@/packages
	mkdir -p $(PKGDIR)/$(CONFIGTOOLLIB)
	tar -xzvf $(CONFIGTOOLLIB_PLATFORMCONFIGPACKAGEDIR)/$(CONFIGTOOLLIB).tgz -C $(PKGDIR)/$(CONFIGTOOLLIB)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/configtoollib.targetinstall:
	@$(call targetinfo)

	@$(call install_init, configtoollib)
	@$(call install_fixup, configtoollib, PRIORITY, optional)
	@$(call install_fixup, configtoollib, SECTION, base)
	@$(call install_fixup, configtoollib, AUTHOR, "<HFS>")
	@$(call install_fixup, configtoollib, DESCRIPTION, missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(CONFIGTOOLLIB_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(CONFIGTOOLLIB_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(CONFIGTOOLLIB_PLATFORMCONFIGPACKAGEDIR)/$(CONFIGTOOLLIB_PACKAGE_NAME).ipk  
	@$(call install_archive, configtoollib, 0, 0, $(CONFIGTOOLLIB_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE	

#	#
#	# example code:; copy all libraries, links and binaries
#	#

	@for i in $(shell cd $(CONFIGTOOLLIB_PKGDIR) && find bin sbin usr/bin usr/sbin -type f); do \
		$(call install_copy, configtoollib, 0, 0, 0755, -, /$$i); \
	done
	@for i in $(shell cd $(CONFIGTOOLLIB_PKGDIR) && find lib usr/lib -name "*.so*"); do \
		$(call install_copy, configtoollib, 0, 0, 0644, -, /$$i); \
	done
	@links="$(shell cd $(CONFIGTOOLLIB_PKGDIR) && find lib usr/lib -type l)"; \
	if [ -n "$$links" ]; then \
		for i in $$links; do \
			from="`readlink $(CONFIGTOOLLIB_PKGDIR)/$$i`"; \
			to="/$$i"; \
			$(call install_link, configtoollib, $$from, $$to); \
		done; \
	fi

#	#
#	# FIXME: add all necessary things here
#	#
endif
	@$(call install_finish, configtoollib)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CONFIGTOOLLIB_PACKAGE_NAME).ipk $(CONFIGTOOLLIB_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/configtoollib.clean:
	@$(call targetinfo)
	@$(call clean_pkg, CONFIGTOOLLIB)
	@-rm -rf $(CONFIGTOOLLIB_DIR)

# vim: syntax=make
