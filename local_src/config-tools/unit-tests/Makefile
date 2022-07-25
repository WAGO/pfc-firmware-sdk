###############################################################################
#  Copyright (c) WAGO GmbH & Co. KG
#
#  PROPRIETARY RIGHTS are involved in the subject matter of this material.
#  All manufacturing, reproduction, use and sales rights pertaining to this
#  subject matter are governed by the license agreement. The recipient of this
#  software implicitly accepts the terms of the license.
###############################################################################

#Set this to @ to keep the makefile quiet
#SILENCE = @

#---- check env vars ----#

ifdef _RUN_ON_HOST
CPPUTEST_CPPFLAGS += -D__UNIT_TESTS_HOST__ 
CPPUTEST_CPPFLAGS += -DDEVICE_MEDIA_FILENAME=\"./data_stubs/DEVICE_MEDIA\"
else
CPPUTEST_CPPFLAGS += -D__UNIT_TESTS_TARGET__
endif

ifndef _SYSROOT
$(error Environment variable _SYSROOT not set properly!)
endif

ifndef CPPUTEST_HOME
$(error Environment variable CPPUTEST_HOME not set properly!)
endif

#---- Outputs ----#
COMPONENT_NAME = config-tools

TARGET_LIB = \
	lib$(COMPONENT_NAME).a
	
TEST_TARGET = \
	$(COMPONENT_NAME)_tests_pac200

#	$(CURDIR)/$(COMPONENT_NAME)_tests_pac200

#--- Inputs ----#
PROJECT_HOME_DIR = .

CPPUTEST_OBJS_DIR = project/objs_test
CPPUTEST_LIB_DIR = project/lib_test

CPP_PLATFORM = Gcc

ifeq ($(USE_CLANG), y)
CC = clang 
CXX = clang++
endif

CPPUTEST_USE_EXTENSIONS = Y
CPPUTEST_USE_GCOV = N

CPPUTEST_WARNINGFLAGS += -Wall 
#CPPUTEST_WARNINGFLAGS += -Werror 
CPPUTEST_WARNINGFLAGS += -Wswitch-default 
CPPUTEST_WARNINGFLAGS += -Wswitch-enum -Wextra 
#CPPUTEST_WARNINGFLAGS += -pedantic -pedantic-errors 

CPPUTEST_CPPFLAGS+= -O0 -ggdb2

CPPUTEST_CFLAGS += -I$(_SYSROOT)/include -I$(_SYSROOT)/usr/include
CPPUTEST_CFLAGS += -std=gnu99 -pthread
CPPUTEST_CFLAGS += -pthread
CPPUTEST_CFLAGS += -fdiagnostics-show-option
CPPUTEST_CFLAGS += -Wstrict-prototypes
CPPUTEST_CFLAGS += -D__CPPUTEST__
CPPUTEST_CFLAGS += -D__HAVE_INITMETHOD_SYSV

# our paths are longer than the standard max value
CPPUTEST_CFLAGS += -DMAX_LENGTH_SYSTEM_CALL=256

CPPUTEST_CPPFLAGS += -I$(_SYSROOT)/usr/include
CPPUTEST_CPPFLAGS += -I$(_SYSROOT)/usr/include/glib-2.0
CPPUTEST_CPPFLAGS += -I$(_SYSROOT)/usr/lib/glib-2.0/include

ifneq ($(_RUN_ON_HOST), yes)
CPPUTEST_LDFLAGS += -L$(_SYSROOT)/lib
CPPUTEST_LDFLAGS += -L$(_SYSROOT)/usr/lib
CPPUTEST_LDFLAGS += -ltypelabel
# libtypelabel is a wago lib
endif

CPPUTEST_LDFLAGS += -rdynamic
CPPUTEST_LDFLAGS += -ludev -lrt




CPPUTEST_LDFLAGS += -lglib-2.0

SRC_DIRS = $(CURDIR)/..

TEST_SRC_DIRS = $(CURDIR)

INCLUDE_DIRS = $(CURDIR)/.. $(CPPUTEST_HOME)/include

include $(CPPUTEST_HOME)/build/MakefileWorker.mk
