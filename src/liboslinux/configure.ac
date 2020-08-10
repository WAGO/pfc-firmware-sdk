dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(liboslinux, 1.0)
AC_PROG_CC

AC_CONFIG_HEADERS([include/OsConfig.h])

AC_CANONICAL_SYSTEM

dnl Initialize automake
AM_INIT_AUTOMAKE

dnl this allows us specify individual liking flags for each target
AM_PROG_CC_C_O 

dnl Initialize Libtool
LT_INIT

dnl Check if Libtool is present
dnl Libtool is used for building share libraries 
AC_PROG_LIBTOOL


AC_MSG_CHECKING([whether to enable trace output])
AC_ARG_ENABLE(trace,
	AS_HELP_STRING([--enable-trace], [enable tracing @<:@default=yes@:>@]),    
[case "${enableval}" in
  yes) trace=true 
  	   AC_DEFINE(OS_TRACE_ENABLED, 1, [tracing]) ;;
  no)  trace=false ;; 
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-trace) ;;
esac])
AM_CONDITIONAL(OS_TRACE_ENABLED, test $trace = true)



AC_CONFIG_FILES(Makefile
                liboslinux/Makefile
                include/Makefile)
AC_OUTPUT
