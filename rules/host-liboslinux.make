# -*-makefile-*-
#
# Copyright (C) 2012 by Jan Sondhauss <jan.sondhauss@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_LIBOSLINUX) += host-liboslinux

#
# Paths and names
#
HOST_LIBOSLINUX_VERSION	:= 1.0.0.0
HOST_LIBOSLINUX			:= liboslinux-$(HOST_LIBOSLINUX_VERSION)
HOST_LIBOSLINUX_SOURCE	:= $(SRCDIR)/$(HOST_LIBOSLINUX)
HOST_LIBOSLINUX_DIR		:= $(HOST_BUILDDIR)/$(HOST_LIBOSLINUX)
HOST_LIBOSLINUX_MD5 :=

HOST_LIBOSLINUX_PATH:= PATH=$(HOST_PATH)
HOST_LIBOSLINUX_ENV:=  $(HOST_ENV)
HOST_LIBOSLINUX_AUTOCONF := $(HOST_AUTOCONF)


$(STATEDIR)/host-liboslinux.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/host-liboslinux.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@mkdir -p $(HOST_LIBOSLINUX_DIR)
	@rsync -a --exclude=.libs/ --exclude="*.o" --exclude="*.a" --exclude="*.so" $(HOST_LIBOSLINUX_SOURCE) $(HOST_BUILDDIR)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/host-liboslinux.prepare:
	@$(call targetinfo)
	echo $(HOST_LIBOSLINUX_CONF_OPT)
	@cd $(HOST_LIBOSLINUX_DIR) && sh autogen.sh
	@$(call world/prepare, HOST_LIBOSLINUX)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/host-liboslinux.compile:
	@$(call targetinfo)
	@cd $(HOST_LIBOSLINUX_DIR)
	@$(call world/compile, HOST_LIBOSLINUX)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-liboslinux.install:
	@$(call targetinfo)
	@$(call world/install, HOST_LIBOSLINUX)
	@mkdir -p $(PTXCONF_SYSROOT_HOST)/usr/lib/
	@rsync --exclude="*.o" $(HOST_LIBOSLINUX_DIR)/liboslinux/.libs/* $(PTXCONF_SYSROOT_HOST)/usr/lib/
	@rsync --include="*.h" $(HOST_LIBOSLINUX_DIR)/include/*.h $(PTXCONF_SYSROOT_HOST)/usr/include/
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------


# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/host-liboslinux.clean:
#	@$(call targetinfo)
#	@-rm -rf $(HOST_LIBOSLINUX_DIR)
#	@-cd $(PTXCONF_SYSROOT_HOST)/usr/include && rm -f OsIncludes.h OsMemory.h OsMutex.h OsRwLock.h OsSemaphore.h OsThread.h OsTime.h OsTrace.h OsTypedefs.h
#	@$(call clean_pkg, HOST_LIBOSLINUX)

# vim: syntax=make
