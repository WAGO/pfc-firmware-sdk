# -*-makefile-*-
#
# Copyright (C) 2012 by <andrej.gantvorg@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_CPPUTEST) += host-cpputest

#
# Paths and names
#
HOST_CPPUTEST_VERSION	:= 3.7.2
HOST_CPPUTEST_MD5       := f4f7d62cb78e360ee3c979ee834f88b4
HOST_CPPUTEST		:= cpputest-$(HOST_CPPUTEST_VERSION)
HOST_CPPUTEST_SUFFIX	:= tar.gz
# Note that prefix 'v' is missing in URL of version 3.7.2: URL must b fixed on update
HOST_CPPUTEST_URL	:= https://github.com/cpputest/cpputest/releases/download/$(HOST_CPPUTEST_VERSION)/cpputest-$(HOST_CPPUTEST_VERSION).$(HOST_CPPUTEST_SUFFIX) 
HOST_CPPUTEST_SOURCE	:= $(SRCDIR)/cpputest-$(HOST_CPPUTEST_VERSION).$(HOST_CPPUTEST_SUFFIX)
HOST_CPPUTEST_DIR	:= $(HOST_BUILDDIR)/$(HOST_CPPUTEST)
HOST_CPPUTEST_HOME	:= $(PTXDIST_SYSROOT_HOST)/usr/local

#-----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# cmake
#
HOST_CPPUTEST_CONF_TOOL	:= cmake
HOST_CPPUTEST_CONF_OPT += -DC++11=ON
HOST_CPPUTEST_CONF_OPT += -DCPPUTEST_FLAGS=OFF


#$(STATEDIR)/host-cpputest.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, HOST_CPPUTEST)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/host-cpputest.compile:
#	@$(call targetinfo)
#	@$(call world/compile, HOST_CPPUTEST)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-cpputest.install:
	@$(call targetinfo)
	@$(call world/install, HOST_CPPUTEST)
	@mkdir -p $(HOST_CPPUTEST_HOME)
	@cp -ar $(HOST_BUILDDIR)/$(HOST_CPPUTEST)/build $(HOST_CPPUTEST_HOME)
	# The project's build dir under build-host has to be referenced via
   # CPPUTEST_HOME variable.
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/host-cpputest.clean:
	@$(call targetinfo)
	@$(call clean_pkg, HOST_CPPUTEST)

# vim: syntax=make
