#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
lib_LTLIBRARIES = liboslinux.la

#######################################
# Build information for each library


pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = oslinux.pc

# Sources for liboslinux
liboslinux_la_SOURCES = OsTrace.c OsTime.c OsTimer.c OsThread.c OsSemaphore.c OsMutex.c OsCondition.c OsMemory.c OsRessource.c OsSleep.c OsRwLock.c

# Linker options libTestProgram
liboslinux_la_LDFLAGS = -lpthread -lrt

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
liboslinux_la_CPPFLAGS = -I$(top_srcdir)/include -pthread -D_GNU_SOURCE -Wall -Wextra -Werror -std=c99 -g3 -O3

