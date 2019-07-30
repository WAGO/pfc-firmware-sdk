# -*-makefile-*-
#
# Copyright (C) 2011 by Bart vdr. Meulen <bartvdrmeu...@gmail.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_EBTABLES) += ebtables

#
# Paths and names
#
EBTABLES_VERSION       := 2.0.10-4
EBTABLES_MD5           := 506742a3d44b9925955425a659c1a8d0
EBTABLES               := ebtables-v$(EBTABLES_VERSION)
EBTABLES_SUFFIX        := tar.gz
EBTABLES_URL           := $(PTXCONF_SETUP_SFMIRROR)/ebtables/ebtables/ebtables-2-0-10-4/$(EBTABLES).$(EBTABLES_SUFFIX)
EBTABLES_SOURCE        := $(SRCDIR)/$(EBTABLES).$(EBTABLES_SUFFIX)
EBTABLES_DIR           := $(BUILDDIR)/$(EBTABLES)
EBTABLES_LICENSE       := GPLv2

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(EBTABLES_SOURCE):
	@$(call targetinfo)
	@$(call get, EBTABLES)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

EBTABLES_CONF_TOOL     := NO
EBTABLES_MAKE_OPT := \
       $(CROSS_ENV_CFLAGS) \
       $(CROSS_ENV_CPPFLAGS) \
       $(CROSS_ENV_LDFLAGS) \
       $(CROSS_ENV_CC) \
       KERNEL_INCLUDES=$(KERNEL_HEADERS_DIR)/include
EBTABLES_INSTALL_OPT := \
       BINDIR=/usr/sbin install

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ebtables.targetinstall:
	@$(call targetinfo)

	@$(call install_init, ebtables)
	@$(call install_fixup, ebtables,PRIORITY,optional)
	@$(call install_fixup, ebtables,SECTION,base)
	@$(call install_fixup, ebtables,AUTHOR,"Bart vdr. Meulen <bartvdrmeu...@gmail.com>")
	@$(call install_fixup, ebtables,DESCRIPTION,missing)

ifdef PTXCONF_EBTABLES_INSTALL_EBTABLES
	@$(call install_copy, ebtables, 0, 0, 0755, -, \
			/usr/sbin/ebtables)
endif

ifdef PTXCONF_EBTABLES_INSTALL_EBTABLES_SAVE
	@$(call install_copy, ebtables, 0, 0, 0755, -, \
			/usr/sbin/ebtables-save)
endif

ifdef PTXCONF_EBTABLES_INSTALL_EBTABLES_RESTORE
	@$(call install_copy, ebtables, 0, 0, 0755, -, \
			/usr/sbin/ebtables-restore)
endif

ifdef  PTXCONF_EBTABLES_INSTALL_TOOLS
	@$(call install_copy, ebtables, 0, 0, 0644, -, \
			/etc/ethertypes)
	@$(call install_tree, ebtables, 0, 0, \
			$(EBTABLES_PKGDIR)/usr/lib, \
			/usr/lib)
endif

	@$(call install_finish, ebtables)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/ebtables.clean:
#      @$(call targetinfo)
#      @$(call clean_pkg, EBTABLES)

# vim: syntax=make
