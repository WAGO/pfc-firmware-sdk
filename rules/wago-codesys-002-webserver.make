# -*-makefile-*-
#
# Copyright (C) 2013 by <Wago>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PLCLINUXRT_WEBSERVER) += plclinuxrt-webserver

#
# Paths and names
#
PLCLINUXRT_WEBSERVER_VERSION	:= 1.1.9.18
PLCLINUXRT_WEBSERVER			:= webserver-$(PLCLINUXRT_WEBSERVER_VERSION)
PLCLINUXRT_WEBSERVER_URL		:= file://$(PTXDIST_WORKSPACE)/wago_intern/$(PLCLINUXRT_WEBSERVER)
PLCLINUXRT_WEBSERVER_SRC		:= $(PTXDIST_WORKSPACE)/wago_intern/$(PLCLINUXRT_WEBSERVER)
PLCLINUXRT_WEBSERVER_DIR		:= $(BUILDDIR)/$(PLCLINUXRT_WEBSERVER)

PLCLINUXRT_WEBSERVER_PACKAGE_NAME := plclinuxrt-webserver_$(PLCLINUXRT_WEBSERVER_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PLCLINUXRT_WEBSERVER_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/plclinuxrt-webserver.extract:
	@$(call targetinfo)
	@$(call clean, $(PLCLINUXRT_WEBSERVER_DIR))
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	cp -r -d $(PLCLINUXRT_WEBSERVER_SRC) $(PLCLINUXRT_WEBSERVER_DIR)
	@$(call patchin, PLCLINUXRT_WEBSERVER)
endif
	@$(call touch)

# The extract.post target is triggered when patches exist.
# It fails in binary mode and needs to be redefined.
$(STATEDIR)/plclinuxrt-webserver.extract.post:
	@$(call targetinfo)
	@$(call touch)
    

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/plclinuxrt-webserver.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/prepare, PLCLINUXRT_WEBSERVER)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/plclinuxrt-webserver.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/compile, PLCLINUXRT_WEBSERVER)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/plclinuxrt-webserver.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/install, PLCLINUXRT_WEBSERVER)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/plclinuxrt-webserver.targetinstall:
	@$(call targetinfo)
	@$(call install_init, plclinuxrt-webserver)
	@$(call install_fixup, plclinuxrt-webserver, PRIORITY, optional)
	@$(call install_fixup, plclinuxrt-webserver, SECTION, base)
	@$(call install_fixup, plclinuxrt-webserver, AUTHOR, "<Wago>")
	@$(call install_fixup, plclinuxrt-webserver, DESCRIPTION, missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	# Extract precompiled binaries from archive
	rm -rf $(PLCLINUXRT_WEBSERVER_PLATFORMCONFIGPACKAGEDIR)/tmp/*  
	cd $(PLCLINUXRT_WEBSERVER_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(PLCLINUXRT_WEBSERVER_PLATFORMCONFIGPACKAGEDIR)/$(PLCLINUXRT_WEBSERVER_PACKAGE_NAME).ipk  
	@$(call install_archive, plclinuxrt-webserver, 0, 0, $(PLCLINUXRT_WEBSERVER_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_copy, plclinuxrt-webserver, 0, 0, 0644, $(PTXDIST_WORKSPACE)/projectroot/etc/webserver_conf.xml, /etc/webserver_conf.xml)
	@$(call install_copy, plclinuxrt-webserver, 0, 0, 0755, $(PLCLINUXRT_WEBSERVER_DIR)/src/webserver, /usr/sbin/webserver)
	@$(call install_copy, plclinuxrt-webserver, 0, 0, 0755, $(PTXDIST_WORKSPACE)/projectroot/etc/init.d/codesys_webserver, /etc/init.d/codesys_webserver, n)
	@$(call install_copy, plclinuxrt-webserver, 0, 0, 0754, $(PTXDIST_WORKSPACE)/projectroot/etc/init.d/generate_password, /etc/init.d/generate_password, n)
# Disable by default
# TODO: integrate into initmethod-bbinit menu
#	@$(call install_link, plclinuxrt-webserver, ../init.d/codesys_webserver, /etc/rc.d/S56_codesys_webserver)

	@$(call install_copy, plclinuxrt-webserver, 0, 0, 0644, $(PTXDIST_WORKSPACE)/projectroot/var/www/webvisu/webvisu.htm, /var/www/webvisu/webvisu.htm)

endif
	@$(call install_finish, plclinuxrt-webserver)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(PLCLINUXRT_WEBSERVER_PACKAGE_NAME).ipk $(PLCLINUXRT_WEBSERVER_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/plclinuxrt-webserver.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, PLCLINUXRT_WEBSERVER)
# 	@$(call touch)


# vim: syntax=make
