# -*-makefile-*-
#
# Copyright by
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WAGO_CUSTOM_INSTALL) += wago-custom-install

# dummy to make ipkg happy
WAGO_CUSTOM_INSTALL_VERSION	:= 1.0.0
WAGO_CUSTOM_INSTALL	:= wago-custom-install

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-custom-install.install:
	@$(call targetinfo)
	@$(call world/install, WAGO_CUSTOM_INSTALL)

	@mkdir -p $(PKGDIR)/$(WAGO_CUSTOM_INSTALL)/etc/specific/features/
	@echo "hardware-available=false" >$(PKGDIR)/$(WAGO_CUSTOM_INSTALL)/etc/specific/features/rs232_485-interface
	@echo "hardware-available=false" >$(PKGDIR)/$(WAGO_CUSTOM_INSTALL)/etc/specific/features/serial-service-interface
	@echo "hardware-available=false" >$(PKGDIR)/$(WAGO_CUSTOM_INSTALL)/etc/specific/features/profibus-dp-slave

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

VIDEO_MODES_TMP_FILE := $(PTXDIST_TEMPDIR)/VIDEO_MODES.tmp
WAGO_CUSTOM_GRUB_MENU_LST := $(call remove_quotes, $(PTXCONF_WAGO_CUSTOM_GRUB_MENU_LST))
WAGO_CUSTOM_GRUB_ENTRY_0_ROOT_DEV := $(subst $(comma),$(space),$(call remove_quotes, $(PTXCONF_WAGO_CUSTOM_GRUB_ENTRY_0_ROOT_DEV)))
WAGO_CUSTOM_GRUB_ENTRY_1_ROOT_DEV := $(subst $(comma),$(space),$(call remove_quotes, $(PTXCONF_WAGO_CUSTOM_GRUB_ENTRY_1_ROOT_DEV)))
WAGO_CUSTOM_GRUB_ENTRY_2_ROOT_DEV := $(subst $(comma),$(space),$(call remove_quotes, $(PTXCONF_WAGO_CUSTOM_GRUB_ENTRY_2_ROOT_DEV)))

# Special case diagnostic.xml: deal with it as with a binary artifact
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
DIAGNOSTIC_XML_SOURCE_DIR=$(PTXDIST_PLATFORMCONFIGDIR)/packages
else
# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
DIAGNOSTIC_XML_SOURCE_DIR=$(SYSROOT)/etc/specific
endif

$(STATEDIR)/wago-custom-install.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  wago-custom-install)
	@$(call install_fixup, wago-custom-install,PRIORITY,optional)
	@$(call install_fixup, wago-custom-install,SECTION,base)
	@$(call install_fixup, wago-custom-install,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, wago-custom-install,DESCRIPTION,missing)


ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_INSTALL_HOME_USER_GUEST
ifdef PTXCONF_ROOTFS_PASSWD_USER
	@$(call install_copy, wago-custom-install, ${PTXCONF_ROOTFS_PASSWD_USER_UID}, ${PTXCONF_ROOTFS_PASSWD_USER_GID}, 0755, /home/user)
endif # PTXCONF_ROOTFS_PASSWD_USER

ifdef PTXCONF_ROOTFS_PASSWD_GUEST
	@$(call install_copy, wago-custom-install, ${PTXCONF_ROOTFS_PASSWD_GUEST_UID}, ${PTXCONF_ROOTFS_PASSWD_GUEST_GID}, 0744, /home/guest)
endif # PTXCONF_ROOTFS_PASSWD_GUEST

ifdef PTXCONF_PLCLINUXRT
	# make /home/admin a link to /home/codesys to provide codesys users with the expected ftp home dir
	@$(call install_link, wago-custom-install, ./codesys, /home/admin)
else
	@$(call install_copy, wago-custom-install, ${PTXCONF_ROOTFS_PASSWD_ADMIN_UID}, ${PTXCONF_ROOTFS_PASSWD_ADMIN_GID}, 0744, /home/admin)
endif # PTXCONF_PLCLINUXRT

endif # PTXCONF_WAGO_CUSTOM_ROOTFS_INSTALL_HOME_USER_GUEST

ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_INSTALL_CHECK_FOR_DEFAULT_PASSWORD
	@$(call install_copy, wago-custom-install, 0, 0, 644, $(PTXDIST_WORKSPACE)/projectroot/etc/profile.passwd, /etc/profile.passwd)

        # install per-user .profile file that sources /etc/profile.passwd for the users root, admin and user
	@$(call install_copy, wago-custom-install, 0, 0, 0700, /root)
	@$(call install_copy, wago-custom-install, 0, 0, 600, $(PTXDIST_WORKSPACE)/projectroot/root/.profile, /root/.profile)

