###############################################################################
#  Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
#
#  PROPRIETARY RIGHTS are involved in the subject matter of this material.
#  All manufacturing, reproduction, use and sales rights pertaining to this
#  subject matter are governed by the license agreement. The recipient of this
#  software implicitly accepts the terms of the license.
###############################################################################
CC=$(CROSS_COMPILE)gcc

GLIB_CFLAGS  = -I$(_SYSROOT)/usr/include/glib-2.0
GLIB_CFLAGS += -I$(_SYSROOT)/usr/lib/glib-2.0/include

ifeq ($(_HW_PLATFORM), vtp-ctp)
GLIB_CFLAGS  += -I$(_SYSROOT)/usr/include
endif

GLIB_LDLIBS = -lglib-2.0

override LDFLAGS += -D$(EXTRAS) -L. -L./libnet -L./liblog -Wl,--as-needed
override LDLIBS += -ludev -lcrypt $(GLIB_LDLIBS)

ifeq ($(WITH_LIBOMS),yes)
override LDLIBS+= -loms
endif

ifeq ($(WITH_LIBMODBUSCONFIG),yes)
override LDLIBS+= -lModbusConfig
endif

ifeq ($(WITH_LIBBACNETCONFIG),yes)
override CFLAGS += -I$(_SYSROOT)/usr/include/bacnet
override LDLIBS += -lbacnetconfig
endif

ifeq ($(WITH_LIBCONFIGMDMD),yes)
override LDLIBS+= -ldiagnostic
endif

ifeq ($(WITH_LIBTYPELABEL),yes)
override LDLIBS+= -ltypelabel
endif

ifeq ($(WITH_LIBWAGODBUS),yes)
override CFLAGS += -I$(_SYSROOT)/usr/include/dbus-1.0
override CFLAGS += -I$(_SYSROOT)/usr/lib/dbus-1.0/include
override LDLIBS += -ldbus-1
override LDLIBS += $$(pkg-config --libs wago_dbus)
override LDLIBS += -lgobject-2.0
override LDLIBS += -lgio-2.0
endif

override CFLAGS += -D$(EXTRAS) $(GLIB_CFLAGS) -D_GNU_SOURCE

override CFLAGS+=-Os -ggdb3

# New network-related config tools that use libctnetwork.
# They are compiled using the same rules as the tools in the main list, the dedicated NEWNET variable
# has solely semantic meaning
NEWNET :=  set_network_interfaces get_actual_eth_config get_eth_config config_ethernet config_interfaces get_dsa_mode set_dsa_mode config_dnsmasq get_switch_settings config_switch ipdatacheck

# Special config-tools needing own recipes
SPECIAL := get_filesystem_data get_rts_info

# Complete list of all config-tools
MAIN_LIST := wdialog show_video_mode crypt config_linux_user get_coupler_details get_rts3scfg_value get_user get_touchscreen_config get_dns_server get_clock_data get_port_state calculate_broadcast print_program_output get_device_data get_snmp_data string_encode get_uimage_size get_typelabel_value get_run_stop_switch_value get_possible_runtimes modbus_config config_mdmd urlencode set_serial_mode bacnet_config bacnet_backup_restore ${NEWNET}

# Config tools build in their own subdirectories with their own makefiles
SUBDIRS := firewall vpncfg

all: $(MAIN_LIST) $(SPECIAL) $(SUBDIRS)

##############################################
${MAIN_LIST}: %: %.o libnet/libctnetwork.so liblog/libctlog.so libctcommon.so
	$(CC) $(LDFLAGS) -o $@ $< $(LDLIBS$(LDLIBS-$(@))) -lctcommon -lctnetwork -lctlog

.PHONY: ${SUBDIRS}
${SUBDIRS}:
	$(MAKE) -C $@

vpncfg:	libctcpp/libctcpp.a liblog/libctlog.so

#############################################
# libctcommon.so (former statically linked config_tool_lib.o)

config_tool_lib.o: config_tool_lib.c config_tool_lib.h
	$(CC) $(CFLAGS) -c -fPIC -o $@ $<

libctcommon.so: config_tool_lib.o
	$(CC) $(LDFLAGS) -o $@ $< $(LDLIBS$(LDLIBS-$(@))) -shared


#############################################
# Basic C++ config-tools helper functions (a static library)
libctcpp/libctcpp.a:
	make --directory ./libctcpp

#############################################
# Common logging library
liblog/libctlog.so:
	make --directory ./liblog

#############################################
# Common network-related functions

# LIBSWCONFIG is a switch configuration library needed to switch DSA mode
ifneq (,$(findstring ENABLE_DSA,$(CFLAGS)))
WITH_SWCONFIG=yes
else
WITH_SWCONFIG=no
endif

libnet/libctnetwork.so:
	make --directory ./libnet WITH_SWCONFIG=$(WITH_SWCONFIG)

##############################################
# get_filesystem_data

GET_FILESYSTEM_DATA_FILES=get_filesystem_data_common.o

get_filesystem_data: $(GET_FILESYSTEM_DATA_FILES) libctcommon.so
	$(CC) $(LDFLAGS) -o $@ $(GET_FILESYSTEM_DATA_FILES) $(LDLIBS$(LDLIBS-$(@))) -lctcommon

##############################################
# get_rts_info

get_rts_info: get_rts_info.o config_tool_msg_com.o libctcommon.so
	$(CC) $(LDFLAGS) -o $@ get_rts_info.o config_tool_msg_com.o config_tool_lib.o $(LDLIBS$(LDLIBS-$(@))) -lrt -lctcommon

clean:
	-rm -f wdialog set_network_interfaces get_actual_eth_config show_video_mode crypt config_linux_user get_rts3scfg_value get_ntp_config get_user get_coupler_details get_eth_config get_touchscreen_config get_dns_server get_clock_data get_port_state calculate_broadcast get_filesystem_data print_program_output get_rts_info get_filesystem_data get_device_data get_snmp_data string_encode get_uimage_size pfc200_ethtool get_typelabel_value get_run_stop_switch_value get_possible_runtimes modbus_config config_mdmd urlencode bacnet_config bacnet_backup_restore *.gdb *.o

rebuild: clean all
