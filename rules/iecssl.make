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
PACKAGES-$(PTXCONF_IECSSL) += iecssl

#
# Paths and names
#
ifdef PTXCONF_IECSSL_TRUNK
IECSSL_VERSION	:= trunk
else
IECSSL_VERSION	:= 1.0.2
endif
IECSSL_NAME		:= iecssl
IECSSL		:= $(IECSSL_NAME)
IECSSL_URL		:= file://$(PTXDIST_WORKSPACE)/wago_intern/wide/$(IECSSL)
IECSSL_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/wide/$(IECSSL)
IECSSL_DIR		:= $(BUILDDIR)/$(IECSSL)
IECSSL_BUILD_OOT	:= NO
IECSSL_LICENSE	:= unknown

IECSSL_PACKAGE_NAME := $(IECSSL_NAME)_$(IECSSL_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
IECSSL_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/iecssl.extract: $(STATEDIR)/autogen-tools
endif

$(STATEDIR)/iecssl.extract:
	@$(call targetinfo)
	@$(call clean, $(IECSSL_DIR))
	#@$(call extract, IECSSL)
	@mkdir -p $(IECSSL_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=".project" --exclude=".cproject"  $(IECSSL_SRC) $(BUILDDIR)
	@$(call patchin, IECSSL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#IECSSL_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
IECSSL_CONF_TOOL	:= autoconf
#IECSSL_CONF_OPT	:= $(CROSS_AUTOCONF_USR)

$(STATEDIR)/iecssl.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	cd $(IECSSL_DIR) && sh autogen.sh
	@$(call world/prepare, IECSSL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/iecssl.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=".project" --exclude=".cproject"  $(IECSSL_SRC) $(BUILDDIR)
	@$(call world/compile, IECSSL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/iecssl.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/install, IECSSL)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup headers for later use in configs/@platform@/packages
	cd $(PKGDIR)/$(IECSSL)/ && \
	tar -czvf $(IECSSL).tgz * && \
	mv $(IECSSL).tgz $(IECSSL_PLATFORMCONFIGPACKAGEDIR)/ 
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Recover header from archive in configs/@platform@/packages
	mkdir -p $(PKGDIR)/$(IECSSL)
	tar -xzvf $(IECSSL_PLATFORMCONFIGPACKAGEDIR)/$(IECSSL).tgz -C $(PKGDIR)/$(IECSSL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/iecssl.targetinstall:
	@$(call targetinfo)

	@$(call install_init, iecssl)
	@$(call install_fixup, iecssl, PRIORITY, optional)
	@$(call install_fixup, iecssl, SECTION, base)
	@$(call install_fixup, iecssl, AUTHOR, "<HFS>")
	@$(call install_fixup, iecssl, DESCRIPTION, missing)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(IECSSL_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(IECSSL_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(IECSSL_PLATFORMCONFIGPACKAGEDIR)/$(IECSSL_PACKAGE_NAME).ipk  
	@$(call install_archive, iecssl, 0, 0, $(IECSSL_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE	

#	#
#	# example code:; copy all libraries, links and binaries
#	#

	@for i in $(shell cd $(IECSSL_PKGDIR) && find bin sbin usr/bin usr/sbin -type f); do \
		$(call install_copy, iecssl, 0, 0, 0755, -, /$$i); \
	done
	@for i in $(shell cd $(IECSSL_PKGDIR) && find lib usr/lib -name "*.so*"); do \
		$(call install_copy, iecssl, 0, 0, 0644, -, /$$i); \
	done
	@links="$(shell cd $(IECSSL_PKGDIR) && find lib usr/lib -type l)" && \
	if [ -n "$$links" ]; then \
		for i in $$links; do \
			from="`readlink $(IECSSL_PKGDIR)/$$i`"; \
			to="/$$i"; \
			$(call install_link, iecssl, $$from, $$to); \
		done; \
	fi
	@$(call install_link, iecssl, ../libiecssl.so, /usr/lib/wide/libiecssl.so)

#	#
#	# FIXME: add all necessary things here
#	#
endif
	@$(call install_finish, iecssl)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(IECSSL_PACKAGE_NAME).ipk $(IECSSL_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/iecssl.clean:
	@$(call targetinfo)
	@$(call clean_pkg, IECSSL)
	@-rm -rf $(IECSSL_DIR)

# vim: syntax=make