ifndef PTXCONF_PLCLINUXRT
	@$(call install_copy, wago-custom-install, ${PTXCONF_ROOTFS_PASSWD_ADMIN_UID}, ${PTXCONF_ROOTFS_PASSWD_ADMIN_GID}, 600, $(PTXDIST_WORKSPACE)/projectroot/root/.profile, /home/admin/.profile)
endif # PTXCONF_PLCLINUXRT

ifdef PTXCONF_ROOTFS_PASSWD_USER
	@$(call install_copy, wago-custom-install, ${PTXCONF_ROOTFS_PASSWD_USER_UID}, ${PTXCONF_ROOTFS_PASSWD_USER_GID}, 600, $(PTXDIST_WORKSPACE)/projectroot/root/.profile, /home/user/.profile)
endif # PTXCONF_ROOTFS_PASSWD_USER
endif # PTXCONF_WAGO_CUSTOM_ROOTFS_INSTALL_CHECK_FOR_DEFAULT_PASSWORD

ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_COPY_SD_INTERN
	@$(call install_copy, wago-custom-install, 0, 0, 0755, $(PTXDIST_WORKSPACE)/projectroot/etc/init.d/cpsd2intern, /etc/init.d/cpsd2intern, n)
endif

ifdef PTXCONF_WAGO_CUSTOM_INITRAMFS_INIT
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, \
		/init.sh, n)
	@$(call install_link, wago-custom-install, \
		../../init.sh, \
		/etc/init.d/rcS)
endif
ifdef PTXCONF_WAGO_CUSTOM_INITRAMFS_FIRMWARE_INSTALL
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, \
		/etc/install-firmware.sh, n)
endif
ifdef PTXCONF_WAGO_CUSTOM_INITRAMFS_PFC200_TEST
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, \
		/usr/bin/pfc200-test.sh, n)
endif

#
# INITFILES in /etc/init.d with rc.d links
#
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_MOUNTRW_STARTSCRIPT
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/init.d/remountrw, \
		/etc/init.d/remountrw, n)
ifneq ($(PTXCONF_WAGO_CUSTOM_ROOTFS_MOUNTRW_BBINIT_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/remountrw, \
		/etc/rc.d/$(PTXCONF_WAGO_CUSTOM_ROOTFS_MOUNTRW_BBINIT_LINK))
endif
endif

ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_MOUNTVARLOG_STARTSCRIPT
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/init.d/mountvarlog, \
		/etc/init.d/mountvarlog, n)
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/logging.conf, \
		/etc/logging.conf, n)

ifneq ($(PTXCONF_WAGO_CUSTOM_ROOTFS_MOUNTVARLOG_BBINIT_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/mountvarlog, \
		/etc/rc.d/$(PTXCONF_WAGO_CUSTOM_ROOTFS_MOUNTVARLOG_BBINIT_LINK))
endif
endif

ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_AUTOFIRMREST_STARTSCRIPT
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/init.d/auto_firmware_restore, \
		/etc/init.d/auto_firmware_restore, n)
ifneq ($(PTXCONF_WAGO_CUSTOM_AUTOFIRMREST_BBINIT_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/auto_firmware_restore, \
		/etc/rc.d/$(PTXCONF_WAGO_CUSTOM_AUTOFIRMREST_BBINIT_LINK))
endif
endif
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_MOUNTHD2_STARTSCRIPT
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, \
		/etc/init.d/mounthd2, n)
ifneq ($(PTXCONF_WAGO_CUSTOM_MOUNTHD2_BBINIT_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/mounthd2, \
		/etc/rc.d/$(PTXCONF_WAGO_CUSTOM_MOUNTHD2_BBINIT_LINK))
endif
endif
ifdef PTXCONF_WAGO_CUSTOM_FINALIZE_ROOT_STARTSCRIPT
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, \
		/etc/init.d/finalize_root, n)
ifneq ($(PTXCONF_WAGO_CUSTOM_FINALIZE_ROOT_BBINIT_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/finalize_root, \
		/etc/rc.d/$(PTXCONF_WAGO_CUSTOM_FINALIZE_ROOT_BBINIT_LINK))
endif
endif
ifdef PTXCONF_WAGO_CUSTOM_DEVICE_SETUP_STARTSCRIPT
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, \
		/etc/init.d/device-setup, n)
ifneq ($(PTXCONF_WAGO_CUSTOM_DEVICE_SETUP_BBINIT_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/device-setup, \
		/etc/rc.d/$(PTXCONF_WAGO_CUSTOM_DEVICE_SETUP_BBINIT_LINK))
endif
endif
ifdef PTXCONF_WAGO_CUSTOM_CONFIG_SERIAL_STARTSCRIPT
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, \
		/etc/init.d/config_serial, n)
