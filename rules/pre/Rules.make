# -*-makefile-*-
#
# This file contains global macro and environment definitions.
#

# ----------------------------------------------------------------------------
# Programs & Local Defines
# ----------------------------------------------------------------------------

# FIXME: cleanup

ifeq ($(PTXCONF_CONFIGFILE_VERSION), "2020.08.0")
GNU_BUILD	:= $(call ptx/force-sh, $(SCRIPTSDIR)/autoconf/config.guess)
GNU_HOST	:= $(call ptx/force-sh, echo $(GNU_BUILD) | sed s/-[a-zA-Z0-9_]*-/-host-/)
else
GNU_BUILD	:= $(shell $(SCRIPTSDIR)/autoconf/config.guess)
GNU_HOST	:= $(shell echo $(GNU_BUILD) | sed s/-[a-zA-Z0-9_]*-/-host-/)
endif

INSTALL		:= install

FAKEROOT	:= $(PTXDIST_SYSROOT_HOST)/bin/fakeroot

CHECK_PIPE_STATUS := \
	for i in  "$${PIPESTATUS[@]}"; do [ $$i -gt 0 ] && {			\
		echo;								\
		echo "error: a command in the pipe returned $$i, bailing out";	\
		echo;								\
		exit $$i;							\
	}									\
	done;									\
	true;

#
# prepare the search path when cross compiling
#
CROSS_PATH := $(PTXDIST_SYSROOT_CROSS)/bin:$(PTXDIST_SYSROOT_CROSS)/sbin:$(PATH)


# ----------------------------------------------------------------------------
# Environment
# ----------------------------------------------------------------------------

#
# Environment variables for toolchain components
#
CROSS_AR		:= $(PTXCONF_COMPILER_PREFIX)ar
CROSS_AS		:= $(PTXCONF_COMPILER_PREFIX)as
CROSS_LD		:= $(PTXCONF_COMPILER_PREFIX)ld
CROSS_NM		:= $(PTXCONF_COMPILER_PREFIX)nm
CROSS_CC		:= $(PTXCONF_COMPILER_PREFIX)gcc
CROSS_CXX		:= $(PTXCONF_COMPILER_PREFIX)g++
CROSS_CPP		:= "$(PTXCONF_COMPILER_PREFIX)gcc -E"
CROSS_RANLIB		:= $(PTXCONF_COMPILER_PREFIX)ranlib
CROSS_READELF		:= $(PTXCONF_COMPILER_PREFIX)readelf
CROSS_OBJCOPY		:= $(PTXCONF_COMPILER_PREFIX)objcopy
CROSS_OBJDUMP		:= $(PTXCONF_COMPILER_PREFIX)objdump
CROSS_STRIP		:= $(PTXCONF_COMPILER_PREFIX)strip
CROSS_DLLTOOL		:= $(PTXCONF_COMPILER_PREFIX)dlltool
CROSS_GNAT		:= $(PTXCONF_COMPILER_PREFIX)gnat
CROSS_GNATBIND		:= $(PTXCONF_COMPILER_PREFIX)gnatbind
CROSS_GNATCHOP		:= $(PTXCONF_COMPILER_PREFIX)gnatchop
CROSS_GNATCLEAN		:= $(PTXCONF_COMPILER_PREFIX)gnatclean
CROSS_GNATFIND		:= $(PTXCONF_COMPILER_PREFIX)gnatfind
CROSS_GNATKR		:= $(PTXCONF_COMPILER_PREFIX)gnatkr
CROSS_GNATLINK		:= $(PTXCONF_COMPILER_PREFIX)gnatlink
CROSS_GNATLS		:= $(PTXCONF_COMPILER_PREFIX)gnatls
CROSS_GNATMAKE		:= $(PTXCONF_COMPILER_PREFIX)gnatmake
CROSS_GNATNAME		:= $(PTXCONF_COMPILER_PREFIX)gnatname
CROSS_GNATPREP		:= $(PTXCONF_COMPILER_PREFIX)gnatprep
CROSS_GNATXREF		:= $(PTXCONF_COMPILER_PREFIX)gnatxref
CROSS_PKG_CONFIG	:= $(PTXCONF_COMPILER_PREFIX)pkg-config

