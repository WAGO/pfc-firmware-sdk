# -*-makefile-*-
# $Id$
#
# Copyright (C) 2008 by Stefanie Meihöfer
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CONFIG_TOOLS) += config-tools

#
# Paths and names
#
CONFIG_TOOLS_VERSION 	      := 1.1.7
CONFIG_TOOLS		            := config-tools
CONFIG_TOOLS_URL            := file://$(PTXDIST_WORKSPACE)/local_src/$(CONFIG_TOOLS)
CONFIG_TOOLS_DIR	          := $(BUILDDIR)/$(CONFIG_TOOLS)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/config-tools.extract:
	@$(call targetinfo)
	@$(call clean, $(CONFIG_TOOLS_DIR))
	rsync -a --exclude=.libs/ --exclude=objs/ --exclude="*.o" --exclude="*.a" --exclude="*.so" --exclude="*.so" \
    $(PTXDIST_WORKSPACE)/local_src/$(CONFIG_TOOLS) $(BUILDDIR)
	@$(call touch)

# Copy ct_error_handling.h before compile. It's needed by some tools. 
	@cp $(CONFIG_TOOLS_DIR)/ct_error_handling.h      $(PTXCONF_SYSROOT_TARGET)/usr/include/ct_error_handling.h

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

CONFIG_TOOLS_PATH	:= PATH=$(CROSS_PATH)
CONFIG_TOOLS_ENV 	:= $(CROSS_ENV) _SYSROOT=$(PTXCONF_SYSROOT_TARGET) CPPUTEST_HOME=$(BUILDDIR)/cpputest-3.1/ 

ifdef PTXCONF_KERNEL_HEADER
CT_CFLAGS += -I$(KERNEL_HEADERS_INCLUDE_DIR)
endif

ifdef PTXCONF_INITMETHOD_BBINIT
CT_CFLAGS += -D__HAVE_INITMETHOD_SYSV
endif

ifdef PTXCONF_INITMETHOD_INITNG
CT_CFLAGS +=-D__HAVE_INITMETHOD_SYSV
endif

ifdef PTXCONF_CT_FEATURE_ETH_SWITCH_DSA
# also triggers libctnetwork.so to be linked against libswconfig.so
CT_CFLAGS += -D__ENABLE_DSA
endif

ifdef PTXCONF_CT_FEATURE_TYPELABEL_SUPPORT
	CT_CFLAGS+= -D__CT_WITH_TYPELABEL
	CONFIG_TOOLS_ENV+=WITH_LIBTYPELABEL=yes
endif

ifdef PTXCONF_CT_FEATURE_WAGODBUS
	CT_CFLAGS+= -D__CT_WITH_WAGODBUS
	CONFIG_TOOLS_ENV+=WITH_LIBWAGODBUS=yes
endif

ifdef PTXCONF_CT_MODBUS_CONFIG
	CONFIG_TOOLS_ENV+=WITH_LIBMODBUSCONFIG=yes
endif

ifdef PTXCONF_CT_BACNET_CONFIG
	CT_CFLAGS+= -D__CT_WITH_LIBBACNETCONFIG
	CONFIG_TOOLS_ENV+=WITH_LIBBACNETCONFIG=yes
endif

ifdef PTXCONF_CT_BACNET_BACKUP_RESTORE
	CT_CFLAGS+= -D__CT_WITH_LIBBACNETCONFIGS
	CONFIG_TOOLS_ENV+=WITH_LIBBACNETCONFIG=yes
endif

ifdef PTXCONF_CT_CONFIG_MDMD
	CONFIG_TOOLS_ENV+=WITH_LIBCONFIGMDMD=yes
endif

ifneq ($(CT_CFLAGS), "")
CONFIG_TOOLS_ENV += CFLAGS="$(CT_CFLAGS)" 
endif

$(STATEDIR)/config-tools.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

CT_MAKE_ARGS=

#ifdef PTXCONF_CT_CONFIG_TOOL_LIB_C
#	CT_MAKE_ARGS+=libctcommon.so
#endif

ifdef PTXCONF_CT_WDIALOG
	CT_MAKE_ARGS+=wdialog
endif

ifdef PTXCONF_CT_CALCULATE_BROADCAST
	CT_MAKE_ARGS+=calculate_broadcast
endif

ifdef PTXCONF_CT_CRYPT
	CT_MAKE_ARGS+=crypt
endif

ifdef PTXCONF_CT_GET_CLOCK_DATA
	CT_MAKE_ARGS+=get_clock_data
endif

ifdef PTXCONF_CT_GET_COUPLER_DETAILS
	CT_MAKE_ARGS+=get_coupler_details
endif


ifdef PTXCONF_CT_GET_DEVICE_DATA
	CT_MAKE_ARGS+=get_device_data
endif

ifdef PTXCONF_CT_GET_DNS_SERVER
	CT_MAKE_ARGS+=get_dns_server
endif

ifdef PTXCONF_CT_GET_FILESYSTEM_DATA
	CT_MAKE_ARGS+=get_filesystem_data
endif