ifneq ($(PTXCONF_WAGO_CUSTOM_CONFIG_SERIAL_BBINIT_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/config_serial, \
		/etc/rc.d/$(PTXCONF_WAGO_CUSTOM_CONFIG_SERIAL_BBINIT_LINK))
endif
endif

ifdef PTXCONF_WAGO_CUSTOM_CONFIG_USB_GADGET_STARTSCRIPT
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, \
		/etc/init.d/config_usb_gadget, n)
	@$(call install_replace, wago-custom-install, \
		/etc/init.d/config_usb_gadget, @USB_GADGET_STORAGE_FILE@, \
		$(PTXCONF_WAGO_CUSTOM_CONFIG_USB_GADGET_STORAGE_FILE))
	@$(call install_alternative, wago-custom-install, 0, 0, 0644, \
		/lib/udev/rules.d/90-usb-gadget.rules)

ifneq ($(PTXCONF_WAGO_CUSTOM_CONFIG_USB_GADGET_BBINIT_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/config_usb_gadget, \
		/etc/rc.d/$(PTXCONF_WAGO_CUSTOM_CONFIG_USB_GADGET_BBINIT_LINK))
endif
endif
ifdef PTXCONF_WAGO_CUSTOM_CONFIG_USB_GADGET_NETWORK_STARTSCRIPT
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, \
		/usr/sbin/usb_gadget_network, n)
ifneq ($(PTXCONF_WAGO_CUSTOM_CONFIG_USB_GADGET_STORAGE_FILE),"")
	@$(call install_alternative, wago-custom-install, 0, 0, 0644, \
		$(PTXCONF_WAGO_CUSTOM_CONFIG_USB_GADGET_STORAGE_FILE), n)
endif
endif


ifdef PTXCONF_WAGO_CUSTOM_SHUTDOWN_STARTSCRIPT
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, \
		/etc/init.d/shutdown, n)
endif
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_PERMISSIONS_STARTSCRIPT
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/init.d/permissions, \
		/etc/init.d/permissions, n)
ifneq ($(PTXCONF_WAGO_CUSTOM_PERMISSIONS_BBINIT_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/permissions, \
		/etc/rc.d/$(PTXCONF_WAGO_CUSTOM_PERMISSIONS_BBINIT_LINK))
endif
endif
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_KBUSPRIO_STARTSCRIPT
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/cfg-kbus-irq.sh, \
		/etc/cfg-kbus-irq.sh, n)
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/init.d/kbusprio, \
		/etc/init.d/kbusprio, n)
ifneq ($(PTXCONF_WAGO_CUSTOM_KBUSPRIO_BBINIT_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/kbusprio, \
		/etc/rc.d/$(PTXCONF_WAGO_CUSTOM_KBUSPRIO_BBINIT_LINK))
endif
endif
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_SCREEN_INIT_STARTSCRIPT
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/init.d/screen_init, \
		/etc/init.d/screen_init, n)
ifneq ($(PTXCONF_WAGO_CUSTOM_ROOTFS_SCREEN_INIT_BBINIT_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/screen_init, \
		/etc/rc.d/$(PTXCONF_WAGO_CUSTOM_ROOTFS_SCREEN_INIT_BBINIT_LINK))
endif
endif
ifdef PTXCONF_WAGO_CUSTOM_LOG_SYSTEMSTART_INIT_STARTSCRIPT
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/init.d/logsystemstart, \
		/etc/init.d/logsystemstart, n)
ifneq ($(PTXCONF_WAGO_CUSTOM_LOG_SYSTEMSTART_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/logsystemstart, \
	/etc/rc.d/$(PTXCONF_WAGO_CUSTOM_LOG_SYSTEMSTART_LINK))
endif
endif
ifdef PTXCONF_WAGO_CUSTOM_CBM_MULTI_PRODUCT_CONFIG_STARTSCRIPT
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, \
                 /etc/init.d/cbm_set_config)
ifneq ($(PTXCONF_WAGO_CUSTOM_CBM_MULTI_PRODUCT_CONFIG_BBINIT_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/cbm_set_config, \
	/etc/rc.d/$(PTXCONF_WAGO_CUSTOM_CBM_MULTI_PRODUCT_CONFIG_BBINIT_LINK))
endif
endif
ifdef PTXCONF_WAGO_CUSTOM_WBM_MULTI_PRODUCT_CONFIG_STARTSCRIPT
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, \
                 /etc/init.d/wbm_set_config)
