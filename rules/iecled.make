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
PACKAGES-$(PTXCONF_IECLED) += iecled

#
# Paths and names
#
ifdef PTXCONF_IECLED_TRUNK
IECLED_VERSION	:= trunk
else
IECLED_VERSION	:= 0.1
endif
IECLED_NAME		:= iecled
IECLED				:= $(IECLED_NAME)-$(IECLED_VERSION)
IECLED_URL		:= file://$(PTXDIST_WORKSPACE)/wago_intern/wide/$(IECLED)
IECLED_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/wide/$(IECLED)
IECLED_DIR		:= $(BUILDDIR)/$(IECLED)
IECLED_BUILD_OOT	:= NO
IECLED_LICENSE	:= unknown

IECLED_PACKAGE_NAME := $(IECLED_NAME)_$(IECLED_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
IECLED_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/iecled.extract: $(STATEDIR)/autogen-tools
endif

$(STATEDIR)/iecled.extract:
	@$(call targetinfo)
	@$(call clean, $(IECLED_DIR))
	#@$(call extract, IECLED)
	@mkdir -p $(IECLED_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=".project" --exclude=".cproject"  $(IECLED_SRC) $(BUILDDIR)
	@$(call patchin, IECLED)
	cd $(IECLED_DIR) && sh autogen.sh
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#IECLED_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
IECLED_CONF_TOOL	:= autoconf
#IECLED_CONF_OPT	:= $(CROSS_AUTOCONF_USR)

$(STATEDIR)/iecled.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/prepare, IECLED)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/iecled.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	 #WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=".project" --exclude=".cproject"  $(IECLED_SRC) $(BUILDDIR)
	@$(call world/compile, IECLED)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/iecled.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/install, IECLED)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup headers for later use in configs/@platform@/packages
	cd $(PKGDIR)/$(IECLED)/ && \
	tar -czvf $(IECLED).tgz * && \
	mv $(IECLED).tgz $(IECLED_PLATFORMCONFIGPACKAGEDIR)/ 
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Recover header from archive in configs/@platform@/packages
	mkdir -p $(PKGDIR)/$(IECLED)
	tar -xzvf $(IECLED_PLATFORMCONFIGPACKAGEDIR)/$(IECLED).tgz -C $(PKGDIR)/$(IECLED)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/iecled.targetinstall:
	@$(call targetinfo)

	@$(call install_init, iecled)
	@$(call install_fixup, iecled, PRIORITY, optional)
	@$(call install_fixup, iecled, SECTION, base)
	@$(call install_fixup, iecled, AUTHOR, "<HFS>")
	@$(call install_fixup, iecled, DESCRIPTION, missing)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(IECLED_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(IECLED_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(IECLED_PLATFORMCONFIGPACKAGEDIR)/$(IECLED_PACKAGE_NAME).ipk  
	@$(call install_archive, iecled, 0, 0, $(IECLED_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE	

#	#
#	# example code:; copy all libraries, links and binaries
#	#

	@for i in $(shell cd $(IECLED_PKGDIR) && find bin sbin usr/bin usr/sbin -type f); do \
		$(call install_copy, iecled, 0, 0, 0755, -, /$$i); \
	done
	@for i in $(shell cd $(IECLED_PKGDIR) && find lib usr/lib -name "*.so*"); do \
		$(call install_copy, iecled, 0, 0, 0644, -, /$$i); \
	done
	@links="$(shell cd $(IECLED_PKGDIR) && find lib usr/lib -type l)"; \
	if [ -n "$$links" ]; then \
		for i in $$links; do \
			from="`readlink $(IECLED_PKGDIR)/$$i`"; \
			to="/$$i"; \
			$(call install_link, iecled, $$from, $$to); \
		done; \
	fi

#	#
#	# FIXME: add all necessary things here
#	#
endif
	@$(call install_finish, iecled)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(IECLED_PACKAGE_NAME).ipk $(IECLED_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/iecled.clean:
	@$(call targetinfo)
	@$(call clean_pkg, IECLED)
	@-rm -rf $(IECLED_DIR)

# vim: syntax=make
