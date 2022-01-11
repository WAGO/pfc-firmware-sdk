# -*-makefile-*-
# $Id$
#
# Copyright (C) 2008 by
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PLCLINUXRT) += plclinuxrt

PLCLINUXRT_VERSION    := 2.4.7.51.1
PLCLINUXRT            := codesys-2.4.7.0
PLCLINUXRT_DIR        := $(BUILDDIR)/$(PLCLINUXRT)
PLCLINUXRT_URL        := file://$(PTXDIST_WORKSPACE)/wago_intern/plc/codesys/$(PLCLINUXRT)
PLCLINUXRT_SRC_DIR    := $(PTXDIST_WORKSPACE)/wago_intern/plc/codesys/$(PLCLINUXRT)

ifdef PTXCONF_PLCLINUXRT_BUILD_CFG_PROFILING_ENABLE
PLCLINUXRT_PROFILING_BUILD = CODESYS_PROFILING_BUILD_ENABLE=y
endif

ifdef PTXCONF_PLCLINUXRT_BUILD_CFG_USE_FEEDBACK_BASED_OPTIMZATION
PLCLINUX_FEEDBACK_BASED_OPTIMIZATION = CODESYS_FEEDBACK_BASED_OPTIMIZATION=y
endif

ifdef PTXCONF_CONFIG_TOOLS
PLCLINUXRT_CONFIG_TOOLS = CODESYS_CONFIG_TOOLS=y
endif