ifneq ($(PTXCONF_WAGO_CUSTOM_WBM_MULTI_PRODUCT_CONFIG_BBINIT_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/wbm_set_config, \
	/etc/rc.d/$(PTXCONF_WAGO_CUSTOM_WBM_MULTI_PRODUCT_CONFIG_BBINIT_LINK))
endif
endif
ifdef PTXCONF_WAGO_CUSTOM_OMS_CONFIG_STARTSCRIPT
#	@$(call install_alternative, wago-custom-install, 0, 0, 0755, \
#		/etc/init.d/omsdaemon)
ifneq ($(PTXCONF_WAGO_CUSTOM_OMS_CONFIG_BBINIT_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/omsdaemon, \
		/etc/rc.d/$(PTXCONF_WAGO_CUSTOM_OMS_CONFIG_BBINIT_LINK))
endif
endif

ifdef PTXCONF_WAGO_CUSTOM_MDMD_STARTSCRIPT
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, \
		/etc/init.d/mdmd, n)
ifneq ($(PTXCONF_WAGO_CUSTOM_MDMD_BBINIT_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/mdmd, \
	/etc/rc.d/$(PTXCONF_WAGO_CUSTOM_MDMD_BBINIT_LINK))
endif
endif

ifdef PTXCONF_WAGO_CUSTOM_RAUC_STARTSCRIPT
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, \
		/etc/init.d/rauc, n)
ifneq ($(PTXCONF_WAGO_CUSTOM_RAUC_BBINIT_LINK), "")
	@$(call install_link, wago-custom-install, ../init.d/rauc, \
	/etc/rc.d/$(PTXCONF_WAGO_CUSTOM_RAUC_BBINIT_LINK))
endif
endif

ifdef PTXCONF_USE_SCREEN_WINDOW
# This script is executed by /etc/profile when /bin/bash does not run in a screen terminal and
# was started as a login shell.
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/wago-screen-prompt.sh, /etc/wago-screen-prompt.sh, n);
endif


# It may seem strange, but there is no logical OR available, thus this construct.
# If one knows how to simplyfy it, please do so. The idea is to install ssl_port
# file and its link if either LIGHTTPD _or_ PUREFTPD variables are set.
ifdef PTXCONF_LIGHTTPD
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
    $(PTXDIST_WORKSPACE)/projectroot/etc/init.d/ssl_post, /etc/init.d/ssl_post);

	@$(call install_link, wago-custom-install, ../init.d/ssl_post, \
	/etc/rc.d/S99_ssl_post)
else
ifdef PTXCONF_PUREFTPD
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
    $(PTXDIST_WORKSPACE)/projectroot/etc/init.d/ssl_post, /etc/init.d/ssl_post);

	@$(call install_link, wago-custom-install, ../init.d/ssl_post, \
	/etc/rc.d/S99_ssl_post)
endif
endif


# ------------------------------------------

#
# TOOLS in /usr/sbin/
#
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_CFCARD_TOOL
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/usr/sbin/cp_cfcard, \
		/usr/sbin/cp_cfcard, n)
endif
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_UPDATE_BOOT_TOOL
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/usr/sbin/update_boot, \
		/usr/sbin/update_boot, n)
endif
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_UPDATE_FW_TOOL
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/usr/sbin/update_fw, \
		/usr/sbin/update_fw, n)
endif
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_UPDATE_MBR_GRUB_TOOL
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/usr/sbin/update_mbr_grub, \
		/usr/sbin/update_mbr_grub, n)
endif
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_CREATE_HOME_INT_TOOL
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/usr/sbin/create_home_int, \
		/usr/sbin/create_home_int, n)
endif

ifdef PTXCONF_WAGO_CUSTOM_INSTALL_BACKUP_ACCOUNT_SETTINGS
# backup and restore user, system account setting and groups
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/usr/sbin/settings_backup_accounts, \
		/usr/sbin/settings_backup_accounts, n)
endif

# ------------------------------------------
ifdef PTXCONF_WAGO_CUSTOM_INSTALL_BACKUP_SETTINGS
# backup and restore settings
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, \
		/usr/sbin/settings_backup_lib)
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, \
		/usr/sbin/settings_backup_store)
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, \
		/usr/sbin/settings_backup_restore)
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
	  $(PTXDIST_WORKSPACE)/projectroot/usr/sbin/settings_backup_checksys, \
		/usr/sbin/settings_backup_checksys, n)
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
	  $(PTXDIST_WORKSPACE)/projectroot/usr/sbin/settings_backup_docker, \
		/usr/sbin/settings_backup_docker, n)
endif

#
# CONFIGS in /etc/
#

ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_REVISIONS
#temporary code for CODESYS3 Transistion
ifdef PTXCONF_PFC_200_G2_CDS3
	@$(call install_copy, wago-custom-install, 0, 0, 0644, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/REVISIONS_CDS3, \
		/etc/REVISIONS, n)
else
	@$(call install_alternative, wago-custom-install, 0, 0, 0644, \
		/etc/REVISIONS)
endif
endif

ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_SHELLS
	@$(call install_copy, wago-custom-install, 0, 0, 0644, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/shells, \
		/etc/shells, n)
endif

