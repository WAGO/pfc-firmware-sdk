###############################################################################
#  Copyright (c) WAGO GmbH & Co. KG
#
#  PROPRIETARY RIGHTS are involved in the subject matter of this material.
#  All manufacturing, reproduction, use and sales rights pertaining to this
#  subject matter are governed by the license agreement. The recipient of this
#  software implicitly accepts the terms of the license.
###############################################################################

RM=rm -rf
LIBNAME=libwagocrypt

.phony: all clean

all: $(LIBNAME)

$(LIBNAME): libwagocrypt.c
	$(CC) -Wall -fPIC -c libwagocrypt.c wide-interface.c
	$(CC) -shared -Wl,-soname,$(LIBNAME).so -o $(LIBNAME).so libwagocrypt.o wide-interface.o -lssl -lcrypto -lpthread -ldl -lrt

clean:
	$(RM) *.o *.so
