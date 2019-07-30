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
PACKAGES-$(PTXCONF_IECDIAGNOSTIC) += iecdiagnostic

#
# Paths and names
#
ifdef PTXCONF_IECDIAGNOSTIC_TRUNK
IECDIAGNOSTIC_VERSION	:= trunk
else
IECDIAGNOSTIC_VERSION	:= 0.1
endif
IECDIAGNOSTIC_NAME		:= iecdiagnostic
IECDIAGNOSTIC		:= $(IECDIAGNOSTIC_NAME)-$(IECDIAGNOSTIC_VERSION)
IECDIAGNOSTIC_URL		:= file://$(PTXDIST_WORKSPACE)/wago_intern/wide/$(IECDIAGNOSTIC)
IECDIAGNOSTIC_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/wide/$(IECDIAGNOSTIC)
IECDIAGNOSTIC_DIR		:= $(BUILDDIR)/$(IECDIAGNOSTIC)
IECDIAGNOSTIC_BUILD_OOT	:= NO
IECDIAGNOSTIC_LICENSE	:= unknown

IECDIAGNOSTIC_PACKAGE_NAME := $(IECDIAGNOSTIC_NAME)_$(IECDIAGNOSTIC_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
IECDIAGNOSTIC_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/iecdiagnostic.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(IECDIAGNOSTIC_DIR))
	#@$(call extract, IECDIAGNOSTIC)
	@mkdir -p $(IECDIAGNOSTIC_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=".project" --exclude=".cproject"  $(IECDIAGNOSTIC_SRC) $(BUILDDIR)
	@$(call patchin, IECDIAGNOSTIC)

ifdef PTXCONF_IECDIAGNOSTIC_TRUNK
	cd $(IECDIAGNOSTIC_DIR) && sh autogen.sh
else
	cd $(IECDIAGNOSTIC_DIR) && [ -f configure ] || sh autogen.sh
endif

endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#IECDIAGNOSTIC_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
IECDIAGNOSTIC_CONF_TOOL	:= autoconf
#IECDIAGNOSTIC_CONF_OPT	:= $(CROSS_AUTOCONF_USR)

$(STATEDIR)/iecdiagnostic.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/prepare, IECDIAGNOSTIC)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/iecdiagnostic.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
		@rsync -a --exclude=".project" --exclude=".cproject"  $(IECDIAGNOSTIC_SRC) $(BUILDDIR)
	@$(call xslt_compile, $(IECDIAGNOSTIC_DIR)/src/xml/codesys.xml)
	@$(call xslt_compile, $(IECDIAGNOSTIC_DIR)/src/xml/IEC-Custom.xml)
	@$(call world/compile, IECDIAGNOSTIC)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/iecdiagnostic.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/install, IECDIAGNOSTIC)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup headers for later use in configs/@platform@/packages
	cd $(PKGDIR)/$(IECDIAGNOSTIC)/ && \
	tar -czvf $(IECDIAGNOSTIC).tgz * && \
	mv $(IECDIAGNOSTIC).tgz $(IECDIAGNOSTIC_PLATFORMCONFIGPACKAGEDIR)/ 
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Recover header from archive in configs/@platform@/packages
	mkdir -p $(PKGDIR)/$(IECDIAGNOSTIC)
	tar -xzvf $(IECDIAGNOSTIC_PLATFORMCONFIGPACKAGEDIR)/$(IECDIAGNOSTIC).tgz -C $(PKGDIR)/$(IECDIAGNOSTIC)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/iecdiagnostic.targetinstall:
	@$(call targetinfo)

	@$(call install_init, iecdiagnostic)
	@$(call install_fixup, iecdiagnostic, PRIORITY, optional)
	@$(call install_fixup, iecdiagnostic, SECTION, base)
	@$(call install_fixup, iecdiagnostic, AUTHOR, "<Hans Florian Scholz>")
	@$(call install_fixup, iecdiagnostic, DESCRIPTION, missing)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(IECDIAGNOSTIC_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(IECDIAGNOSTIC_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(IECDIAGNOSTIC_PLATFORMCONFIGPACKAGEDIR)/$(IECDIAGNOSTIC_PACKAGE_NAME).ipk  
	@$(call install_archive, iecdiagnostic, 0, 0, $(IECDIAGNOSTIC_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE	

#	#
#	# example code:; copy all libraries, links and binaries
#	#

	@for i in $(shell cd $(IECDIAGNOSTIC_PKGDIR) && find bin sbin usr/bin usr/sbin -type f); do \
		$(call install_copy, iecdiagnostic, 0, 0, 0755, -, /$$i); \
	done
	@for i in $(shell cd $(IECDIAGNOSTIC_PKGDIR) && find lib usr/lib -name "*.so*"); do \
		$(call install_copy, iecdiagnostic, 0, 0, 0644, -, /$$i); \
	done
	@links="$(shell cd $(IECDIAGNOSTIC_PKGDIR) && find lib usr/lib -type l)"; \
	if [ -n "$$links" ]; then \
		for i in $$links; do \
			from="`readlink $(IECDIAGNOSTIC_PKGDIR)/$$i`"; \
			to="/$$i"; \
			$(call install_link, iecdiagnostic, $$from, $$to); \
		done; \
	fi

#	#
#	# FIXME: add all necessary things here
#	#
endif
	@$(call install_finish, iecdiagnostic)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(IECDIAGNOSTIC_PACKAGE_NAME).ipk $(IECDIAGNOSTIC_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/iecdiagnostic.clean:
	@$(call xslt_clean, $(IECDIAGNOSTIC_DIR)/src/xml/codesys.xml)
	@$(call xslt_clean, $(IECDIAGNOSTIC_DIR)/src/xml/IEC-Custom.xml)
	@$(call targetinfo)
	@-rm -f $(IECDIAGNOSTIC_SRC)/{configure,config.sub,config.guess,Makefile.in,src/Makefile.in}
	@$(call clean_pkg, IECDIAGNOSTIC)
	@-rm -rf $(IECDIAGNOSTIC_DIR)

# vim: syntax=make
