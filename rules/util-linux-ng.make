# -*-makefile-*-
#
# Copyright (C) 2008 by Robert Schwebel
#               2010 by Marc Kleine-Budde <mkl@penutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_UTIL_LINUX_NG) += util-linux-ng

#
# Paths and names
#
UTIL_LINUX_NG_VERSION	:= 2.36
UTIL_LINUX_NG_MD5	:= fe7c0f7e439f08970e462c9d44599903
UTIL_LINUX_NG		:= util-linux-$(UTIL_LINUX_NG_VERSION)
UTIL_LINUX_NG_SUFFIX	:= tar.xz
UTIL_LINUX_NG_BASENAME	:= v$(call ptx/sh, echo $(UTIL_LINUX_NG_VERSION) | sed -e 's/\([0-9]*\.[0-9]*\)[^0-9].*\?/\1/g')
UTIL_LINUX_NG_URL	:= $(call ptx/mirror, KERNEL, utils/util-linux/$(UTIL_LINUX_NG_BASENAME)/$(UTIL_LINUX_NG).$(UTIL_LINUX_NG_SUFFIX))
UTIL_LINUX_NG_SOURCE	:= $(SRCDIR)/$(UTIL_LINUX_NG).$(UTIL_LINUX_NG_SUFFIX)
UTIL_LINUX_NG_DIR	:= $(BUILDDIR)/$(UTIL_LINUX_NG)
UTIL_LINUX_NG_LICENSE	:= GPL-2.0-only AND GPL-2.0-or-later AND LGPL-2.0-or-later AND BSD-3-Clause AND BSD-4-Clause AND public_domain
UTIL_LINUX_NG_LICENSE_FILES := \
	file://Documentation/licenses/COPYING.GPL-2.0-or-later;md5=b234ee4d69f5fce4486a80fdaf4a4263 \
	file://Documentation/licenses/COPYING.BSD-3-Clause;md5=58dcd8452651fc8b07d1f65ce07ca8af \
	file://Documentation/licenses/COPYING.BSD-4-Clause-UC;md5=263860f8968d8bafa5392cab74285262 \
	file://Documentation/licenses/COPYING.LGPL-2.1-or-later;md5=4fbd65380cdd255951079008b364516c

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

UTIL_LINUX_NG_CONF_ENV	:= \
	$(CROSS_ENV) \
	$(call ptx/ncurses, PTXCONF_UTIL_LINUX_NG_USES_NCURSES) \
	scanf_cv_type_modifier=as \
	ac_cv_path_BLKID=no \
	ac_cv_path_PERL=no \
	ac_cv_path_VOLID=no

