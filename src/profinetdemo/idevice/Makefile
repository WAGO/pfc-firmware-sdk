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
###  \version  $Revision: 53944 $
###
###  \brief    Makefile of the Profinet iDevice "C" Application
###
###  \author   Dietrich Derksen : WAGO Kontakttechnik GmbH & Co. KG
##----------------------------------------------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Defines
#------------------------------------------------------------------------------

$(info $(SYSROOT))

#------------------------------------------------------------------------------
# Target name
#------------------------------------------------------------------------------

TARGET = profinetdemo_idevice

#------------------------------------------------------------------------------
# Source files
#------------------------------------------------------------------------------

SRC = profinetdemo_idevice.c

#------------------------------------------------------------------------------
# Compiler flags
#------------------------------------------------------------------------------

CFLAGS += -std=gnu99

# Debug
CFLAGS += -O0 -g3 

# Warning flags
CFLAGS += -Wall -Wstrict-prototypes -fno-strict-aliasing

# Use thread save versions
CFLAGS += -D_REENTRANT 

#------------------------------------------------------------------------------
# Compiler includes
#------------------------------------------------------------------------------

CFLAGS += -I$(SYSROOT)/usr/include

#------------------------------------------------------------------------------
# Linker flags
#------------------------------------------------------------------------------

LDFLAGS += -lelf -lprofinet
LDFLAGS += -DLINUX -lpthread -lboost_system -lboost_thread -lrt

#------------------------------------------------------------------------------
# Linker search directories
#------------------------------------------------------------------------------

LDFLAGS += -L$(SYSROOT)/lib
LDFLAGS += -L$(SYSROOT)/usr/lib

#------------------------------------------------------------------------------
# PKG_CONFIG 
#------------------------------------------------------------------------------

CONFIG_LIBS=glib-2.0

CFLAGS += $(if $(strip $(CONFIG_LIBS)), $$($(PKG_CONFIG) --cflags $(CONFIG_LIBS) || echo "pkg-config-failed"),)
LDFLAGS += $(if $(strip $(CONFIG_LIBS)), $$($(PKG_CONFIG) --libs $(CONFIG_LIBS) || echo "pkg-config-failed"),)

#------------------------------------------------------------------------------
# Object files
#------------------------------------------------------------------------------

# Generate a object list from the source files
OBJ = $(SRC:.c=.o)

#------------------------------------------------------------------------------
# Targets
#------------------------------------------------------------------------------
all: $(OBJ)

	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS) 

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	@rm -f $(TARGET)
	@rm -f *.o

install:
	mkdir -p $(DESTDIR)/usr/sbin/
	cp $(TARGET) $(DESTDIR)/usr/sbin/

