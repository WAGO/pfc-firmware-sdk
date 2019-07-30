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
PACKAGES-$(PTXCONF_WAGO_PFC_DIAGNOSTIC) += wago-pfc-diagnostic

#
# Paths and names
#
ifdef PTXCONF_WAGO_PFC_DIAGNOSTIC_TRUNK
WAGO_PFC_DIAGNOSTIC_VERSION	:= trunk
WAGO_PFC_DIAGNOSTIC		:= wago-pfc-diagnostic
else
WAGO_PFC_DIAGNOSTIC_VERSION	:= 1.0
WAGO_PFC_DIAGNOSTIC		:= wago-pfc-diagnostic
WAGO_PFC_DIAGNOSTIC_VER		:= wago-pfc-diagnostic-$(WAGO_PFC_DIAGNOSTIC_VERSION)
endif

WAGO_PFC_DIAGNOSTIC_URL		:= file://$(PTXDIST_WORKSPACE)/local_src/$(WAGO_PFC_DIAGNOSTIC)
WAGO_PFC_DIAGNOSTIC_SRC		:= $(PTXDIST_WORKSPACE)/local_src/$(WAGO_PFC_DIAGNOSTIC)
WAGO_PFC_DIAGNOSTIC_DIR		:= $(BUILDDIR)/$(WAGO_PFC_DIAGNOSTIC)
LED_SERVER_DIR		:= $(WAGO_PFC_DIAGNOSTIC_DIR)
WAGO_PFC_DIAGNOSTIC_BUILD_OOT	:= NO
WAGO_PFC_DIAGNOSTIC_LICENSE	:= unknown
LED_SERVER_DIAG_DIR   := $(WAGO_PFC_DIAGNOSTIC_DIR)/src/diagnostic

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-pfc-diagnostic.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(WAGO_PFC_DIAGNOSTIC_DIR))
	@mkdir -p $(WAGO_PFC_DIAGNOSTIC_DIR)
	@rsync -a --exclude=".project" --exclude=".cproject"  $(WAGO_PFC_DIAGNOSTIC_SRC) $(BUILDDIR)
	@$(call patchin, WAGO_PFC_DIAGNOSTIC)

ifdef PTXCONF_WAGO_PFC_DIAGNOSTIC_TRUNK
	cd $(WAGO_PFC_DIAGNOSTIC_DIR) && sh autogen.sh
else
	cd $(WAGO_PFC_DIAGNOSTIC_DIR) && sh autogen.sh
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#WAGO_PFC_DIAGNOSTIC_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
WAGO_PFC_DIAGNOSTIC_CONF_TOOL	:= autoconf
WAGO_PFC_DIAGNOSTIC_CONF_OPT	:= $(CROSS_AUTOCONF_USR)
WAGO_PFC_DIAGNOSTIC_CONF_OPT += LIBS=-lrt

ifdef PTXCONF_WAGO_PFC_DIAGNOSTIC_DEBUGGING
	WAGO_PFC_DIAGNOSTIC_CONF_OPT += --enable-debug 
endif

#$(STATEDIR)/wago-pfc-diagnostic.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, WAGO_PFC_DIAGNOSTIC)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-pfc-diagnostic.compile:
	@$(call xslt_clean, $(WAGO_PFC_DIAGNOSTIC_DIR)/src/xml/common.xml)
	@$(call xslt_clean, $(WAGO_PFC_DIAGNOSTIC_DIR)/src/xml/led_server.xml)
	@$(call xslt_compile, $(WAGO_PFC_DIAGNOSTIC_DIR)/src/xml/common.xml)
	@$(call xslt_compile, $(WAGO_PFC_DIAGNOSTIC_DIR)/src/xml/led_server.xml)
	@$(call targetinfo)
	@rsync -a --exclude=".project" --exclude=".cproject"  $(WAGO_PFC_DIAGNOSTIC_SRC) $(BUILDDIR)
	@$(call world/compile, WAGO_PFC_DIAGNOSTIC)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/wago-pfc-diagnostic.install:
#	@$(call targetinfo)
#	@$(call world/install, WAGO_PFC_DIAGNOSTIC)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-pfc-diagnostic.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wago-pfc-diagnostic)
	@$(call install_fixup, wago-pfc-diagnostic, PRIORITY, optional)
	@$(call install_fixup, wago-pfc-diagnostic, SECTION, base)
	@$(call install_fixup, wago-pfc-diagnostic, AUTHOR, "Hans Florian Scholz <hans-florian.scholz@wago.com>")
	@$(call install_fixup, wago-pfc-diagnostic, DESCRIPTION, missing)