ifdef PTXCONF_CT_GET_PORT_STATE
	CT_MAKE_ARGS+=get_port_state
endif

ifdef PTXCONF_CT_GET_RTS3SCFG_VALUE
	CT_MAKE_ARGS+=get_rts3scfg_value
endif

ifdef PTXCONF_CT_GET_RTS_INFO
	CT_MAKE_ARGS+=get_rts_info
endif

ifdef PTXCONF_CT_GET_RUN_STOP_SWITCH_VALUE
	CT_MAKE_ARGS+=get_run_stop_switch_value
	CONFIG_TOOLS_ENV+=WITH_LIBOMS=yes
endif

ifdef PTXCONF_CT_GET_SNMP_DATA
	CT_MAKE_ARGS+=get_snmp_data
endif

ifdef PTXCONF_CT_GET_TOUCHSCREEN_CONFIG
	CT_MAKE_ARGS+=get_touchscreen_config
endif

ifdef PTXCONF_CT_GET_TYPELABEL_VALUE
	CT_MAKE_ARGS+=get_typelabel_value
endif

ifdef PTXCONF_CT_GET_UIMAGE_SIZE
	CT_MAKE_ARGS+=get_uimage_size
endif

ifdef PTXCONF_CT_GET_USER
	CT_MAKE_ARGS+=get_user
endif

ifdef PTXCONF_CT_URLENCODE
	CT_MAKE_ARGS+=urlencode
endif

#ifdef PTXCONF_CT_PI
#	CT_MAKE_ARGS+=pi
#endif

ifdef PTXCONF_CT_PRINT_PROGRAM_OUTPUT
	CT_MAKE_ARGS+=print_program_output
endif

#ifdef PTXCONF_CT_READPI
#	CT_MAKE_ARGS+=READPI
#endif

ifdef PTXCONF_CT_SHOW_VIDEO_MODE
	CT_MAKE_ARGS+=show_video_mode
endif

ifdef PTXCONF_CT_STRING_ENCODE
	CT_MAKE_ARGS+=string_encode
endif

#ifdef PTXCONF_CT_WRITEPI
#	CT_MAKE_ARGS+=WRITEPI
#endif

ifdef PTXCONF_CT_MODBUS_CONFIG
	CT_MAKE_ARGS+=modbus_config
endif

ifdef PTXCONF_CT_BACNET_CONFIG
	CT_MAKE_ARGS+=bacnet_config
endif

ifdef PTXCONF_CT_BACNET_BACKUP_RESTORE
  CT_MAKE_ARGS+=bacnet_backup_restore
endif

ifdef PTXCONF_CT_CONFIG_MDMD
	CT_MAKE_ARGS+=config_mdmd
endif

ifdef PTXCONF_CT_CONFIG_DNSMASQ
	CT_MAKE_ARGS+=config_dnsmasq
endif

ifdef PTXCONF_CT_GET_ETH_CONFIG
	CT_MAKE_ARGS+=get_eth_config
endif

ifdef PTXCONF_CT_GET_ACTUAL_ETH_CONFIG
	CT_MAKE_ARGS+=get_actual_eth_config
endif

ifdef PTXCONF_CT_GET_DSA_MODE
	CT_MAKE_ARGS+=get_dsa_mode
endif
ifdef PTXCONF_CT_GET_SWITCH_SETTINGS
	CT_MAKE_ARGS+=get_switch_settings
endif
ifdef PTXCONF_CT_CONFIG_SWITCH
	CT_MAKE_ARGS+=config_switch
endif
ifdef PTXCONF_CT_IPDATACHECK
	CT_MAKE_ARGS+=ipdatacheck
endif
ifdef PTXCONF_CT_SET_SERIAL_MODE
	CT_MAKE_ARGS+=set_serial_mode
endif

ifdef PTXCONF_CT_VPNCFG
	CT_MAKE_ARGS+=vpncfg
endif 

ifdef PTXCONF_CT_GET_POSSIBLE_RUNTIMES
	CT_MAKE_ARGS+=get_possible_runtimes
endif

# config-tools use barebox' headers
$(STATEDIR)/config-tools.compile: $(STATEDIR)/barebox.install
	@$(call targetinfo)
	@echo "Building following config-tools: " $(CT_MAKE_ARGS)

	@if [ "$(PTXCONF_PLATFORM)" = "IO-IPC-Codesys-Tele" ] || [ "$(PTXCONF_PLATFORM)" = "IO-IPC-P-Codesys-Tele" ]; then \
		cd $(CONFIG_TOOLS_DIR) && echo $$PWD && $(CONFIG_TOOLS_ENV) $(CONFIG_TOOLS_PATH) $(MAKE) CROSS_COMPILE=$(COMPILER_PREFIX) EXTRAS=TELECONTROL $(PARALLELMFLAGS); \
	else \
		cd $(CONFIG_TOOLS_DIR) && echo $$PWD && $(CONFIG_TOOLS_ENV) $(CONFIG_TOOLS_PATH) _HW_PLATFORM=$(PTXCONF_PLATFORM) \
		 $(MAKE) $(CONFIG_TOOLS_ENV) EXTRAS=NO $(PARALLELMFLAGS) $(CT_MAKE_ARGS) || exit 1; \
	fi 

