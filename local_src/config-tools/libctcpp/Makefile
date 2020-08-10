###############################################################################
#  Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
#
#  PROPRIETARY RIGHTS are involved in the subject matter of this material.
#  All manufacturing, reproduction, use and sales rights pertaining to this
#  subject matter are governed by the license agreement. The recipient of this
#  software implicitly accepts the terms of the license.
###############################################################################
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

NAME     = libctcpp.a

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
    OPTIMIZE = -O3 -g3 -gdwarf-4
endif
WARNINGS     = -Werror -Wall -Wextra -pedantic -Wshadow

INCLUDES    = -I./
LIBRARIES   =

L_CFLAGS    += -std=c99
L_CFLAGS    += $(OPTIMIZE)
L_CFLAGS    += $(WARNINGS)
L_CFLAGS    +=  $(shell $(PKG_CONFIG) --cflags glib-2.0 gio-2.0 libxml-2.0)
L_CFLAGS    += $(CONFIG)
#L_CFLAGS    += -fPIC

L_CXXFLAGS  += -std=c++0x
L_CXXFLAGS  += $(OPTIMIZE)
L_CXXFLAGS  += $(WARNINGS)
# -Weffc++ removed because of json library.
L_CXXFLAGS  += $(WARNINGS) -Weffc++
L_CXXFLAGS  +=  $(shell $(PKG_CONFIG) --cflags glib-2.0 gio-2.0 libxml-2.0)
L_CXXFLAGS  += $(CONFIG)
#L_CXXFLAGS  += -fPIC

ifneq ($(DEBUGPC), 0)
    L_CPPFLAGS  += -I../
    L_CPPFLAGS  += -DDEBUGPC
endif

L_LDCONFIG  +=  $(shell $(PKG_CONFIG) --libs glib-2.0 gio-2.0 libxml-2.0)
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

$(NAME): error.o filesys.o logs.o regex.o system.o x509.o xmlhlp.o
	$(AR) rsc $(NAME) $^
#	$(CXX) $(LDFLAGS) $(LDCONFIG) -o $(NAME) $^ -shared

error.o: error.cpp error.hpp Makefile
filesys.o: filesys.cpp filesys.hpp Makefile
logs.o: logs.cpp logs.hpp Makefile
regex.o: regex.cpp regex.hpp Makefile
system.o: system.cpp system.hpp Makefile
x509.o: x509.cpp x509.hpp Makefile
xmlhlp.o: xmlhlp.cpp xmlhlp.hpp Makefile


asm: asmcpp

asmcpp: error.cpp filesys.cpp logs.cpp regex.cpp system.cpp x509.cpp xmlhlp.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -g0 -S $^
