# -*-makefile-*-
#
# Copyright (C) 2012 by <AGa,JSo>
#
# See CREDITS for details about who has contributed to this project.i
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOSTLIBLOADER) += hostlibloader

#
# Paths and names
#
HOSTLIBLOADER_VERSION	:= 1.0
HOSTLIBLOADER			:= LibraryLoader
HOSTLIBLOADER_SUFFIX		:= 
HOSTLIBLOADER_URL		:= 
HOSTLIBLOADER_SOURCE		:= $(SRCDIR)/$(HOSTLIBLOADER)
HOSTLIBLOADER_DIR		:= $(HOST_BUILDDIR)/$(HOSTLIBLOADER)
HOSTLIBLOADER_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(HOSTLIBLOADER_SOURCE):
	@$(call targetinfo)
	@$(call touch)

$(STATEDIR)/hostlibloader.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------
$(STATEDIR)/hostlibloader.extract: $(STATEDIR)/autogen-tools
	$(call targetinfo)
	mkdir -p $(HOSTLIBLOADER_DIR)
	rsync -a --exclude=".*" \
			--exclude=project/objs/ \
			--exclude=project/objs_test/ \
			--exclude=lib \
			--exclude=project/lib_test\
			 $(HOSTLIBLOADER_SOURCE)/* $(HOSTLIBLOADER_DIR)/
	@$(call touch)
	

#-----------------------------------------------------------------------------
# TODO: Fix path to cpputest. Should be CPPUTEST_HOME=$(HOST_CPPUTEST_HOME), 
# but for reasons unknown $(HOST_CPPUTEST_HOME) is empty (although its not empty for the dal rules file).
# HOST_CPPUTEST_HOME is set by the rules file host_cpputest.make.
#-----------------------------------------------------------------------------		
HOSTLIBLOADER_ENV:= $(HOST_ENV) CPPUTEST_HOME=$(PTXCONF_SYSROOT_HOST)/usr/local SYSROOT=$(PTXCONF_SYSROOT_HOST) PATH=$(HOST_PATH)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/hostlibloader.compile:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_TRUNK
#	 Unit tests
	cd $(HOSTLIBLOADER_DIR) && $(HOSTLIBLOADER_ENV) $(MAKE) -f tests/Makefile $(PARALLELMFLAGS)
endif	
#	 build library
	cd $(HOSTLIBLOADER_DIR) && $(HOSTLIBLOADER_ENV) $(MAKE) -f MakefileRelease.mk $(PARALLELMFLAGS)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/hostlibloader.install:
	@$(call targetinfo)
#	@$(call world/install, hostlibloader)
	cp $(HOSTLIBLOADER_DIR)/include/LibraryLoader.h $(PTXCONF_SYSROOT_HOST)/include/LibraryLoader.h
	cp $(HOSTLIBLOADER_DIR)/lib/.libs/lib* $(PTXCONF_SYSROOT_HOST)/lib/
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------
# Host, image and cross packages don’t need to install anything in the target
# file system. Therefore, PTXdist only respects the targetinstall and
# targetinstall.post stages for packages whose name doesn’t start with host-,
# image-, or cross-.

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/hostlibloader.clean:
	@$(call targetinfo)
	rm -rf $(STATEDIR)/hostlibloader.*
	rm -rf $(PKGDIR)/hostlibloader_*
	rm -rf $(HOSTLIBLOADER_DIR)
	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/hostlibloader*
	@$(call clean_pkg, hostlibloader)

# vim: syntax=make