CROSS_ENV_AR		:= AR=$(CROSS_AR)
CROSS_ENV_AS		:= AS=$(CROSS_AS)
CROSS_ENV_LD		:= LD=$(CROSS_LD)
CROSS_ENV_NM		:= NM=$(CROSS_NM)
CROSS_ENV_CC		:= CC=$(CROSS_CC)
CROSS_ENV_CXX		:= CXX=$(CROSS_CXX)
CROSS_ENV_CPP		:= CPP=$(CROSS_CPP)
CROSS_ENV_RANLIB	:= RANLIB=$(CROSS_RANLIB)
CROSS_ENV_READELF	:= READELF=$(CROSS_READELF)
CROSS_ENV_OBJCOPY	:= OBJCOPY=$(CROSS_OBJCOPY)
CROSS_ENV_OBJDUMP	:= OBJDUMP=$(CROSS_OBJDUMP)
CROSS_ENV_STRIP		:= STRIP=$(CROSS_STRIP)
CROSS_ENV_DLLTOOL	:= DLLTOOL=$(CROSS_DLLTOOL)
CROSS_ENV_GNAT		:= GNAT=$(CROSS_GNAT)
CROSS_ENV_GNATBIND	:= GNATBIND=$(CROSS_GNATBIND)
CROSS_ENV_GNATCHOP	:= GNATCHOP=$(CROSS_GNATCHOP)
CROSS_ENV_GNATCLEAN	:= GNATCLEAN=$(CROSS_GNATCLEAN)
CROSS_ENV_GNATFIND	:= GNATFIND=$(CROSS_GNATFIND)
CROSS_ENV_GNATKR	:= GNATKR=$(CROSS_GNATKR)
CROSS_ENV_GNATLINK	:= GNATLINK=$(CROSS_GNATLINK)
CROSS_ENV_GNATLS	:= GNATLS=$(CROSS_GNATLS)
CROSS_ENV_GNATMAKE	:= GNATMAKE=$(CROSS_GNATMAKE)
CROSS_ENV_GNATNAME	:= GNATNAME=$(CROSS_GNATNAME)
CROSS_ENV_GNATPREP	:= GNATPREP=$(CROSS_GNATPREP)
CROSS_ENV_GNATXREF	:= GNATXREF=$(CROSS_GNATXREF)
CROSS_ENV_CC_FOR_BUILD	:= CC_FOR_BUILD=$(HOSTCC)
CROSS_ENV_CPP_FOR_BUILD	:= CPP_FOR_BUILD="$(HOSTCC) -E"
CROSS_ENV_LINK_FOR_BUILD:= LINK_FOR_BUILD=$(HOSTCC)
CROSS_ENV_PKG_CONFIG	:= PKG_CONFIG=$(CROSS_PKG_CONFIG)



#
# CROSS_ENV is the environment usually set for all configure and compile
# calls in the packet makefiles.
#
# The ac_cv_* variables are needed to tell configure scripts not to use
# AC_TRY_RUN and run cross compiled things on the development host
#
CROSS_ENV_PROGS := \
	$(CROSS_ENV_AR) \
	$(CROSS_ENV_AS) \
	$(CROSS_ENV_LD) \
	$(CROSS_ENV_NM) \
	$(CROSS_ENV_CC) \
	$(CROSS_ENV_CXX) \
	$(CROSS_ENV_CPP) \
	$(CROSS_ENV_RANLIB) \
	$(CROSS_ENV_READELF) \
	$(CROSS_ENV_OBJCOPY) \
	$(CROSS_ENV_OBJDUMP) \
	$(CROSS_ENV_STRIP) \
	$(CROSS_ENV_DLLTOOL) \
	$(CROSS_ENV_GNAT) \
	$(CROSS_ENV_GNATBIND) \
	$(CROSS_ENV_GNATCHOP) \
	$(CROSS_ENV_GNATCLEAN) \
	$(CROSS_ENV_GNATFIND) \
	$(CROSS_ENV_GNATKR) \
	$(CROSS_ENV_GNATLINK) \
	$(CROSS_ENV_GNATLS) \
	$(CROSS_ENV_GNATMAKE) \
	$(CROSS_ENV_GNATNAME) \
	$(CROSS_ENV_GNATPREP) \
	$(CROSS_ENV_GNATXREF) \
	$(CROSS_ENV_CC_FOR_BUILD) \
	$(CROSS_ENV_CPP_FOR_BUILD) \
	$(CROSS_ENV_LINK_FOR_BUILD) \
	$(CROSS_ENV_PKG_CONFIG)

CROSS_LIB_DIR   := $(shell ptxd_get_lib_dir)