ifdef CONFIG_TOOLS_WITH_UNIT_TESTS	
  	@cd $(CONFIG_TOOLS_DIR)/unit-tests && echo $$PWD && $(CONFIG_TOOLS_ENV) $(CONFIG_TOOLS_PATH) \
	_HW_PLATFORM=$(PTXCONF_PLATFORM) _SYSROOT=$(PTXCONF_SYSROOT_TARGET) CPPUTEST_HOME=$(BUILDDIR)/cpputest-3.1/ $(MAKE) clean;  \
	cd $(CONFIG_TOOLS_DIR)/unit-tests && echo $$PWD && $(CONFIG_TOOLS_ENV) $(CONFIG_TOOLS_PATH) \
	_SYSROOT=$(PTXCONF_SYSROOT_TARGET) CPPUTEST_HOME=$(BUILDDIR)/cpputest-3.1/ $(MAKE) all_no_tests
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/config-tools.install: $(config-tools_install_deps_default)
	@$(call targetinfo)
# FIXME: rsc: this needs a proper SYSROOT description!
#
# TODO:	For files that are required at compiletime (headers, libs to link against)
#	you can copy these files to the sysroot directory.
#	Use macro $(PTXCONF_SYSROOT_TARGET) for host files and $(PTXCONF_GNU_TARGET)
#	for target files
#
#	Example for a host header file:
#		@cp friesel.h $(PTXCONF_SYSROOT_TARGET)/include
#	Example for a host library file:
#		@cp friesel.so $(PTXCONF_SYSROOT_TARGET)/lib
#	Example for a target file:
#		@cp frasel.h  $(PTXCONF_GNU_TARGET)/include
#	Example for a target library file:
#		@cp frasel.so $(PTXCONF_GNU_TARGET)/lib
	
	@cp $(CONFIG_TOOLS_DIR)/libnet/ct_libnet.h      $(PTXCONF_SYSROOT_TARGET)/usr/include/libctnetwork.h
	@cp $(CONFIG_TOOLS_DIR)/libnet/libctnetwork.so  $(PTXCONF_SYSROOT_TARGET)/usr/lib/libctnetwork.so
	
	@cp $(CONFIG_TOOLS_DIR)/liblog/ct_liblog.h      $(PTXCONF_SYSROOT_TARGET)/usr/include/ct_liblog.h
	@cp $(CONFIG_TOOLS_DIR)/liblog/libctlog.so      $(PTXCONF_SYSROOT_TARGET)/usr/lib/libctlog.so
	
	@cp $(CONFIG_TOOLS_DIR)/config_tool_lib.h      $(PTXCONF_SYSROOT_TARGET)/usr/include/config_tool_lib.h
	@cp $(CONFIG_TOOLS_DIR)/libctcommon.so      $(PTXCONF_SYSROOT_TARGET)/usr/lib/libctcommon.so

ifdef PTXCONF_CT_FEATURE_WAGODBUS
	@cp $(CONFIG_TOOLS_DIR)/ct_dbus.h               $(PTXCONF_SYSROOT_TARGET)/usr/include/ct_dbus.h
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/config-tools.targetinstall:
	@$(call targetinfo)

	@$(call install_init, config-tools)
	@$(call install_fixup,config-tools,PRIORITY,optional)
	@$(call install_fixup,config-tools,SECTION,base)
	@$(call install_fixup,config-tools,AUTHOR,"Wago Kontakttechnik")
	@$(call install_fixup,config-tools,DESCRIPTION,missing)

	@$(call install_copy, config-tools, 0, 0, 0755, /etc/config-tools);
	@$(call install_copy, config-tools, 0, 0, 0755, /etc/config-tools/post_netconfig.d)
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/codesys_after_download_hook, /etc/config-tools/codesys_after_download_hook);

#### Install shell-script based functions.

ifdef PTXCONF_CT_ACTIVATE_DOWNLOAD
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/activate_download, /etc/config-tools/activate_download);
endif

ifdef PTXCONF_CT_AUTO_FIRMWARE_RESTORE
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/auto_firmware_restore, /etc/config-tools/auto_firmware_restore);
endif

ifdef PTXCONF_CT_CF_CARD_BACKUP
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/cf_card_backup, /etc/config-tools/cf_card_backup);
endif

ifdef PTXCONF_CT_CHANGE_BOOTFLAG
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/change_bootflag, /etc/config-tools/change_bootflag);
endif

ifdef PTXCONF_CT_CHANGE_DEFAULT_WEBSERVER
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/change_default_webserver, /etc/config-tools/change_default_webserver);
endif

ifdef PTXCONF_CT_CHANGE_HOSTNAME
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/change_hostname, /etc/config-tools/change_hostname);
	@$(call install_alternative, config-tools, 0, 0, 0644, /etc/specific/host.conf)
endif