#	#
#	# example code:; copy all libraries, links and binaries
#	#


	@$(call install_copy, wago-pfc-diagnostic, 0, 0, 0755, -, /bin/ledserverd)
	@$(call install_copy, wago-pfc-diagnostic, 0, 0, 0755, -, /bin/eactingbox)
	@$(call install_link, wago-pfc-diagnostic, "../../bin/eactingbox", "/usr/bin/eventmsg")
	@$(call install_link, wago-pfc-diagnostic, "../../bin/eactingbox", "/usr/bin/getledstate")
	@$(call install_link, wago-pfc-diagnostic, "../../bin/eactingbox", "/usr/bin/getidstate")
	@$(call install_copy, wago-pfc-diagnostic, 0, 0, 0755, -, /usr/share/dbus-1/system-services/wago.diagnostic.service)
	@$(call install_link, wago-pfc-diagnostic, "../../bin/eactingbox", "/usr/bin/decodeid")
	@$(call install_link, wago-pfc-diagnostic, "../../bin/eactingbox", "/usr/bin/logforward")
	@$(call install_link, wago-pfc-diagnostic, "../../bin/eactingbox", "/usr/bin/addcstdiag")
	@$(call install_copy, wago-pfc-diagnostic, 0, 0, 0755, -, /usr/lib/libdiagnostic.so)
	@$(call install_copy, wago-pfc-diagnostic, 0, 0, 0755, -, /usr/lib/libdiagxml.so)

	@$(call install_copy, wago-pfc-diagnostic, 0, 0, 0755, -, /etc/specific/led.0x000C.PFC100.xml)
	@$(call install_link, wago-pfc-diagnostic, "../../etc/specific/led.0x000C.PFC100.xml", "/etc/specific/led.0x0008.PFC100.xml")
	@$(call install_link, wago-pfc-diagnostic, "../../etc/specific/led.0x000C.PFC100.xml", "/etc/specific/led.0x0028.PFC100.xml")
	@$(call install_copy, wago-pfc-diagnostic, 0, 0, 0755, -, /etc/specific/led.0x001C.PFC200.xml)
	@$(call install_copy, wago-pfc-diagnostic, 0, 0, 0755, -, /etc/specific/led.0x000C.PFC200.xml)
	@$(call install_copy, wago-pfc-diagnostic, 0, 0, 0755, -, /etc/specific/led.0x000C.PFC200_0000-0100.xml)
	@$(call install_copy, wago-pfc-diagnostic, 0, 0, 0755, -, /etc/specific/led.0x000A.PFC200.xml)
	@$(call install_copy, wago-pfc-diagnostic, 0, 0, 0755, -, /etc/specific/led.0x000E.PFC200.xml)
	@$(call install_copy, wago-pfc-diagnostic, 0, 0, 0755, -, /etc/specific/led.0x000F.PFC200.xml)
	@$(call install_copy, wago-pfc-diagnostic, 0, 0, 0755, -, /etc/specific/led.0x0182.PFC200.xml)
	@$(call install_copy, wago-pfc-diagnostic, 0, 0, 0755, -, /etc/specific/led.0x1000.TP600.xml)
	@$(call install_copy, wago-pfc-diagnostic, 0, 0, 0755, -, /etc/specific/led.0x1001.TP600.xml)
	@$(call install_copy, wago-pfc-diagnostic, 0, 0, 0755, -, /etc/specific/led.0x1002.TP600.xml)
	@$(call install_copy, wago-pfc-diagnostic, 0, 0, 0755, -, /etc/specific/led.0x1003.TP600.xml)
	@$(call install_link, wago-pfc-diagnostic, "../../etc/specific/led.0x000C.PFC200.xml", "/etc/specific/led.0x1004.PFC200.xml")
	@$(call install_copy, wago-pfc-diagnostic, 0, 0, 0755, -, /etc/specific/led.0x108E.CTP.xml)
	@$(call install_link, wago-pfc-diagnostic, "../../etc/specific/led.0x000F.PFC200.xml", "/etc/specific/led.0x004E.PFC200.xml")
	@$(call install_copy, wago-pfc-diagnostic, 0, 0, 0755, -, /etc/specific/led.0x000C.SRC.xml)
	@$(call install_link, wago-pfc-diagnostic, "../../tmp/led.xml", "/etc/specific/led.xml")

ifdef PTXCONF_LOGFORWARD_STARTSCRIPT
	@$(call install_copy, wago-pfc-diagnostic, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/init.d/logforward, \
		/etc/init.d/logforward, n)
ifneq ($(PTXCONF_LOGFORWARD_BBINIT_LINK),"")
	@$(call install_link, wago-pfc-diagnostic, ../init.d/logforward, \
		/etc/rc.d/$(PTXCONF_LOGFORWARD_BBINIT_LINK))
endif
endif
ifdef PTXCONF_LED_SERVER_2_STARTSCRIPT
	@$(call install_copy, wago-pfc-diagnostic, 0, 0, 0755, \
		$(PTXDIST_WORKSPACE)/projectroot/etc/init.d/ledserver, \
		/etc/init.d/ledserver, n)
ifneq ($(PTXCONF_LED_SERVER_BBINIT_LINK),"")
	@$(call install_link, wago-pfc-diagnostic, ../init.d/ledserver, \
		/etc/rc.d/$(PTXCONF_LED_SERVER_BBINIT_LINK))
endif
endif
#	#
#	# FIXME: add all necessary things here
#	#

	@$(call install_finish, wago-pfc-diagnostic)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-pfc-diagnostic.clean:
	@$(call targetinfo)
	@$(call clean_pkg, WAGO_PFC_DIAGNOSTIC)
	@-rm -rf $(WAGO_PFC_DIAGNOSTIC_DIR)

# vim: syntax=make
