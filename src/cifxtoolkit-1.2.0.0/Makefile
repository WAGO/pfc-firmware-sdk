#Set this to @ to keep the makefile quiet
SILENCE =

#-------------------------------------------------------------------
# Outputs
#-------------------------------------------------------------------
COMPONENT_NAME = cifXToolkit


#-------------------------------------------------------------------
# Inputs
#-------------------------------------------------------------------
PROJECT_HOME_DIR = .

DEBUG = n


#-------------------------------------------------------------------
# Compiler to use
#-------------------------------------------------------------------

ifeq ($(USE_CLANG), y)
CC=clang
LD=llvm-ld
CXX=clang++
endif


# sysroot-target directory (is controlled by ptx-dist rule)
# for local builds set this to your ptxdist platform path/sysroot-target

ifndef SYSROOT
$(error Environment variable SYSROOT not set properly!)
endif

VERSION=$(CIFXTOOLKIT_VERSION)

ifndef CIFXTOOLKIT_VERSION
VERSION=1.2.0.0
endif


#-------------------------------------------------------------------
# Compiler Warnings
#-------------------------------------------------------------------

MK_WARNINGFLAGS += -Wall
MK_WARNINGFLAGS += -Werror
MK_WARNINGFLAGS += -Wstrict-prototypes
MK_WARNINGFLAGS += -Wpointer-arith
MK_WARNINGFLAGS += -Wformat-y2k -Wno-format-extra-args -Wformat-security -Wno-format-zero-length

ifeq ($(USE_CLANG), y)
#our gcc version does not like struct initializer bla = {0};
#when using -Wextra. So we restrict this flag to clang for now..
MK_WARNINGFLAGS += -Wextra
endif

#MK_WARNINGFLAGS += -Wswitch-default
#MK_WARNINGFLAGS += -Wswitch-enum

#-------------------------------------------------------------------
# Compiler Optimization
#-------------------------------------------------------------------
MK_LDFLAGS := $(LDFLAGS)
MK_CFLAGS  := $(CFLAGS)
ifeq ($(DEBUG),n)
MK_CFLAGS += -Os -g
else
MK_CFLAGS += -ggdb3 -O0
endif

#-------------------------------------------------------------------
# Compiler Flags
#-------------------------------------------------------------------
MK_CFLAGS += \
	-fPIC\
	-DPIC\
	-std=c99\
	-DCIFX_TOOLKIT\
	-fdiagnostics-show-option\
	-D_GNU_SOURCE\


#-------------------------------------------------------------------
# Libraries
#-------------------------------------------------------------------

MK_LDFLAGS += -lrt
MK_LDFLAGS += -pthread
MK_LDFLAGS += -loslinux
MK_LDFLAGS += -L $(SYSROOT)/lib
MK_LDFLAGS += -L $(SYSROOT)/usr/lib
MK_LDFLAGS += -rdynamic


#-------------------------------------------------------------------
# Sources
#-------------------------------------------------------------------

SRC_DIRS = \
	Source\
	OSAbstraction\

#-------------------------------------------------------------------
# Includes
#-------------------------------------------------------------------

INCLUDE_DIRS = \
	Source\
	OSAbstraction\

#-------------------------------------------------------------------
# System Includes
#-------------------------------------------------------------------

MK_CFLAGS += -isystem $(SYSROOT)/usr/include
MK_CFLAGS += -isystem $(SYSROOT)/include


include Build/LibraryMakefileWorker.mk
