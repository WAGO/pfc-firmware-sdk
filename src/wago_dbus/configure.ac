AC_PREREQ(2.59)
esyscmd([echo "define(myversion, $(dir=$(basename $(pwd)); if test \"${dir%%-trunk}\" != \"$dir\"; then echo 99999999-9; else prefix=${dir%%-[0-9]*-[0-9]}; echo ${dir##$prefix-}; fi))"])

AC_INIT([wago_dbus], 1.3.0, [bugs@pengutronix.de])
AC_USE_SYSTEM_EXTENSIONS
AC_CONFIG_SRCDIR([testtool])
AC_CONFIG_HEADERS([config.h])
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
LT_CURRENT=7
LT_REVISION=0
LT_AGE=5
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)


#
# Checks for programs.
#
AC_PROG_CC

AM_INIT_AUTOMAKE([foreign no-exeext dist-bzip2])


##
# libsomething
##
#REQUIRES_LIBSOMETHING="libsomething >= 20090402-1"
#AC_SUBST(REQUIRES_LIBSOMETHING)
#PKG_CHECK_MODULES(libsomething, $REQUIRES_LIBSOMETHING)
DBUS_GLIB_REQUIRED=0.92
PKG_CHECK_MODULES(DBUS_GLIB, [dbus-glib-1 >= $DBUS_GLIB_REQUIRED])

##
# check libs
##
AC_CHECK_LIB(pthread, pthread_once)
AC_CHECK_FUNCS_ONCE(pthread_setname_np)


AC_ARG_ENABLE(testtool,
[--enable-testtool    Build Testool for testing the library],
[case "${enableval}" in
  yes) testtool=true 
  	   AC_DEFINE(TEST_TOOL, 1, [Testtool]);;
  no)  testtool=false ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-testtool) ;;
esac],[testtool=false])
AM_CONDITIONAL(TEST_TOOL, test $testtool = true)


#
# Debugging
#
#AC_MSG_CHECKING([whether to enable debugging])
#AC_ARG_ENABLE(debug,
#    AS_HELP_STRING([--enable-debug], [enable debugging @<:@default=yes@:>@]),
#	[case "$enableval" in
#	y | yes) CONFIG_DEBUG=yes ;;
#        *) CONFIG_DEBUG=no ;;
#    esac],
#    [CONFIG_DEBUG=yes])
#AC_MSG_RESULT([${CONFIG_DEBUG}])
#if test "${CONFIG_DEBUG}" = "yes"; then
#    CFLAGS="${CFLAGS} -Werror -Wsign-compare -Wfloat-equal -Wformat-security -g -O1"
#    AC_DEFINE(DEBUG, 1, [debugging])
#else
#    CFLAGS="${CFLAGS} -O2"
#fi

CFLAGS="${CFLAGS} -g -O2"
#CFLAGS="${CFLAGS} -g -O0"

AC_CONFIG_FILES([
	Makefile \
	data/Makefile \
	data/wago_dbus.pc \
	include/Makefile \
	src/Makefile \
	testtool/Makefile
])
AC_OUTPUT