ifdef PTXCONF_CT_CHANGE_KEYBOARD_LAYOUT
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/change_keyboard_layout, /etc/config-tools/change_keyboard_layout);
endif

ifdef PTXCONF_CT_CHANGE_RTS_CONFIG
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/change_rts_config, /etc/config-tools/change_rts_config);
endif

ifdef PTXCONF_CT_CONFIG_CODESYS3
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_codesys3, /etc/config-tools/config_codesys3);
endif

ifdef PTXCONF_CT_CHANGE_SCREEN_STATE
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/change_screen_state, /etc/config-tools/change_screen_state);
endif

ifdef PTXCONF_CT_CHANGE_VIDEO_MODE
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/change_video_mode, /etc/config-tools/change_video_mode);
endif

ifdef PTXCONF_CT_CODESYS_WEBSERVER_WATCHDOG
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/codesys_webserver_watchdog, /etc/config-tools/codesys_webserver_watchdog);
endif

ifdef PTXCONF_CT_CONFIG_CLOCK
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_clock, /etc/config-tools/config_clock);
endif

ifdef PTXCONF_CT_CONFIG_CLOCK_DISPLAY_MODE
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_clock_display_mode, /etc/config-tools/config_clock_display_mode);
endif

ifdef PTXCONF_CT_CONFIG_DEFAULT_GATEWAY
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_default_gateway, /etc/config-tools/config_default_gateway);
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/get_default_gateway_config, /etc/config-tools/get_default_gateway_config);
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/events/networking/update_config, /etc/config-tools/events/networking/update_config);
	@$(call install_alternative, config-tools, 0, 0, 0644, /etc/specific/static_gateway.conf)
endif

ifdef PTXCONF_CT_CONFIG_OVERWRITE_DHCP_GATEWAY
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/set_overwrite_dhcp_gateway, /etc/config-tools/set_overwrite_dhcp_gateway);
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/get_overwrite_dhcp_gateway, /etc/config-tools/get_overwrite_dhcp_gateway);
	@$(call install_alternative, config-tools, 0, 0, 0640, /etc/specific/dhcp_gateway.conf)
endif

ifdef PTXCONF_CT_CONFIG_MODBUS
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_modbus, /etc/config-tools/config_modbus);
endif

ifdef PTXCONF_CT_CONFIG_MOUSEPOINTER
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_mousepointer, /etc/config-tools/config_mousepointer);
endif

#ifdef PTXCONF_CT_CONFIG_PLC
#	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_plc, /etc/config-tools/config_plc);
#endif

ifdef PTXCONF_CT_CONFIG_PORT
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_port, /etc/config-tools/config_port);
endif

ifdef PTXCONF_CT_CONFIG_RS232
	@$(call install_alternative, config-tools, 0, 0, 0750, /etc/config-tools/config_RS232);
endif

ifdef PTXCONF_CT_CONFIG_SERVICE_INTERFACE
	@$(call install_alternative, config-tools, 0, 0, 0750, /etc/config-tools/config_service_interface);
endif

ifdef PTXCONF_CT_GET_SERVICE_INTERFACE_CONFIG
	@$(call install_alternative, config-tools, 0, 0, 0750, /etc/config-tools/get_service_interface_config);
endif

ifdef PTXCONF_CT_CONFIG_SNMP
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_snmp, /etc/config-tools/config_snmp);
endif

ifdef PTXCONF_CT_CONFIG_SNTP
	@$(call install_alternative, config-tools, 0, 0, 0750, /etc/config-tools/config_sntp);
	@$(call install_alternative, config-tools, 0, 0, 0750, /etc/config-tools/get_ntp_config);
	@$(call install_alternative, config-tools, 0, 0, 0644, /etc/specific/ntpclient-base.conf);
	@$(call install_alternative, config-tools, 0, 0, 0644, /etc/specific/ntpclient-static-servers);
	@$(call install_alternative, config-tools, 0, 0, 0750, /etc/config-tools/events/networking/update_ntpclient_conf);
endif

ifdef PTXCONF_CT_CONFIG_TIMEZONE
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_timezone, /etc/config-tools/config_timezone);
endif

ifdef PTXCONF_CT_CONFIG_TOOL_LIB_BASH
	@$(call install_alternative, config-tools, 0, 0, 0750, /etc/config-tools/config_tool_lib);

ifdef PTXCONF_BUSYBOX_FEATURE_PASSWD_WEAK_CHECK
	@$(call install_replace, config-tools, /etc/config-tools/config_tool_lib, @PASSWORD_WEAKCHECK@, "check_passwd");
	@$(call install_replace, config-tools, /etc/config-tools/config_tool_lib, @PASSWORD_MINLEN@, $(PTXCONF_BUSYBOX_PASSWORD_MINLEN));
else
	@$(call install_replace, config-tools, /etc/config-tools/config_tool_lib, @PASSWORD_WEAKCHECK@, "/usr/bin/true");
	@$(call install_replace, config-tools, /etc/config-tools/config_tool_lib, @PASSWORD_MINLEN@, 0);
