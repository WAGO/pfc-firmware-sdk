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
PACKAGES-$(PTXCONF_PLCSNMP_MANAGER) += plcsnmp_manager

#
# Paths and names
#
ifdef PTXCONF_PLCSNMP_MANAGER_TRUNK
PLCSNMP_MANAGER_VERSION	:= trunk
else
PLCSNMP_MANAGER_VERSION	:= 2.0.0
endif
PLCSNMP_MANAGER_NAME		:= plcsnmp_manager
PLCSNMP_MANAGER		:= $(PLCSNMP_MANAGER_NAME)
PLCSNMP_MANAGER_URL		:= file://$(PTXDIST_WORKSPACE)/wago_intern/wide/$(PLCSNMP_MANAGER)
PLCSNMP_MANAGER_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/wide/$(PLCSNMP_MANAGER)
PLCSNMP_MANAGER_DIR		:= $(BUILDDIR)/$(PLCSNMP_MANAGER)
PLCSNMP_MANAGER_BUILD_OOT	:= NO
PLCSNMP_MANAGER_LICENSE	:= unknown

PLCSNMP_MANAGER_PACKAGE_NAME := plcsnmp-manager_$(PLCSNMP_MANAGER_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PLCSNMP_MANAGER_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/plcsnmp_manager.extract: $(STATEDIR)/autogen-tools
endif

$(STATEDIR)/plcsnmp_manager.extract:
	@$(call targetinfo)
	@$(call clean, $(PLCSNMP_MANAGER_DIR))
	#@$(call extract, PLCSNMP_MANAGER)
	@mkdir -p $(PLCSNMP_MANAGER_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=".project" --exclude=".cproject"  $(PLCSNMP_MANAGER_SRC) $(BUILDDIR)
	@$(call patchin, PLCSNMP_MANAGER)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#PLCSNMP_MANAGER_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
PLCSNMP_MANAGER_CONF_TOOL	:= autoconf
PLCSNMP_MANAGER_CONF_OPT	:= $(CROSS_AUTOCONF_USR)
#PLCSNMP_MANAGER_CONF_OPT        += --enable-tests
#PLCSNMP_MANAGER_CONF_OPT        += --enable-debug

$(STATEDIR)/plcsnmp_manager.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	cd $(PLCSNMP_MANAGER_DIR) && sh autogen.sh
	@$(call world/prepare, PLCSNMP_MANAGER)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/plcsnmp_manager.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=".project" --exclude=".cproject"  $(PLCSNMP_MANAGER_SRC) $(BUILDDIR)
	@$(call world/compile, PLCSNMP_MANAGER)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/plcsnmp_manager.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/install, PLCSNMP_MANAGER)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup headers for later use in configs/@platform@/packages
	cd $(PKGDIR)/$(PLCSNMP_MANAGER)/ && \
	tar -czvf $(PLCSNMP_MANAGER).tgz * && \
	mv $(PLCSNMP_MANAGER).tgz $(PLCSNMP_MANAGER_PLATFORMCONFIGPACKAGEDIR)/ 
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Recover header from archive in configs/@platform@/packages
	mkdir -p $(PKGDIR)/$(PLCSNMP_MANAGER)
	tar -xzvf $(PLCSNMP_MANAGER_PLATFORMCONFIGPACKAGEDIR)/$(PLCSNMP_MANAGER).tgz -C $(PKGDIR)/$(PLCSNMP_MANAGER)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/plcsnmp_manager.targetinstall:
	@$(call targetinfo)

	@$(call install_init, plcsnmp_manager)
	@$(call install_fixup, plcsnmp_manager, PRIORITY, optional)
	@$(call install_fixup, plcsnmp_manager, SECTION, base)
	@$(call install_fixup, plcsnmp_manager, AUTHOR, "<HFS>")
	@$(call install_fixup, plcsnmp_manager, DESCRIPTION, missing)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(PLCSNMP_MANAGER_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(PLCSNMP_MANAGER_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(PLCSNMP_MANAGER_PLATFORMCONFIGPACKAGEDIR)/$(PLCSNMP_MANAGER_PACKAGE_NAME).ipk  
	@$(call install_archive, plcsnmp_manager, 0, 0, $(PLCSNMP_MANAGER_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE	

#	#
#	# example code:; copy all libraries, links and binaries
#	#

	@for i in $(shell cd $(PLCSNMP_MANAGER_PKGDIR) && find bin sbin usr/bin usr/sbin -type f); do \
		$(call install_copy, plcsnmp_manager, 0, 0, 0755, -, /$$i); \
	done
	@for i in $(shell cd $(PLCSNMP_MANAGER_PKGDIR) && find lib usr/lib -name "*.so*"); do \
		$(call install_copy, plcsnmp_manager, 0, 0, 0644, -, /$$i); \
	done
	@links="$(shell cd $(PLCSNMP_MANAGER_PKGDIR) && find lib usr/lib -type l)" && \
	if [ -n "$$links" ]; then \
		for i in $$links; do \
			from="`readlink $(PLCSNMP_MANAGER_PKGDIR)/$$i`"; \
			to="/$$i"; \
			$(call install_link, plcsnmp_manager, $$from, $$to); \
		done; \
	fi
	@$(call install_link, plcsnmp_manager, ../libplcsnmp_manager.so, /usr/lib/wide/libplcsnmp_manager.so)
	
#	#
#	# FIXME: add all necessary things here
#	#
endif
	@$(call install_finish, plcsnmp_manager)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(PLCSNMP_MANAGER_PACKAGE_NAME).ipk $(PLCSNMP_MANAGER_PLATFORMCONFIGPACKAGEDIR)/
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/plcsnmp_manager.clean:
	@$(call targetinfo)
	@$(call clean_pkg, PLCSNMP_MANAGER)
	@-rm -rf $(PLCSNMP_MANAGER_DIR)

# vim: syntax=make
