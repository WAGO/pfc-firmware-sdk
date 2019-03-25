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
###  \version  $Revision: 28122 $
###
###  \brief    Makefile of the "C" Application
###
###  \author   wue : WAGO Kontakttechnik GmbH & Co. KG
##----------------------------------------------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Defines
#------------------------------------------------------------------------------

CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld

# target name
BUILDTARGET = candemo_layer2

#------------------------------------------------------------------------------
# Build options (these have to be defined here)
#------------------------------------------------------------------------------

# Debug (compile with debugging information and without optimization)
DEBUG = n

#------------------------------------------------------------------------------
# Source files
#------------------------------------------------------------------------------

# plc sources
SRC = candemo_layer2.c

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

# include ct-build rules
#CC_WARNINGS_DISABLE_STRICT=T
include $(PTXDIST_PLATFORMDIR)/sysroot-host/lib/ct-build/cc_warnings_1.mk

#------------------------------------------------------------------------------
# Compiler includes
#------------------------------------------------------------------------------

CFLAGS += -I$(DIST_DIR)/sysroot-target/usr/include 
CFLAGS += -I$(DIST_DIR)/sysroot-target/usr/include/glib-2.0
CFLAGS += -I$(DIST_DIR)/sysroot-target/usr/lib/glib-2.0/include

#------------------------------------------------------------------------------
# Linker flags
#------------------------------------------------------------------------------

LDDFLAGS += -I$(DIST_DIR)/sysroot-target/usr/lib/libdal.a
LDDFLAGS += -Wl,-rpath
LDDFLAGS += -lgcc_s -lm -ldal -ldl -lrt -ldbus-glib-1 -ldbus-1 
LDDFLAGS += -lgobject-2.0 -lglib-2.0 -lgthread-2.0 -lffi -llibloader
LDDFLAGS += -lpthread 

EXTRALIBS += $(DIST_DIR)/sysroot-target/usr/lib/libgobject-2.0.so
EXTRALIBS += $(DIST_DIR)/sysroot-target/usr/lib/libgio-2.0.so.0 
EXTRALIBS += $(DIST_DIR)/sysroot-target/usr/lib/libgmodule-2.0.so.0
EXTRALIBS += $(DIST_DIR)/sysroot-target/usr/lib/libz.so.1
EXTRALIBS += $(DIST_DIR)/sysroot-target/usr/lib/libsocketcan.so

#------------------------------------------------------------------------------
# Linker search directories
#------------------------------------------------------------------------------

LDDFLAGS += -L$(DIST_DIR)/sysroot-target/lib
LDDFLAGS += -L$(DIST_DIR)/sysroot-target/usr/lib

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
all: .build $(OBJ) $(BUILDTARGET)

$(BUILDTARGET): $(OBJ)
	@echo "LD $(BUILDTARGET)"
	@$(CC) $(CFLAGS) $(OBJ) $(LDDFLAGS) $(EXTRALIBS) -o $@

.build:
	@echo " "
	@echo "Compiling the sources..."

.copy_target:
	cp $(BUILDTARGET) $(DIST_DIR)/root/usr/sbin
	
.clean:
	@echo "RM *.o"
	@find -name '*.o' | xargs rm -f
	@echo "RM $(BUILDTARGET)"
	@rm -f $(BUILDTARGET)
	
clean:
	@echo "RM *.o"
	@find -name '*.o' | xargs rm -f
	@echo "RM $(BUILDTARGET)"
	@rm -f $(BUILDTARGET)
