# -*-makefile-*-
#
# Copyright (C) 2012 by <AGa,JSo>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBLIBLOADER) += liblibloader

#
# Paths and names
#
LIBLIBLOADER_VERSION	:= 1.0
LIBLIBLOADER			:= LibraryLoader
LIBLIBLOADER_SUFFIX		:= 
LIBLIBLOADER_URL		:= 
#LIBLIBLOADER_SOURCE		:= $(SRCDIR)/$(LIBLIBLOADER)
LIBLIBLOADER_DIR		:= $(BUILDDIR)/$(LIBLIBLOADER)
LIBLIBLOADER_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(LIBLIBLOADER_SOURCE):
#	@$(call targetinfo)
#	@$(call get, LIBLIBLOADER)
	
# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------
$(STATEDIR)/liblibloader.extract:
	$(call targetinfo)
	mkdir -p $(LIBLIBLOADER_DIR)
	rsync -a --exclude=".*" \
			--exclude=project/objs/ \
			--exclude=project/objs_test/ \
			--exclude=lib \
			--exclude=project/lib_test\
			 $(SRCDIR)/$(LIBLIBLOADER) $(BUILDDIR)
	@$(call touch)
	

#-----------------------------------------------------------------------------
# TODO: Fix absolute path to cpputest. Should be CPPUTEST_HOME=$(HOST_CPPUTEST_DIR), 
# but for reasons unknown $(HOST_CPPUTEST_DIR) is empty (although its not empty for the dal rules file).
# HOST_CPPUTEST_DIR is set by the rules file host_cpputest.make.
#-----------------------------------------------------------------------------		
LIBLOADER_TEST_ENV:= $(HOST_ENV) CPPUTEST_HOME=$(HOST_BUILDDIR)/cpputest-3.1 SYSROOT=$(PTXCONF_SYSROOT_HOST) PATH=$(HOST_PATH)

LIBTOOL_CROSS:=$(PTXCONF_SYSROOT_CROSS)/bin/
LIBLIBLOADER_ENV:= $(CROSS_ENV) SYSROOT=$(PTXCONF_SYSROOT_TARGET) CROSS_COMPILE=$(COMPILER_PREFIX) CROSS_LIBTOOL=$(LIBTOOL_CROSS)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	LIBLIBLOADER_ENV+=CFLAGS="$(CROSS_CFLAGS) -DNDEBUG"
endif

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/liblibloader.compile:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_TRUNK
#	 Unit tests
	cd $(LIBLIBLOADER_DIR) && $(LIBLOADER_TEST_ENV) $(MAKE) -f tests/Makefile $(PARALLELMFLAGS)
endif
	
#	 build library
	cd $(LIBLIBLOADER_DIR) && $(LIBLIBLOADER_ENV) $(MAKE) -f MakefileRelease.mk $(PARALLELMFLAGS)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/liblibloader.install:
	@$(call targetinfo)
#	@$(call world/install, LIBLIBLOADER)
	cp $(LIBLIBLOADER_DIR)/include/LibraryLoader.h $(PTXCONF_SYSROOT_TARGET)/usr/include/LibraryLoader.h
	cp $(LIBLIBLOADER_DIR)/lib/.libs/lib* $(PTXCONF_SYSROOT_TARGET)/usr/lib/
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/liblibloader.targetinstall:
	@$(call targetinfo)

	@$(call install_init, liblibloader)
	@$(call install_fixup, liblibloader,PRIORITY,optional)
	@$(call install_fixup, liblibloader,SECTION,base)
	@$(call install_fixup, liblibloader,AUTHOR,"<AGa,JSo>")
	@$(call install_fixup, liblibloader,DESCRIPTION,missing)

	cp $(LIBLIBLOADER_DIR)/lib/.libs/lib* $(PTXCONF_SYSROOT_TARGET)/usr/lib/
	@$(call install_copy, liblibloader, 0, 0, 0755, $(LIBLIBLOADER_DIR)/lib/.libs/liblibloader.so.0.0.0, /usr/lib/liblibloader.so.0.0.0)
	
	@$(call install_link, liblibloader, liblibloader.so.0.0.0, /usr/lib/liblibloader.so.0)
	@$(call install_link, liblibloader, liblibloader.so.0.0.0, /usr/lib/liblibloader.so)

	@$(call install_finish, liblibloader)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/liblibloader.clean:
	@$(call targetinfo)
	rm -rf $(STATEDIR)/liblibloader.*
	rm -rf $(PKGDIR)/liblibloader_*
	rm -rf $(LIBLIBLOADER_DIR)
	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/liblibloader*
	@$(call clean_pkg, LIBLIBLOADER)

# vim: syntax=make
