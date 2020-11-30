# -*-makefile-*-
#
# Copyright (C) 2014 by Benedikt Spranger <b.spranger@linutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#A
PACKAGES-$(PTXCONF_LIBNSL) += libnsl

#
# Paths and names
#
LIBNSL_SUFFIX	:= tar.gz
LIBNSL_VERSION	:= 1.2.0
LIBNSL_MD5	:= 05222024777e12e2f6d48f2e7d067ac7
LIBNSL		:= libnsl-$(LIBNSL_VERSION)
LIBNSL_URL	:= https://github.com/thkukuk/libnsl/archive/v$(LIBNSL_VERSION).$(LIBNSL_SUFFIX) 
LIBNSL_SOURCE	:= $(SRCDIR)/$(LIBNSL).$(LIBNSL_SUFFIX)
LIBNSL_DIR	:= $(BUILDDIR)/$(LIBNSL)
LIBNSL_BUILD_ROOT:= YES
LIBNSL_LICENSE	:= lgpl
LIBNSL_LICENSE_FILE := COPYING

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libnsl.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(LIBNSL_DIR))
	@$(call extract, LIBNSL)
	@$(call patchin, LIBNSL)
	cd $(LIBNSL_DIR) && [ -f configure ] || (autoreconf -i -f && sh autogen.sh)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#LIBNSL_CONF_ENV	:= $(CROSS_ENV)

LIBNSL_CONF_TOOL	:= autoconf
LIBNSL_CONF_OPT	:= $(CROSS_AUTOCONF_USR)
#
# autoconf
#

$(STATEDIR)/libnsl.prepare:
	@$(call targetinfo)
	@$(call world/prepare, LIBNSL)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/libnsl.compile:
#	@$(call targetinfo)
#	@$(call world/compile, LIBNSL)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/libnsl.install:
#	@$(call targetinfo)
#	@$(call world/install, LIBNSL)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libnsl.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libnsl)
	@$(call install_fixup, libnsl, PRIORITY, optional)
	@$(call install_fixup, libnsl, SECTION, base)
	@$(call install_fixup, libnsl, AUTHOR, "Benedikt Spranger <b.spranger@linutronix.de>")
	@$(call install_fixup, libnsl, DESCRIPTION, missing)

	@$(call install_lib, libnsl, 0, 0, 0644, libnsl)
	@$(call install_copy, libnsl, 0, 0, 0644, $(LIBNSL_DIR)/$(LIBNSL_LICENSE_FILE), /usr/share/licenses/oss/license.libnsl_$(LIBNSL_VERSION).txt)

	@$(call install_finish, libnsl)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/libnsl.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, LIBNSL)

# vim: syntax=make
