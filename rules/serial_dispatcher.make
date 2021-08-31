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
PACKAGES-$(PTXCONF_SERIAL_DISPATCHER) += serial_dispatcher

#
# Paths and names
#
ifdef PTXCONF_SERIAL_DISPATCHER_TRUNK
SERIAL_DISPATCHER_VERSION	:= trunk
else
SERIAL_DISPATCHER_VERSION	:= 1
endif
SERIAL_DISPATCHER		:= serial_dispatcher
SERIAL_DISPATCHER_URL		:= file://$(PTXDIST_WORKSPACE)/local_src/$(SERIAL_DISPATCHER)
SERIAL_DISPATCHER_SRC		:= $(PTXDIST_WORKSPACE)/local_src/$(SERIAL_DISPATCHER)
SERIAL_DISPATCHER_DIR		:= $(BUILDDIR)/$(SERIAL_DISPATCHER)
SERIAL_DISPATCHER_BUILD_OOT	:= NO
SERIAL_DISPATCHER_LICENSE	:= GPLv2
SERIAL_DISPATCHER_DEVICE	:= $(call remove_quotes,$(PTXCONF_SERIAL_DISPATCHER_DEVICE))

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/serial_dispatcher.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(SERIAL_DISPATCHER_DIR))
	#@$(call extract, SERIAL_DISPATCHER)
	@mkdir -p $(SERIAL_DISPATCHER_DIR)
	@rsync -a --exclude=".project" --exclude=".cproject"  $(SERIAL_DISPATCHER_SRC) $(BUILDDIR)
	@$(call patchin, SERIAL_DISPATCHER)

ifdef PTXCONF_SERIAL_DISPATCHER_TRUNK
	cd $(SERIAL_DISPATCHER_DIR) && sh autogen.sh
else
	#cd $(SERIAL_DISPATCHER_DIR) && [ -f configure ] || sh autogen.sh
	cd $(SERIAL_DISPATCHER_DIR) && sh autogen.sh
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#SERIAL_DISPATCHER_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
SERIAL_DISPATCHER_CONF_TOOL	:= autoconf
SERIAL_DISPATCHER_CONF_OPT	:= $(CROSS_AUTOCONF_USR)

$(STATEDIR)/serial_dispatcher.prepare:
	@$(call targetinfo)
	@$(call world/prepare, SERIAL_DISPATCHER)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/serial_dispatcher.compile:
	@$(call targetinfo)
	@$(call world/compile, SERIAL_DISPATCHER)
	@rsync -a --exclude=".project" --exclude=".cproject"  $(SERIAL_DISPATCHER_SRC) $(BUILDDIR)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/serial_dispatcher.install:
#	@$(call targetinfo)
#	@$(call world/install, SERIAL_DISPATCHER)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/serial_dispatcher.targetinstall:
	@$(call targetinfo)

	@$(call install_init, serial_dispatcher)
	@$(call install_fixup, serial_dispatcher, PRIORITY, optional)
	@$(call install_fixup, serial_dispatcher, SECTION, base)
	@$(call install_fixup, serial_dispatcher, AUTHOR, "Hans Florian Scholz <hans-florian.scholz@wago.com>")
	@$(call install_fixup, serial_dispatcher, DESCRIPTION, missing)

#	#
#	# example code:; copy all libraries, links and binaries
#	#

	@for i in $(shell cd $(SERIAL_DISPATCHER_PKGDIR) && find bin sbin usr/bin usr/sbin -type f); do \
		$(call install_copy, serial_dispatcher, 0, 0, 0755, -, /$$i); \
	done
	@for i in $(shell cd $(SERIAL_DISPATCHER_PKGDIR) && find lib usr/lib -name "*.so*"); do \
		$(call install_copy, serial_dispatcher, 0, 0, 0644, -, /$$i); \
	done
	@links="$(shell cd $(SERIAL_DISPATCHER_PKGDIR) && find lib usr/lib -type l)"; \
	if [ -n "$$links" ]; then \
		for i in $$links; do \
			from="`readlink $(SERIAL_DISPATCHER_PKGDIR)/$$i`"; \
			to="/$$i"; \
			$(call install_link, serial_dispatcher, $$from, $$to); \
		done; \
	fi

	@$(call install_alternative, serial_dispatcher, 0, 0, 0755, /etc/init.d/serial_dispatcher, n)
	
	@$(call install_alternative, serial_dispatcher, 0, 0, 0640, /etc/serial_dispatcher.conf, n)
	@$(call install_replace, serial_dispatcher, /etc/serial_dispatcher.conf, @SERIAL_DISPATCHER_DEVICE@, $(SERIAL_DISPATCHER_DEVICE))


ifneq ($(call remove_quotes,$(PTXCONF_SERIAL_DISPATCHER_BBINIT_LINK)),)
	@$(call install_link, serial_dispatcher, \
		../init.d/serial_dispatcher, \
		/etc/rc.d/$(PTXCONF_SERIAL_DISPATCHER_BBINIT_LINK))
endif

	@$(call install_copy, serial_dispatcher, 0, 0, 0644, $(SERIAL_DISPATCHER_DIR)/LICENSE, /usr/share/licenses/oss/license.serial-dispatcher_$(SERIAL_DISPATCHER_VERSION).txt)
#	#
#	# FIXME: add all necessary things here
#	#

	@$(call install_finish, serial_dispatcher)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/serial_dispatcher.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, SERIAL_DISPATCHER)

# vim: syntax=make