#
# autoconf
#
UTIL_LINUX_NG_CONF_TOOL	:= autoconf
UTIL_LINUX_NG_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--bindir=/usr/bin \
	--sbindir=/usr/sbin \
	--disable-werror \
	--disable-asan \
	--disable-ubsan \
	--enable-shared \
	--disable-static \
	--enable-symvers \
	--disable-gtk-doc \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--enable-assert \
	--disable-nls \
	--disable-rpath \
	--disable-static-programs \
	--enable-all-programs=undefined \
	--enable-tls \
	--disable-widechar \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_LIBUUID)-libuuid \
	--disable-libuuid-force-uuidd \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_LIBBLKID)-libblkid \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_LIBMOUNT)-libmount \
	--disable-libmount-support-mtab \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_LIBSMARTCOLS)-libsmartcols \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_LIBFDISK)-libfdisk \
	$(call ptx/ifdef, PTXCONF_UTIL_LINUX_NG_FDISKS,,--disable-fdisks) \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_MOUNT)-mount \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_LOSETUP)-losetup \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_ZRAMCTL)-zramctl \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_FSCK)-fsck \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_PARTX_TOOLS)-partx \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_UUIDD)-uuidd \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_MOUNTPOINT)-mountpoint \
	--disable-fallocate \
	--disable-unshare \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_NSENTER)-nsenter \
	--disable-setpriv \
	--disable-hardlink \
	--disable-eject \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_AGETTY)-agetty \
	--disable-plymouth_support \
	--disable-cramfs \
	--disable-bfs \
	--disable-minix \
	--disable-fdformat \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_HWCLOCK)-hwclock \
	--disable-lslogins \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_WDCTL)-wdctl \
	--disable-cal \
	--disable-logger \
	--disable-switch_root \
	--disable-pivot_root \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_LSMEM)-lsmem \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_CHMEM)-chmem \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_IPCRM)-ipcrm \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_IPCS)-ipcs \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_IRQTOP)-irqtop \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_LSIRQ)-lsirq \
	--disable-rfkill \
	--disable-tunelp \
	--disable-kill \
	--disable-last \
	--disable-utmpdump \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_LINE)-line \
	--disable-mesg \
	--disable-raw \
	--disable-rename \
	--disable-vipw \
	--disable-newgrp \
	--disable-chfn-chsh-password \
	--disable-chfn-chsh \
	--disable-chsh-only-listed \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_LOGIN)-login \
	--disable-login-chown-vcs \
	--disable-login-stat-mail \
	--disable-nologin \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_SULOGIN)-sulogin \
	--disable-su \
	--disable-runuser \
	--disable-ul \
	--disable-more \
	--disable-pg \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_SETTERM)-setterm \
	--$(call ptx/endis, PTXCONF_UTIL_LINUX_NG_SCHEDUTILS)-schedutils \
	--disable-wall \
	--disable-write \
	--disable-bash-completion \
	--disable-pylibmount \
	--disable-pg-bell \
	--disable-use-tty-group \
	--disable-sulogin-emergency-mount \
	--disable-usrdir-path \
	--disable-makeinstall-chown \
	--disable-makeinstall-setuid \
	--disable-colors-default \
	--without-libiconv-prefix \
	--without-libintl-prefix \
	--with-util \
	--without-selinux \
	--without-audit \
	--without-udev \
	--without-ncursesw \
	--$(call ptx/wwo, PTXCONF_UTIL_LINUX_NG_USES_NCURSES)-ncurses \
	--without-slang \
	--without-tinfo \
	--without-readline \
	--without-utempter \
	--without-cap-ng \
	--without-libmagic \
	--without-libz \
	--without-user \
	--without-btrfs \
	--without-systemd \
	--with-systemdsystemunitdir=/usr/lib/systemd/system \
	--without-smack \
	--without-econf \
	--without-python

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/util-linux-ng.targetinstall:
	@$(call targetinfo)

	@$(call install_init, util-linux-ng)
	@$(call install_fixup, util-linux-ng,PRIORITY,optional)
	@$(call install_fixup, util-linux-ng,SECTION,base)
	@$(call install_fixup, util-linux-ng,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, util-linux-ng,DESCRIPTION,missing)

ifdef PTXCONF_UTIL_LINUX_NG_LIBUUID
	@$(call install_lib, util-linux-ng, 0, 0, 0644, libuuid)
endif
ifdef PTXCONF_UTIL_LINUX_NG_LIBBLKID
	@$(call install_lib, util-linux-ng, 0, 0, 0644, libblkid)
endif
ifdef PTXCONF_UTIL_LINUX_NG_LIBMOUNT
	@$(call install_lib, util-linux-ng, 0, 0, 0644, libmount)
endif
ifdef PTXCONF_UTIL_LINUX_NG_LIBSMARTCOLS
	@$(call install_lib, util-linux-ng, 0, 0, 0644, libsmartcols)
endif
ifdef PTXCONF_UTIL_LINUX_NG_LIBFDISK
	@$(call install_lib, util-linux-ng, 0, 0, 0644, libfdisk)
endif

ifdef PTXCONF_UTIL_LINUX_NG_LSMEM
	@$(call install_copy, util-linux-ng, root, root, 0755, -, /usr/bin/lsmem)
endif
ifdef PTXCONF_UTIL_LINUX_NG_CHMEM
	@$(call install_copy, util-linux-ng, root, root, 0755, -, /usr/bin/chmem)
endif
ifdef PTXCONF_UTIL_LINUX_NG_COLUMN
	@$(call install_copy, util-linux-ng, root, root, 0755, -, /usr/bin/column)
