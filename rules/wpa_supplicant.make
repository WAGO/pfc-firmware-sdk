# -*-makefile-*-
#
# Copyright (C) 2009 by Markus Rathgeb <rathgeb.markus@googlemail.com>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WPA_SUPPLICANT) += wpa_supplicant

#
# Paths and names
#
WPA_SUPPLICANT_NAME	:= wpa_supplicant
WPA_SUPPLICANT_VERSION	:= 2.10
WPA_SUPPLICANT_MD5	:= d26797fcb002898d4ee989179346e1cc
WPA_SUPPLICANT		:= $(WPA_SUPPLICANT_NAME)-$(WPA_SUPPLICANT_VERSION)
WPA_SUPPLICANT_SUFFIX	:= tar.gz
WPA_SUPPLICANT_URL	:= https://w1.fi/releases/$(WPA_SUPPLICANT).$(WPA_SUPPLICANT_SUFFIX)
WPA_SUPPLICANT_SOURCE	:= $(SRCDIR)/$(WPA_SUPPLICANT).$(WPA_SUPPLICANT_SUFFIX)
WPA_SUPPLICANT_DIR	:= $(BUILDDIR)/$(WPA_SUPPLICANT)
WPA_SUPPLICANT_SUBDIR	:= $(WPA_SUPPLICANT_NAME)
# Use '=' to delay $(shell ...) calls until this is needed
# ptxdist update: ptx/get_alternative has been renamed to ptx/get-alternative.
# FIXME PTXDIST_UPDATE
ifeq ($(PTXCONF_CONFIGFILE_VERSION), "2020.08.0")
WPA_SUPPLICANT_CONFIG	 = $(call ptx/get-alternative, config/wpasupplicant, defconfig)
else
WPA_SUPPLICANT_CONFIG	 = $(call ptx/get_alternative, config/wpasupplicant, defconfig)
endif
WPA_SUPPLICANT_DOTCONFIG:= $(BUILDDIR)/$(WPA_SUPPLICANT)/$(WPA_SUPPLICANT_SUBDIR)/.config
WPA_SUPPLICANT_LICENSE	:= BSD-3-Clause

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

WPA_SUPPLICANT_PATH	:= PATH=$(CROSS_PATH)
WPA_SUPPLICANT_MAKE_ENV	:= \
	$(CROSS_ENV) \
	LIBDIR=/usr/lib \
	BINDIR=/usr/sbin

#
# autoconf
#
WPA_SUPPLICANT_AUTOCONF := $(CROSS_AUTOCONF_USR)

$(STATEDIR)/wpa_supplicant.prepare:
	@$(call targetinfo)

