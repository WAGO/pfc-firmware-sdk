#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
#
#######################################################################################################################
#Set this to @ to keep the makefile quiet
#SILENCE = @

#---- check env vars ----#

ifdef _RUN_ON_HOST
CPPUTEST_CPPFLAGS += -D__UNIT_TESTS_HOST__ 
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
COMPONENT_NAME = ct_libnet

TARGET_LIB = \
	lib$(COMPONENT_NAME).a
	
TEST_TARGET = \
	$(COMPONENT_NAME)_tests_pfc200

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

CPPUTEST_CXXFLAGS += -std=gnu++0x

CPPUTEST_USE_EXTENSIONS = Y
CPPUTEST_USE_GCOV = N

CPPUTEST_WARNINGFLAGS += -Wall 
CPPUTEST_WARNINGFLAGS += -Werror 
CPPUTEST_WARNINGFLAGS += -Wswitch-default 
CPPUTEST_WARNINGFLAGS += -Wswitch-enum -Wextra 
#CPPUTEST_WARNINGFLAGS += -pedantic -pedantic-errors 

CPPUTEST_CPPFLAGS+= -O0 -ggdb2

CPPUTEST_CFLAGS += -I$(_SYSROOT)/include -I$(_SYSROOT)/usr/include
CPPUTEST_CFLAGS += -std=gnu99
CPPUTEST_CFLAGS += -fdiagnostics-show-option
CPPUTEST_CFLAGS += -Wstrict-prototypes
CPPUTEST_CFLAGS += -D__CPPUTEST__
CPPUTEST_CFLAGS += -D__ENABLE_DSA

######################################################################################
#
# Stuff specific to our code
#
######################################################################################

ifdef _RUN_ON_HOST
LIBXML_CFLAGS=$$(pkg-config --cflags libxml-2.0)
LIBXML_LDLIBS=$$(pkg-config --libs libxml-2.0)

LIBNL_CFLAGS=$$(pkg-config --cflags libnl-3.0)
LIBNL_LDLIBS=$$(pkg-config --libs libnl-3.0 libnl-route-3.0 libnl-genl-3.0)

LIBXSLT_CFLAGS=$$(pkg-config --cflags libxslt)
LIBXSLT_LDLIBS=$$(pkg-config --libs libxslt)

GLIB_CFLAGS=$$(pkg-config --cflags glib-2.0)
GLIB_LDLIBS=$$(pkg-config --libs glib-2.0)

CPPUTEST_CFLAGS += -D__HAVE_INITMETHOD_SYSV $(LIBXML_CFLAGS) $(LIBNL_CFLAGS) $(LIBXSLT_CFLAGS) $(GLIB_CFLAGS)
CPPUTEST_CXXFLAGS += $(GLIB_CFLAGS)
endif


ifneq ($(_RUN_ON_HOST), yes)
CPPUTEST_LDFLAGS += -L$(_SYSROOT)/lib
CPPUTEST_LDFLAGS += -L$(_SYSROOT)/usr/lib
endif

CPPUTEST_LDFLAGS += -rdynamic -Wl,--unresolved-symbols=ignore-in-object-files

CPPUTEST_LDFLAGS += $(LIBXML_LDLBS) $(LIBNL_LDLIBS) $(LIBXSLT_LDLIBS) $(GLIB_LDLIBS)

SRC_DIRS = $(CURDIR)/..

TEST_SRC_DIRS = $(CURDIR)

INCLUDE_DIRS = $(CURDIR)/.. $(CPPUTEST_HOME)/include

.PHONY: network-interfaces.xml

include $(CPPUTEST_HOME)/build/MakefileWorker.mk

network-interfaces.xml:
	cp $(shell readlink -f ../../../../)/ptxproj.2013.03.0/projectroot/etc/specific/network-interfaces.xml . 