# Device Media
#	@$(call install_copy, wago-custom-install, 0, 0, 0644, \
#		$(PTXDIST_WORKSPACE)/projectroot/etc/DEVICE_MEDIA, \
#		/etc/DEVICE_MEDIA, n)
	@$(call install_alternative, wago-custom-install, 0, 0, 0644, \
                 /etc/DEVICE_MEDIA)

	@$(call install_replace, wago-custom-install, /etc/DEVICE_MEDIA, \
                 @HDA_MEM_TYPE@, $(PTXCONF_WAGO_CUSTOM_ROOTFS_DEVICE_MEDIA_HDA))
	@$(call install_replace, wago-custom-install, /etc/DEVICE_MEDIA, \
                 @HDB_MEM_TYPE@, $(PTXCONF_WAGO_CUSTOM_ROOTFS_DEVICE_MEDIA_HDB))
	@$(call install_replace, wago-custom-install, /etc/DEVICE_MEDIA, \
                 @SDA_USB_NO@, $(PTXCONF_WAGO_CUSTOM_ROOTFS_DEVICE_MEDIA_SDA))
	@$(call install_replace, wago-custom-install, /etc/DEVICE_MEDIA, \
                 @SDB_USB_NO@, $(PTXCONF_WAGO_CUSTOM_ROOTFS_DEVICE_MEDIA_SDB))
	@$(call install_replace, wago-custom-install, /etc/DEVICE_MEDIA, \
                 @SDC_USB_NO@, $(PTXCONF_WAGO_CUSTOM_ROOTFS_DEVICE_MEDIA_SDC))
	@$(call install_replace, wago-custom-install, /etc/DEVICE_MEDIA, \
                 @SDD_USB_NO@, $(PTXCONF_WAGO_CUSTOM_ROOTFS_DEVICE_MEDIA_SDD))
	@$(call install_replace, wago-custom-install, /etc/DEVICE_MEDIA, \
                 @SDE_USB_NO@, $(PTXCONF_WAGO_CUSTOM_ROOTFS_DEVICE_MEDIA_SDE))
	@$(call install_replace, wago-custom-install, /etc/DEVICE_MEDIA, \
                 @SDF_USB_NO@, $(PTXCONF_WAGO_CUSTOM_ROOTFS_DEVICE_MEDIA_SDF))
	@$(call install_replace, wago-custom-install, /etc/DEVICE_MEDIA, \
                 @SDG_USB_NO@, $(PTXCONF_WAGO_CUSTOM_ROOTFS_DEVICE_MEDIA_SDG))
	@$(call install_replace, wago-custom-install, /etc/DEVICE_MEDIA, \
                 @SDH_USB_NO@, $(PTXCONF_WAGO_CUSTOM_ROOTFS_DEVICE_MEDIA_SDH))

# Video Modes
	@$(shell cp $(PTXDIST_WORKSPACE)/projectroot/etc/VIDEO_MODES \
	            $(VIDEO_MODES_TMP_FILE))
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_VIDEO_MODE_769
	@$(shell echo "769:   640x480, 256 color" >> $(VIDEO_MODES_TMP_FILE))
endif
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_VIDEO_MODE_771
	@$(shell echo "771:   800x600, 256 color" >> $(VIDEO_MODES_TMP_FILE))
endif
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_VIDEO_MODE_773
	@$(shell echo "773:  1024x768, 256 color" >> $(VIDEO_MODES_TMP_FILE))
endif
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_VIDEO_MODE_775
	@$(shell echo "775: 1280x1024, 256 color" >> $(VIDEO_MODES_TMP_FILE))
endif
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_VIDEO_MODE_785
	@$(shell echo "785:   640x480, 16 bit" >> $(VIDEO_MODES_TMP_FILE))
endif
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_VIDEO_MODE_786
	@$(shell echo "786:   640x480, 32 bit" >> $(VIDEO_MODES_TMP_FILE))
endif
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_VIDEO_MODE_788
	@$(shell echo "788:   800x600, 16 bit" >> $(VIDEO_MODES_TMP_FILE))
endif
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_VIDEO_MODE_789
	@$(shell echo "789:   800x600, 32 bit" >> $(VIDEO_MODES_TMP_FILE))
endif
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_VIDEO_MODE_791
	@$(shell echo "791:  1024x768, 16 bit" >> $(VIDEO_MODES_TMP_FILE))
endif
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_VIDEO_MODE_792
	@$(shell echo "792:  1024x768, 32 bit" >> $(VIDEO_MODES_TMP_FILE))
endif
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_VIDEO_MODE_794
	@$(shell echo "794: 1280x1024, 16 bit" >> $(VIDEO_MODES_TMP_FILE))
endif
ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_VIDEO_MODE_795
	@$(shell echo "795: 1280x1024, 32 bit" >> $(VIDEO_MODES_TMP_FILE))
endif
	@$(call install_copy, wago-custom-install, 0, 0, 0644, \
		$(VIDEO_MODES_TMP_FILE), \
		/etc/VIDEO_MODES, n)

ifdef PTXCONF_WAGO_CUSTOM_CRONJOBS_ROOT
	@$(call install_alternative, wago-custom-install, 0, 0, 0600, \
		/var/spool/cron/crontabs/root, n)
endif