#	# run 'make clean' as wpa_supplicant's build system does not recognize config changes
	@-cd $(WPA_SUPPLICANT_DIR)/$(WPA_SUPPLICANT_SUBDIR) && \
		$(WPA_SUPPLICANT_MAKE_ENV) $(WPA_SUPPLICANT_PATH) $(MAKE) clean

	@echo "CC=$(CROSS_CC)"				>  $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CFLAGS += -I$(PTXDIST_SYSROOT_TARGET)/include"		>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CFLAGS += -I$(PTXDIST_SYSROOT_TARGET)/usr/include"	>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "LDFLAGS += -L$(PTXDIST_SYSROOT_TARGET)/lib"		>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "LDFLAGS += -L$(PTXDIST_SYSROOT_TARGET)/usr/lib"		>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CONFIG_BACKEND=file"			>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CONFIG_WPS=y"				>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CONFIG_WPS2=y"				>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CONFIG_IEEE80211N=y"			>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CONFIG_CTRL_IFACE=y"			>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CONFIG_WPA_CLI_EDIT=y"			>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CONFIG_EAP_GTC=y"				>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CONFIG_EAP_LEAP=y"			>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CONFIG_EAP_MD5=y"				>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CONFIG_EAP_MSCHAPV2=y"			>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CONFIG_EAP_OTP=y"				>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CONFIG_EAP_PEAP=y"			>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CONFIG_EAP_TLS=y"				>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CONFIG_EAP_TTLS=y"			>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CONFIG_IEEE8021X_EAPOL=y"			>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CONFIG_PEERKEY=y"				>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CONFIG_PKCS12=y"				>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CONFIG_SMARTCARD=y"			>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CONFIG_DRIVER_WIRED=y"			>> $(WPA_SUPPLICANT_DOTCONFIG) && \
	echo "CONFIG_IEEE80211N=y"			>> $(WPA_SUPPLICANT_DOTCONFIG)

	@grep -e PTXCONF_WPA_SUPPLICANT_ $(PTXDIST_PTXCONFIG) | \
		sed -e 's/PTXCONF_WPA_SUPPLICANT_/CONFIG_/g' >> $(WPA_SUPPLICANT_DOTCONFIG)

	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wpa_supplicant.install:
	@$(call targetinfo)
	@$(call world/install, WPA_SUPPLICANT)

	install -D -m 644 "$(WPA_SUPPLICANT_DIR)/$(WPA_SUPPLICANT_SUBDIR)/dbus/dbus-wpa_supplicant.conf" \
		"$(WPA_SUPPLICANT_PKGDIR)/etc/dbus-1/system.d/wpa_supplicant.conf"
	install -D -m 644 "$(WPA_SUPPLICANT_DIR)/$(WPA_SUPPLICANT_SUBDIR)/dbus/fi.w1.wpa_supplicant1.service" \
		"$(WPA_SUPPLICANT_PKGDIR)/usr/share/dbus-1/system-services/fi.w1.wpa_supplicant1.service"

	@install -vD -m 644 "$(WPA_SUPPLICANT_DIR)/$(WPA_SUPPLICANT_SUBDIR)/systemd/wpa_supplicant.service" \
		"$(WPA_SUPPLICANT_PKGDIR)/usr/lib/systemd/system/wpa_supplicant.service"

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wpa_supplicant.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  wpa_supplicant)
	@$(call install_fixup, wpa_supplicant,PRIORITY,optional)
	@$(call install_fixup, wpa_supplicant,SECTION,base)
	@$(call install_fixup, wpa_supplicant,AUTHOR,"Markus Rathgeb <rathgeb.markus@googlemail.com>")
	@$(call install_fixup, wpa_supplicant,DESCRIPTION,missing)

	@$(call install_copy, wpa_supplicant, 0, 0, 0755, -, \
		/usr/sbin/wpa_supplicant)

ifdef PTXCONF_WPA_SUPPLICANT_PASSPHRASE
	@$(call install_copy, wpa_supplicant, 0, 0, 0755, -, \
		/usr/sbin/wpa_passphrase)
endif

ifdef PTXCONF_WPA_SUPPLICANT_INSTALL_CLI
	@$(call install_copy, wpa_supplicant, 0, 0, 0755, -, /usr/sbin/wpa_cli)
endif

ifdef PTXCONF_WPA_SUPPLICANT_CTRL_IFACE_DBUS
	@$(call install_alternative, wpa_supplicant, 0, 0, 0644, \
		/etc/dbus-1/system.d/wpa_supplicant.conf)
	@$(call install_alternative, wpa_supplicant, 0, 0, 0644, \
		/usr/share/dbus-1/system-services/fi.w1.wpa_supplicant1.service)
endif
ifdef PTXCONF_INITMETHOD_SYSTEMD
	@$(call install_alternative, wpa_supplicant, 0, 0, 0644, \
		/usr/lib/systemd/system/wpa_supplicant.service)
endif

	@$(call install_alternative, wpa_supplicant, 0, 0, 0644, \
		/etc/wpa_supplicant.conf)

	@$(call install_finish, wpa_supplicant)

	@$(call touch)

# vim: syntax=make
