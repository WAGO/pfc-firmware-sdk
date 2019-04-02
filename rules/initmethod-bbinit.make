# -*-makefile-*-
#
# Copyright (C) 2009 by Robert Schwebel <r.schwebel@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_INITMETHOD_BBINIT) += initmethod-bbinit

#
# Paths and names
#
INITMETHOD_BBINIT_VERSION	:= 1.0.0
INITMETHOD_BBINIT		:= initmethod-bbinit-$(INITMETHOD_BBINIT_VERSION)
INITMETHOD_BBINIT_DIR		:= $(BUILDDIR)/$(INITMETHOD_BBINIT)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(INITMETHOD_BBINIT_SOURCE):
	@$(call targetinfo)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/initmethod-bbinit.extract:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/initmethod-bbinit.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/initmethod-bbinit.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/initmethod-bbinit.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/initmethod-bbinit.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  initmethod-bbinit)
	@$(call install_fixup, initmethod-bbinit, PRIORITY, optional)
	@$(call install_fixup, initmethod-bbinit, SECTION, base)
	@$(call install_fixup, initmethod-bbinit, AUTHOR, "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, initmethod-bbinit, DESCRIPTION, missing)

#	# first of all: generate the required directories & rcS
	@$(call install_copy, initmethod-bbinit, 0, 0, 0755, /etc/init.d)
	@$(call install_copy, initmethod-bbinit, 0, 0, 0755, /etc/rc.d)
	@$(call install_alternative, initmethod-bbinit, 0, 0, 0755, /etc/init.d/rcS, n)
	@$(call install_alternative, initmethod-bbinit, 0, 0, 0644, /etc/inittab, n)
	@$(call install_replace, initmethod-bbinit, /etc/inittab, \
		@CONSOLE@, $(PTXCONF_CONSOLE_NAME))
	@$(call install_replace, initmethod-bbinit, /etc/inittab, \
		@SPEED@, $(PTXCONF_CONSOLE_SPEED))
	@$(call install_alternative, initmethod-bbinit, 0, 0, 0755, /usr/lib/init/initmethod-bbinit-functions.sh)

ifdef PTXCONF_IPCCONFIG_INITTAB_ENTRY
	@$(call install_replace, initmethod-bbinit, /etc/inittab, \
		@IPCCONFIG_RESPAWN@, "tty3:23:respawn:/etc/init.d/ipcconfig start")
else
	@$(call install_replace, initmethod-bbinit, /etc/inittab, \
		@IPCCONFIG_RESPAWN@, "")
endif


#	#
#	# start scripts
#	#
ifdef PTXCONF_INITMETHOD_BBINIT_ETC_INITD_BANNER
	@$(call install_alternative, initmethod-bbinit, 0, 0, 0755, /etc/init.d/banner, n)

	@$(call install_replace, initmethod-bbinit, /etc/init.d/banner, \
		@PROJECT@, $(PTXCONF_PROJECT))
	@$(call install_replace, initmethod-bbinit, /etc/init.d/banner, \
		@PRJVERSION@, $(PTXCONF_PROJECT_VERSION))

	@$(call install_replace, initmethod-bbinit, /etc/init.d/banner, \
		@PTXDIST@, ptxdist)
	@$(call install_replace, initmethod-bbinit, /etc/init.d/banner, \
		@VERSION@, $(PTXDIST_VERSION_YEAR))
	@$(call install_replace, initmethod-bbinit, /etc/init.d/banner, \
		@PATCHLEVEL@, $(PTXDIST_VERSION_MONTH))
	@$(call install_replace, initmethod-bbinit, /etc/init.d/banner, \
		@SUBLEVEL@, $(PTXDIST_VERSION_BUGFIX))
	@$(call install_replace, initmethod-bbinit, /etc/init.d/banner, \
		@EXTRAVERSION@, $(PTXDIST_VERSION_SCM))
	@$(call install_replace, initmethod-bbinit, /etc/init.d/banner, \
		@DATE@, $(shell date -Iseconds))

	@$(call install_replace, initmethod-bbinit, /etc/init.d/banner, \
		@VENDOR@, $(PTXCONF_PROJECT_VENDOR))
endif

ifdef PTXCONF_INITMETHOD_BBINIT_ETC_INITD_MODULES
	@$(call install_alternative, initmethod-bbinit, 0, 0, 0755, /etc/init.d/modules, n)
	@$(call install_alternative, initmethod-bbinit, 0, 0, 0644, /etc/modules, n)
endif

ifdef PTXCONF_INITMETHOD_BBINIT_ETC_INITD_LOGROTATE
	@$(call install_alternative, initmethod-bbinit, 0, 0, 0755, /etc/init.d/logrotate, n)
endif

ifdef PTXCONF_INITMETHOD_BBINIT_ETC_INITD_NETWORKING
	@$(call install_alternative, initmethod-bbinit, 0, 0, 0755, /etc/init.d/networking, n)
	@$(call install_alternative, initmethod-bbinit, 0, 0, 0644, /etc/network/interfaces, n)
	@$(call install_copy, initmethod-bbinit, 0, 0, 0755, /etc/network/if-down.d)
	@$(call install_copy, initmethod-bbinit, 0, 0, 0755, /etc/network/if-up.d)
	@$(call install_copy, initmethod-bbinit, 0, 0, 0755, /etc/network/if-post-down.d)
	@$(call install_copy, initmethod-bbinit, 0, 0, 0755, /etc/network/if-pre-up.d)

ifdef PTXCONF_INITMETHOD_BBINIT_ETC_INITD_NETWORKING_FINISH
	@$(call install_alternative, initmethod-bbinit, 0, 0, 0755, /etc/init.d/networking-finish, n)
endif

endif # PTXCONF_INITMETHOD_BBINIT_ETC_INITD_NETWORKING

ifdef PTXCONF_INITMETHOD_BBINIT_ETC_INITD_NETWORKING_ROUTES
	@$(call install_alternative, initmethod-bbinit, 0, 0, 0755, /etc/network/if-up.d/routes)
	@$(call install_alternative, initmethod-bbinit, 0, 0, 0755, /etc/network/if-down.d/cfgupdate)
endif
ifdef PTXCONF_INITMETHOD_BBINIT_ETC_INITD_RT_SET_BANDWIDTH
	@$(call install_alternative, initmethod-bbinit, 0, 0, 0755, /etc/init.d/rt-set-bandwidth, n)
endif

#	#
#	# collect start links
#	#

ifneq ($(call remove_quotes,$(PTXCONF_INITMETHOD_BBINIT_LINK_BANNER)),)
	@$(call install_link, initmethod-bbinit, \
		../init.d/banner, \
		/etc/rc.d/$(PTXCONF_INITMETHOD_BBINIT_LINK_BANNER))
endif

ifneq ($(call remove_quotes,$(PTXCONF_INITMETHOD_BBINIT_LINK_LOGROTATE)),)
	@$(call install_link, initmethod-bbinit, \
		../init.d/logrotate, \
		/etc/rc.d/$(PTXCONF_INITMETHOD_BBINIT_LINK_LOGROTATE))
endif

ifneq ($(call remove_quotes,$(PTXCONF_INITMETHOD_BBINIT_LINK_MODULES)),)
	@$(call install_link, initmethod-bbinit, \
		../init.d/modules, \
		/etc/rc.d/$(PTXCONF_INITMETHOD_BBINIT_LINK_MODULES))
endif

ifneq ($(call remove_quotes,$(PTXCONF_INITMETHOD_BBINIT_LINK_NETWORKING)),)
	@$(call install_link, initmethod-bbinit, \
		../init.d/networking, \
		/etc/rc.d/$(PTXCONF_INITMETHOD_BBINIT_LINK_NETWORKING))

ifneq ($(call remove_quotes,$(PTXCONF_INITMETHOD_BBINIT_LINK_NETWORKING_FINISH)),)
	@$(call install_link, initmethod-bbinit, \
		../init.d/networking-finish, \
		/etc/rc.d/$(PTXCONF_INITMETHOD_BBINIT_LINK_NETWORKING_FINISH))
endif

endif  # PTXCONF_INITMETHOD_BBINIT_LINK_NETWORKING != ""

ifneq ($(call remove_quotes,$(PTXCONF_INITMETHOD_BBINIT_LINK_RT_SET_BANDWIDTH)),)
	@$(call install_link, initmethod-bbinit, \
		../init.d/rt-set-bandwidth, \
		/etc/rc.d/$(PTXCONF_INITMETHOD_BBINIT_LINK_RT_SET_BANDWIDTH))
endif

	@$(call install_finish, initmethod-bbinit)

	@$(call touch)

# vim: syntax=make

