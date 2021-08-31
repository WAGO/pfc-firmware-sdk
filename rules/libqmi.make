# -*-makefile-*-
#
# Copyright (C) 2015 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBQMI) += libqmi

#
# Paths and names
#
LIBQMI_VERSION	:= 1.24.4
LIBQMI_MD5	:= be6539fde54fec1fc9d852db201c8560
LIBQMI		:= libqmi-$(LIBQMI_VERSION)
LIBQMI_SUFFIX	:= tar.xz
LIBQMI_URL	:= http://www.freedesktop.org/software/libqmi/$(LIBQMI).$(LIBQMI_SUFFIX)
LIBQMI_SOURCE	:= $(SRCDIR)/$(LIBQMI).$(LIBQMI_SUFFIX)
LIBQMI_DIR	:= $(BUILDDIR)/$(LIBQMI)
LIBQMI_LICENSE	:= GPL-2.0+, LGPL-2.1+

#Filename for license text of libqmi-glib (LGPL)
LIBQMI_LICENSE_FILE_LIB :=  COPYING.LIB
#Filename for license text of qmicli and qmi-network (GPL)
#Currently not in use because the binaries are not needed
LIBQMI_LICENSE_FILE := COPYING
#Filenmae for author list
LIBQMI_AUTHORS := AUTHORS

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
LIBQMI_CONF_TOOL	:= autoconf
LIBQMI_CONF_OPT		:= \
	$(CROSS_AUTOCONF_USR) \
        CFLAGS="$(CFLAGS) -g -Os" \
	--disable-more-warnings \
	--disable-gtk-doc \
	--disable-gtk-doc-html \
	--disable-gtk-doc-pdf \
	--disable-firmware-update \
	--disable-mm-runtime-check \
	--with-udev-base-dir=/lib/udev 

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libqmi.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libqmi)
	@$(call install_fixup, libqmi,PRIORITY,optional)
	@$(call install_fixup, libqmi,SECTION,base)
	@$(call install_fixup, libqmi,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, libqmi,DESCRIPTION,missing)

	@#Binaries are unused
	@#@$(call install_copy, libqmi, 0, 0, 0755, -, /usr/bin/qmicli)
	@#@$(call install_copy, libqmi, 0, 0, 0755, -, /usr/bin/qmi-network)
	@#@$(call install_copy, libqmi, 0, 0, 0755, -, /usr/libexec/qmi-proxy)
	
	@#Combine authors and license for libqmi-glib
	@cat $(LIBQMI_DIR)/$(LIBQMI_AUTHORS) $(LIBQMI_DIR)/$(LIBQMI_LICENSE_FILE_LIB) > $(LIBQMI_DIR)/libqmi-license
	@$(call install_copy, libqmi, 0, 0, 0644, $(LIBQMI_DIR)/libqmi-license, /usr/share/licenses/oss/license.libqmi_$(LIBQMI_VERSION).txt)
	@$(call install_lib, libqmi, 0, 0, 0644, libqmi-glib)
	
	@$(call install_finish, libqmi)
	@$(call touch)
	
	# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libqmi.clean:
	@$(call targetinfo)
	@$(call clean_pkg, libqmi)
	
# vim: syntax=make