ifdef PTXCONF_WAGO_CUSTOM_ROOTFS_CONFIGS_PARTITIONS
	@$(call install_copy, wago-custom-install, 0, 0, 0644, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/bootpartition, \
		/etc/bootpartition, n)
	@$(call install_copy, wago-custom-install, 0, 0, 0644, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/rootpartition, \
		/etc/rootpartition, n)
	@$(call install_copy, wago-custom-install, 0, 0, 0644, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/homepartition, \
		/etc/homepartition, n)
endif
ifdef PTXCONF_WAGO_CUSTOM_DIAGNOSTICS


	@$(call install_copy, wago-custom-install, 0, 0, 0644, \
            $(DIAGNOSTIC_XML_SOURCE_DIR)/diagnostic.xml, /etc/specific/diagnostic.xml, n)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# backup generated diagnostic.xml file for BINARIES mode
	@cp $(DIAGNOSTIC_XML_SOURCE_DIR)/diagnostic.xml $(PTXDIST_PLATFORMCONFIGDIR)/packages/
endif

endif # PTXCONF_WAGO_CUSTOM_DIAGNOSTICS

ifdef PTXCONF_WAGO_CUSTOM_PAC100_ETH_WORKAROUND
	@$(call install_alternative, wago-custom-install, 0, 0, 0644, \
		/etc/tmpfiles.d/eth_workaround.conf, n)

endif

ifdef PTXCONF_WAGO_CUSTOM_INSTALL_OPENSSH_DEFAULT_KEYS
	@$(call install_alternative, wago-custom-install, 0, 0, 0400, \
		/etc/ssh/ssh_host_rsa_key)
	@$(call install_alternative, wago-custom-install, 0, 0, 0400, \
		/etc/ssh/ssh_host_dsa_key)
endif

ifdef PTXCONF_GRUB

#
# Grub Install
# copy and edit grub menu file

ifdef WAGO_CUSTOM_GRUB_MENU_LST
	@$(call install_copy, wago-custom-install, 0, 0, 0644, $(PTXDIST_WORKSPACE)/projectroot/boot/grub/menu.lst, \
		/boot/grub/menu.lst); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_CONSOLE@, $(PTXCONF_WAGO_CUSTOM_GRUB_CONSOLE)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_CONSOLE_SPEED@, $(PTXCONF_WAGO_CUSTOM_GRUB_CONSOLE_SPEED)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_DEFAULT@, $(PTXCONF_WAGO_CUSTOM_GRUB_DEFAULT)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_TIMEOUT@, $(PTXCONF_WAGO_CUSTOM_GRUB_TIMEOUT)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_KERNEL_CONSOLE@, $(PTXCONF_WAGO_CUSTOM_GRUB_KERNEL_CONSOLE)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_KERNEL_CONSOLE_SPEED@, $(PTXCONF_WAGO_CUSTOM_GRUB_KERNEL_CONSOLE_SPEED)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_KERNEL_DEFAULT_PARAM@, $(PTXCONF_WAGO_CUSTOM_GRUB_KERNEL_DEFAULT_PARAM)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_ENTRY_0_TITLE@, $(PTXCONF_WAGO_CUSTOM_GRUB_ENTRY_0_TITLE)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_ENTRY_0_ROOT_DEV@,"$(word 1, $(WAGO_CUSTOM_GRUB_ENTRY_0_ROOT_DEV))\$(comma)$(word 2, $(WAGO_CUSTOM_GRUB_ENTRY_0_ROOT_DEV))"); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_ENTRY_0_KERNEL_ROOT_DEV@, $(PTXCONF_WAGO_CUSTOM_GRUB_ENTRY_0_KERNEL_ROOT_DEV)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_ENTRY_0_KERNEL_FILE@, $(PTXCONF_WAGO_CUSTOM_GRUB_ENTRY_0_KERNEL_FILE)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_ENTRY_0_ADD_KERNEL_PARAM@, $(PTXCONF_WAGO_CUSTOM_GRUB_ENTRY_0_ADD_KERNEL_PARAM)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_ENTRY_1_TITLE@, $(PTXCONF_WAGO_CUSTOM_GRUB_ENTRY_1_TITLE)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_ENTRY_1_ROOT_DEV@,"$(word 1, $(WAGO_CUSTOM_GRUB_ENTRY_1_ROOT_DEV))\$(comma)$(word 2, $(WAGO_CUSTOM_GRUB_ENTRY_1_ROOT_DEV))"); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_ENTRY_1_KERNEL_ROOT_DEV@, $(PTXCONF_WAGO_CUSTOM_GRUB_ENTRY_1_KERNEL_ROOT_DEV)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_ENTRY_1_KERNEL_FILE@, $(PTXCONF_WAGO_CUSTOM_GRUB_ENTRY_1_KERNEL_FILE)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_ENTRY_1_ADD_KERNEL_PARAM@, $(PTXCONF_WAGO_CUSTOM_GRUB_ENTRY_1_ADD_KERNEL_PARAM)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_ENTRY_2_TITLE@, $(PTXCONF_WAGO_CUSTOM_GRUB_ENTRY_2_TITLE)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_ENTRY_2_ROOT_DEV@,"$(word 1, $(WAGO_CUSTOM_GRUB_ENTRY_2_ROOT_DEV))\$(comma)$(word 2, $(WAGO_CUSTOM_GRUB_ENTRY_2_ROOT_DEV))"); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_ENTRY_2_KERNEL_ROOT_DEV@, $(PTXCONF_WAGO_CUSTOM_GRUB_ENTRY_2_KERNEL_ROOT_DEV)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_ENTRY_2_KERNEL_FILE@, $(PTXCONF_WAGO_CUSTOM_GRUB_ENTRY_2_KERNEL_FILE)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GRUB_ENTRY_2_ADD_KERNEL_PARAM@, $(PTXCONF_WAGO_CUSTOM_GRUB_ENTRY_2_ADD_KERNEL_PARAM)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@IPADDR@, $(PTXCONF_WAGO_CUSTOM_GRUB_ENTRY_2_IPADDR)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@SERVERIP@, $(PTXCONF_WAGO_CUSTOM_GRUB_ENTRY_2_SERVERIP)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@GATEWAY@, $(PTXCONF_WAGO_CUSTOM_GRUB_ENTRY_2_GATEWAY)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@NETMASK@, $(PTXCONF_WAGO_CUSTOM_GRUB_ENTRY_2_NETMASK)); \
	@$(call install_replace, wago-custom-install, /boot/grub/menu.lst, \
		@NFSROOT@, $(PTXCONF_WAGO_CUSTOM_GRUB_ENTRY_2_NFSROOT));
