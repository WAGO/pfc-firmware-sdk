# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#THIS PART CAN BE MODIFIED
ifneq (CROSS_COMPILE, "")
ifdef $(CROSS_COMPILE)
CC=$(CROSS_COMPILE)gcc
CXX=$(CROSS_COMPILE)g++
LD=$(CROSS_COMPILE)ld
endif
endif

#Binary Name
LIBNAME=pthreadcgroup
#Version of your binary
ifdef LIBPTHREADWRAPPER_VERSION
VERSION=$(LIBPTHREADWRAPPER_VERSION)
else
VERSION=0.0.1
endif
# Target file
TARGET=lib$(LIBNAME).a

#Sources
SRC = pthread_wrapper.c helper.c cgrules.c log.c
#DBGMODE can also be overwritten by rule-file
DBGMODE=-g
#OPTIMIZE can also be overwritten by rule-file
OPTIMIZE=-Os

#do you want any special C-FLAGS

# compiler warnings
WARNINGS =   					\
	-Wall 						\
	-Wextra 					\
	-Wcast-qual 				\
	-Wcast-align 				\
	-Wconversion 				\
	-Wlogical-op 				\
	-pedantic					\

#call pkg_config if neccessary

PKG_CONFIGS= libcgroup

CFLAGS += $(if $(strip $(PKG_CONFIGS)), $$($(PKG_CONFIG) --cflags $(PKG_CONFIGS) || echo "pkg-config-failed"),)
LDFLAGS += $(if $(strip $(PKG_CONFIGS)), $$($(PKG_CONFIG) --libs $(PKG_CONFIGS) || echo "pkg-config-failed"),)


#create object names
OBJ = $(SRC:.c=.o)
# dependency files
DEPS =$(SRC:.c=.d)
# add some CFLAGS. do not emove the first += !!!
CFLAGS += -fpic
CFLAGS += -pthread -D_GNU_SOURCE -std=c99 $(WARNINGS)
CFLAGS += $(CPPFLAGS)
CFLAGS += $(DBGMODE) $(OPTIMIZE)
LDFLAGS += -ldl -lc -pthread
CFLAGS += 						\
	-fno-common 				\
	-fmessage-length=0 			\
	-fno-omit-frame-pointer		\
	-rdynamic					\


all: $(TARGET)

#some make rules
$(TARGET): $(OBJ)
#	$(CC) -shared $(CFLAGS) $(OBJ) $(LDFLAGS) -Wl,-soname,lib$(LIBNAME).so -o $@
	$(AR) rcs $@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -MD -c $< -o $@

## clean all kinds of objects
clean:
	-@rm -rf *.o *.so.* *.d *.so

-include $(DEPS)
