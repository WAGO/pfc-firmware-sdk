# -*-makefile-*-
#
# Copyright (C) 2005-2008 by Robert Schwebel
#               2012 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_UDEV_LEGACY) += udev-legacy

#
# Paths and names
#
UDEV_LEGACY_VERSION	:= 182
UDEV_LEGACY_MD5		:= e31c83159b017e8ab0fa2f4bca758a41
UDEV_LEGACY		:= udev-$(UDEV_LEGACY_VERSION)
UDEV_LEGACY_SUFFIX	:= tar.bz2
UDEV_LEGACY_URL		:= $(call ptx/mirror, KERNEL, utils/kernel/hotplug/$(UDEV_LEGACY).$(UDEV_LEGACY_SUFFIX))
UDEV_LEGACY_SOURCE	:= $(SRCDIR)/$(UDEV_LEGACY).$(UDEV_LEGACY_SUFFIX)
UDEV_LEGACY_DIR		:= $(BUILDDIR)/$(UDEV_LEGACY)
UDEV_LEGACY_LICENSE	:= GPL-2.0-only

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
UDEV_LEGACY_CONF_ENV := \
	$(CROSS_ENV) \
	CPPFLAGS="-isystem $(KERNEL_HEADERS_INCLUDE_DIR) $(CROSS_CPPFLAGS)"

UDEV_LEGACY_CONF_TOOL	:= autoconf
UDEV_LEGACY_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--libexecdir=/usr/lib \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--disable-static \
	--enable-shared \
	--disable-gtk-doc \
	--disable-gtk-doc-html \
	--disable-gtk-doc-pdf \
	--$(call ptx/endis,PTXCONF_UDEV_LEGACY_DEBUG)-debug \
	--$(call ptx/endis,PTXCONF_UDEV_LEGACY_SYSLOG)-logging \
	--disable-manpages \
	--disable-gudev \
	--disable-introspection \
	--$(call ptx/endis,PTXCONF_UDEV_LEGACY_KEYMAPS)-keymap \
	--$(call ptx/endis,PTXCONF_UDEV_LEGACY_MTD_PROBE)-mtd_probe \
	--$(call ptx/endis,PTXCONF_UDEV_LEGACY_PERSISTENT_GENERATOR)-rule_generator \
	--disable-floppy \
	--without-selinux \
	--with-usb-ids-path=/usr/share/usb.ids \
	--with-pci-ids-path=/usr/share/pci.ids$(call ptx/ifdef, PTXCONF_PCIUTILS_COMPRESS,.gz,)

UDEV_LEGACY_RULES-y := \
	50-udev-default.rules \
	60-persistent-alsa.rules \
	60-persistent-input.rules \
	60-persistent-serial.rules \
	60-persistent-storage-tape.rules \
	60-persistent-storage.rules \
	75-net-description.rules \
	75-tty-description.rules \
	78-sound-card.rules \
	95-udev-late.rules

UDEV_LEGACY_RULES-$(PTXCONF_UDEV_LEGACY_PERSISTENT_CDROM)	+= 60-cdrom_id.rules
UDEV_LEGACY_RULES-$(PTXCONF_UDEV_LEGACY_PERSISTENT_V4L)		+= 60-persistent-v4l.rules
UDEV_LEGACY_RULES-$(PTXCONF_UDEV_LEGACY_ACCELEROMETER)		+= 61-accelerometer.rules
UDEV_LEGACY_RULES-$(PTXCONF_UDEV_LEGACY_PERSISTENT_GENERATOR)	+= 75-cd-aliases-generator.rules
UDEV_LEGACY_RULES-$(PTXCONF_UDEV_LEGACY_PERSISTENT_GENERATOR)	+= 75-persistent-net-generator.rules
UDEV_LEGACY_RULES-$(PTXCONF_UDEV_LEGACY_MTD_PROBE)		+= 75-probe_mtd.rules
UDEV_LEGACY_RULES-$(PTXCONF_UDEV_LEGACY_DRIVERS_RULES)		+= 80-drivers.rules
UDEV_LEGACY_RULES-$(PTXCONF_UDEV_LEGACY_KEYMAPS)		+= 95-keyboard-force-release.rules
UDEV_LEGACY_RULES-$(PTXCONF_UDEV_LEGACY_KEYMAPS)		+= 95-keymap.rules