#
# prepare to use pkg-config with wrapper which takes care of
# $(PTXDIST_SYSROOT_TARGET). The wrapper's magic doesn't work when
# pkg-config strips out /usr/lib and other system libs/cflags, so we
# leave them in; the wrapper replaces them by proper
# $(PTXDIST_SYSROOT_TARGET) correspondees.
#
CROSS_ENV_PKG_CONFIG := \
	SYSROOT="$(PTXDIST_SYSROOT_TARGET)" \
	$(PTXDIST_CROSS_ENV_PKG_CONFIG) \
	PKG_CONFIG="$(PTXDIST_SYSROOT_CROSS)/bin/$(COMPILER_PREFIX)pkg-config"

#
# The ac_cv_* variables are needed to tell configure scripts not to
# use AC_TRY_RUN and run cross compiled things on the development host
#
CROSS_ENV_AC := \
	enable_option_checking=fatal \
	enable_maintainer_mode=no \
	enable_static=no \
	ac_cv_file__dev_random=yes \
	ac_cv_file__proc_self_exe=yes \
	ac_cv_file__proc_self_fd=yes \
	ac_cv_file__proc_self_maps=yes \
	ac_cv_func_dcgettext=yes \
	ac_cv_func_getpgrp_void=yes \
	ac_cv_func_getrlimit=yes \
	ac_cv_func_malloc_0_nonnull=yes \
	ac_cv_func_memcmp_clean=yes \
	ac_cv_func_memcmp_working=yes \
	ac_cv_func_nonposix_getgrgid_r=no \
	ac_cv_func_nonposix_getpwuid_r=no \
	ac_cv_func_posix_getgrgid_r=yes \
	ac_cv_func_posix_getpwnam_r=yes \
	ac_cv_func_posix_getpwuid_r=yes \
	ac_cv_func_printf_unix98=yes \
	ac_cv_func_realloc_0_nonnull=yes \
	ac_cv_func_setgrent_void=yes \
	ac_cv_func_setpgrp_void=yes \
	ac_cv_func_setvbuf_reversed=no \
	ac_cv_func_strstr=yes \
	ac_cv_func_strtod=yes \
	ac_cv_func_strtoul=yes \
	ac_cv_func_vsnprintf_c99=yes \
	ac_cv_func_wait3_rusage=yes \
	ac_cv_have_abstract_sockets=yes \
	ac_cv_lib_c_inet_aton=yes \
	ac_cv_sizeof_long_double=$(PTXCONF_SIZEOF_LONG_DOUBLE) \
	ac_cv_sizeof_long_long=8 \
	ac_cv_sysv_ipc=yes \
	ac_cv_type_uintptr_t=yes \
	bash_cv_func_ctype_nonascii=yes \
	bash_cv_func_sigsetjmp=present \
	bash_cv_func_strcoll_broken=no \
	bash_cv_must_reinstall_sighandlers=no \
	glib_cv_long_long_format="ll" \
	gt_cv_func_gettext_libintl=yes

ifdef PTXCONF_HAS_MMU
CROSS_ENV_AC += \
	ac_cv_func_fork=yes \
	ac_cv_func_fork_works=yes
else
CROSS_ENV_AC += ac_cv_func_fork=no
endif

ifdef PTXCONF_ICONV
CROSS_ENV_AC += ac_cv_func_iconv_open=yes
else
CROSS_ENV_AC += ac_cv_func_iconv_open=no
endif

#
# CROSS_ENV is the environment usually set for all configure and
# compile calls in the packet makefiles.
#
CROSS_ENV := \
	$(CROSS_ENV_PROGS) \
	$(CROSS_ENV_PKG_CONFIG) \
	$(CROSS_ENV_AC)


#
# We want to use DESTDIR and --prefix=/usr, to get no build paths in our
# binaries. Unfortunately, not all packages support this, especially
# libtool based packets seem to be broken. See for example:
#
# https://bugzilla.redhat.com/bugzilla/show_bug.cgi?id=58664
#
# for a longer discussion [RSC]
#

CROSS_AUTOCONF_SYSROOT_USR := \
	--prefix=/usr --sysconfdir=/etc --localstatedir=/var --libdir=/usr/$(CROSS_LIB_DIR)

CROSS_AUTOCONF_SYSROOT_ROOT := \
	--libdir=/$(CROSS_LIB_DIR) --prefix=

CROSS_AUTOCONF_ARCH := \
	--build=$(GNU_HOST) \
	--host=$(PTXCONF_GNU_TARGET)

