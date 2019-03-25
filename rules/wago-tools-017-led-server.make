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
PACKAGES-$(PTXCONF_LED_SERVER_META) += led_server_meta

#
# Paths and names
#
ifdef PTXCONF_LED_SERVER_META_TRUNK
LED_SERVER_META_VERSION	:= trunk
LED_SERVER_META		:= led_server_meta
else
LED_SERVER_META_VERSION	:= 2.0
LED_SERVER_META		:= led_server_meta
LED_SERVER_META_VER		:= led_server_meta-$(LED_SERVER_META_VERSION)
endif

LED_SERVER_META_URL		:= file://$(PTXDIST_WORKSPACE)/local_src//$(LED_SERVER_META)
LED_SERVER_META_SRC		:= $(PTXDIST_WORKSPACE)/local_src//$(LED_SERVER_META)
LED_SERVER_META_DIR		:= $(BUILDDIR)/$(LED_SERVER_META)
LED_SERVER_META_DIR		:= $(LED_SERVER_META_DIR)
LED_SERVER_META_BUILD_OOT	:= NO
LED_SERVER_META_LICENSE	:= unknown
LED_SERVER_META_DIAG_DIR   := $(LED_SERVER_META_DIR)/src/diagnostic

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/led_server_meta.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@mkdir -p $(LED_SERVER_META_DIR)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/led_server_meta.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/led_server_meta.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/led_server_meta.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/led_server_meta.targetinstall:
	@$(call targetinfo)

# Packet contains no artifcats.

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/led_server_meta.clean:
	@$(call clean_pkg, LED_SERVER_META)
# vim: syntax=make
