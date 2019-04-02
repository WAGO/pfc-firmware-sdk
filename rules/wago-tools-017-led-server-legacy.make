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
PACKAGES-$(PTXCONF_LED_SERVER_LEGACY) += led_server_legacy

#
# Paths and names
#
ifdef PTXCONF_LED_SERVER_LEGACY_TRUNK
LED_SERVER_LEGACY_VERSION	:= trunk
LED_SERVER_LEGACY		:= led_server_legacy
else
LED_SERVER_LEGACY_VERSION	:= 1.0
LED_SERVER_LEGACY		:= led_server_legacy
LED_SERVER_LEGACY_VER		:= led_server_legacy-$(LED_SERVER_LEGACY_VERSION)
endif

LED_SERVER_LEGACY_URL		:= file://$(SRCDIR)/$(LED_SERVER_LEGACY)
LED_SERVER_LEGACY_SRC		:= $(SRCDIR)/$(LED_SERVER_LEGACY)
LED_SERVER_LEGACY_DIR		:= $(BUILDDIR)/$(LED_SERVER_LEGACY)
#LED_SERVER_DIR		:= $(LED_SERVER_LEGACY_DIR)
LED_SERVER_LEGACY_BUILD_OOT	:= NO
LED_SERVER_LEGACY_LICENSE	:= unknown
LED_SERVER_DIAG_DIR   := $(LED_SERVER_LEGACY_DIR)/src/diagnostic

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/led_server_legacy.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(LED_SERVER_LEGACY_DIR))
	#@$(call extract, LED_SERVER_LEGACY)
	@mkdir -p $(LED_SERVER_LEGACY_DIR)
	@rsync -a --exclude=".project" --exclude=".cproject"  $(LED_SERVER_LEGACY_SRC) $(BUILDDIR)
#	cp -r $(SRCDIR)/$(LED_SERVER_LEGACY) $(LED_SERVER_LEGACY_DIR)
	@$(call patchin, LED_SERVER_LEGACY)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#LED_SERVER_LEGACY_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
LED_SERVER_LEGACY_CONF_TOOL	:= autoconf
LED_SERVER_LEGACY_CONF_OPT	:= $(CROSS_AUTOCONF_USR)
ifdef PTXCONF_LED_SERVER_LEGACY_CMD_TOOLS
	LED_SERVER_LEGACY_CONF_OPT	+=  --enable-cmdtools
endif
#ifdef PTXCONF_PLATFORM_PAC400
#	LED_SERVER_LEGACY_CONF_OPT	+=  --enable-platformfamily=pac400
#else ifdef PTXCONF_PLATFORM_PENTIUM
#	LED_SERVER_LEGACY_CONF_OPT	+=  --enable-platformfamily=pentium
#else ifdef PTXCONF_PLATFORM_GEODE
#	LED_SERVER_LEGACY_CONF_OPT	+=  --enable-platformfamily=geode
#else
#	LED_SERVER_LEGACY_CONF_OPT	+=  --enable-platformfamily=pac200
#endif
#ifeq ($(PTXCONF_ARCH_STRING),"i386")
#	LED_SERVER_LEGACY_CONF_OPT	+=  --enable-platformfamily=pentium
#else ifeq ($(PTXCONF_ARCH_STRING),"arm")
#	LED_SERVER_LEGACY_CONF_OPT	+=  --enable-platformfamily=pac200
#else
#	LED_SERVER_LEGACY_CONF_OPT	+=  --enable-platformfamily=pac400
#endif

	LED_SERVER_LEGACY_CONF_OPT += LIBS=-lrt

ifdef PTXCONF_LED_SERVER_LEGACY_DEBUGGING
	LED_SERVER_LEGACY_CONF_OPT += --enable-debug 
endif

$(STATEDIR)/led_server_legacy.prepare:
	@$(call targetinfo)

ifdef PTXCONF_LED_SERVER_LEGACY_TRUNK
	#cd $(LED_SERVER_LEGACY_DIR) && sh autogen.sh
	cd $(LED_SERVER_LEGACY_DIR) && sh autogen.sh
else
	#cd $(LED_SERVER_LEGACY_DIR) && [ -f configure ] || sh autogen.sh
	cd $(LED_SERVER_LEGACY_DIR) && sh autogen.sh
endif

	@$(call world/prepare, LED_SERVER_LEGACY)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/led_server_legacy.compile:
	@$(call xslt_clean, $(LED_SERVER_LEGACY_DIR)/src/xml/led_server.xml)
#	@$(call xslt_clean, $(LED_SERVER_LEGACY_DIR)/src/xml/common.xml)
	@$(call xslt_compile, $(LED_SERVER_LEGACY_DIR)/src/xml/led_server.xml)
#	@$(call xslt_compile, $(LED_SERVER_LEGACY_DIR)/src/xml/common.xml)
	@$(call targetinfo)
	@rsync -a --exclude=".project" --exclude=".cproject"  $(LED_SERVER_LEGACY_SRC) $(BUILDDIR)
	@$(call world/compile, LED_SERVER_LEGACY)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/led_server_legacy.install:
