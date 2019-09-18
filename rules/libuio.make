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
PACKAGES-$(PTXCONF_LIBUIO) += libuio

#
# Paths and names
#
LIBUIO_SUFFIX	:= tar.gz
LIBUIO_VERSION	:= 0.2.8
LIBUIO_MD5	:= 89d008fa21de2a6d0a2e07d10b17d6d8
LIBUIO		:= libuio-$(LIBUIO_VERSION)
LIBUIO_URL	:= https://github.com/missinglinkelectronics/libuio/archive/releases/v$(LIBUIO_VERSION).$(LIBUIO_SUFFIX) 
LIBUIO_SOURCE	:= $(SRCDIR)/libuio-releases-v$(LIBUIO_VERSION).$(LIBUIO_SUFFIX)
LIBUIO_DIR	:= $(BUILDDIR)/$(LIBUIO)
LIBUIO_BUILD_ROOT:= YES
LIBUIO_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libuio.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(LIBUIO_DIR))
	@$(call extract, LIBUIO)
	@$(call patchin, LIBUIO)
	cd $(LIBUIO_DIR) && [ -f configure ] || sh autogen.sh
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#LIBUIO_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#

LIBUIO_CONF_OPT = $(filter-out  --libdir=/usr/lib,$(CROSS_AUTOCONF_USR))  \
                 CFLAGS="$(CFLAGS) -std=gnu11 -fPIC -g -O2" \
		 --enable-shared --enable-static

$(STATEDIR)/libuio.prepare:
	@$(call targetinfo)
	@$(call world/prepare, LIBUIO)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/libuio.compile:
#	@$(call targetinfo)
#	@$(call world/compile, LIBUIO)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/libuio.install:
#	@$(call targetinfo)
#	@$(call world/install, LIBUIO)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libuio.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libuio)
	@$(call install_fixup, libuio, PRIORITY, optional)
	@$(call install_fixup, libuio, SECTION, base)
	@$(call install_fixup, libuio, AUTHOR, "Benedikt Spranger <b.spranger@linutronix.de>")
	@$(call install_fixup, libuio, DESCRIPTION, missing)

ifdef PTXCONF_LSUIO_GIT
	@$(call install_copy, libuio, 0, 0, 0755, -, /usr/bin/lsuio)
endif
ifdef PTXCONF_READUIO_GIT
	@$(call install_copy, libuio, 0, 0, 0755, -, /usr/bin/readuio)
endif

	@$(call install_lib, libuio, 0, 0, 0644, libuio)

	@$(call install_finish, libuio)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/libuio.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, LIBUIO)

# vim: syntax=make
