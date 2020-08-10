# -*-makefile-*-
#
# Copyright (C) 2012 by <WAGO Kontakttechnik GmbH & Co. KG>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBOSLINUX) += liboslinux

#
# Paths and names
#
LIBOSLINUX_VERSION	:= 1.0.1.0
LIBOSLINUX		:= liboslinux-$(LIBOSLINUX_VERSION)
LIBOSLINUX_SRC		:= $(SRCDIR)/liboslinux
LIBOSLINUX_DIR		:= $(BUILDDIR)/$(LIBOSLINUX)
LIBOSLINUX_LICENSE	:= unknown


LIBOSLINUX_PATH:= PATH=$(CROSS_PATH)
LIBOSLINUX_ENV:=  $(CROSS_ENV)
LIBOSLINUX_AUTOCONF := $(CROSS_AUTOCONF)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/liboslinux.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@mkdir -p $(LIBOSLINUX_DIR)
	@rsync -a --exclude=.libs/ --exclude="*.o" --exclude="*.a" --exclude="*.so" --exclude="*.so" $(LIBOSLINUX_SRC)/ $(LIBOSLINUX_DIR)
	@cd $(LIBOSLINUX_DIR) && sh autogen.sh
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#LIBOSLINUX_PATH	:= PATH=$(CROSS_PATH)

ifdef PTXCONF_LIBOSLINUX_TRACE_ENABLED
LIBOSLINUX_CONF_OPT:= $(CROSS_AUTOCONF_USR) --enable-trace
endif

$(STATEDIR)/liboslinux.prepare:
	@$(call targetinfo)
	echo $(LIBOSLINUX_CONF_OPT)
	@$(call world/prepare, LIBOSLINUX)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/liboslinux.compile:
#	@$(call targetinfo)
#	@$(call world/compile, LIBOSLINUX)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/liboslinux.install:
#	@$(call targetinfo)
#	@$(call world/install, LIBOSLINUX)
#	@rsync --exclude="*.o" $(LIBOSLINUX_DIR)/liboslinux/.libs/* $(PTXCONF_SYSROOT_TARGET)/usr/lib/
#	@rsync --include="*.h" $(LIBOSLINUX_DIR)/include/*.h $(PTXCONF_SYSROOT_TARGET)/usr/include/
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/liboslinux.targetinstall:
	@$(call targetinfo)

	@$(call install_init, liboslinux)
	@$(call install_fixup, liboslinux,PRIORITY,optional)
	@$(call install_fixup, liboslinux,SECTION,base)
	@$(call install_fixup, liboslinux,AUTHOR,"<WAGO Kontakttechnik GmbH \& Co. KG>")
	@$(call install_fixup, liboslinux,DESCRIPTION,missing)

#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish
#
	@$(call install_copy, liboslinux, 0, 0, 0755, $(LIBOSLINUX_DIR)/liboslinux/.libs/liboslinux.so.0.0.0, /usr/lib/liboslinux.so.0.0.0)
	@$(call install_link, liboslinux, liboslinux.so.0.0.0, /usr/lib/liboslinux.so)
	@$(call install_link, liboslinux, liboslinux.so.0.0.0, /usr/lib/liboslinux.so.0)

	@$(call install_finish, liboslinux)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#PTX_ROOT=$(PTXCONF_SYSROOT_TARGET)/../root
#PTX_ROOT_DEBUG=$(PTXCONF_SYSROOT_TARGET)/../root-debug

#$(STATEDIR)/liboslinux.clean:
#	@$(call targetinfo)
#	@-rm -rf $(LIBOSLINUX_DIR)
#	@-rm -f $(PTX_ROOT)/usr/lib/liboslinux*
#	@-rm -f $(PTX_ROOT_DEBUG)/usr/lib/liboslinux*
#	@-rm -f $(PTXCONF_SYSROOT_TARGET)/usr/lib/liboslinux.*
#	@-cd $(PTXCONF_SYSROOT_TARGET)/usr/include && rm -f OsIncludes.h OsMemory.h OsMutex.h OsRwLock.h OsSemaphore.h OsThread.h OsTime.h OsTrace.h OsTypedefs.h
#	@-cd $(LIBOSLINUX_DIR) && \
#		$(LIBOSLINUX_ENV) $(LIBOSLINUX_PATH) $(MAKE) clean
#	@$(call clean_pkg, LIBOSLINUX)

# vim: syntax=make