endif # PTXCONF_BUSYBOX_FEATURE_PASSWD_WEAK_CHECK
endif # PTXCONF_CT_CONFIG_TOOL_LIB_BASH

ifdef PTXCONF_CT_CONFIG_TOOL_DEFINES
	@$(call install_alternative, config-tools, 0, 0, 0750, /etc/config-tools/config_tool_defines);
endif

ifdef PTXCONF_CT_CONFIG_TOUCH_PARAMETER
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_touch_parameter, /etc/config-tools/config_touch_parameter);
endif

ifdef PTXCONF_CT_CONFIG_TOUCHSCREEN
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_touchscreen, /etc/config-tools/config_touchscreen);
endif

ifdef PTXCONF_CT_CONFIG_USER
	@$(call install_alternative, config-tools, 0, 0, 0750, /etc/config-tools/config_user);
endif

ifdef PTXCONF_CT_CONFIG_OPCUA
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_opcua, /etc/config-tools/config_opcua);
endif

ifdef PTXCONF_CT_CONFIG_SSL
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_ssl, /etc/config-tools/config_ssl);
endif

ifdef PTXCONF_CT_COPY_FIRMWARE_IMAGE
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/copy_firmware_image, /etc/config-tools/copy_firmware_image);
endif

ifdef PTXCONF_CT_GET_MIN_SD_CARD_SIZE
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/get_min_sd_card_size, /etc/config-tools/get_min_sd_card_size);
endif

ifdef PTXCONF_CT_EDIT_DNS_SERVER
ifdef PTXCONF_CT_EDIT_DNS_SERVER_DHCP
	@$(call install_alternative, config_tools, 0, 0, 750, /etc/config-tools/edit_dns_server);
else
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/edit_dns_server, /etc/config-tools/edit_dns_server);
endif
endif

ifdef PTXCONF_CT_FORMAT_MEDIUM
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/format_medium, /etc/config-tools/format_medium);
endif

ifdef PTXCONF_CT_FAT_FORMAT
	@$(call install_link, config-tools, /etc/config-tools/format_medium, /etc/config-tools/fat_format)
endif

ifdef PTXCONF_CT_FIRMWARE_BACKUP
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/firmware_backup, /etc/config-tools/firmware_backup);
endif

ifdef PTXCONF_CT_FIRMWARE_RESTORE
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/firmware_restore, /etc/config-tools/firmware_restore);
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/firmware_restore_admin, /etc/config-tools/firmware_restore_admin);
endif

ifdef PTXCONF_CT_GET_PLC_CONFIG
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/get_plc_config, /etc/config-tools/get_plc_config);
endif

ifdef PTXCONF_CT_GET_TOUCHSCREEN_EVENT
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/get_touchscreen_event, /etc/config-tools/get_touchscreen_event);
endif

ifdef PTXCONF_CT_PBDP_CONFIG
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/pbdp_config, /etc/config-tools/pbdp_config);
endif

ifdef PTXCONF_CT_RESTART_WEBSERVER
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/restart_webserver, /etc/config-tools/restart_webserver);
endif

ifdef PTXCONF_CT_RS232_OWNER
	@$(call install_alternative, config-tools, 0, 0, 0640, /etc/config-tools/RS232_OWNER);
endif

ifdef PTXCONF_CT_SETTINGS_BACKUP
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/settings_backup, /etc/config-tools/settings_backup);
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/settings_factory, /etc/config-tools/settings_factory);
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/settings_factory, /etc/config-tools/settings_factory);
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/rc.once.d/save-factory-defaults, /etc/rc.once.d/save-factory-defaults);
	@$(call install_copy, config-tools, 0, 0, 0755, /etc/config-tools/backup-restore);
endif

ifdef PTXCONF_CT_START_REBOOT
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/start_reboot, /etc/config-tools/start_reboot);
endif

ifdef PTXCONF_CT_SWITCH_BOOTLOADER
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/switch_bootloader, /etc/config-tools/switch_bootloader);
endif

ifdef PTXCONF_CT_TERMINATE_CODESYS
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/terminate_codesys, /etc/config-tools/terminate_codesys);
endif

ifdef PTXCONF_CT_TERMINATE_PROCESS
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/terminate_process, /etc/config-tools/terminate_process);
endif

ifdef PTXCONF_CT_TIME_FORMAT
	@$(call install_copy, config-tools, 0, 0, 0640, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/TIME_FORMAT, /etc/config-tools/TIME_FORMAT);
endif

ifdef PTXCONF_CT_GET_WBM_DIAGLIST
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/get_wbm_diaglist, /etc/config-tools/get_wbm_diaglist);
endif

ifdef PTXCONF_CT_GET_LED_CONFIG
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/get_led_config, /etc/config-tools/get_led_config);
endif

ifdef PTXCONF_CT_GET_WBM_UPLOADS
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/get_wbm_uploads, /etc/config-tools/get_wbm_uploads);
endif

ifdef PTXCONF_CT_CONFIG_SSH
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_ssh, /etc/config-tools/config_ssh);
endif

ifdef PTXCONF_CT_CONFIG_TFTP
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_tftp, /etc/config-tools/config_tftp);
endif