#	@$(call targetinfo)
#	@$(call world/install, LED_SERVER_LEGACY)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/led_server_legacy.targetinstall:
	@$(call targetinfo)

	@$(call install_init, led_server_legacy)
	@$(call install_fixup, led_server_legacy, PRIORITY, optional)
	@$(call install_fixup, led_server_legacy, SECTION, base)
	@$(call install_fixup, led_server_legacy, AUTHOR, "Hans Florian Scholz <hans-florian.scholz@wago.com>")
	@$(call install_fixup, led_server_legacy, DESCRIPTION, missing)

#	#
#	# example code:; copy all libraries, links and binaries
#	#



#@for i in $(shell cd $(LED_SERVER_LEGACY_PKGDIR) && find bin sbin usr/bin usr/sbin -type f); do \
#		$(call install_copy, led_server_legacy, 0, 0, 0755, -, /$$i); \
#	done
	@$(call install_copy, led_server_legacy, 0, 0, 0755, -, /bin/ledserverd)
ifdef PTXCONF_LED_SERVER_LEGACY_CMD_TOOLS
	@$(call install_copy, led_server_legacy, 0, 0, 0755, -, /bin/eactingbox)
	@$(call install_link, led_server_legacy, "../../bin/eactingbox", "/usr/bin/eventmsg")
	@$(call install_link, led_server_legacy, "../../bin/eactingbox", "/usr/bin/getledstate")
	@$(call install_link, led_server_legacy, "../../bin/eactingbox", "/usr/bin/getidstate")
	@$(call install_copy, led_server_legacy, 0, 0, 0755, -, /usr/share/dbus-1/system-services/wago.diagnostic.service)
	@$(call install_link, led_server_legacy, "../../bin/eactingbox", "/usr/bin/decodeid")
	@$(call install_link, led_server_legacy, "../../bin/eactingbox", "/usr/bin/logforward")
	@$(call install_link, led_server_legacy, "../../bin/eactingbox", "/usr/bin/addcstdiag")
else
	@$(call install_copy, led_server_legacy, 0, 0, 0755, -, /usr/bin/logforward)
endif
	@$(call install_copy, led_server_legacy, 0, 0, 0755, -, /usr/lib/libdiagnostic.so)
	@$(call install_copy, led_server_legacy, 0, 0, 0755, -, /usr/lib/libdiagxml.so)

#	@$(call install_copy, led_server_legacy, 0, 0, 0755, -, /etc/specific/led.xml)
	@$(call install_copy, led_server_legacy, 0, 0, 0755, -, /etc/specific/led.0x001C.xml)
	@$(call install_copy, led_server_legacy, 0, 0, 0755, -, /etc/specific/led.0x000C.xml)
	@$(call install_copy, led_server_legacy, 0, 0, 0755, -, /etc/specific/led.0x000A.xml)
	@$(call install_copy, led_server_legacy, 0, 0, 0755, -, /etc/specific/led.0x000E.xml)
	@$(call install_copy, led_server_legacy, 0, 0, 0755, -, /etc/specific/led.0x000F.xml)
	@$(call install_link, led_server_legacy, "../../tmp/led.xml", "/etc/specific/led.xml")

#	@for i in $(shell cd $(LED_SERVER_LEGACY_PKGDIR) && find lib usr/lib -name "*.so*"); do \
#		$(call install_copy, led_server_legacy, 0, 0, 0644, -, /$$i); \
#	done
#	@links="$(shell cd $(LED_SERVER_LEGACY_PKGDIR) && find lib usr/lib -type l)"; \
#	if [ -n "$$links" ]; then \
#		for i in $$links; do \
#			from="`readlink $(LED_SERVER_LEGACY_PKGDIR)/$$i`"; \
#			to="/$$i"; \
#			$(call install_link, led_server_legacy, $$from, $$to); \
#		done; \
#	fi


ifdef PTXCONF_LED_SERVER_LEGACY_STARTSCRIPT
	@$(call install_copy, led_server_legacy, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/init.d/ledserver, \
		/etc/init.d/ledserver, n)
ifneq ($(PTXCONF_LED_SERVER_BBINIT_LINK),"")
	@$(call install_link, led_server_legacy, ../init.d/ledserver, \
		/etc/rc.d/$(PTXCONF_LED_SERVER_BBINIT_LINK))
endif
endif

ifdef PTXCONF_LOGFORWARD_STARTSCRIPT
	@$(call install_copy, led_server_legacy, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/init.d/logforward, \
		/etc/init.d/logforward, n)
ifneq ($(PTXCONF_LOGFORWARD_BBINIT_LINK),"")
	@$(call install_link, led_server_legacy, ../init.d/logforward, \
		/etc/rc.d/$(PTXCONF_LOGFORWARD_BBINIT_LINK))
endif
endif

#	#
#	# FIXME: add all necessary things here
#	#

	@$(call install_finish, led_server_legacy)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/led_server_legacy.clean:
#	@$(call xslt_clean, $(LED_SERVER_LEGACY_DIR)/src/xml/led_server_legacy.xml)
#	@$(call xslt_clean, $(LED_SERVER_LEGACY_DIR)/src/xml/common.xml)
	@$(call targetinfo)
	#rm -f $(LED_SERVER_LEGACY_DIR)/{configure,config.sub,config.guess}
	@$(call clean_pkg, LED_SERVER_LEGACY)
	@-rm -rf $(LED_SERVER_LEGACY_DIR)

# vim: syntax=make
