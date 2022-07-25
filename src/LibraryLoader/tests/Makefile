# Copyright (c) WAGO GmbH & Co. KG
#
# PROPRIETARY RIGHTS are involved in the subject matter of this material. All
# manufacturing, reproduction, use, and sales rights pertaining to this
# subject matter are governed by the license agreement. The recipient of this
# software implicitly accepts the terms of the license.

#Set this to @ to keep the makefile quiet
SILENCE = @

#---- check env vars ----#

ifndef CPPUTEST_HOME
$(error Environment variable CPPUTEST_HOME not set properly!)
endif

ifndef SYSROOT
$(error Environment variable SYSROOT not set properly!)
endif


GLIB_INCLUDES += $(SYSROOT)/include/glib-2.0 $(SYSROOT)/lib/glib-2.0/include
#GLIB_LDFLAGS += -lglib-2.0 -L$(SYSROOT)/lib

# for ffi.h and friends
CPPUTEST_CPPFLAGS += -I$(SYSROOT)/include 
CPPUTEST_CPPFLAGS += -D__CPPUTEST


GLIB_INCLUDES += $(SYSROOT)/usr/include/glib-2.0 $(SYSROOT)/usr/lib/glib-2.0/include
GLIB_LDFLAGS = -L$(SYSROOT)/lib
GLIB_LDLIBS = -lglib-2.0 

CPPUTEST_CPPFLAGS += -isystem $(SYSROOT)/usr/include
CPPUTEST_CPPFLAGS += -isystem $(SYSROOT)/include

#----Compiler----#

CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar

ifeq ($(USE_CLANG), y)
CC = clang 
CXX = clang++
LD = llvm-ld
#AR = llvm-ar
endif


GLIB_CFLAGS = $(addprefix -I,$(GLIB_INCLUDES))

#---- Outputs ----#
COMPONENT_NAME = library_loader

TARGET_LIB = \
	lib$(COMPONENT_NAME).a
	
TEST_TARGET = \
	tests/$(COMPONENT_NAME)_tests

#--- Inputs ----#
PROJECT_HOME_DIR = .

CPPUTEST_OBJS_DIR = project/objs_test
CPPUTEST_LIB_DIR = project/lib_test

CPP_PLATFORM = Gcc

CPPUTEST_USE_EXTENSIONS = Y
CPPUTEST_USE_GCOV = N

CPPUTEST_WARNINGFLAGS += -Wall 
CPPUTEST_WARNINGFLAGS += -Werror 
CPPUTEST_WARNINGFLAGS += -Wswitch-default 
CPPUTEST_WARNINGFLAGS += -Wswitch-enum -Wextra 
#CPPUTEST_WARNINGFLAGS += -pedantic -pedantic-errors 

CPPUTEST_CPPFLAGS+= -O0 -ggdb2

CPPUTEST_CFLAGS += -std=c99
CPPUTEST_CFLAGS += -fdiagnostics-show-option
CPPUTEST_CFLAGS += -Wstrict-prototypes
CPPUTEST_CFLAGS += -D__STATIC=
CPPUTEST_CFLAGS += -D_GNU_SOURCE
CPPUTEST_CFLAGS += -D__CPPUTEST
CPPUTEST_CFLAGS += -DLOGLEVEL=6 #INFO


CPPUTEST_CFLAGS += $(GLIB_CFLAGS)

#CPPUTEST_CPPFLAGS += -R tests

#Set the library search path to 'tests'
CPPUTEST_LDFLAGS += -Wl,-rpath,tests -Wl,-rpath,$(SYSROOT)/lib 

CPPUTEST_LDFLAGS += $(GLIB_LDFLAGS)
CPPUTEST_LDFLAGS += -L$(SYSROOT)/lib
CPPUTEST_LDFLAGS += -L$(SYSROOT)/usr/lib
CPPUTEST_LDFLAGS += -rdynamic

LD_LIBRARIES = -ldl -lm $(GLIB_LDLIBS)

SRC_DIRS = \
	source\


TEST_SRC_DIRS = \
	tests\
	tests/LibraryLoader_tests\
	
MOCKS_SRC_DIRS = \
	mocks\
	
	
		
INCLUDE_DIRS =\
	mocks\
	$(CPPUTEST_HOME)/include/\
	include\
	$(GLIB_INCLUDES)

include $(CPPUTEST_HOME)/build/MakefileWorker.mk