ifdef PTXCONF_CT_CONFIG_DHCPD
	@$(call install_alternative, config-tools, 0, 0, 0750, /etc/config-tools/config_dhcpd)
endif

ifdef PTXCONF_CT_CONFIG_DNS_SERVICE
	@$(call install_alternative, config-tools, 0, 0, 0750, /etc/config-tools/config_dns_service)
endif

ifdef PTXCONF_CT_CONFIG_IOCHECKPORT
	@$(call install_alternative, config-tools, 0, 0, 0750, /etc/config-tools/config_iocheckport)
endif

ifdef PTXCONF_CT_GET_SSH_CONFIG
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/get_ssh_config, /etc/config-tools/get_ssh_config);
endif

ifdef PTXCONF_CT_GET_TFTP_CONFIG
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/get_tftp_config, /etc/config-tools/get_tftp_config);
endif

ifdef PTXCONF_CT_GET_DHCPD_CONFIG
	@$(call install_alternative, config-tools, 0, 0, 0750, /etc/config-tools/get_dhcpd_config)
endif

ifdef PTXCONF_CT_GET_DNS_SERVICE_CONFIG
	@$(call install_alternative, config-tools, 0, 0, 0750, /etc/config-tools/get_dns_service_config)
endif

ifdef PTXCONF_CT_GET_IOCHECKPORT_CONFIG
	@$(call install_alternative, config-tools, 0, 0, 0750, /etc/config-tools/get_iocheckport_config)
endif

ifdef PTXCONF_CT_GET_USER_INFO
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/get_user_info, /etc/config-tools/get_user_info);
endif

ifdef PTXCONF_CT_GET_USER_HASH
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/get_user_hash, /etc/config-tools/get_user_hash);
endif

ifdef PTXCONF_CT_DETERMINE_HOSTNAME
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/determine_hostname, /etc/config-tools/determine_hostname);
endif

ifdef PTXCONF_CT_GET_FIRMWARE_RESTORE_PACKAGES
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/get_firmware_restore_packages, /etc/config-tools/get_firmware_restore_packages);
endif


ifdef PTXCONF_CT_CONFIG_RUNTIME
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_runtime, /etc/config-tools/config_runtime)
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_runtime_common, /etc/config-tools/config_runtime_common)
endif

ifdef PTXCONF_CT_GET_RUNTIME_CONFIG
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/get_runtime_config, /etc/config-tools/get_runtime_config)
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_runtime_common, /etc/config-tools/config_runtime_common)
endif


ifdef PTXCONF_CT_GET_POSSIBLE_RUNTIMES
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/get_possible_runtimes, /etc/config-tools/get_possible_runtimes);
	@$(call install_alternative, config-tools, 0, 0, 0440, /etc/specific/rtsbl);
endif

ifdef PTXCONF_CT_UPDATE_MODEM_FIRMWARE
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/update_modem_firmware, /etc/config-tools/update_modem_firmware)
endif

ifdef PTXCONF_CT_HTTPS_CIPHER_SELECTION
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_https_tls, /etc/config-tools/config_https_tls)
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/get_https_tls_config, /etc/config-tools/get_https_tls_config)
endif

ifdef PTXCONF_CT_GET_TELECONTROL_STATE
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/get_telecontrol_state, /etc/config-tools/get_telecontrol_state);
	@$(call install_alternative, config-tools, 0, 0, 0640, /etc/specific/telecontrol_states);
	@$(call install_alternative, config-tools, 0, 0, 0750, /etc/init.d/telecontrol);
	@$(call install_link, config-tools, ../init.d/telecontrol, /etc/rc.d/S90_telecontrol)
endif

ifdef PTXCONF_CT_BACNET_WBM_DIAGLIST
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/bacnet_wbm_diaglist, /etc/config-tools/bacnet_wbm_diaglist);
endif

ifdef PTXCONF_CT_GET_SYSTEMINFO
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/get_systeminfo, /etc/config-tools/get_systeminfo);
endif

ifdef PTXCONF_CT_WBM_MANAGE_AIDE
	@$(call install_alternative, config-tools, 0, 0, 0750, /etc/config-tools/manage_integrity);
endif

#### Install C functions.

ifdef PTXCONF_CT_CONFIG_TOOL_LIB_C
	@$(call install_copy, config-tools, 0, 0, 0644, $(CONFIG_TOOLS_DIR)/libctcommon.so, /usr/lib/libctcommon.so);
endif

ifdef PTXCONF_CT_CALCULATE_BROADCAST
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/calculate_broadcast, /etc/config-tools/calculate_broadcast);
endif

ifdef PTXCONF_CT_CRYPT
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/crypt, /etc/config-tools/crypt);
endif

ifdef PTXCONF_CT_GET_CLOCK_DATA
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/get_clock_data, /etc/config-tools/get_clock_data);
endif

ifdef PTXCONF_CT_GET_COUPLER_DETAILS
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/get_coupler_details, /etc/config-tools/get_coupler_details);
endif

