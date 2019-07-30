# -*-makefile-*-
#
# Copyright (C) 2006 by Robert Schwebel
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_DBUS) += host-dbus

#
# Paths and names
#
HOST_DBUS_DIR	= $(HOST_BUILDDIR)/$(DBUS)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_DBUS_CONF_TOOL	:= autoconf
HOST_DBUS_CONF_OPT	:= \
	$(HOST_AUTOCONF) \
	--enable-silent-rules \
	--disable-developer \
	--disable-debug \
	--enable-largefile \
	--disable-static \
	--disable-ansi \
	--disable-verbose-mode \
	--disable-asserts \
	--disable-checks \
	--disable-xml-docs \
	--disable-doxygen-docs \
	--disable-ducktype-docs \
	--disable-selinux \
	--disable-apparmor \
	--disable-libaudit \
	--disable-inotify \
	--disable-kqueue \
	--disable-console-owner-file \
	--disable-launchd \
	--disable-systemd \
	--disable-embedded-tests \
	--disable-modular-tests \
	--disable-tests \
	--disable-installed-tests \
	--disable-code-coverage \
	--disable-epoll \
	--disable-x11-autolaunch \
	--disable-compile-warnings \
	--disable-Werror \
	--disable-relocation \
	--disable-stats \
	--disable-user-session \
	--without-valgrind \
	--without-x

# vim: syntax=make
