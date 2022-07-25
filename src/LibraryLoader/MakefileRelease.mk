# Copyright (c) WAGO GmbH & Co. KG
#
# PROPRIETARY RIGHTS are involved in the subject matter of this material. All
# manufacturing, reproduction, use, and sales rights pertaining to this
# subject matter are governed by the license agreement. The recipient of this
# software implicitly accepts the terms of the license.

#Set this to @ to keep the makefile quiet
SILENCE = @

#-------------------------------------------------------------------
# Outputs
#-------------------------------------------------------------------
COMPONENT_NAME = libloader

MK_OBJS_DIR = project/objs

#-------------------------------------------------------------------
# Inputs
#-------------------------------------------------------------------
PROJECT_HOME_DIR = .

DEBUG = y


#-------------------------------------------------------------------
# Compiler to use
#-------------------------------------------------------------------
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
AR=$(CROSS_COMPILE)ar
CXX=$(CROSS_COMPILE)g++

LIBTOOL=$(CROSS_LIBTOOL)libtool

ifeq ($(USE_CLANG), y)
CC=clang
LD=llvm-ld
CXX=clang++
endif


# SYSROOT-target directory (is controlled by ptx-dist rule)
# for local builds set this to your ptxdist platform path/SYSROOT-target

ifndef SYSROOT
$(error Environment variable SYSROOT not set properly!)
endif


#-------------------------------------------------------------------
# Compiler Warnings
#-------------------------------------------------------------------

MK_WARNINGFLAGS += -Wall 
MK_WARNINGFLAGS += -Werror
MK_WARNINGFLAGS += -Wstrict-prototypes
MK_WARNINGFLAGS += -Wpointer-arith
MK_WARNINGFLAGS += -Wformat-y2k -Wno-format-extra-args -Wformat-nonliteral -Wformat-security -Wno-format-zero-length

ifeq ($(USE_CLANG), y)
#our gcc version does not like struct initializer bla = {0};
#when using -Wextra. So we restrict this flag to clang for now.. 
MK_WARNINGFLAGS += -Wextra
endif
  
MK_WARNINGFLAGS += -Wswitch-default 
MK_WARNINGFLAGS += -Wswitch-enum

#-------------------------------------------------------------------
# Compiler Optimization
#-------------------------------------------------------------------
ifeq ($(DEBUG),n)
MK_CFLAGS += -O2
else
MK_CFLAGS += -g2 -O0
endif

#-------------------------------------------------------------------
# Compiler Flags
#-------------------------------------------------------------------

MK_CFLAGS += \
	-std=c99\
	-D_GNU_SOURCE\
	-fPIC\
	-DPIC\
	-fdiagnostics-show-option\
	-fstrict-aliasing\


#-------------------------------------------------------------------
# Libraries
#-------------------------------------------------------------------

MK_LDFLAGS += -rpath /usr/lib
#MK_LDFLAGS += -Wl,-rpath -Wl,/usr/lib -Wl,-rpath-link -Wl,$(SYSROOT)/usr/lib -L$(SYSROOT)/lib -L$(SYSROOT)/usr/lib
MK_LDFLAGS += -L$(SYSROOT)/lib
MK_LDFLAGS += -L$(SYSROOT)/usr/lib
MK_LDFLAGS += -rdynamic
MK_LDFLAGS += -lglib-2.0 -lm

#-------------------------------------------------------------------
# Sources
#-------------------------------------------------------------------

SRC_DIRS = \
	source\

#-------------------------------------------------------------------
# Includes
#-------------------------------------------------------------------

#we use both include paths: HOST and TARGET, since only one is valid at a time
INCLUDE_DIRS =\
	  $(SYSROOT)/usr/include/glib-2.0\
	  $(SYSROOT)/usr/lib/glib-2.0/include\
	  include\
	  $(SYSROOT)/include/glib-2.0\
	  $(SYSROOT)/lib/glib-2.0/include\

	  
#-------------------------------------------------------------------
# System Includes
#-------------------------------------------------------------------
	  
MK_CFLAGS += -isystem $(SYSROOT)/usr/include
MK_CFLAGS += -isystem $(SYSROOT)/include


include project/LibraryMakefileWorker.mk