ifdef PTXCONF_CT_GET_DEVICE_DATA
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/get_device_data, /etc/config-tools/get_device_data);
endif

ifdef PTXCONF_CT_GET_DNS_SERVER
#ifdef PTXCONF_CT_EDIT_DNS_SERVER_DHCP - not needed because its allready in the c file
#	@$(call install_alternative, config-tools, 0, 0, 0750, /etc/config-tools/get_dns_server);
#else
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/get_dns_server, /etc/config-tools/get_dns_server);
#endif
endif

ifdef PTXCONF_CT_EDIT_DNS_SERVER_DHCP
	@$(call install_alternative, config-tools, 0, 0, 644, /etc/dns_static_list);
	@$(call install_alternative, config-tools, 0, 0, 750, /etc/config-tools/events/networking/update_dns_conf);
endif

ifdef PTXCONF_CT_BLENDER
	@$(call install_alternative, config-tools, 0, 0, 750, /etc/config-tools/blender.sh);
endif

ifdef PTXCONF_CT_FEATURE_DETECT
	@$(call install_alternative, config-tools, 0, 0, 750, /etc/config-tools/detectfeature);
	@$(call install_copy, config-tools, 0, 0, 750, /etc/specific/features/);
endif

ifdef PTXCONF_CT_GET_FILESYSTEM_DATA
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/get_filesystem_data, /etc/config-tools/get_filesystem_data);
endif

ifdef PTXCONF_CT_GET_PORT_STATE
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/get_port_state, /etc/config-tools/get_port_state);
endif

ifdef PTXCONF_CT_GET_RTS3SCFG_VALUE
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/get_rts3scfg_value, /etc/config-tools/get_rts3scfg_value);
endif

ifdef PTXCONF_CT_GET_RTS_INFO
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/get_rts_info, /etc/config-tools/get_rts_info);
endif

ifdef PTXCONF_CT_GET_RUN_STOP_SWITCH_VALUE
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/get_run_stop_switch_value, /etc/config-tools/get_run_stop_switch_value);
endif

ifdef PTXCONF_CT_GET_SNMP_DATA
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/get_snmp_data, /etc/config-tools/get_snmp_data);
endif

ifdef PTXCONF_CT_GET_TOUCHSCREEN_CONFIG
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/get_touchscreen_config, /etc/config-tools/get_touchscreen_config);
endif

ifdef PTXCONF_CT_GET_TYPELABEL_VALUE
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/get_typelabel_value, /etc/config-tools/get_typelabel_value);
endif

ifdef PTXCONF_CT_GET_UIMAGE_SIZE
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/get_uimage_size, /etc/config-tools/get_uimage_size);
endif

ifdef PTXCONF_CT_GET_USER
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/get_user, /etc/config-tools/get_user);
endif

ifdef PTXCONF_CT_URLENCODE
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/urlencode, /etc/config-tools/urlencode, y);
endif

#ifdef PTXCONF_CT_PI
#		@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/pi, /etc/config-tools/pi);
#endif

ifdef PTXCONF_CT_PRINT_PROGRAM_OUTPUT
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/print_program_output, /etc/config-tools/print_program_output);
endif

#ifdef PTXCONF_CT_READPI
#		@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/READPI, /etc/config-tools/READPI);
#endif

ifdef PTXCONF_CT_SHOW_VIDEO_MODE
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/show_video_mode, /etc/config-tools/show_video_mode);
endif

ifdef PTXCONF_CT_STRING_ENCODE
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/string_encode, /etc/config-tools/string_encode);
endif

#ifdef PTXCONF_CT_WRITEPI
#		@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/WRITEPI, /etc/config-tools/WRITEPI);
#endif

ifdef PTXCONF_CT_MODBUS_CONFIG
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/modbus_config, /etc/config-tools/modbus_config);
endif

ifdef PTXCONF_CT_CONFIG_MDMD
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/config_mdmd, /etc/config-tools/config_mdmd);
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/settings_backup_mdmd, /etc/config-tools/settings_backup_mdmd);
	@$(call install_copy, config-tools, 0, 0, 0755, $(PTXDIST_WORKSPACE)/projectroot/etc/init.d/mdmd_check, /etc/init.d/mdmd_check);	
	@$(call install_link, config-tools, ../init.d/mdmd_check, /etc/rc.d/S90_mdmd_check);
endif

ifdef PTXCONF_CT_BACNET_CONFIG
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/bacnet_config, /etc/config-tools/bacnet_config);
endif

ifdef PTXCONF_CT_BACNET_BACKUP_RESTORE
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/bacnet_backup_restore, /etc/config-tools/backup-restore/bacnet_backup_restore);
endif

#
# New networking tools
#

	@$(call install_copy, config-tools, 0, 0, 0755, $(CONFIG_TOOLS_DIR)/liblog/libctlog.so, /usr/lib/libctlog.so);

