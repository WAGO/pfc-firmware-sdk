##----------------------------------------------------------------------------------------------------------------------
### Copyright (c) WAGO GmbH & Co. KG
###
### PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
### the subject matter of this material. All manufacturing, reproduction,
### use, and sales rights pertaining to this subject matter are governed
### by the license agreement. The recipient of this software implicitly
### accepts the terms of the license.
###
##----------------------------------------------------------------------------------------------------------------------
##----------------------------------------------------------------------------------------------------------------------
###
###  \file     makefile
###
###  \version  $Id:
###
###  \brief    This is the makefile of the software component "libosal".
###							Makefile parameters are:
###							- CROSS_COMPILE            cross compiler specific präfix(e.g. <arm->)
###							- SYSROOT_TARGET           path to the targets system root (base path for header and libraries)
###							- TARGET_PLATFORM          name of the target platform is used for output directory name
###							- TARGET_OS                operating system of the target (<LINUX>,<ECOS>,<WIN32>)
###							- DEBUG          					 <n> : build as release else build with debug information
###
###  \author   Wauer : WAGO GmbH & Co. KG
##----------------------------------------------------------------------------------------------------------------------


#-----------------------------------------------------------------------------------------------------------------------
# Defines
#-----------------------------------------------------------------------------------------------------------------------

# environment
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar

# target name
TARGET = osal

# target version
VERSION = 1.2.1

# target library name
TARGET_STATIC_LIB = $(addsuffix .a,$(addprefix lib,$(TARGET)))

# target library name
TARGET_SHARED_OBJ = $(addsuffix .so,$(addprefix lib,$(TARGET)))

# target executable name
TARGET_EXEC = $(TARGET)

# path to the project root
PATH_TO_PROJECT_ROOT = .

# source directory
SRC_DIR = $(PATH_TO_PROJECT_ROOT)/sources

# include directory
INCLUDE_DIR = $(PATH_TO_PROJECT_ROOT)/includes

# output directory
ifeq ($(DEBUG),n)
  OUT_DIR = $(PATH_TO_PROJECT_ROOT)/output/$(TARGET_PLATFORM)/release
else
  OUT_DIR = $(PATH_TO_PROJECT_ROOT)/output/$(TARGET_PLATFORM)/debug
endif

# common make targets
MAKE_TARGETS = $(PATH_TO_PROJECT_ROOT)/targets.mak

#-----------------------------------------------------------------------------------------------------------------------
# Build options (these have to be defined here)
#-----------------------------------------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------------------------------------
# Source files
#-----------------------------------------------------------------------------------------------------------------------

#osal source files
SRC += $(SRC_DIR)/linux_csection.c
SRC += $(SRC_DIR)/linux_memory.c
SRC += $(SRC_DIR)/linux_mutex.c
SRC += $(SRC_DIR)/linux_semaphore.c
SRC += $(SRC_DIR)/linux_thread.c
SRC += $(SRC_DIR)/linux_time.c
SRC += $(SRC_DIR)/linux_timer.c
SRC += $(SRC_DIR)/linux.c

#-----------------------------------------------------------------------------------------------------------------------
# Compiler flags
#-----------------------------------------------------------------------------------------------------------------------

# Debug
ifeq ($(DEBUG),n)
CFLAGS += -O3
else
CFLAGS += -O0 -ggdb3
endif

# warning options
CFLAGS += -Wall -Wextra -Wstrict-prototypes -fno-strict-aliasing

# position-independent code (for shared objects)
CFLAGS += -fPIC

#-----------------------------------------------------------------------------------------------------------------------
# Compiler definitions
#-----------------------------------------------------------------------------------------------------------------------

# osal defines
CFLAGS += -D$(TARGET_OS)

# Target architecture ARM definition
ifeq ($(TARGET_ARCH),arm)
CFLAGS += -D__ARM_ARCH__=$(ARM_ARCH_VERSION)
endif

# glibc
CFLAGS += -D_GNU_SOURCE

#-----------------------------------------------------------------------------------------------------------------------
# Compiler includes
#-----------------------------------------------------------------------------------------------------------------------

# system include paths
CFLAGS += -isystem $(SYSROOT_TARGET)/usr/include

# common include paths
CFLAGS += -I$(INCLUDE_DIR)

#-----------------------------------------------------------------------------------------------------------------------
# Linker flags
#-----------------------------------------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------------------------------------
# Linker libraries
#-----------------------------------------------------------------------------------------------------------------------

LDDFLAGS += -lrt

#-----------------------------------------------------------------------------------------------------------------------
# Linker search directories
#-----------------------------------------------------------------------------------------------------------------------

LDDFLAGS += -L$(SYSROOT_TARGET)/usr/lib

#-----------------------------------------------------------------------------------------------------------------------
# Object files
#-----------------------------------------------------------------------------------------------------------------------

# Generate a object list from the source files
OBJ            = $(subst .c,.o,$(SRC))

# Generate a output object list from the object list
OUT_OBJ        = $(addprefix $(OUT_DIR)/,$(subst $(PATH_TO_PROJECT_ROOT)/,,$(OBJ)))

#-----------------------------------------------------------------------------------------------------------------------
# Common object file lists
#-----------------------------------------------------------------------------------------------------------------------

# object to be compiled (with '.c' preefix sources)
CC_OUT_OBJ 				 = $(OUT_OBJ)
# object files for the static library
STATIC_LIB_OUT_OBJ = $(OUT_OBJ)
# object files for the shared library
SHARED_LIB_OUT_OBJ = $(OUT_OBJ)

#-----------------------------------------------------------------------------------------------------------------------
# Include common targets
#-----------------------------------------------------------------------------------------------------------------------
include $(MAKE_TARGETS)