UDEV_LEGACY_HELPER-$(PTXCONF_UDEV_LEGACY_ACCELEROMETER)		+= accelerometer
UDEV_LEGACY_HELPER-$(PTXCONF_UDEV_LEGACY_PERSISTENT_ATA)	+= ata_id
UDEV_LEGACY_HELPER-$(PTXCONF_UDEV_LEGACY_PERSISTENT_CDROM)	+= cdrom_id
UDEV_LEGACY_HELPER-$(PTXCONF_UDEV_LEGACY_COLLECT)		+= collect
UDEV_LEGACY_HELPER-$(PTXCONF_UDEV_LEGACY_KEYMAPS)		+= findkeyboards
UDEV_LEGACY_HELPER-$(PTXCONF_UDEV_LEGACY_KEYMAPS)		+= keyboard-force-release.sh
UDEV_LEGACY_HELPER-$(PTXCONF_UDEV_LEGACY_KEYMAPS)		+= keymap
UDEV_LEGACY_HELPER-$(PTXCONF_UDEV_LEGACY_MTD_PROBE)		+= mtd_probe
UDEV_LEGACY_HELPER-$(PTXCONF_UDEV_LEGACY_PERSISTENT_GENERATOR)	+= rule_generator.functions
UDEV_LEGACY_HELPER-$(PTXCONF_UDEV_LEGACY_PERSISTENT_SCSI)	+= scsi_id
UDEV_LEGACY_HELPER-$(PTXCONF_UDEV_LEGACY_PERSISTENT_V4L)	+= v4l_id
UDEV_LEGACY_HELPER-$(PTXCONF_UDEV_LEGACY_PERSISTENT_GENERATOR)	+= write_cd_rules
UDEV_LEGACY_HELPER-$(PTXCONF_UDEV_LEGACY_PERSISTENT_GENERATOR)	+= write_net_rules

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/udev-legacy.targetinstall:
	@$(call targetinfo)

	@$(call install_init, udev)
	@$(call install_fixup, udev,PRIORITY,optional)
	@$(call install_fixup, udev,SECTION,base)
	@$(call install_fixup, udev,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, udev,DESCRIPTION,"udev standalone")

ifdef PTXCONF_UDEV_LEGACY_ETC_CONF
	@$(call install_alternative, udev, 0, 0, 0644, /etc/udev/udev.conf)
endif

	@$(call install_copy, udev, 0, 0, 0755, -, /usr/lib/udev/udevd)
	@$(call install_copy, udev, 0, 0, 0755, -, /usr/bin/udevadm)

	@$(foreach rule, $(UDEV_LEGACY_RULES-y), \
		$(call install_copy, udev, 0, 0, 0644, -, \
			/usr/lib/udev/rules.d/$(rule));)

ifdef PTXCONF_UDEV_LEGACY_KEYMAPS
	@cd $(UDEV_LEGACY_PKGDIR) && \
	for keymap in `find usr/lib/udev/keymaps/ -type f`; do \
		$(call install_copy, udev, 0, 0, 0644, -, /$$keymap); \
	done
endif

ifdef PTXCONF_UDEV_LEGACY_CUST_RULES
	@$(call install_alternative_tree, udev, 0, 0, /usr/lib/udev/rules.d)
endif

	@$(foreach helper, $(UDEV_LEGACY_HELPER-y), \
		$(call install_copy, udev, 0, 0, 0755, -, \
			/usr/lib/udev/$(helper));)

ifdef PTXCONF_UDEV_LEGACY_LIBUDEV
	@$(call install_lib, udev, 0, 0, 0644, libudev)
endif

ifdef PTXCONF_UDEV_LEGACY_STARTSCRIPT
ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, udev, 0, 0, 0755, /etc/init.d/udev)

ifneq ($(call remove_quotes,$(PTXCONF_UDEV_BBINIT_LINK)),)
	@$(call install_link, udev, \
		../init.d/udev, \
		/etc/rc.d/$(PTXCONF_UDEV_BBINIT_LINK))
endif
endif
endif
	@$(call install_finish, udev)

	@$(call touch)

# vim: syntax=make