ifdef PTXCONF_CT_LIBCTNETWORK
	@$(call install_alternative, config-tools, 0, 0, 0644, /etc/specific/network-interfaces.xml)

	@$(call install_copy, config-tools, 0, 0, 0644, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/events/README, /etc/config-tools/events/README);
	@$(call install_copy, config-tools, 0, 0, 0755, /etc/config-tools/events/codesys/);
	@$(call install_copy, config-tools, 0, 0, 0755, /etc/config-tools/events/dhcp/);
	@$(call install_copy, config-tools, 0, 0, 0755, /etc/config-tools/events/dns/);
	@$(call install_copy, config-tools, 0, 0, 0755, /etc/config-tools/events/modbus/);
	@$(call install_copy, config-tools, 0, 0, 0755, /etc/config-tools/events/snmp/);
	@$(call install_copy, config-tools, 0, 0, 0755, /etc/config-tools/events/ssh/);
	@$(call install_copy, config-tools, 0, 0, 0755, /etc/config-tools/events/ssl/);
ifneq ($(PTXCONF_PLATFORM), wago-pac100)
	@$(call install_copy, config-tools, 0, 0, 0755, /etc/config-tools/events/telnet/);
	@$(call install_copy, config-tools, 0, 0, 0755, /etc/config-tools/events/tftp/);
endif
	@$(call install_copy, config-tools, 0, 0, 0755, /etc/config-tools/events/iocheckport/);
	@$(call install_copy, config-tools, 0, 0, 0755, /etc/config-tools/events/networking/);


	@$(call install_copy, config-tools, 0, 0, 0755, $(CONFIG_TOOLS_DIR)/libnet/libctnetwork.so, /usr/lib/libctnetwork.so);
endif

ifdef PTXCONF_CT_CONFIG_DNSMASQ
	# Does not install /etc/dnsmasq.conf because dnsmasq packet has already installed or will install our config file.
	@$(call install_copy,         config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/config_dnsmasq, /etc/config-tools/config_dnsmasq_c)
	@$(call install_copy,        config-tools, 0, 0, 0750, /etc/dnsmasq.d)
	@$(call install_alternative, config-tools, 0, 0, 0644, /etc/dnsmasq.d/dnsmasq_default.conf)
	@$(call install_alternative, config-tools, 0, 0, 0750, /etc/config-tools/post_netconfig.d/30_dnsmasq)
	@$(call install_alternative, config-tools, 0, 0, 0644, /etc/specific/network-services.xml)
	@$(call install_alternative, config-tools, 0, 0, 0700, /etc/config-tools/events/dns/addLocalhostToResolvConf)
endif

ifdef PTXCONF_CT_GET_ETH_CONFIG
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/get_eth_config, /etc/config-tools/get_eth_config);
endif

ifdef PTXCONF_CT_GET_ACTUAL_ETH_CONFIG
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/get_actual_eth_config, /etc/config-tools/get_actual_eth_config);
endif

ifdef PTXCONF_CT_CONFIG_ETHERNET
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_ethernet, /etc/config-tools/config_ethernet);
endif

ifndef PTXCONF_NETCONFD
ifdef PTXCONF_CT_CONFIG_INTERFACES
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_interfaces, /etc/config-tools/config_interfaces);
endif
ifdef PTXCONF_CT_SET_DSA_MODE
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/set_dsa_mode, /etc/config-tools/set_dsa_mode);
endif

ifdef PTXCONF_CT_GET_DSA_MODE
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/get_dsa_mode, /etc/config-tools/get_dsa_mode);
endif
endif #PTXCONF_NETCONFD
ifdef PTXCONF_CT_GET_SWITCH_SETTINGS
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/get_switch_settings, /etc/config-tools/get_switch_settings);
	@$(call install_alternative, config-tools, 0, 0, 0640, /etc/switch_settings.conf)
	@$(call install_alternative, config-tools, 0, 0, 0750, /etc/config-tools/events/networking/update_switch_config);
endif
ifdef PTXCONF_CT_CONFIG_SWITCH
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/config_switch, /etc/config-tools/config_switch);
endif
ifdef PTXCONF_CT_IPDATACHECK
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/ipdatacheck, /etc/config-tools/ipdatacheck);
endif
ifdef PTXCONF_CT_SET_SERIAL_MODE
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/set_serial_mode, /etc/config-tools/set_serial_mode);
endif
ifdef PTXCONF_CT_WWAN_INTERFACE
ifndef PTXCONF_NETCONFD
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/config_wwan, /etc/config-tools/config_wwan);
endif #PTXCONF_NETCONFD
	@$(call install_alternative, config-tools, 0, 0, 0644, /etc/specific/wwan.conf);
endif

ifdef PTXCONF_CT_VPNCFG
	@$(call install_copy, config-tools, 0, 0, 0750, $(CONFIG_TOOLS_DIR)/vpncfg/vpncfg, /etc/config-tools/vpncfg);
	@$(call install_copy, config-tools, 0, 0, 0750, $(PTXDIST_WORKSPACE)/projectroot/etc/config-tools/settings_backup_vpn, /etc/config-tools/settings_backup_vpn);
endif


	@$(call install_finish,config-tools)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#config-tools_clean:
#	rm -rf $(STATEDIR)/config-tools.*


# vim: syntax=make