endif
ifdef PTXCONF_UTIL_LINUX_NG_LINE
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/line)
endif
ifdef PTXCONF_UTIL_LINUX_NG_GETOPT
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/getopt)
endif
ifdef PTXCONF_UTIL_LINUX_NG_DMESG
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/dmesg)
endif
ifdef PTXCONF_UTIL_LINUX_NG_MOUNTPOINT
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/mountpoint)
endif
ifdef PTXCONF_UTIL_LINUX_NG_NSENTER
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/nsenter)
endif
ifdef PTXCONF_UTIL_LINUX_NG_ADDPART
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/addpart)
endif
ifdef PTXCONF_UTIL_LINUX_NG_DELPART
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/delpart)
endif
ifdef PTXCONF_UTIL_LINUX_NG_PARTX
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/partx)
endif
ifdef PTXCONF_UTIL_LINUX_NG_AGETTY
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/agetty)
endif
ifdef PTXCONF_UTIL_LINUX_NG_MKSWAP
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/mkswap)
endif
ifdef PTXCONF_UTIL_LINUX_NG_SWAPON
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/swapon)
	@$(call install_link, util-linux-ng, swapon, /usr/sbin/swapoff)
endif
ifdef PTXCONF_UTIL_LINUX_NG_LOSETUP
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/losetup)
endif
ifdef PTXCONF_UTIL_LINUX_NG_MOUNT
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/mount)
endif
ifdef PTXCONF_UTIL_LINUX_NG_UMOUNT
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/umount)
endif
ifdef PTXCONF_UTIL_LINUX_NG_FLOCK
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/flock)
endif
ifdef PTXCONF_UTIL_LINUX_NG_FSCK
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/fsck)
endif
ifdef PTXCONF_UTIL_LINUX_NG_FSFREEZE
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/fsfreeze)
endif
ifdef PTXCONF_UTIL_LINUX_NG_FSTRIM
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/fstrim)
endif
ifdef PTXCONF_UTIL_LINUX_NG_IPCS
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/ipcs)
endif
ifdef PTXCONF_UTIL_LINUX_NG_IRQTOP
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/irqtop)
endif
ifdef PTXCONF_UTIL_LINUX_NG_LSIRQ
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/lsirq)
endif
ifdef PTXCONF_UTIL_LINUX_NG_IPCRM
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/ipcrm)
endif
ifdef PTXCONF_UTIL_LINUX_NG_READPROFILE
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/readprofile)
endif
ifdef PTXCONF_UTIL_LINUX_NG_FDISK
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/fdisk)
endif
ifdef PTXCONF_UTIL_LINUX_NG_SFDISK
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/sfdisk)
endif
ifdef PTXCONF_UTIL_LINUX_NG_CFDISK
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/cfdisk)
endif
ifdef PTXCONF_UTIL_LINUX_NG_SETTERM
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/setterm)
endif
ifdef PTXCONF_UTIL_LINUX_NG_CHRT
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/chrt)
endif
ifdef PTXCONF_UTIL_LINUX_NG_HWCLOCK
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/hwclock)
endif
ifdef PTXCONF_UTIL_LINUX_NG_WDCTL
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/wdctl)
endif
ifdef PTXCONF_UTIL_LINUX_NG_IONICE
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/ionice)
endif
ifdef PTXCONF_UTIL_LINUX_NG_TASKSET
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/taskset)
endif
ifdef PTXCONF_UTIL_LINUX_NG_MCOOKIE
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/mcookie)
endif
ifdef PTXCONF_UTIL_LINUX_NG_LDATTACH
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/ldattach)
endif
ifdef PTXCONF_UTIL_LINUX_NG_BLKID
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/blkid)
endif
ifdef PTXCONF_UTIL_LINUX_NG_LSBLK
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/lsblk)
endif
ifdef PTXCONF_UTIL_LINUX_NG_UUIDD
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/uuidd)
endif
ifdef PTXCONF_UTIL_LINUX_NG_UUIDGEN
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/uuidgen)
endif
ifdef PTXCONF_UTIL_LINUX_NG_FINDFS
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/findfs)
endif
ifdef PTXCONF_UTIL_LINUX_NG_WIPEFS
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/wipefs)
endif
ifdef PTXCONF_UTIL_LINUX_NG_ZRAMCTL
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/zramctl)
endif
ifdef PTXCONF_UTIL_LINUX_NG_MKFS
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/mkfs)
endif
ifdef PTXCONF_UTIL_LINUX_NG_LSCPU
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/lscpu)
endif
ifdef PTXCONF_UTIL_LINUX_NG_LOGIN
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/bin/login)
endif
ifdef PTXCONF_UTIL_LINUX_NG_SULOGIN
	@$(call install_copy, util-linux-ng, 0, 0, 0755, -, /usr/sbin/sulogin)
endif

	@$(call install_finish, util-linux-ng)

	@$(call touch)

# vim: syntax=make
