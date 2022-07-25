# -*-makefile-*-
#
# Copyright (C) 2003-2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBPCAP) += libpcap

#
# Paths and names
#
LIBPCAP_VERSION	:= 1.10.1
LIBPCAP_MD5	:= 28e17495004036567c2cc884b51eba45
LIBPCAP		:= libpcap-$(LIBPCAP_VERSION)
LIBPCAP_SUFFIX	:= tar.gz
LIBPCAP_URL	:= http://www.tcpdump.org/release/$(LIBPCAP).$(LIBPCAP_SUFFIX)
LIBPCAP_SOURCE	:= $(SRCDIR)/$(LIBPCAP).$(LIBPCAP_SUFFIX)
LIBPCAP_DIR	:= $(BUILDDIR)/$(LIBPCAP)
LIBPCAP_LICENSE	:= BSD-3-Clause

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBPCAP_CONF_ENV  := \
	$(CROSS_ENV) \
	ac_cv_lbl_hci_channel_monitor_is_defined=no

LIBPCAP_COMPILE_ENV := \
	$(CROSS_ENV_CFLAGS) \
	$(CROSS_ENV_CPPFLAGS) \
	$(CROSS_ENV_LDFLAGS) \
	$(CROSS_ENV_AR)

#
# autoconf
#
LIBPCAP_CONF_TOOL	:= autoconf
LIBPCAP_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--enable-protochain \
	$(GLOBAL_IPV6_OPTION) \
	--disable-remote \
	--disable-optimizer-dbg \
	--disable-yydebug \
	--disable-universal \
	--enable-shared \
	--disable-usb \
	--disable-netmap \
	--$(call ptx/endis, PTXCONF_LIBPCAP_BLUETOOTH)-bluetooth \
	--disable-dbus \
	--disable-rdma \
	--with-pcap=linux \
	--with-libnl \
	--without-dag \
	--without-septel \
	--without-snf \
	--without-turbocap

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libpcap.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  libpcap)
	@$(call install_fixup, libpcap,PRIORITY,optional)
	@$(call install_fixup, libpcap,SECTION,base)
	@$(call install_fixup, libpcap,AUTHOR,"Marc Kleine-Budde <mkl@pengutronix.de>")
	@$(call install_fixup, libpcap,DESCRIPTION,missing)

	@$(call install_lib, libpcap, 0, 0, 0644, libpcap)

	@$(call install_finish, libpcap)

	@$(call touch)

# vim: syntax=make
