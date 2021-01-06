# -*-makefile-*-
#
# Copyright (C) 2006 by Robert Schwebel
#               2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_E2FSPROGS) += host-e2fsprogs

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_E2FSPROGS_CONF_TOOL	:= autoconf
HOST_E2FSPROGS_CONF_OPT		:= \
	$(HOST_AUTOCONF) \
	--disable-symlink-install \
	--disable-relative-symlinks \
	--disable-symlink-build \
	--disable-verbose-makecmds \
	--disable-elf-shlibs \
	--disable-bsd-shlibs \
	--disable-profile \
	--disable-gcov \
	--disable-hardening \
	--disable-jbd-debug \
	--disable-blkid-debug \
	--disable-testio-debug \
	--enable-libuuid \
	--enable-libblkid \
	--disable-backtrace \
	--disable-debugfs \
	--disable-imager \
	--disable-resizer \
	--disable-defrag \
	--enable-fsck \
	--disable-e2initrd-helper \
	--disable-tls \
	--disable-uuidd \
	--enable-mmp \
	--enable-tdb \
	--disable-bmap-stats \
	--disable-bmap-stats-ops \
	--disable-nls \
	--enable-threads=posix \
	--disable-rpath \
	--disable-fuse2fs \
	--without-included-gettext

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

HOST_E2FSPROGS_INSTALL_OPT	:= install install-libs

$(STATEDIR)/host-e2fsprogs.install.post:
	@$(call targetinfo)
	@$(call world/install.post, HOST_E2FSPROGS)
	@sed -i -e 's,/share,$(PTXCONF_SYSROOT_HOST)/share,' \
		$(PTXCONF_SYSROOT_HOST)/bin/compile_et
	@sed -i -e 's,/share,$(PTXCONF_SYSROOT_HOST)/share,' \
		$(PTXCONF_SYSROOT_HOST)/bin/mk_cmds
	@$(call touch)

# vim: syntax=make
