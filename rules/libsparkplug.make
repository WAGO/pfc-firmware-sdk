# -*-makefile-*-
#
# Copyright (C) 2018 by WAGO GmbH
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBSPARKPLUG) += libsparkplug

#
# Paths and names
#
LIBSPARKPLUG_VERSION	:= 0.9a
LIBSPARKPLUG_MD5		:= 38d26264ea4bd27a4e00759c3fccba5e
LIBSPARKPLUG		:= libsparkplug-$(LIBSPARKPLUG_VERSION)
LIBSPARKPLUG_SUFFIX	:= tar.gz
LIBSPARKPLUG_URL		:= https://github.com/Cirrus-Link/Sparkplug/archive/v1.1.8.tar.gz
                           # The referenced archive v1.1.8 produces memory leaks
						   # v0.9 is patched v1.1.8
LIBSPARKPLUG_SOURCE	:= $(SRCDIR)/$(LIBSPARKPLUG).$(LIBSPARKPLUG_SUFFIX)
LIBSPARKPLUG_DIR		:= $(BUILDDIR)/$(LIBSPARKPLUG)
LIBSPARKPLUG_BUILD_C_DIR := $(LIBSPARKPLUG_DIR)/client_libraries/c
LIBSPARKPLUG_LICENSE	 := EPL
LIBSPARKPLUG_PATH        := PATH=$(CROSS_PATH)
LIBSPARKPLUG_MAKE_ENV	 := $(CROSS_ENV)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------
$(LIBSPARKPLUG_SOURCE):
	@$(call targetinfo)
	@$(call get, LIBSPARKPLUG)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------
$(STATEDIR)/libsparkplug.extract:
	@$(call targetinfo)
	@$(call clean, $(LIBSPARKPLUG_DIR))
	@$(call extract, LIBSPARKPLUG)
	@$(call patchin, LIBSPARKPLUG)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------
#$(STATEDIR)/LIBSPARKPLUG.prepare:
#	 @$(call targetinfo)
#	 @$(call prepare, LIBSPARKPLUG)
#	 @$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
$(STATEDIR)/libsparkplug.compile:
	 @$(call targetinfo)
	 cd $(LIBSPARKPLUG_BUILD_C_DIR) && $(MAKE) sparkplug_libs $(LIBSPARKPLUG_MAKE_ENV)
	 @$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------
$(STATEDIR)/libsparkplug.install:
	@$(call targetinfo)

	@mkdir -p $(PKGDIR)/$(LIBSPARKPLUG)/usr/include/libsparkplug
	@install -m 0644 $(LIBSPARKPLUG_BUILD_C_DIR)/include/*.h $(PKGDIR)/$(LIBSPARKPLUG)/usr/include/libsparkplug

	@mkdir -p $(PKGDIR)/$(LIBSPARKPLUG)/usr/lib/libsparkplug
	@install -m 0644 $(LIBSPARKPLUG_BUILD_C_DIR)/lib/libsparkplug_b.so $(PKGDIR)/$(LIBSPARKPLUG)/usr/lib/libsparkplug

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libsparkplug.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libsparkplug)
	@$(call install_fixup, libsparkplug, PRIORITY, optional)
	@$(call install_fixup, libsparkplug, SECTION, base)
	@$(call install_fixup, libsparkplug, AUTHOR, "WAGO GmbH")
	@$(call install_fixup, libsparkplug, DESCRIPTION, missing)
	@$(call install_copy, libsparkplug, 0, 0, 0755, $(LIBSPARKPLUG_BUILD_C_DIR)/lib/libsparkplug_b.so, /usr/lib/libsparkplug_b.so)
	@$(call install_finish, libsparkplug)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------
$(STATEDIR)/libsparkplug.clean:
	@$(call targetinfo)
	@$(call clean_pkg, LIBSPARKPLUG)

# vim: syntax=make
