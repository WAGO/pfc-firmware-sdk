# -*-makefile-*-
#
# Copyright (C) 2019 by WAGO Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_NTFS_3G) += ntfs-3g

#
# Paths and names
#
NTFS_3G_VERSION	:= 2017.3.23
NTFS_3G_MD5		:= d97474ae1954f772c6d2fa386a6f462c
NTFS_3G		    := ntfs-3g_ntfsprogs-$(NTFS_3G_VERSION)
NTFS_3G_SUFFIX  := tgz
NTFS_3G_URL		:= http://svsv01003.wago.local/wago-ptxdist-src/$(NTFS_3G).$(NTFS_3G_SUFFIX)
NTFS_3G_SOURCE		:= $(SRCDIR)/$(NTFS_3G).$(NTFS_3G_SUFFIX)
NTFS_3G_DIR		:= $(BUILDDIR)/$(NTFS_3G)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/ntfs-3g.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(NTFS_3G_DIR))
	@$(call extract, NTFS_3G)
	@$(call patchin, NTFS_3G)
	cd $(NTFS_3G_DIR) && [ -f configure ] || sh autogen.sh
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#NTFS_3G_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
NTFS_3G_CONF_TOOL	:= autoconf
NTFS_3G_CONF_OPT	:= $(CROSS_AUTOCONF_ROOT) --disable-ldconfig --disable-ntfsprogs

$(STATEDIR)/ntfs-3g.prepare:
	@$(call targetinfo)
	@$(call world/prepare, NTFS_3G)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/ntfs-3g.compile:
#	@$(call targetinfo)
#	@$(call world/compile, NTFS_3G)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/ntfs-3g.install:
#	@$(call targetinfo)
#	@$(call world/install, NTFS_3G)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ntfs-3g.targetinstall:
	@$(call targetinfo)

	@$(call install_init, ntfs-3g)
	@$(call install_fixup, ntfs-3g, PRIORITY, optional)
	@$(call install_fixup, ntfs-3g, SECTION, base)
	@$(call install_fixup, ntfs-3g, AUTHOR, "<>")
	@$(call install_fixup, ntfs-3g, DESCRIPTION, missing)

	@$(call install_copy, ntfs-3g, 0, 0, 0755, $(NTFS_3G_DIR)/src/ntfs-3g.probe, /bin/ntfs-3g.probe, y)
# Executables usermap and secaudit now part of ntfsprogs (currently a disabled option)
#	@$(call install_copy, ntfs-3g, 0, 0, 0755, $(NTFS_3G_DIR)/src/ntfs-3g.usermap, /bin/ntfs-3g.usermap, y)
#	@$(call install_copy, ntfs-3g, 0, 0, 0755, $(NTFS_3G_DIR)/src/ntfs-3g.secaudit, /bin/ntfs-3g.secaudit, y)
	@$(call install_copy, ntfs-3g, 0, 0, 0755, $(NTFS_3G_DIR)/src/ntfs-3g, /bin/ntfs-3g, y)
	@$(call install_copy, ntfs-3g, 0, 0, 0755, $(NTFS_3G_DIR)/src/lowntfs-3g, /bin/lowntfs-3g, y)
	@$(call install_copy, ntfs-3g, 0, 0, 0755, $(NTFS_3G_DIR)/libntfs-3g/.libs/libntfs-3g.so.88.0.0, /lib/libntfs-3g.so.88.0.0, y)

	@$(call install_link, ntfs-3g, libntfs-3g.so.88.0.0, /lib/libntfs-3g.so.88)
	@$(call install_link, ntfs-3g, libntfs-3g.so.88.0.0, /lib/libntfs-3g.so)
	@$(call install_link, ntfs-3g, ../bin/ntfs-3g, /sbin/mount.ntfs-3g)
	@$(call install_link, ntfs-3g, ../bin/lowntfs-3g, /sbin/mount.lowntfs-3g)

	@$(call install_finish, ntfs-3g)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/ntfs-3g.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, NTFS_3G)

# vim: syntax=make
