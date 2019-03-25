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

MOSQUITTO_VERSION	:= 1.4.11
MOSQUITTO_SUFFIX        := tar.gz
MOSQUITTO_MD5		:= c217dea4bdc7573a2eaea8387c18a19e

MOSQUITTO		:= mosquitto-$(MOSQUITTO_VERSION)
MOSQUITTO_URL		:= https://mosquitto.org/files/source/$(MOSQUITTO).$(MOSQUITTO_SUFFIX)
MOSQUITTO_SOURCE        := $(SRCDIR)/$(MOSQUITTO).$(MOSQUITTO_SUFFIX)
MOSQUITTO_DIR		:= $(BUILDDIR)/$(MOSQUITTO)
MOSQUITTO_LICENSE	:= BSD

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
    -DWITH_SRV:BOOL=OFF


# ----------------------------------------------------------------------------
# Compile (default)
# ----------------------------------------------------------------------------

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
	@$(call install_fixup, mosquitto, PRIORITY, optional)
	@$(call install_fixup, mosquitto, SECTION, base)
	@$(call install_fixup, mosquitto, AUTHOR, "M\&M Software GmbH")
	@$(call install_fixup, mosquitto, DESCRIPTION, missing)
	@$(call install_lib, mosquitto, 0, 0, 0644, libmosquitto)
#	@$(call install_lib, mosquitto, 0, 0, 0644, libmosquittopp)
	@$(call install_copy, mosquitto, 0, 0, 0755, -, /usr/bin/mosquitto_pub)
	@$(call install_copy, mosquitto, 0, 0, 0755, -, /usr/bin/mosquitto_sub)
#	@$(call install_copy, mosquitto, 0, 0, 0755, -, /usr/bin/mosquitto_passwd)
#	@$(call install_copy, mosquitto, 0, 0, 0755, -, /usr/sbin/mosquitto)
	@$(call install_finish, mosquitto)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/MOSQUITTO.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, MOSQUITTO)

# vim: syntax=make
