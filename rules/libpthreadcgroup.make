# -*-makefile-*-
#
# Copyright (C) 2015 by <jso@wago.com> JSo
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBPTHREADCGROUP) += libpthreadcgroup

#
# Paths and names
#
LIBPTHREADCGROUP_VERSION	:= 0.0.1
LIBPTHREADCGROUP_MD5		:=
LIBPTHREADCGROUP		    := libpthreadcgroup
LIBPTHREADCGROUP_URL		:= $(PTXDIST_WORKSPACE)/local_src/$(LIBPTHREADCGROUP)
LIBPTHREADCGROUP_DIR		:= $(BUILDDIR)/$(LIBPTHREADCGROUP)
LIBPTHREADCGROUP_LICENSE	:= LGPL
LIBPTHREADCGROUP_FILE      := $(LIBPTHREADCGROUP).so.$(LIBPTHREADCGROUP_VERSION)


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libpthreadcgroup.extract:
	@$(call targetinfo)
	@$(call clean, $(LIBPTHREADCGROUP_DIR))
	@rsync -a $(LIBPTHREADCGROUP_URL)  $(BUILDDIR) 
	@$(call patchin, LIBPTHREADCGROUP)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#LIBPTHREADCGROUP_PATH	:= PATH=$(CROSS_PATH)
LIBPTHREADCGROUP_CONF_TOOL	:= NO
LIBPTHREADCGROUP_MAKE_ENV	:= $(CROSS_ENV)

#$(STATEDIR)/libpthreadcgroup.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, LIBPTHREADCGROUP)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/libpthreadcgroup.compile:
#	@$(call targetinfo)
#	@$(call world/compile, LIBPTHREADCGROUP)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libpthreadcgroup.install:
	@$(call targetinfo)
#	@$(call world/install, LIBPTHREADCGROUP)
	cp -d $(LIBPTHREADCGROUP_DIR)/$(LIBPTHREADCGROUP).a $(PTXCONF_SYSROOT_TARGET)/usr/lib/
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libpthreadcgroup.targetinstall:
	@$(call targetinfo)

# Packet contains no artifcats.

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libpthreadcgroup.clean:
	@$(call targetinfo)
	@-cd $(LIBPTHREADCGROUP_DIR) && \
		$(LIBPTHREADCGROUP_MAKE_ENV) $(LIBPTHREADCGROUP_PATH) $(MAKE) clean
	@$(call clean_pkg, LIBPTHREADCGROUP)

# vim: syntax=make