endif


endif


ifdef PTXCONF_WAGO_CUSTOM_INSTALL_TOUCH_WBMAPP
	@$(call install_alternative_tree, wago-custom-install, 0, 0,  /var/www/WagoWBM);
	@$(call install_link, wago-custom-install, /var/www/WagoWBM, /var/www/app);
endif

ifdef PTXCONF_WAGO_CUSTOM_INSTALL_BOOT_WDG

ifneq ($(PTXCONF_WAGO_CUSTOM_INSTALL_BOOT_WDG_DEVICE),)
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, /etc/init.d/boot_wdg, n)
	@$(call install_replace, wago-custom-install, /etc/init.d/boot_wdg, @WATCHDOG_DEV@, $(PTXCONF_WAGO_CUSTOM_INSTALL_BOOT_WDG_DEVICE))

ifneq ($(PTXCONF_WAGO_CUSTOM_BOOT_WDG_BBINIT_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/boot_wdg, /etc/rc.d/$(PTXCONF_WAGO_CUSTOM_BOOT_WDG_BBINIT_LINK))

endif
endif
endif

ifdef PTXCONF_WAGO_CUSTOM_INSTALL_LINKIN_DEVS

ifneq ($(PTXCONF_WAGO_CUSTOM_INSTALL_LINKIN_DEVS_NAMES),)
	@$(eval _DEVS := $(call remove_quotes,$(PTXCONF_WAGO_CUSTOM_INSTALL_LINKIN_DEVS_NAMES)))
	@$(eval _TMP_FILE := $(PTXDIST_TEMPDIR)/.devs_to_link)

	@$(foreach dev, $(_DEVS), \
		echo $(dev) >> $(_TMP_FILE);)

	@$(call install_copy, wago-custom-install, 0, 0, 0755, $(_TMP_FILE), /etc/specific/.devs_to_link, n)
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, /etc/init.d/link_devices, n)
	@$(call install_copy, wago-custom-install, 0, 0, 0750, /etc/specific/features/)
	@$(call install_copy, wago-custom-install, 0, 0, 0644, -, /etc/specific/features/rs232_485-interface)
	@$(call install_copy, wago-custom-install, 0, 0, 0644, -, /etc/specific/features/serial-service-interface)
	@$(call install_copy, wago-custom-install, 0, 0, 0644, -, /etc/specific/features/profibus-dp-slave)

ifneq ($(PTXCONF_WAGO_CUSTOM_LINK_DEVICES_BBINIT_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/link_devices, \
		/etc/rc.d/$(PTXCONF_WAGO_CUSTOM_LINK_DEVICES_BBINIT_LINK))
endif

ifneq ($(PTXCONF_WAGO_CUSTOM_RUNTIME_STARTSCRIPT),)
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, /etc/init.d/runtime, n)
	@$(call install_replace, wago-custom-install, /etc/init.d/runtime, @RTS_CGROUPS@, $(CGROUP_ENV))
	@$(call install_replace, wago-custom-install, /etc/init.d/runtime, @CDS3_PLCCONFIGFILE@, $(PTXCONF_CDS3_PLCCONFIGFILE));
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, /usr/bin/rtswrapper, n)

