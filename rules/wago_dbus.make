# -*-makefile-*-
#
# Copyright (C) 2012 by Hans Florian Scholz <hans-florian.scholz@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WAGO_DBUS) += wago_dbus

#
# Paths and names 
#
ifdef PTXCONF_WAGO_DBUS_TRUNK
WAGO_DBUS_VERSION	:= trunk
else
WAGO_DBUS_VERSION	:= 1.0
endif
WAGO_DBUS		:= wago_dbus
WAGO_DBUS_URL		:= file://$(SRCDIR)/$(WAGO_DBUS)
WAGO_DBUS_SRC		:= $(SRCDIR)/$(WAGO_DBUS)
WAGO_DBUS_DIR		:= $(BUILDDIR)/$(WAGO_DBUS)
WAGO_DBUS_BUILD_OOT	:= NO
WAGO_DBUS_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/wago_dbus.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(WAGO_DBUS_DIR))
	#@$(call extract, WAGO_DBUS)
	@mkdir -p $(WAGO_DBUS_DIR)
	@rsync -a --exclude=".project" --exclude=".cproject"  $(WAGO_DBUS_SRC) $(BUILDDIR)
#	cp -r $(SRCDIR)/$(WAGO_DBUS) $(WAGO_DBUS_DIR)
	@$(call patchin, WAGO_DBUS)

ifdef PTXCONF_WAGO_DBUS_TRUNK
	cd $(WAGO_DBUS_DIR) && sh autogen.sh
else
	#cd $(WAGO_DBUS_DIR) && [ -f configure ] || sh autogen.sh
	cd $(WAGO_DBUS_DIR) && sh autogen.sh
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#WAGO_DBUS_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
WAGO_DBUS_CONF_TOOL	:= autoconf
WAGO_DBUS_CONF_OPT	:= $(CROSS_AUTOCONF_USR)

ifdef PTXCONF_WAGO_DBUS_INSTALL_TEST_TOOL
        WAGO_DBUS_CONF_OPT += --enable-testtool
        WAGO_DBUS_CONF_OPT += LIBS=-lrt
endif


#$(STATEDIR)/wago_dbus.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, WAGO_DBUS)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wago_dbus.compile:
	@$(call targetinfo)
#	@rsync -a --exclude=".project" --exclude=".cproject"  $(WAGO_DBUS_SRC) $(BUILDDIR)
	@$(call world/compile, WAGO_DBUS)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/wago_dbus.install:
#	@$(call targetinfo)
#	@$(call world/install, WAGO_DBUS)
#	@$(call touch)
#	cp $(WAGO_DBUS_DIR)/wago_dbus_API.h $(PTXCONF_SYSROOT_TARGET)/usr/include
#	cp -d $(WAGO_DBUS_DIR)/.libs/libwago.a $(PTXCONF_SYSROOT_TARGET)/usr/lib
#	cp -d $(WAGO_DBUS_DIR)/.libs/libwago_dbus.so $(PTXCONF_SYSROOT_TARGET)/usr/lib
#	cp -d $(WAGO_DBUS_DIR)/.libs/libwago_dbus.so.0.0.0 $(PTXCONF_SYSROOT_TARGET)/usr/lib

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago_dbus.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wago_dbus)
	@$(call install_fixup, wago_dbus, PRIORITY, optional)
	@$(call install_fixup, wago_dbus, SECTION, base)
	@$(call install_fixup, wago_dbus, AUTHOR, "Hans Florian Scholz <hans-florian.scholz@wago.com>")
	@$(call install_fixup, wago_dbus, DESCRIPTION, missing)

#	#
#	# example code:; copy all libraries, links and binaries
#	#

	@for i in $(shell cd $(WAGO_DBUS_PKGDIR) && find bin sbin usr/bin usr/sbin -type f); do \
		$(call install_copy, wago_dbus, 0, 0, 0755, -, /$$i); \
	done
	@for i in $(shell cd $(WAGO_DBUS_PKGDIR) && find lib usr/lib -name "*.so*"); do \
		$(call install_copy, wago_dbus, 0, 0, 0644, -, /$$i); \
	done
	@links="$(shell cd $(WAGO_DBUS_PKGDIR) && find lib usr/lib -type l)"; \
	if [ -n "$$links" ]; then \
		for i in $$links; do \
			from="`readlink $(WAGO_DBUS_PKGDIR)/$$i`"; \
			to="/$$i"; \
			$(call install_link, wago_dbus, $$from, $$to); \
		done; \
	fi

#	#
#	# FIXME: add all necessary things here
#	#

	@$(call install_finish, wago_dbus)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wago_dbus.clean:
	@$(call targetinfo)
	#rm -f $(WAGO_DBUS_DIR)/{configure,config.sub,config.guess}
	@$(call clean_pkg, WAGO_DBUS)
	@-rm -rf $(WAGO_DBUS_DIR)

# vim: syntax=make