CROSS_AUTOCONF_USR  := $(CROSS_AUTOCONF_SYSROOT_USR)  $(CROSS_AUTOCONF_ARCH)
CROSS_AUTOCONF_ROOT := $(CROSS_AUTOCONF_SYSROOT_ROOT) $(CROSS_AUTOCONF_ARCH)

CROSS_CMAKE_USR	 := \
	-DCMAKE_INSTALL_PREFIX=/usr \
	-DCMAKE_BUILD_TYPE:STRING=RelWithDebInfo \
	-DCMAKE_TOOLCHAIN_FILE='${PTXDIST_CMAKE_TOOLCHAIN_TARGET}'

CROSS_CMAKE_ROOT := \
	-DCMAKE_INSTALL_PREFIX=/ \
	-DCMAKE_BUILD_TYPE:STRING=RelWithDebInfo \
	-DCMAKE_TOOLCHAIN_FILE='${PTXDIST_CMAKE_TOOLCHAIN_TARGET}'

CROSS_QMAKE_OPT := \
	-recursive \
	$(if $(filter 0,$(PTXDIST_VERBOSE)),CONFIG+=silent)

CROSS_PYTHON_INSTALL := install --prefix=/usr
HOST_PYTHON_INSTALL := install --prefix=/.

CROSS_MESON_USR := \
	--cross-file '${PTXDIST_MESON_CROSS_FILE}' \
	--wrap-mode nodownload \
	-Dbackend=ninja \
	-Dbuildtype=debugoptimized \
	-Dlibdir=$(CROSS_LIB_DIR) \
	-Dprefix=/usr

CROSS_MESON_ENV = \
	$(HOST_ENV_PROGS)

ifdef PTXCONF_GLOBAL_IPV6
GLOBAL_IPV6_OPTION := --enable-ipv6
else
GLOBAL_IPV6_OPTION := --disable-ipv6
endif

ifdef PTXCONF_GLOBAL_LARGE_FILE
GLOBAL_LARGE_FILE_OPTION := --enable-largefile
else
GLOBAL_LARGE_FILE_OPTION := --disable-largefile
endif

ifdef PTXCONF_GLOBAL_PAM
GLOBAL_PAM_OPTION := --enable-pam
else
GLOBAL_PAM_OPTION := --disable-pam
endif

ifdef PTXCONF_GLOBAL_SELINUX
GLOBAL_SELINUX_OPTION := --enable-selinux
else
GLOBAL_SELINUX_OPTION := --disable-selinux
endif

# ----------------------------------------------------------------------------
# HOST stuff
# ----------------------------------------------------------------------------

HOST_PATH	:= $(PATH)

HOST_ENV_AC := \
	enable_option_checking=fatal \
	enable_maintainer_mode=no \
	enable_static=no

HOST_ENV_CC		:= CC="$(HOSTCC)"
HOST_ENV_CXX		:= CXX="$(HOSTCXX)"

HOST_ENV_PROGS := \
	$(HOST_ENV_CC) \
	$(HOST_ENV_CXX)

HOST_ENV	:= \
	$(HOST_ENV_AC) \
	$(HOST_ENV_PROGS) \
	$(HOST_ENV_PKG_CONFIG)


HOST_AUTOCONF  := --prefix=
HOST_AUTOCONF_SYSROOT := --prefix=$(PTXDIST_SYSROOT_HOST)

HOST_CMAKE_OPT := \
	-DCMAKE_INSTALL_PREFIX= \
	-DCMAKE_BUILD_TYPE:STRING=RelWithDebInfo \
	-DCMAKE_TOOLCHAIN_FILE='${PTXDIST_CMAKE_TOOLCHAIN_HOST}'

HOST_CMAKE_OPT_SYSROOT := \
	-DCMAKE_INSTALL_PREFIX=$(PTXDIST_SYSROOT_HOST) \
	-DCMAKE_BUILD_TYPE:STRING=RelWithDebInfo \
	-DCMAKE_TOOLCHAIN_FILE='${PTXDIST_CMAKE_TOOLCHAIN_HOST}'

HOST_MESON_OPT := \
	--wrap-mode nodownload \
	-Dbackend=ninja \
	-Dbuildtype=debugoptimized \
	-Dlibdir=lib \
	-Dprefix=/

# ----------------------------------------------------------------------------
# HOST_CROSS stuff
# ----------------------------------------------------------------------------

HOST_CROSS_PATH := $(CROSS_PATH)

