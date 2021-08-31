# -*-makefile-*-
#
# Copyright (C) 2003-2010 by Pengutronix e.K., Hildesheim, Germany
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_COREUTILS) += coreutils

#
# Paths and names
#
COREUTILS_VERSION	:= 8.31
COREUTILS_MD5		:= 0009a224d8e288e8ec406ef0161f9293
COREUTILS		:= coreutils-$(COREUTILS_VERSION)
COREUTILS_SUFFIX	:= tar.xz
COREUTILS_URL		:= $(call ptx/mirror, GNU, coreutils/$(COREUTILS).$(COREUTILS_SUFFIX))
COREUTILS_SOURCE	:= $(SRCDIR)/$(COREUTILS).$(COREUTILS_SUFFIX)
COREUTILS_DIR		:= $(BUILDDIR)/$(COREUTILS)
COREUTILS_LICENSE	:= GPL-3.0-or-later
COREUTILS_LICENSE_FILES	:= file://COPYING;md5=d32239bcb673463ab874e80d47fae504

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#   --enable-install-program=PROG_LIST
#                           install the programs in PROG_LIST (comma-separated,
#                           default: none)
#   --enable-no-install-program=PROG_LIST
#                           do NOT install the programs in PROG_LIST
#                           (comma-separated, default: arch,hostname,su)
#   --with-libiconv-prefix[=DIR]  search for libiconv in DIR/include and DIR/lib
#   --without-libiconv-prefix     don't search for libiconv in includedir and libdir
#   --with-libpth-prefix[=DIR]  search for libpth in DIR/include and DIR/lib
#   --without-libpth-prefix     don't search for libpth in includedir and libdir
#   --without-included-regex
#                           don't compile regex; this is the default on systems
#                           with recent-enough versions of the GNU C Library
#                           (use with caution on other systems).
#   --with-packager         String identifying the packager of this software
#   --with-packager-version Packager-specific version information
#   --with-packager-bug-reports
#                           Packager info for bug reports (URL/e-mail/...)
#   --without-gmp           do not use the GNU MP library for arbitrary
#                           precision calculation (default: use it if available)
#   --with-libintl-prefix[=DIR]  search for libintl in DIR/include and DIR/lib
#   --without-libintl-prefix     don't search for libintl in includedir and libdir