PLCLINUXRT_PACKAGE_NAME := plclinuxrt_$(PLCLINUXRT_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PLCLINUXRT_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/plclinuxrt.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/plclinuxrt.extract:
	@$(call targetinfo)
	mkdir -p $(PLCLINUXRT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	rsync -a --exclude=".*" --exclude="*.d" --exclude="*.o" --exclude="plclinux_rt" $(PLCLINUXRT_SRC_DIR) $(BUILDDIR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/plclinuxrt.prepare:
	@$(call targetinfo)
#	@$(call xslt_compile, $(PLCLINUXRT_DIR)/diagnose/diagnose.xml)
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
PLCLINUXRT_PATH:= PATH=$(CROSS_PATH)

PLCLINUXRT_ENV:= $(CROSS_ENV)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	PLCLINUXRT_ENV+=CFLAGS="$(CROSS_CFLAGS) -DNDEBUG"
endif

PLCLINUXRT_BUILD_PARAMS :=\
		CODESYS_DEBUG_LEVEL=$(PTXCONF_PLCLINUXRT_BUILD_CFG_DEBUG_LEVEL)\
		CODESYS_OPTIMIZATION_LEVEL=$(PTXCONF_PLCLINUXRT_BUILD_CFG_OPTIMIZATION)\
		CODESYS_ARCH=$(PTXCONF_ARCH_STRING) \
		CODESYS_SNMP_PLUGIN=$(PTXCONF_PLCLINUXRTSNMP) \
		$(PLCLINUXRT_CONFIG_TOOLS)\
        $(PLCLINUXRT_PROFILING_BUILD)\
        $(PLCLINUX_FEEDBACK_BASED_OPTIMIZATION)


$(STATEDIR)/plclinuxrt.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	cd $(PLCLINUXRT_DIR) && \
		$(PLCLINUXRT_PATH) \
		$(PLCLINUXRT_ENV) \
		$(PLCLINUXRT_BUILD_PARAMS) \
		$(MAKE) $(PARALLELMFLAGS) all
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

WIDE_INCLUDE := $(PTXCONF_SYSROOT_TARGET)/usr/include/wide

$(STATEDIR)/plclinuxrt.install:
	@$(call targetinfo)
	mkdir -p $(WIDE_INCLUDE)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	cp $(PLCLINUXRT_SRC_DIR)/source/Linux/Standard/wago/wago_io_driver_extension/wide_api.h $(WIDE_INCLUDE)
	cp $(PLCLINUXRT_SRC_DIR)/source/Linux/Standard/wago/wago_io_driver_extension/wide_types.h $(WIDE_INCLUDE)
	cp $(PLCLINUXRT_SRC_DIR)/source/Linux/Standard/wago/wago_io_driver_extension/wide_codesys_types.h $(WIDE_INCLUDE)
	cp $(PLCLINUXRT_SRC_DIR)/source/Linux/Standard/wago/wago_io_driver_extension/wide_events.h $(WIDE_INCLUDE)
	cp $(PLCLINUXRT_SRC_DIR)/source/Linux/Standard/wago/wago_io_driver_extension/wide_hooks.h $(WIDE_INCLUDE)
	cp $(PLCLINUXRT_SRC_DIR)/source/Linux/Standard/wago/wago_io_driver_extension/wide_rts_exceptions.h $(WIDE_INCLUDE)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	#Backup header files as archive for later use in configs/@platform@/packages
	cd $(PTXCONF_SYSROOT_TARGET) && \
	tar -czvf $(PLCLINUXRT_PLATFORMCONFIGPACKAGEDIR)/$(PLCLINUXRT_PACKAGE_NAME).tgz \
		usr/include/wide/wide_api.h \
		usr/include/wide/wide_types.h \
		usr/include/wide/wide_codesys_types.h \
		usr/include/wide/wide_events.h \
		usr/include/wide/wide_hooks.h \
		usr/include/wide/wide_rts_exceptions.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	tar -xzvf $(PLCLINUXRT_PLATFORMCONFIGPACKAGEDIR)/$(PLCLINUXRT_PACKAGE_NAME).tgz -C $(PTXCONF_SYSROOT_TARGET)
	tar -xzvf $(PLCLINUXRT_PLATFORMCONFIGPACKAGEDIR)/$(PLCLINUXRT_PACKAGE_NAME).tgz -C $(PKGDIR)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

ifdef PTXCONF_PLCLINUXRT_RTS_CGROUPS_SUPPORT
CGROUP_ENV="if [ -e /etc/cgroup.env ]; then\n\tsource /etc/cgroup.env\nfi"
else
CGROUP_ENV=
endif
CDS_INSTALL_DIR="/home/codesys2_skel"

$(STATEDIR)/plclinuxrt.targetinstall:
	@$(call targetinfo)
	@$(call install_init, plclinuxrt)

	@$(call install_fixup,plclinuxrt,PRIORITY,optional)
	@$(call install_fixup,plclinuxrt,SECTION,base)
	@$(call install_fixup,plclinuxrt,AUTHOR,"3S")
	@$(call install_fixup,plclinuxrt,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Extract precompiled binaries from archive
	rm -rf $(PLCLINUXRT_PLATFORMCONFIGPACKAGEDIR)/tmp/*
	cd $(PLCLINUXRT_PLATFORMCONFIGPACKAGEDIR)/tmp && \
	ar -xov $(PLCLINUXRT_PLATFORMCONFIGPACKAGEDIR)/$(PLCLINUXRT_PACKAGE_NAME).ipk
	@$(call install_archive, plclinuxrt, 0, 0, $(PLCLINUXRT_PLATFORMCONFIGPACKAGEDIR)/tmp/data.tar.gz, /)
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

ifdef PTXCONF_PLATFORM
	@$(call install_copy, plclinuxrt, 0, 0, 0755, $(PLCLINUXRT_DIR)/plclinux_rt, /usr/sbin/plclinux_rt);
ifdef PTXCONF_PLCLINUXRT_INSTALL_RTS3S_CFG
	@$(call install_copy, plclinuxrt, 0, 0, 0644, $(PTXDIST_WORKSPACE)/projectroot/etc/rts3s.cfg, /etc/rts3s.cfg, n);
	@$(call install_replace, plclinuxrt, /etc/rts3s.cfg, @TCP_IP_PORT@, $(PTXCONF_PLCLINUXRT_INSTALL_RTS3S_CFG_TCP_IP_PORT));

	@$(call install_replace, plclinuxrt, /etc/rts3s.cfg, @TASK_JITTER@, $(PTXCONF_PLCLINUXRT_INSTALL_RTS3S_CFG_TASK_JITTER));
	@$(call install_replace, plclinuxrt, /etc/rts3s.cfg, @MIN_TASK_INTERVAL@, $(PTXCONF_PLCLINUXRT_INSTALL_RTS3S_CFG_MIN_TASK_INTERVAL));
	@$(call install_replace, plclinuxrt, /etc/rts3s.cfg, @KBUS_TIMEOUT@, $(PTXCONF_PLCLINUXRT_INSTALL_RTS3S_CFG_KBUS_TIMEOUT));
	@$(call install_replace, plclinuxrt, /etc/rts3s.cfg, @RS232_PORT@, $(PTXCONF_PLCLINUXRT_INSTALL_RTS3S_CFG_RS232_PORT));
	@$(call install_replace, plclinuxrt, /etc/rts3s.cfg, @MODBUS_RTU_INTERFACE@, $(PTXCONF_PLCLINUXRT_INSTALL_RTS3S_CFG_MODBUS_RTU_INTERFACE));
	@$(call install_replace, plclinuxrt, /etc/rts3s.cfg, @TTY_DEVICEFILE@, $(PTXCONF_PLCLINUXRT_INSTALL_RTS3S_CFG_TTY_DEVICEFILE));
	@$(call install_replace, plclinuxrt, /etc/rts3s.cfg, @MIN_IO_TASK_INTERVAL@, $(PTXCONF_PLCLINUXRT_INSTALL_RTS3S_CFG_MIN_IO_TASK_INTERVAL_US));

#Watchdog

	@$(call install_replace, plclinuxrt, /etc/rts3s.cfg, @WDG_INTERVAL_MS@, $(PTXCONF_PLCLINUXRT_INSTALL_RTS3S_CFG_WATCHDOG_INTERVAL_MS));
	@$(call install_replace, plclinuxrt, /etc/rts3s.cfg, @WDG_TASK_CYCLE_TIME_MS@, $(PTXCONF_PLCLINUXRT_INSTALL_RTS3S_CFG_WATCHDOG_TASK_CYCLE_TIME_MS));
	@$(call install_replace, plclinuxrt, /etc/rts3s.cfg, @WDG_FACTOR@, $(PTXCONF_PLCLINUXRT_INSTALL_RTS3S_CFG_WATCHDOG_FACTOR));

ifdef PTXCONF_PLCLINUXRT_INSTALL_RTS3S_CFG_FILE_PATH
	@$(call install_replace, plclinuxrt, /etc/rts3s.cfg, @FILE_PATH@, $(PTXCONF_PLCLINUXRT_INSTALL_RTS3S_CFG_FILE_PATH));
else
	@$(call install_replace, plclinuxrt, /etc/rts3s.cfg, @FILE_PATH@, "/home/codesys/");
endif
endif

#ifdef PTXCONF_PLCLINUXRT_CODESYS_STARTSCRIPT
#	@$(call install_alternative, plclinuxrt, 0, 0, 0755, /etc/init.d/codesys, n)
#	@$(call install_replace, plclinuxrt, /etc/init.d/codesys, @CDS_CGROUPS@, $(CGROUP_ENV))
#ifdef PTXCONF_PLCLINUXRT_USE_SCREEN
#	@$(call install_replace, plclinuxrt, /etc/init.d/codesys, @CODESYS_START_CMD@, "/usr/sbin/plclinux_rt");
#	@$(call install_copy, plclinuxrt, 0, 0, 0755, $(PTXDIST_WORKSPACE)/projectroot/etc/init.d/codesys_screen, /etc/init.d/codesys_screen, n);
#ifneq ($(PTXCONF_PLCLINUXRT_CODESYS_LINK),"")
#	@$(call install_link, plclinuxrt, ../init.d/codesys_screen, /etc/rc.d/$(PTXCONF_PLCLINUXRT_CODESYS_LINK));
#endif
#else
#	@$(call install_replace, plclinuxrt, /etc/init.d/codesys, @CODESYS_START_CMD@, "/usr/sbin/plclinux_rt\ >\ /dev/null\ 2>\&1\ \&");
#ifneq ($(PTXCONF_PLCLINUXRT_CODESYS_LINK),"")
#	@$(call install_link, plclinuxrt, ../init.d/codesys, /etc/rc.d/$(PTXCONF_PLCLINUXRT_CODESYS_LINK));
#endif
#endif
#endif

ifdef PTXCONF_PLCLINUXRT_CODESYS_HOME
  # make /home/codesys_root and friends belong to group 'admin'
	@$(call install_copy, plclinuxrt, 0, ${PTXCONF_ROOTFS_PASSWD_ADMIN_GID}, 0775, $(CDS_INSTALL_DIR));
	@$(call install_copy, plclinuxrt, 0, ${PTXCONF_ROOTFS_PASSWD_ADMIN_GID}, 0775, $(CDS_INSTALL_DIR)/fontz);
	@$(call install_copy, plclinuxrt, 0, ${PTXCONF_ROOTFS_PASSWD_ADMIN_GID}, 0775, $(CDS_INSTALL_DIR)/fontz/truetype);
	@cd $(PTXDIST_WORKSPACE)/projectroot/home/codesys && \
	for file in *; do \
		if test -f $$file; then \
			$(call install_copy, plclinuxrt, 0, 0, 0644, $(PTXDIST_WORKSPACE)/projectroot/home/codesys/$$file, \
				$(CDS_INSTALL_DIR)/$$file, n); \
		fi \
	done && \
	cd $(PTXDIST_WORKSPACE)/projectroot/home/codesys/fontz/truetype && \
	for file in *; do \
		$(call install_copy, plclinuxrt, 0, 0, 0644, $(PTXDIST_WORKSPACE)/projectroot/home/codesys/fontz/truetype/$$file, \
		$(CDS_INSTALL_DIR)/fontz/truetype/$$file, n); \
	done

# Workaround for opkg conflict with codesys3 (breaks "ptxdist images")
#	@$(call install_link, plclinuxrt, /home/codesys_root, /home/codesys);

# Moved here from plclinuxrt-webserver rules file to prevent overwriting the access permissions to /home/codesys2 set above
ifdef PTXCONF_PLCLINUXRT_WEBSERVER
	@$(call install_copy, plclinuxrt, 0, 0, 0644, $(PTXDIST_WORKSPACE)/projectroot/var/www/webvisu/webvisu.htm, $(CDS_INSTALL_DIR)/webvisu.htm)
	@$(call install_copy, plclinuxrt, 0, 0, 0644, $(PTXDIST_WORKSPACE)/projectroot/home/codesys/error_ini.xml, $(CDS_INSTALL_DIR)/error_ini.xml)
endif

endif
ifdef PTXCONF_PLCLINUXRT_INSTALL_ETC_TELE
	@$(call install_copy, plclinuxrt, 0, 0, 0644, $(PTXDIST_WORKSPACE)/projectroot/etc/tele, /etc/tele);
endif
endif

endif
	@$(call install_finish,plclinuxrt)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(PLCLINUXRT_PACKAGE_NAME).ipk $(PLCLINUXRT_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/plclinuxrt.clean:
	rm -rf $(STATEDIR)/plclinuxrt.*
	rm -rf $(PKGDIR)/plclinuxrt_*
	rm -rf $(PLCLINUXRT_DIR)
	rm -rf $(WIDE_INCLUDE)
#	$(call xslt_clean, $(PLCLINUXRT_DIR)/diagnose/diagnose.xml)
