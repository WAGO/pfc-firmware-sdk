# -*-makefile-*-
#
# Copyright (C) 2016 by M&M Software GmbH
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_MOSQUITTO) += mosquitto

#
# Paths and names
#
MOSQUITTO_VERSION	:= 1.6.7
MOSQUITTO_MD5		:= ec9074c4f337f64eaa9a4320c6dab020
MOSQUITTO		:= mosquitto-$(MOSQUITTO_VERSION)
MOSQUITTO_SUFFIX	:= tar.gz
MOSQUITTO_URL		:= https://mosquitto.org/files/source/$(MOSQUITTO).$(MOSQUITTO_SUFFIX)
MOSQUITTO_SOURCE	:= $(SRCDIR)/$(MOSQUITTO).$(MOSQUITTO_SUFFIX)
MOSQUITTO_DIR		:= $(BUILDDIR)/$(MOSQUITTO)
# "Eclipse Distribution License - v 1.0" is in fact BSD-3-Clause
MOSQUITTO_LICENSE	:= EPL-1.0 AND BSD-3-Clause
MOSQUITTO_LICENSE_FILES	:= file://epl-v10;md5=8d383c379e91d20ba18a52c3e7d3a979 \
	file://edl-v10;md5=c09f121939f063aeb5235972be8c722c

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(MOSQUITTO_SOURCE):
	@$(call targetinfo)
	@$(call get, MOSQUITTO)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------
$(STATEDIR)/MOSQUITTO.extract:
	@$(call targetinfo)
	@$(call clean, $(MOSQUITTO_DIR))
	@$(call extract, MOSQUITTO)
	@$(call patchin, MOSQUITTO)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------
MOSQUITTO_CONF_ENV	:= $(CROSS_ENV)
# cmake
MOSQUITTO_CONF_TOOL	:= cmake

MOSQUITTO_CONF_OPT	:= $(CROSS_CMAKE_USR) -v --jobs=1 \
    -DCMAKE_CXX_FLAGS:STRING="-std=gnu++11 -fPIC"  \
    -DCMAKE_C_FLAGS:STRING="-std=gnu99 -fPIC" \
    -DWITH_SRV:BOOL=OFF \
    -DWITH_STATIC_LIBRARIES=yes


# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------
#$(STATEDIR)/MOSQUITTO.install:
#	@$(call targetinfo)
#	make -C $(MOSQUITTO_DIR)-build  DESTDIR=$(PTXCONF_SYSROOT_TARGET) install
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/mosquitto.targetinstall:
	@$(call targetinfo)

	@$(call install_init, mosquitto)
	@$(call install_fixup, mosquitto,PRIORITY,optional)
	@$(call install_fixup, mosquitto,SECTION,base)
	@$(call install_fixup, mosquitto,AUTHOR, "M\&M Software GmbH")
	@$(call install_fixup, mosquitto,DESCRIPTION,missing)

	@$(call install_lib, mosquitto, 0, 0, 0644, libmosquitto)
#	@$(call install_lib, mosquitto, 0, 0, 0644, libmosquittopp)

	@$(call install_copy, mosquitto, 0, 0, 0755, -, /usr/bin/mosquitto_pub)
	@$(call install_copy, mosquitto, 0, 0, 0755, -, /usr/bin/mosquitto_sub)
#	@$(call install_copy, mosquitto, 0, 0, 0755, -, /usr/bin/mosquitto_passwd)
#	@$(call install_copy, mosquitto, 0, 0, 0755, -, /usr/sbin/mosquitto)

	@$(call install_copy, mosquitto, 0, 0, 0644, $(MOSQUITTO_DIR)/edl-v10, /usr/share/licenses/oss/license.mosquitto_$(MOSQUITTO_VERSION).txt)
	@$(call install_finish, mosquitto)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/MOSQUITTO.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, MOSQUITTO)

# vim: syntax=make