HOST_CROSS_ENV := $(HOST_ENV)


HOST_CROSS_AUTOCONF_ARCH := --target=$(PTXCONF_GNU_TARGET)

HOST_CROSS_AUTOCONF := --prefix= $(HOST_CROSS_AUTOCONF_ARCH)
HOST_CROSS_AUTOCONF_SYSROOT := --prefix=$(PTXDIST_SYSROOT_CROSS) $(HOST_CROSS_AUTOCONF_ARCH)

# ----------------------------------------------------------------------------
# Convenience macros
# ----------------------------------------------------------------------------

#
# add_locale
#
# add locale support to locales-archive, if not exist, a new locale
# archive will be created automaticly
#
# $1: localename: localename (i.E. zh_CN or zh_CN.GBK)
# $2: localedef; locale definition file (i.E. de_DE or de_DE@euro)
# $3: charmap; charachter encoding map (i.E. ISO-8859-1)
# $4: prefix; installation prefix for locales-archive
#
#
add_locale =							\
	LOCALE_NAME=$(strip $(1));				\
	LOCALE_DEF=$(strip $(2));				\
	CHARMAP=$(strip $(3));					\
	PREF=$(strip $(4));					\
	LE=$(PTXCONF_ENDIAN_LITTLE);				\
	BE=$(PTXCONF_ENDIAN_BIG);				\
	if [ "$$LE" = "y" ]; then				\
		ENDIAN=-l;					\
	elif [ "$$BE" = "y" ]; then				\
		ENDIAN=-b;					\
	else							\
		exit 1;						\
	fi;							\
	${CROSS_ENV_CC} $(CROSS_ENV_STRIP)			\
	$(SCRIPTSDIR)/make_locale.sh 				\
		-e $(PTXDIST_SYSROOT_HOST)/bin/localedef 	\
		-f $$CHARMAP -i $$LOCALE_DEF 			\
		-p $$PREF 					\
		-n $$LOCALE_NAME				\
		$$ENDIAN


#
# add_zoneinfo
#
# add zoneinfo support to glibc-archive
#
# $1: zoneinfoname: zoneinfoname (i.E. Europe)
# $2: prefix; installation prefix for glibc-archive
#
#
add_zoneinfo =							\
	ZONEINFO_NAME=$(strip $(1));				\
	PREF=$(strip $(2));					\
	SYSROOT=$(strip $(3));					\
	${CROSS_ENV_CC} $(CROSS_ENV_STRIP)			\
	$(SCRIPTSDIR)/make_zoneinfo.sh				\
	-n "$$ZONEINFO_NAME"					\
	-p "$$PREF"						\
	-s "$$SYSROOT"


#
# enable_c
#
# Enables a define, removes /* */
#
# (often found in .c or .h files)
#
# $1: file
# $2: parameter
#
enable_c =											\
	FILENAME="$(strip $(1))";								\
	PARAMETER="$(strip $(2))";								\
	sed -i -e										\
		"s,^\s*\(\/\*\)\?\s*\(\\\#\s*define\s\+$$PARAMETER\)\s*\(\*\/\)\?$$,\2,"	\
		$$FILENAME

#
# disable_c
#
# Disables a define with, adds /* */
#
# (often found in .c or .h files)
#
# $1: file
# $2: parameter
#
disable_c =											\
	FILENAME="$(strip $(1))";								\
	PARAMETER="$(strip $(2))";								\
	sed -i -e										\
		"s,^\s*\(\/\*\)\?\s*\(\\\#\s*define\s\+$$PARAMETER\)\s*\(\*\/\)\?$$,\/\*\2\*\/," \
		$$FILENAME


#
# enable_sh
#
# Enabled something, removes #
#
# often found in shell scripts, Makefiles
#
# $1: file
# $2: parameter
#
enable_sh =						\
	FILENAME="$(strip $(1))";			\
	PARAMETER="$(strip $(2))";			\
	sed -i -e					\
		"s,^\s*\(\#\)\?\s*\($$PARAMETER\),\2,"	\
		$$FILENAME

#
# disable_sh
#
# Disables a comment, adds #
#
# often found in shell scripts, Makefiles
#
# $1: file
# $2: parameter
#
disable_sh =						\
	FILENAME="$(strip $(1))";			\
	PARAMETER="$(strip $(2))";			\
	sed -i -e					\
		"s,^\s*\(\#\)\?\s*\($$PARAMETER\),\#\2,"\
		$$FILENAME

# vim: syntax=make
