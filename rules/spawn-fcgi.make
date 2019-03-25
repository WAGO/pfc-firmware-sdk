# -*-makefile-*-
#
# Copyright (C) 2013 by <AGa>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_SPAWN_FCGI) += spawn_fcgi

#
# Paths and names
#
SPAWN_FCGI_VERSION	:= 1.6.3
SPAWN_FCGI		:= spawn-fcgi-$(SPAWN_FCGI_VERSION)
SPAWN_FCGI_SUFFIX	:= tar.bz2
SPAWN_FCGI_URL		:= http://download.lighttpd.net/spawn-fcgi/releases-1.6.x/$(SPAWN_FCGI).$(SPAWN_FCGI_SUFFIX)
SPAWN_FCGI_SOURCE	:= $(SRCDIR)/$(SPAWN_FCGI).$(SPAWN_FCGI_SUFFIX)
SPAWN_FCGI_DIR		:= $(BUILDDIR)/$(SPAWN_FCGI)
SPAWN_FCGI_LICENSE	:= Custom License
SPAWN_FCGI_MD5      := 787ed2f88d2204bf1fe4fbd6e509d1d7

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(SPAWN_FCGI_SOURCE):
	@$(call targetinfo)
	@$(call get, SPAWN_FCGI)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#SPAWN_FCGI_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
SPAWN_FCGI_CONF_TOOL	:= autoconf
#SPAWN_FCGI_CONF_OPT	:= $(CROSS_AUTOCONF_USR)

#$(STATEDIR)/spawn_fcgi.prepare:
#	@$(call targetinfo)
#	@$(call clean, $(SPAWN_FCGI_DIR)/config.cache)
#	cd $(SPAWN_FCGI_DIR) && \
#		$(SPAWN_FCGI_PATH) $(SPAWN_FCGI_ENV) \
#		./configure $(SPAWN_FCGI_CONF_OPT)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/spawn_fcgi.compile:
#	@$(call targetinfo)
#	@$(call world/compile, SPAWN_FCGI)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/spawn_fcgi.install:
#	@$(call targetinfo)
#	@$(call world/install, SPAWN_FCGI)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/spawn_fcgi.targetinstall:
	@$(call targetinfo)

	@$(call install_init, spawn_fcgi)
	@$(call install_fixup, spawn_fcgi,PRIORITY,optional)
	@$(call install_fixup, spawn_fcgi,SECTION,base)
	@$(call install_fixup, spawn_fcgi,AUTHOR,"<AGa>")
	@$(call install_fixup, spawn_fcgi,DESCRIPTION,missing)

	@$(call install_copy, spawn_fcgi, 0, 0, 0755, $(SPAWN_FCGI_DIR)/src/spawn-fcgi, \
		/usr/bin/spawn-fcgi)

	@$(call install_finish, spawn_fcgi)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/spawn_fcgi.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, SPAWN_FCGI)

# vim: syntax=make
