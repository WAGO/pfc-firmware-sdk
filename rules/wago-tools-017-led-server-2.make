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
PACKAGES-$(PTXCONF_LED_SERVER_2) += led-server-2

#
# Paths and names
#
ifdef PTXCONF_LED_SERVER_2_TRUNK
LED_SERVER_2_VERSION	:= trunk
LED_SERVER_2		:= led-server-2
else
LED_SERVER_2_VERSION	:= 2.0.3
LED_SERVER_2		:= led-server-2
LED_SERVER_2_VER		:= led-server-2-$(LED_SERVER_2_VERSION)
endif

LED_SERVER_2_URL		:= file://$(PTXDIST_WORKSPACE)/local_src//$(LED_SERVER_2)
LED_SERVER_2_SRC		:= $(PTXDIST_WORKSPACE)/local_src//$(LED_SERVER_2)
LED_SERVER_2_DIR		:= $(BUILDDIR)/$(LED_SERVER_2)
#LED_SERVER_DIR		:= $(LED_SERVER_2_DIR)
LED_SERVER_2_BUILD_OOT	:= NO
LED_SERVER_2_LICENSE	:= unknown
LED_SERVER_DIAG_DIR   := $(LED_SERVER_2_DIR)/src/diagnostic

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/led-server-2.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(LED_SERVER_2_DIR))
	#@$(call extract, LED_SERVER_2)
	@mkdir -p $(LED_SERVER_2_DIR)
	@rsync -a --exclude=".project" --exclude=".cproject"  $(LED_SERVER_2_SRC) $(BUILDDIR)
#	cp -r $(SRCDIR)/$(LED_SERVER_2) $(LED_SERVER_2_DIR)
	@$(call patchin, LED_SERVER_2)

ifdef PTXCONF_LED_SERVER_2_TRUNK
	#cd $(LED_SERVER_2_DIR) && sh autogen.sh
	cd $(LED_SERVER_2_DIR) && sh autogen.sh
else
	#cd $(LED_SERVER_2_DIR) && [ -f configure ] || sh autogen.sh
	cd $(LED_SERVER_2_DIR) && sh autogen.sh
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#LED_SERVER_2_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
LED_SERVER_2_CONF_TOOL	:= autoconf
LED_SERVER_2_CONF_OPT	:= $(CROSS_AUTOCONF_USR)

ifdef PTXCONF_LED_SERVER_2_HAL_PCA955X
#	LED_SERVER_2_CONF_OPT +=  --enable-hal=pca955x
	LED_SERVER_2_CONF_OPT +=  --with-hal=pca955x
else
#	LED_SERVER_2_CONF_OPT +=  --enable-hal=gpio
	LED_SERVER_2_CONF_OPT +=  --with-hal=gpio
endif
	
ifdef PTXCONF_LED_SERVER_2_HAL_GPIO_PFC200
	LED_SERVER_2_CONF_OPT +=  --with-gpiovariant=pfc200
else ifdef PTXCONF_LED_SERVER_2_HAL_GPIO_PFC200_EVAL
	LED_SERVER_2_CONF_OPT +=  --with-gpiovariant=pfc200eval
else ifdef PTXCONF_LED_SERVER_2_HAL_GPIO_PERSPECTO
	LED_SERVER_2_CONF_OPT +=  --with-gpiovariant=perspecto
endif
	
ifdef PTXCONF_LED_SERVER_2_DEBUGGING
	LED_SERVER_2_CONF_OPT += --enable-debug 
endif

#$(STATEDIR)/led-server-2.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, LED_SERVER_2)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/led-server-2.compile:
	@$(call targetinfo)
	@rsync -a --exclude=".project" --exclude=".cproject"  $(LED_SERVER_2_SRC) $(BUILDDIR)
	@$(call world/compile, LED_SERVER_2)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/led-server-2.install:
#	@$(call targetinfo)
#	@$(call world/install, LED_SERVER_2)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/led-server-2.targetinstall:
	@$(call targetinfo)

	@$(call install_init, led-server-2)
	@$(call install_fixup, led-server-2, PRIORITY, optional)
	@$(call install_fixup, led-server-2, SECTION, base)
	@$(call install_fixup, led-server-2, AUTHOR, "Hans Florian Scholz <hans-florian.scholz@wago.com>")
	@$(call install_fixup, led-server-2, DESCRIPTION, missing)

#	#
#	# example code:; copy all libraries, links and binaries
#	#

	@$(call install_copy, led-server-2, 0, 0, 0755, -, /usr/lib/libledserver2.so)


#	#
#	# FIXME: add all necessary things here
#	#

	@$(call install_finish, led-server-2)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/led-server-2.clean:
	@$(call targetinfo)
	@$(call clean_pkg, LED_SERVER_2)
	@-rm -rf $(LED_SERVER_2_DIR)

# vim: syntax=make
