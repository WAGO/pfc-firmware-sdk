# -*-makefile-*-
#
# Copyright (C) 2003-2006 by Pengutronix e.K., Hildesheim, Germany
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_NFSUTILS) += nfsutils

#
# Paths and names
#
NFSUTILS_VERSION	:= 2.2.1
NFSUTILS_MD5		:= 6e7e0dc10664ac40bd84df1a881bad85
NFSUTILS		:= nfs-utils-$(NFSUTILS_VERSION)
NFSUTILS_SUFFIX		:= tar.bz2
NFSUTILS_URL		:= $(call ptx/mirror, SF, nfs/$(NFSUTILS).$(NFSUTILS_SUFFIX))
NFSUTILS_SOURCE		:= $(SRCDIR)/$(NFSUTILS).$(NFSUTILS_SUFFIX)
NFSUTILS_DIR		:= $(BUILDDIR)/$(NFSUTILS)
NFSUTILS_LICENSE	:= GPL-2.0+

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

NFSUTILS_CONF_ENV	:= \
	$(CROSS_ENV) \
	knfsd_cv_bsd_signals=no

#
# autoconf
#
NFSUTILS_CONF_TOOL	:= autoconf
NFSUTILS_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--exec-prefix=/usr \
	--disable-nfsv4 \
	--disable-nfsv41 \
	--disable-gss \
	--disable-svcgss \
	--disable-kprefix \
	--disable-uuid \
	--$(call ptx/endis, PTXCONF_NFSUTILS_CLIENT)-mount \
	--$(call ptx/endis, PTXCONF_NFSUTILS_CLIENT)-libmount-mount \
	--$(call ptx/endis, PTXCONF_GLOBAL_IPV6)-tirpc \
	$(GLOBAL_IPV6_OPTION) \
	--disable-mountconfig \
	--disable-nfsdcltrack \
	--enable-osdlogin \
	--disable-caps \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--with-statedir=/var/lib/nfs \
	--with-statdpath=/var/lib/nfs \
	--with-statduser=rpcuser \
	--with-systemd=/usr/lib/systemd/system \
	--with-rpcgen=internal \
	--without-mountfile \
	--without-tcp-wrappers \
	--without-krb5 \
	--without-gssglue

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/nfsutils.targetinstall:
	@$(call targetinfo)

	@$(call install_init, nfsutils)
	@$(call install_fixup, nfsutils,PRIORITY,optional)
	@$(call install_fixup, nfsutils,SECTION,base)
	@$(call install_fixup, nfsutils,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, nfsutils,DESCRIPTION,missing)

	@$(call install_copy, nfsutils, 0, 0, 0755, -, /usr/sbin/nfsstat)
	@$(call install_copy, nfsutils, 0, 0, 0755, -, /usr/sbin/nfsiostat)
	@$(call install_copy, nfsutils, 0, 0, 0755, -, /usr/sbin/showmount)

	@$(call install_copy, nfsutils, 0, 0, 0755, -, /usr/sbin/rpc.statd)
	@$(call install_copy, nfsutils, 0, 0, 0755, -, /usr/sbin/start-statd)
	@$(call install_copy, nfsutils, 0, 0, 0755, -, /usr/sbin/sm-notify)

	@$(call install_copy, nfsutils, rpcuser, 0, 0755, \
		/var/lib/nfs)

ifdef PTXCONF_NFSUTILS_SYSTEMD_UNIT
	@$(call install_alternative, nfsutils, 0, 0, 0644, \
		/usr/lib/tmpfiles.d/nfs.conf)
else
	@$(call install_copy, nfsutils, rpcuser, 0, 0644, -, \
		/var/lib/nfs/etab)
	@$(call install_copy, nfsutils, rpcuser, 0, 0644, -, \
		/var/lib/nfs/rmtab)
	@$(call install_copy, nfsutils, rpcuser, 0, 0644, -, \
		/var/lib/nfs/state)
	@$(call install_copy, nfsutils, rpcuser, 0, 0755, \
		/var/lib/nfs/sm)
	@$(call install_copy, nfsutils, rpcuser, 0, 0755, \
		/var/lib/nfs/sm.bak)
endif

ifdef PTXCONF_NFSUTILS_CLIENT
	@$(call install_copy, nfsutils, 0, 0, 0755, -, /sbin/mount.nfs)
	@$(call install_link, nfsutils, mount.nfs, /sbin/umount.nfs)
endif

ifdef PTXCONF_NFSUTILS_SERVER
	@$(call install_copy, nfsutils, 0, 0, 0755, -, /usr/sbin/rpc.nfsd)
	@$(call install_copy, nfsutils, 0, 0, 0755, -, /usr/sbin/rpc.mountd)
	@$(call install_copy, nfsutils, 0, 0, 0755, -, /usr/sbin/exportfs)

	@$(call install_alternative, nfsutils, 0, 0, 0644, \
		/etc/exports)
endif

#	#
#	# busybox init: start scripts
#	#
ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_NFSUTILS_NFSD_STARTSCRIPT
	@$(call install_alternative, nfsutils, 0, 0, 0755, /etc/init.d/nfsd)

ifneq ($(call remove_quotes,$(PTXCONF_NFSUTILS_NFSD_BBINIT_LINK)),)
	@$(call install_link, nfsutils, \
		../init.d/nfsd, \
		/etc/rc.d/$(PTXCONF_NFSUTILS_NFSD_BBINIT_LINK))
endif
endif
endif

ifdef PTXCONF_NFSUTILS_SYSTEMD_UNIT
	@$(call install_alternative, nfsutils, 0, 0, 0644, \
		/usr/lib/systemd/system/rpc-statd.service)
	@$(call install_alternative, nfsutils, 0, 0, 0644, \
		/usr/lib/systemd/system/rpc-statd-notify.service)
	@$(call install_alternative, nfsutils, 0, 0, 0644, \
		/usr/lib/systemd/system/var-lib-nfs.mount)
	@$(call install_link, nfsutils, ../var-lib-nfs.mount, \
		/usr/lib/systemd/system/local-fs.target.requires/var-lib-nfs.mount)

ifdef PTXCONF_NFSUTILS_CLIENT
	@$(call install_alternative, nfsutils, 0, 0, 0644, \
		/usr/lib/systemd/system/nfs-client.target)
	@$(call install_link, nfsutils, ../nfs-client.target, \
		/usr/lib/systemd/system/multi-user.target.wants/nfs-client.target)
	@$(call install_link, nfsutils, ../nfs-client.target, \
		/usr/lib/systemd/system/remote-fs.target.wants/nfs-client.target)
endif
ifdef PTXCONF_NFSUTILS_SERVER
	@$(call install_alternative, nfsutils, 0, 0, 0644, \
		/usr/lib/systemd/system/proc-fs-nfsd.mount)
	@$(call install_alternative, nfsutils, 0, 0, 0644, \
		/usr/lib/systemd/system/nfs-mountd.service)
	@$(call install_alternative, nfsutils, 0, 0, 0644, \
		/usr/lib/systemd/system/nfs-server.service)
	@$(call install_link, nfsutils, ../nfs-server.service, \
		/usr/lib/systemd/system/multi-user.target.wants/nfs-server.service)
endif
endif
	@$(call install_finish, nfsutils)
	@$(call touch)

# vim: syntax=make