COREUTILS_INST- =
COREUTILS_INST-y =
COREUTILS_INST-$(PTXCONF_COREUTILS_BASE64)	+= base64
COREUTILS_INST-$(PTXCONF_COREUTILS_BASENAME)	+= basename
COREUTILS_INST-$(PTXCONF_COREUTILS_CAT)		+= cat
COREUTILS_INST-$(PTXCONF_COREUTILS_CHCON)	+= chcon
COREUTILS_INST-$(PTXCONF_COREUTILS_CHGRP)	+= chgrp
COREUTILS_INST-$(PTXCONF_COREUTILS_CHMOD)	+= chmod
COREUTILS_INST-$(PTXCONF_COREUTILS_CHOWN)	+= chown
COREUTILS_INST-$(PTXCONF_COREUTILS_CHROOT)	+= chroot
COREUTILS_INST-$(PTXCONF_COREUTILS_CKSUM)	+= cksum
COREUTILS_INST-$(PTXCONF_COREUTILS_COMM)	+= comm
COREUTILS_INST-$(PTXCONF_COREUTILS_CP)		+= cp
COREUTILS_INST-$(PTXCONF_COREUTILS_CSPLIT)	+= csplit
COREUTILS_INST-$(PTXCONF_COREUTILS_CUT)		+= cut
COREUTILS_INST-$(PTXCONF_COREUTILS_DATE)	+= date
COREUTILS_INST-$(PTXCONF_COREUTILS_DD)		+= dd
COREUTILS_INST-$(PTXCONF_COREUTILS_DF)		+= df
COREUTILS_INST-$(PTXCONF_COREUTILS_DIR)		+= dir
COREUTILS_INST-$(PTXCONF_COREUTILS_DIRCOLORS)	+= dircolors
COREUTILS_INST-$(PTXCONF_COREUTILS_DIRNAME)	+= dirname
COREUTILS_INST-$(PTXCONF_COREUTILS_DU)		+= du
COREUTILS_INST-$(PTXCONF_COREUTILS_ECHO)	+= echo
COREUTILS_INST-$(PTXCONF_COREUTILS_ENV)		+= env
COREUTILS_INST-$(PTXCONF_COREUTILS_EXPAND)	+= expand
COREUTILS_INST-$(PTXCONF_COREUTILS_EXPR)	+= expr
COREUTILS_INST-$(PTXCONF_COREUTILS_FACTOR)	+= factor
COREUTILS_INST-$(PTXCONF_COREUTILS_FALSE)	+= false
COREUTILS_INST-$(PTXCONF_COREUTILS_FMT)		+= fmt
COREUTILS_INST-$(PTXCONF_COREUTILS_FOLD)	+= fold
COREUTILS_INST-$(PTXCONF_COREUTILS_GROUPS)	+= groups
COREUTILS_INST-$(PTXCONF_COREUTILS_HEAD)	+= head
COREUTILS_INST-$(PTXCONF_COREUTILS_HOSTID)	+= hostid
COREUTILS_INST-$(PTXCONF_COREUTILS_ID)		+= id
COREUTILS_INST-$(PTXCONF_COREUTILS_INSTALL)	+= install
COREUTILS_INST-$(PTXCONF_COREUTILS_JOIN)	+= join
COREUTILS_INST-$(PTXCONF_COREUTILS_KILL)	+= kill
COREUTILS_INST-$(PTXCONF_COREUTILS_LINK)	+= link
COREUTILS_INST-$(PTXCONF_COREUTILS_LN)		+= ln
COREUTILS_INST-$(PTXCONF_COREUTILS_LOGNAME)	+= logname
COREUTILS_INST-$(PTXCONF_COREUTILS_LS)		+= ls
COREUTILS_INST-$(PTXCONF_COREUTILS_MD5SUM)	+= md5sum
COREUTILS_INST-$(PTXCONF_COREUTILS_MKDIR)	+= mkdir
COREUTILS_INST-$(PTXCONF_COREUTILS_MKFIFO)	+= mkfifo
COREUTILS_INST-$(PTXCONF_COREUTILS_MKNOD)	+= mknod
COREUTILS_INST-$(PTXCONF_COREUTILS_MKTEMP)	+= mktemp
COREUTILS_INST-$(PTXCONF_COREUTILS_MV)		+= mv
COREUTILS_INST-$(PTXCONF_COREUTILS_NICE)	+= nice
COREUTILS_INST-$(PTXCONF_COREUTILS_NL)		+= nl
COREUTILS_INST-$(PTXCONF_COREUTILS_NOHUP)	+= nohup
COREUTILS_INST-$(PTXCONF_COREUTILS_NPROC)	+= nproc
COREUTILS_INST-$(PTXCONF_COREUTILS_NUMFMT)	+= numfmt
COREUTILS_INST-$(PTXCONF_COREUTILS_OD)		+= od
COREUTILS_INST-$(PTXCONF_COREUTILS_PASTE)	+= paste
COREUTILS_INST-$(PTXCONF_COREUTILS_PATHCHK)	+= pathchk
COREUTILS_INST-$(PTXCONF_COREUTILS_PINKY)	+= pinky
COREUTILS_INST-$(PTXCONF_COREUTILS_PR)		+= pr
COREUTILS_INST-$(PTXCONF_COREUTILS_PRINTENV)	+= printenv
COREUTILS_INST-$(PTXCONF_COREUTILS_PRINTF)	+= printf
COREUTILS_INST-$(PTXCONF_COREUTILS_PTX)		+= ptx
COREUTILS_INST-$(PTXCONF_COREUTILS_PWD)		+= pwd
COREUTILS_INST-$(PTXCONF_COREUTILS_READLINK)	+= readlink
COREUTILS_INST-$(PTXCONF_COREUTILS_REALPATH)	+= realpath
COREUTILS_INST-$(PTXCONF_COREUTILS_RM)		+= rm
COREUTILS_INST-$(PTXCONF_COREUTILS_RMDIR)	+= rmdir
COREUTILS_INST-$(PTXCONF_COREUTILS_RUNCON)	+= runcon
COREUTILS_INST-$(PTXCONF_COREUTILS_SEQ)		+= seq
COREUTILS_INST-$(PTXCONF_COREUTILS_SHA1SUM)	+= sha1sum
COREUTILS_INST-$(PTXCONF_COREUTILS_SHA224SUM)	+= sha224sum
COREUTILS_INST-$(PTXCONF_COREUTILS_SHA256SUM)	+= sha256sum
COREUTILS_INST-$(PTXCONF_COREUTILS_SHA384SUM)	+= sha384sum
COREUTILS_INST-$(PTXCONF_COREUTILS_SHA512SUM)	+= sha512sum
COREUTILS_INST-$(PTXCONF_COREUTILS_SHRED)	+= shred
COREUTILS_INST-$(PTXCONF_COREUTILS_SHUF)	+= shuf
COREUTILS_INST-$(PTXCONF_COREUTILS_SLEEP)	+= sleep
COREUTILS_INST-$(PTXCONF_COREUTILS_SORT)	+= sort
COREUTILS_INST-$(PTXCONF_COREUTILS_SPLIT)	+= split
COREUTILS_INST-$(PTXCONF_COREUTILS_STAT)	+= stat
COREUTILS_INST-$(PTXCONF_COREUTILS_STDBUF)	+= stdbuf
COREUTILS_INST-$(PTXCONF_COREUTILS_STTY)	+= stty
COREUTILS_INST-$(PTXCONF_COREUTILS_SUM)		+= sum
COREUTILS_INST-$(PTXCONF_COREUTILS_SYNC)	+= sync
COREUTILS_INST-$(PTXCONF_COREUTILS_TAC)		+= tac
COREUTILS_INST-$(PTXCONF_COREUTILS_TAIL)	+= tail
COREUTILS_INST-$(PTXCONF_COREUTILS_TEE)		+= tee
COREUTILS_INST-$(PTXCONF_COREUTILS_TEST)	+= test
COREUTILS_INST-$(PTXCONF_COREUTILS_TEST)	+= [
COREUTILS_INST-$(PTXCONF_COREUTILS_TIMEOUT)	+= timeout
COREUTILS_INST-$(PTXCONF_COREUTILS_TOUCH)	+= touch
COREUTILS_INST-$(PTXCONF_COREUTILS_TR)		+= tr
COREUTILS_INST-$(PTXCONF_COREUTILS_TRUE)	+= true
COREUTILS_INST-$(PTXCONF_COREUTILS_TRUNCATE)	+= truncate
COREUTILS_INST-$(PTXCONF_COREUTILS_TSORT)	+= tsort
COREUTILS_INST-$(PTXCONF_COREUTILS_TTY)		+= tty
COREUTILS_INST-$(PTXCONF_COREUTILS_UNAME)	+= uname
COREUTILS_INST-$(PTXCONF_COREUTILS_UNEXPAND)	+= unexpand
COREUTILS_INST-$(PTXCONF_COREUTILS_UNIQ)	+= uniq
COREUTILS_INST-$(PTXCONF_COREUTILS_UNLINK)	+= unlink
COREUTILS_INST-$(PTXCONF_COREUTILS_UPTIME)	+= uptime
COREUTILS_INST-$(PTXCONF_COREUTILS_USERS)	+= users
COREUTILS_INST-$(PTXCONF_COREUTILS_VDIR)	+= vdir
COREUTILS_INST-$(PTXCONF_COREUTILS_WC)		+= wc
COREUTILS_INST-$(PTXCONF_COREUTILS_WHO)		+= who
COREUTILS_INST-$(PTXCONF_COREUTILS_WHOAMI)	+= whoami
COREUTILS_INST-$(PTXCONF_COREUTILS_YES)		+= yes

COREUTILS_CONF_TOOL	:= autoconf
COREUTILS_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--enable-threads=posix \
	--disable-acl \
	--disable-assert \
	--disable-rpath \
	--disable-libsmack \
	--disable-xattr \
	--disable-libcap \
	--disable-gcc-warnings \
	--disable-single-binary \
	--disable-single-binary-exceptions \
	--enable-no-install-program=$(subst $(space),$(comma),$(strip $(COREUTILS_INST-))) \
	--disable-nls \
	--without-linux-crypto \
	--without-openssl \
	--$(call ptx/wwo, PTXCONF_GLOBAL_SELINUX)-selinux \
	--with-tty-group=tty \
	--without-gmp

COREUTILS_MAKE_OPT	:= \
	man1_MANS=

COREUTILS_INSTALL_OPT	:= \
	$(COREUTILS_MAKE_OPT) \
	install

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/coreutils.targetinstall:
	@$(call targetinfo)

	@$(call install_init, coreutils)
	@$(call install_fixup, coreutils,PRIORITY,optional)
	@$(call install_fixup, coreutils,SECTION,base)
	@$(call install_fixup, coreutils,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, coreutils,DESCRIPTION,missing)

ifdef PTXCONF_COREUTILS_STDBUF
	@$(call install_copy, coreutils, 0, 0, 0644, -, /usr/libexec/coreutils/libstdbuf.so)
endif

	@for i in $(COREUTILS_INST-y); do \
		$(call install_copy, coreutils, 0, 0, 0755, -, /usr/bin/$$i) \
	done

	@$(call install_finish, coreutils)

	@$(call touch)

# vim: syntax=make
