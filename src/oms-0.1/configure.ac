AC_PREREQ(2.59)

esyscmd([echo "define(myversion, $(dir=$(basename $(pwd)); if test \"${dir%%-trunk}\" != \"$dir\"; then echo 99999999-9; else prefix=${dir%%-[0-9]*-[0-9]}; echo ${dir##$prefix-}; fi))"])

AC_INIT([oms], 0.1, [bugs@pengutronix.de])
AC_USE_SYSTEM_EXTENSIONS
#AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([src/config.h])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

LT_INIT(disable-static)

AM_MAINTAINER_MODE

CFLAGS="${CFLAGS} -W -Wall"

#
# libtool library versioning stuff
#
# Library code modified:            REVISION++
# Interfaces changed/added/removed: CURRENT++   REVISION=0
# Interfaces added:                 AGE++
# Interfaces removed:               AGE=0
LT_CURRENT=0
LT_REVISION=0
LT_AGE=0
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)


#
# Checks for programs.
#
AC_PROG_CC

AM_INIT_AUTOMAKE([foreign no-exeext dist-bzip2])

AC_ARG_ENABLE(omsd,
[--disable-omsd    Exclude oms-daemon from Compiling],
[case "${enableval}" in
  yes) omsd=true ;;
  no)  omsd=false ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --disable-omsd) ;;
esac],[omsd=true])
if test $omsd = true; then
    AC_DEFINE(WITH_DAEMON, 1, [Buils omsd])
fi
AM_CONDITIONAL(BUILD_DAEMON, test $omsd = true)

#AC_ARG_ENABLE(codesyslib,
#[--enable-codesyslib    Build CoDeSys-Module],
#[case "${enableval}" in
#  yes) codesyslib=true ;;
#  no)  codesyslib=false ;;
#  *) AC_MSG_ERROR(bad value ${enableval} for --enable-codesyslib) ;;
#esac],[codesyslib=false])
#if test $codesyslib = true; then
#    AC_DEFINE(WITH_CODESYS_LIB, 1, [CoDeSys Modul])
#fi
#AM_CONDITIONAL(BUILD_CODESYS_LIB, test $codesyslib = true)

##
# libsomething
##
#REQUIRES_LIBSOMETHING="libsomething >= 20090402-1"
#AC_SUBST(REQUIRES_LIBSOMETHING)
#PKG_CHECK_MODULES(libsomething, $REQUIRES_LIBSOMETHING)

##
# check libs
##
#AC_CHECK_LIB(pthread, pthread_once)
#AC_CHECK_FUNCS_ONCE(pthread_setname_np)

#
# Debugging
#
AC_MSG_CHECKING([whether to enable debugging])
AC_ARG_ENABLE(debug,
    AS_HELP_STRING([--enable-debug], [enable debugging @<:@default=yes@:>@]),
	[case "$enableval" in
	y | yes) CONFIG_DEBUG=yes ;;
        *) CONFIG_DEBUG=no ;;
    esac],
    [CONFIG_DEBUG=no])
AC_MSG_RESULT([${CONFIG_DEBUG}])
if test "${CONFIG_DEBUG}" = "yes"; then
  	CFLAGS="${CFLAGS} -ansi -std=c99 -pedantic-errors -Werror -Wextra -Wsign-compare -Wfloat-equal -Wformat-security -g -O0"
   #CFLAGS="${CFLAGS} -O2"
    AC_DEFINE(DEBUG, 1, [debugging])
else
    CFLAGS="${CFLAGS} -O2"
fi


AC_CONFIG_FILES([
	Makefile
	src/Makefile
	include/Makefile
	etc/Makefile
	src/liboms/Makefile
	src/omsd/Makefile
	data/Makefile
	data/oms.pc
])
AC_OUTPUT