ifneq ($(PTXCONF_RUNTIME_RC_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/runtime, \
		/etc/rc.d/$(PTXCONF_RUNTIME_RC_LINK))
endif
endif



else
	@$(error "ERROR: please define one or more dev-names that need to be linked, or unselect WAGO_CUSTOM_INSTALL_LINKIN_DEVS")
endif

endif

ifdef PTXCONF_WAGO_CUSTOM_INSTALL_3G_MODEM_VERSION_INFO_FILE
	@$(eval _VERSION := $(call remove_quotes,$(PTXCONF_WAGO_CUSTOM_INSTALL_3G_MODEM_VERSION)))
	@$(eval _TMP_VERSION_FILE := $(PTXDIST_TEMPDIR)/modem_version)
	@echo -e $(_VERSION) > $(_TMP_VERSION_FILE)

	@$(call install_copy, wago-custom-install, 0, 0, 0755, $(_TMP_VERSION_FILE), /etc/specific/modem_version, n)
endif

ifdef PTXCONF_WAGO_CUSTOM_INSTALL_DETERMINE_HOSTNAME
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, /etc/init.d/determine_hostname, n)
ifneq ($(PTXCONF_WAGO_CUSTOM_DETERMINE_HOSTNAME_BBINIT_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/determine_hostname, \
		/etc/rc.d/$(PTXCONF_WAGO_CUSTOM_DETERMINE_HOSTNAME_BBINIT_LINK))
endif
endif

ifdef PTXCONF_WAGO_CUSTOM_INSTALL_CHECK_RTC
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, /etc/init.d/check_rtc, n)
ifneq ($(PTXCONF_WAGO_CUSTOM_CHECK_RTC_BBINIT_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/check_rtc, \
		/etc/rc.d/$(PTXCONF_WAGO_CUSTOM_CHECK_RTC_BBINIT_LINK))
endif
endif

ifdef PTXCONF_WAGO_CUSTOM_INSTALL_RTSVERSION
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, /etc/specific/rtsversion, n)
endif

ifdef PTXCONF_WAGO_CUSTOM_INSTALL_RMD_FW_DOWNLOAD
	@$(call install_alternative, wago-custom-install, 0, 0, 0755, /etc/init.d/rmd-fw-download, n)

ifneq ($(PTXCONF_WAGO_CUSTOM_RMD_FW_DOWNLOAD_BBINIT_LINK),)
	@$(call install_link, wago-custom-install, ../init.d/rmd-fw-download, /etc/rc.d/$(PTXCONF_WAGO_CUSTOM_RMD_FW_DOWNLOAD_BBINIT_LINK))

endif
endif


ifdef PTXCONF_WAGO_ADJUST_DEFAULT_SETTINGS

ifdef PTXCONF_INITMETHOD_SYSTEMD
	$(error "rc-once interface has to be confirmed to work with systemd!")
endif

ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, wago-custom-install, 0, 0, 0750, /etc/rc.once.d/adjust_defaults, n)
endif

endif #WAGO_ADJUST_DEFAULT_SETTINGS

ifdef PTXCONF_WAGO_CUSTOM_INSTALL_PROTOCOL_TFTP_ON
	@touch $(PKGDIR)/$(WAGO_CUSTOM_INSTALL)/etc/specific/features/tftp
	@$(call install_copy, wago-custom-install, 0, 0, 0644, -, /etc/specific/features/tftp)
endif

ifdef PTXCONF_WAGO_CUSTOM_INSTALL_PROTOCOL_BOOTP_ON
	@touch $(PKGDIR)/$(WAGO_CUSTOM_INSTALL)/etc/specific/features/bootp
	@$(call install_copy, wago-custom-install, 0, 0, 0644, -, /etc/specific/features/bootp)
endif

ifdef PTXCONF_WAGO_CUSTOM_INSTALL_IOCHECK_ON
	@touch $(PKGDIR)/$(WAGO_CUSTOM_INSTALL)/etc/specific/features/iocheck
	@$(call install_copy, wago-custom-install, 0, 0, 0644, -, /etc/specific/features/iocheck)
endif

	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/usr/sbin/setup_ssh_keys, \
		/usr/sbin/setup_ssh_keys, n)
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/usr/sbin/setup_https_key, \
		/usr/sbin/setup_https_key, n)
	@$(call install_copy, wago-custom-install, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/usr/sbin/random_seed, \
		/usr/sbin/random_seed, n)

#build IPK's 
ifdef PTXCONF_CDS3_TSCIOBACNET
	#--- bacnet ipk ---
	$(PTXDIST_WORKSPACE)/scripts/bacnet-helpers/make-metaipk_bacnet.sh $(BACNET_VERSION) $(BACNETSTACK_REVISION) $(PTXCONF_OPKG_OPKG_CONF_URL)
endif

	@$(call install_finish, wago-custom-install)

	@$(call touch)



# vim: syntax=make
