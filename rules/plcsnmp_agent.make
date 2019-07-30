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
PACKAGES-$(PTXCONF_PLCSNMP_AGENT) += plcsnmp_agent

#
# Paths and names
#
ifdef PTXCONF_PLCSNMP_AGENT_TRUNK
PLCSNMP_AGENT_VERSION	:= trunk
else
PLCSNMP_AGENT_VERSION	:= 3.0.0
endif
PLCSNMP_AGENT_NAME		:= plcsnmp_agent
PLCSNMP_AGENT		:= $(PLCSNMP_AGENT_NAME)
PLCSNMP_AGENT_URL		:= file://$(PTXDIST_WORKSPACE)/wago_intern/wide/$(PLCSNMP_AGENT)
PLCSNMP_AGENT_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/wide/$(PLCSNMP_AGENT)
PLCSNMP_AGENT_DIR		:= $(BUILDDIR)/$(PLCSNMP_AGENT)
PLCSNMP_AGENT_BUILD_OOT	:= NO
PLCSNMP_AGENT_LICENSE	:= unknown

PLCSNMP_AGENT_PACKAGE_NAME := plcsnmp-agent_$(PLCSNMP_AGENT_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PLCSNMP_AGENT_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/plcsnmp.extract: $(STATEDIR)/autogen-tools
endif

$(STATEDIR)/plcsnmp_agent.extract:
	@$(call targetinfo)
	@$(call clean, $(PLCSNMP_AGENT_DIR))
	#@$(call extract, PLCSNMP_AGENT)
	@mkdir -p $(PLCSNMP_AGENT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=".project" --exclude=".cproject"  $(PLCSNMP_AGENT_SRC) $(BUILDDIR)
	@$(call patchin, PLCSNMP_AGENT)
	cd $(PLCSNMP_AGENT_DIR) && sh autogen.sh
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#PLCSNMP_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
PLCSNMP_AGENT_CONF_TOOL	:= autoconf
PLCSNMP_AGENT_CONF_OPT	:= $(CROSS_AUTOCONF_USR)
PLCSNMP_AGENT_CONF_OPT        += --enable-debug


$(STATEDIR)/plcsnmp_agent.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/prepare, PLCSNMP_AGENT)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/plcsnmp_agent.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@rsync -a --exclude=".project" --exclude=".cproject"  $(PLCSNMP_AGENT_SRC) $(BUILDDIR)
	@$(call world/compile, PLCSNMP_AGENT)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/plcsnmp_agent.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/install, PLCSNMP_AGENT)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup headers for later use in configs/@platform@/packages
	cd $(PKGDIR)/$(PLCSNMP_AGENT)/ && \
	tar -czvf $(PLCSNMP_AGENT).tgz * && \
	mv $(PLCSNMP_AGENT).tgz $(PLCSNMP_AGENT_PLATFORMCONFIGPACKAGEDIR)/ 
endif
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Recover header from archive in configs/@platform@/packages
	mkdir -p $(PKGDIR)/$(PLCSNMP_AGENT)
	tar -xzvf $(PLCSNMP_AGENT_PLATFORMCONFIGPACKAGEDIR)/$(PLCSNMP_AGENT).tgz -C $(PKGDIR)/$(PLCSNMP_AGENT)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/plcsnmp_agent.targetinstall:
	@$(call targetinfo)

	@$(call install_init, plcsnmp_agent)
	@$(call install_fixup, plcsnmp_agent, PRIORITY, optional)
	@$(call install_fixup, plcsnmp_agent, SECTION, base)
	@$(call install_fixup, plcsnmp_agent, AUTHOR, "<HFS>")
	@$(call install_fixup, plcsnmp_agent, DESCRIPTION, missing)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(PLCSNMP_AGENT_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(PLCSNMP_AGENT_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(PLCSNMP_AGENT_PLATFORMCONFIGPACKAGEDIR)/$(PLCSNMP_AGENT_PACKAGE_NAME).ipk  
	@$(call install_archive, plcsnmp_agent, 0, 0, $(PLCSNMP_AGENT_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE	

#	#
#	# example code:; copy all libraries, links and binaries
#	#

	@for i in $(shell cd $(PLCSNMP_AGENT_PKGDIR) && find bin sbin usr/bin usr/sbin -type f); do \
		$(call install_copy, plcsnmp_agent, 0, 0, 0755, -, /$$i); \
	done
	@for i in $(shell cd $(PLCSNMP_AGENT_PKGDIR) && find lib usr/lib -name "*.so*"); do \
		$(call install_copy, plcsnmp_agent, 0, 0, 0644, -, /$$i); \
	done
	@links="$(shell cd $(PLCSNMP_AGENT_PKGDIR) && find lib usr/lib -type l)" && \
	if [ -n "$$links" ]; then \
		for i in $$links; do \
			from="`readlink $(PLCSNMP_AGENT_PKGDIR)/$$i`"; \
			to="/$$i"; \
			$(call install_link, plcsnmp_agent, $$from, $$to); \
		done; \
	fi
	@$(call install_link, plcsnmp_agent, ../libplcsnmp_agent.so, /usr/lib/wide/libplcsnmp_agent.so)

#	#
#	# FIXME: add all necessary things here
#	#
endif
	@$(call install_finish, plcsnmp_agent)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(PLCSNMP_AGENT_PACKAGE_NAME).ipk $(PLCSNMP_AGENT_PLATFORMCONFIGPACKAGEDIR)/
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/plcsnmp_agent.clean:
	@$(call targetinfo)
	@$(call clean_pkg, PLCSNMP_AGENT)
	@-rm -rf $(PLCSNMP_AGENT_DIR)

# vim: syntax=make
