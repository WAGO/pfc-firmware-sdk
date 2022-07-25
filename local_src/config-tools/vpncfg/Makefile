# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG
#
#
# FEW NOTES ABOUT THE MAKEFILE
#
# -O3 -g3 -gdwarf-4
# Since O3 optimization is way more aggresive than Os and O2 it may reveal some
# issues which could stay hidden otherwise. Of course this makes debugging quite
# hard, so for regural checks just change it into O0.
# Even with optimized program some debug capabilites may be in order. For end
# compilation this may be changed to -g0, but for development let it be.
#
# -std=c++0x
# Please be careful about using c++11 features. The toolchain which is in use is
# based on gcc 4.5.x which has only few of the c++11 capabilities. TEST if all
# code using it is actually compiling for the target platform.
#

NAME     = vpncfg

# Unrem the line if error messages should be shown.
# CONFIG      += -DSHOW_ERRORS

# Set to 0 in order to produce release code, to anything else for debug code.
DEBUG        = 0

# Set to value other than 0 in order to compile for (test) PC platform. This
# disables logging to common file which requires additional library.
DEBUGPC      = 0


ifeq ($(DEBUG), 0)
    OPTIMIZE = -Os -g0 -DNDEBUG
else
    OPTIMIZE = -O0 -g3 -gdwarf-4
endif
WARNINGS     = -Werror -Wall -Wextra -pedantic -Wshadow

#OPTIONS      = -fdump-ipa-all -fdump-tree-all

INCLUDES    = -I../libctcpp/
LIBRARIES   = -lssl -lcrypto


L_CFLAGS    += -std=c99
L_CFLAGS    += $(OPTIMIZE)
L_CFLAGS    += $(OPTIONS)
L_CFLAGS    += $(WARNINGS)
L_CFLAGS    += `pkg-config --cflags glib-2.0 gio-2.0 libxml-2.0 jsoncpp`
L_CFLAGS    += $(CONFIG)
L_CFLAGS    += $(INCLUDES)

L_CXXFLAGS  += -std=c++0x
L_CXXFLAGS  += $(OPTIMIZE)
L_CXXFLAGS  += $(OPTIONS)
L_CXXFLAGS  += $(WARNINGS) -Weffc++
L_CXXFLAGS  += `pkg-config --cflags glib-2.0 gio-2.0 libxml-2.0 jsoncpp`
L_CXXFLAGS  += $(CONFIG)
L_CXXFLAGS  += $(INCLUDES)

#L_LDCONFIG  += `pkg-config --libs glib-2.0 gio-2.0 libxml-2.0 jsoncpp`
L_LDCONFIG  += $(LIBRARIES)

ifneq ($(DEBUGPC), 0)
    L_CPPFLAGS  += -I../
    L_CPPFLAGS  += -DDEBUGPC
endif

L_LDCONFIG  += `pkg-config --libs glib-2.0 gio-2.0 libxml-2.0 jsoncpp`
ifeq ($(DEBUGPC), 0)
    L_LDCONFIG  += -L../liblog -lctlog
endif


override CFLAGS += $(L_CFLAGS)
override CXXFLAGS += $(L_CXXFLAGS)
override CPPFLAGS += $(L_CPPFLAGS)
override LDCONFIG += $(L_LDCONFIG)


.PHONY: all clean asm asmcpp

all: $(NAME)

clean:
	$(RM) *.s
	$(RM) *.o
	$(RM) $(NAME)

$(NAME): main.o services.o vpncfg.o
	$(CXX) $(LDFLAGS) -o $(NAME) $^ ../libctcpp/libctcpp.a $(LDCONFIG)

main.o: main.cpp Makefile
services.o: services.cpp services.hpp Makefile
vpncfg.o: vpncfg.cpp vpncfg.hpp Makefile

asm: asmcpp

asmcpp: main.cpp services.cpp vpncfg.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -g0 -S $^

