# -*-makefile-*-
#
# Copyright (C) 2012 by Jan Sondhauss <jan.sondhauss@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CIFXTOOLKIT) += cifxtoolkit

#
# Paths and names
#
CIFXTOOLKIT_VERSION	:= 1.2.0.0
CIFXTOOLKIT_SHORT_VERSION := 1
CIFXTOOLKIT			:= cifxtoolkit-$(CIFXTOOLKIT_VERSION)
CIFXTOOLKIT_SRC_DIR	:= $(SRCDIR)/$(CIFXTOOLKIT)
CIFXTOOLKIT_DIR		:= $(BUILDDIR)/$(CIFXTOOLKIT)
CIFXTOOLKIT_LICENSE	:= Hilscher Open Source License
CIFXTOOLKIT_ENV     := $(CROSS_ENV) CIFXTOOLKIT_VERSION=$(CIFXTOOLKIT_VERSION)
# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cifxtoolkit.extract:
	@$(call targetinfo)
	@mkdir -p $(CIFXTOOLKIT_DIR)
	@rsync -a --exclude=".*" --exclude=objs/ --exclude=.lib/ --exclude="*.d" --exclude="*.o"  $(CIFXTOOLKIT_SRC_DIR) $(BUILDDIR)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

CIFXTOOLKIT_PATH	:= PATH=$(CROSS_PATH)

$(STATEDIR)/cifxtoolkit.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/cifxtoolkit.compile:
	@$(call targetinfo)
	cd $(CIFXTOOLKIT_DIR) && $(CIFXTOOLKIT_ENV) $(MAKE) $(PARALLELMFLAGS)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------
CIFX_INCLUDES:= $(CIFXTOOLKIT_SRC_DIR)/Source
CIFX_LIBS:= $(CIFXTOOLKIT_DIR)/lib
SYSROOT_CIFX_INCLUDES:= $(PTXCONF_SYSROOT_TARGET)/usr/include/cifx
SYSROOT_CIFX_LIBS:= $(PTXCONF_SYSROOT_TARGET)/usr/lib

$(STATEDIR)/cifxtoolkit.install:
	@$(call targetinfo)
	@mkdir -p $(SYSROOT_CIFX_INCLUDES)
	@ln -sf $(SYSROOT_CIFX_INCLUDES) $(PTXCONF_SYSROOT_HOST)/include
	cp $(CIFXTOOLKIT_DIR)/OSAbstraction/*.h $(SYSROOT_CIFX_INCLUDES)
	cp $(CIFX_INCLUDES)/*.h  $(SYSROOT_CIFX_INCLUDES)
	cp $(CIFX_LIBS)/* $(SYSROOT_CIFX_LIBS)
	cd $(SYSROOT_CIFX_LIBS) && ln -sf libcifXToolkit.so.$(CIFXTOOLKIT_VERSION) libcifXToolkit.so.$(CIFXTOOLKIT_SHORT_VERSION)
	cd $(SYSROOT_CIFX_LIBS) && ln -sf libcifXToolkit.so.$(CIFXTOOLKIT_VERSION) libcifXToolkit.so

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cifxtoolkit.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cifxtoolkit)
	@$(call install_fixup, cifxtoolkit,PRIORITY,optional)
	@$(call install_fixup, cifxtoolkit,SECTION,base)
	@$(call install_fixup, cifxtoolkit,AUTHOR,"Jan Sondhauss <jan.sondhauss@wago.com>")
	@$(call install_fixup, cifxtoolkit,DESCRIPTION,missing)

	@$(call install_copy, cifxtoolkit, 0, 0, 0755, $(CIFX_LIBS)/libcifXToolkit.so.$(CIFXTOOLKIT_VERSION), /usr/lib/libcifXToolkit.so.$(CIFXTOOLKIT_VERSION))
	@$(call install_link, cifxtoolkit, libcifXToolkit.so.$(CIFXTOOLKIT_VERSION), /usr/lib/libcifXToolkit.so)
	@$(call install_link, cifxtoolkit, libcifXToolkit.so.$(CIFXTOOLKIT_VERSION), /usr/lib/libcifXToolkit.so.$(CIFXTOOLKIT_SHORT_VERSION))

	@$(call install_finish, cifxtoolkit)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cifxtoolkit.clean:
	rm -rf $(SYSROOT_CIFX_INCLUDES)
	rm -f $(STATEDIR)/cifxtoolkit.*
	rm -rf $(PKGDIR)/cifxtoolkit*
	rm -f $(SYSROOT_CIFX_LIBS)/libcifXToolkit*
	rm -rf $(CIFXTOOLKIT_DIR)

# vim: syntax=make
