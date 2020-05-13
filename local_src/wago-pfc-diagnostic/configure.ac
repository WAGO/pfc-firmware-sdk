AC_PREREQ(2.59)

esyscmd([echo "define(myversion, $(dir=$(basename $(pwd)); if test \"${dir%%-trunk}\" != \"$dir\"; then echo 99999999-9; else prefix=${dir%%-[0-9]*-[0-9]}; echo ${dir##$prefix-}; fi))"])

AC_INIT([wago-pfc-diagnostic], [2.0.0], [bugs@pengutronix.de])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([src/config.h])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET



AM_MAINTAINER_MODE

CFLAGS="${CFLAGS} -W -Wall -g3"

#
# libtool library versioning stuff
#
# Library code modified:            REVISION++
# Interfaces changed/added/removed: CURRENT++   REVISION=0
# Interfaces added:                 AGE++
# Interfaces removed:               AGE=0
LT_CURRENT=4
LT_REVISION=0
LT_AGE=0
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)


#
# Checks for programs.
#
AC_PROG_CC
AC_PROG_CXX
#AC_PROG_LIBTOOL
LT_INIT(disable-static)

AM_INIT_AUTOMAKE([foreign no-exeext dist-bzip2 subdir-objects])

##
# libsomething
##
#REQUIRES_LIBSOMETHING="libsomething >= 20090402-1"
#AC_SUBST(REQUIRES_LIBSOMETHING)
#PKG_CHECK_MODULES(libsomething, $REQUIRES_LIBSOMETHING)
PKG_CHECK_MODULES(WAGO_DBUS, [wago_dbus >= 1.0])
PKG_CHECK_MODULES(LIBXML, [libxml-2.0 >= 2.7.7])
PKG_CHECK_MODULES(LEDSERVER2, [ledserver2])
AC_SUBST(DBUS_CFLAGS)
AC_SUBST(DBUS_LIBS)

##
# check libs
##
AC_CHECK_LIB(pthread, pthread_once)
AC_CHECK_FUNCS_ONCE(pthread_setname_np)

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
    CFLAGS="${CFLAGS} -Werror -Wsign-compare -Wfloat-equal -Wformat-security -g -O0"
    #CFLAGS=" -Wall -g3 -O0"
    AC_DEFINE(DEBUG, 1, [debugging])
else
    CFLAGS="${CFLAGS} -Os -g"
fi

#AC_CONFIG_FILES([
#	Makefile
#])
AC_CONFIG_FILES([Makefile 
				 include/Makefile
				 include/diagnostic/Makefile
                 src/Makefile   
                 src/EActingBox/Makefile 
                 src/ledserverd/Makefile
                 src/diag_lib/Makefile 
                 src/diag_lib/wago_diagnostic.pc
                 src/diag_lib/wago_diagnostic_xml.pc
                 testapp/Makefile ])
AC_OUTPUT

