AC_PREREQ(2.59)

esyscmd([echo "define(myversion, $(dir=$(basename $(pwd)); if test \"${dir%%-trunk}\" != \"$dir\"; then echo 99999999-9; else prefix=${dir%%-[0-9]*-[0-9]}; echo ${dir##$prefix-}; fi))"])

AC_INIT([led_server], [2.0.3], [bugs@pengutronix.de])
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
LT_CURRENT=3
LT_REVISION=3
LT_AGE=2
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)


#
# Checks for programs.
#
AC_PROG_CC
#AC_PROG_LIBTOOL
LT_INIT(disable-static)

AM_INIT_AUTOMAKE([foreign no-exeext dist-bzip2])


##
# libsomething
##
#REQUIRES_LIBSOMETHING="libsomething >= 20090402-1"
#AC_SUBST(REQUIRES_LIBSOMETHING)
#PKG_CHECK_MODULES(libsomething, $REQUIRES_LIBSOMETHING)
#PKG_CHECK_MODULES(WAGO_DBUS, [wago_dbus >= 1.0])
#PKG_CHECK_MODULES(LIBXML, [libxml-2.0 >= 2.7.7])
#PKG_CHECK_MODULES(WAGO_DIAG, [wago_diagnostic >= 1.2])
#PKG_CHECK_MODULES(WAGO_DIAG_XML, [wago_diagnostic_xml >= 1.2])
#AC_SUBST(DBUS_CFLAGS)
#AC_SUBST(DBUS_LIBS)
#REQUIRES_LIBGLIB="glib-2.0 >= 2.28.1"
#AC_SUBST(REQUIRES_LIBGLIB)
#PKG_CHECK_MODULES(GLIB, $REQUIRES_LIBGLIB)


#AC_ARG_ENABLE(hal,
# [  --enable-hal[=hardware abstraction] Specify the hal:
#                      pca955x [default]: use i2c driver
#                      gpio: use gpio-interface]
# [case "${enableval}" in
#  pca955x) hal="pca955x" 
#  	   AC_DEFINE(HAL, "pca955x", [hal]);;
#  gpio)  hal="gpio" 
#  	     AC_DEFINE(HAL, "gpio", [hal])	;;
#  *) AC_MSG_ERROR(bad value ${enableval} for --enable-hal) ;;
#esac],[hal="pca955x"])
#    AM_CONDITIONAL(HAL_PCA955X, test "$hal" = "pca955x")
#   AM_CONDITIONAL(HAL_GPIO, test "$hal" = "gpio")

AC_MSG_CHECKING([for hardware abstraction])
AC_ARG_WITH(hal,
  AS_HELP_STRING([--with-hal=abstraction],
  [pca955x, gpio]),
  [hal="$withval"],
  [hal=""])

AC_DEFINE_UNQUOTED(HAL, "$hal", [Hardware Abstraction])
AC_MSG_RESULT([$hal])
AM_CONDITIONAL(HAL_PCA955X, test "x$hal" = "xpca955x")
AM_CONDITIONAL(HAL_GPIO, test "x$hal" = "xgpio")

AC_MSG_CHECKING([gpio variant])
AC_ARG_WITH(gpiovariant,
  AS_HELP_STRING([--with-gpiovariant=variant],
  [pfc200, pfc200eval, perspecto]),
  [gpiovariant="$withval"],
  [gpiovariant=""])

AS_IF([test "x$gpiovariant" = "xpfc200"],
	[
	  AC_DEFINE(PFC200,1,[GPIO-Variant PFC200])
	])
	
AS_IF([test "x$gpiovariant" = "xpfc200eval"],
	[
	  AC_DEFINE(PFC200_EVALBOARD,1,[GPIO-Variant PFC200 Evalboard])
	])
	
AS_IF([test "x$gpiovariant" = "xperspecto"],
	[
	  AC_DEFINE(PERSPECTO,1,[GPIO-Variant PERSPECTO])
	])


##
# check libs
##
AC_CHECK_LIB(pthread, pthread_once)
AC_CHECK_FUNCS_ONCE(pthread_setname_np)

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
                 data/Makefile
				 data/ledserver2.pc
                 src/Makefile
                 src/kernel/Makefile
                 src/sequence/Makefile
                 testapp/Makefile ])
AC_OUTPUT

