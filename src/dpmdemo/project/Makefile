##----------------------------------------------------------------------------------------------------------------------
### Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
###
### PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
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
###  \version  $Revision: 17216 $
###
###  \brief    Makefile of the "C" Application
###
###  \author   rs : WAGO Kontakttechnik GmbH & Co. KG
##----------------------------------------------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Defines
#------------------------------------------------------------------------------

CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld

# target name
TARGET = dpmdemo

#------------------------------------------------------------------------------
# Build options (these have to be defined here)
#------------------------------------------------------------------------------

# Debug (compile with debugging information and without optimization)
DEBUG = y

#------------------------------------------------------------------------------
# Source files
#------------------------------------------------------------------------------
# path to the project root
PATH_TO_PROJECT_ROOT = ..

# source directory
SRC_DIR = $(PATH_TO_PROJECT_ROOT)/source


# plc sources
SRC = $(SRC_DIR)/dpm_demo.c
SRC += $(SRC_DIR)/menu/menu.c
SRC += $(SRC_DIR)/sdi/sdi.c
SRC += $(SRC_DIR)/device/device_type.c
SRC += $(SRC_DIR)/master/profibus_master.c

#------------------------------------------------------------------------------
# Compiler flags
#------------------------------------------------------------------------------

# Debug
ifeq ($(DEBUG),n)
CFLAGS += -Os 
else
CFLAGS += -O0 -g3 
endif

# warning flags
CFLAGS += -Wall -Wstrict-prototypes -fno-strict-aliasing

# use thread save versions
CFLAGS += -D_REENTRANT 

#------------------------------------------------------------------------------
# Compiler includes
#------------------------------------------------------------------------------

CFLAGS += -I$(DIST_DIR)/sysroot-target/usr/include 
CFLAGS += -I$(DIST_DIR)/sysroot-target/usr/include/glib-2.0
CFLAGS += -I$(DIST_DIR)/sysroot-target/usr/lib/glib-2.0/include

# PROFIBUS slave application includes
CFLAGS += -I$(SYSROOT_TARGET)/usr/include/pbdpm

# DAL include
CFLAGS += -I$(SYSROOT_TARGET)/usr/include/dal

CFLAGS += -I$(SRC_DIR)
CFLAGS += -I$(SRC_DIR)/menu
CFLAGS += -I$(SRC_DIR)/sdi
CFLAGS += -I$(SRC_DIR)/device
CFLAGS += -I$(SRC_DIR)/master


#------------------------------------------------------------------------------
# Linker flags
#------------------------------------------------------------------------------

LDDFLAGS += -I$(DIST_DIR)/sysroot-target/usr/lib/libdal.a
LDDFLAGS += -Wl,-rpath
LDDFLAGS += -lgcc_s -lm -ldal -ldl -lrt -ldbus-glib-1 -ldbus-1 
LDDFLAGS += -lgobject-2.0 -lglib-2.0 -lgthread-2.0 -lffi -llibloader
LDDFLAGS += -lpthread -luio

#------------------------------------------------------------------------------
# Linker search directories
#------------------------------------------------------------------------------

LDDFLAGS += -L$(DIST_DIR)/sysroot-target/lib
LDDFLAGS += -L$(DIST_DIR)/sysroot-target/usr/lib

EXTRALIBS += $(DIST_DIR)/sysroot-target/usr/lib/libgobject-2.0.so
EXTRALIBS += $(DIST_DIR)/sysroot-target/usr/lib/libgio-2.0.so.0 
EXTRALIBS += $(DIST_DIR)/sysroot-target/usr/lib/libgmodule-2.0.so.0


#------------------------------------------------------------------------------
# Object files
#------------------------------------------------------------------------------

# Generate a object list from the source files
OBJ = $(SRC:.c=.o)

#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

# Compilation Rule
%.o: %.c 
	@echo "CC $<"
	@$(CC) -c $(CFLAGS) $*.c -o $*.o

#------------------------------------------------------------------------------
# Targets
#------------------------------------------------------------------------------
all: .build $(OBJ) $(TARGET)

$(TARGET): $(OBJ)
	@echo "LD $(TARGET)"
	@$(CC) $(CFLAGS) $(OBJ) $(LDDFLAGS) $(EXTRALIBS) -o $@

.build:
	@echo " "
	@echo "Compiling the sources..."

.copy_target:
	cp $(TARGET) $(DIST_DIR)/root/usr/sbin
	
.clean:
	@echo "RM *.o"
	@find -name '*.o' | xargs rm -f
	@echo "RM $(TARGET)"
	@rm -f $(TARGET)
	
clean:
	@echo "RM *.o"
	@find -name '*.o' | xargs rm -f
	@echo "RM $(TARGET)"
	@rm -f $(TARGET)
