# -*-makefile-*-
#
# Copyright (C) 2014 by <Jaroslav Imrich>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_IPWATCHD) += ipwatchd

#
# Paths and names
#
IPWATCHD_VERSION	:= 1.2.1
IPWATCHD_MD5		:= a54f5b815cb98d4e17770cad7ff42679
IPWATCHD		:= ipwatchd-$(IPWATCHD_VERSION)
IPWATCHD_SUFFIX		:= tar.gz
IPWATCHD_URL		:= http://svsv01003.wago.local/wago-ptxdist-src/$(IPWATCHD).$(IPWATCHD_SUFFIX)
IPWATCHD_SOURCE		:= $(SRCDIR)/$(IPWATCHD).$(IPWATCHD_SUFFIX)
IPWATCHD_DIR		:= $(BUILDDIR)/$(IPWATCHD)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#IPWATCHD_PATH	:= PATH=$(CROSS_PATH)
IPWATCHD_CONF_TOOL	:= NO
IPWATCHD_MAKE_ENV	:= $(CROSS_ENV)
IPWATCHD_MAKE_OPT := \
       $(CROSS_ENV_CFLAGS) \
       $(CROSS_ENV_CPPFLAGS) \
       $(CROSS_ENV_LDFLAGS) \
       $(CROSS_ENV_CC)

# $(STATEDIR)/ipwatchd.prepare:
# 	@$(call targetinfo)
# 	@$(call world/prepare, IPWATCHD)
# 	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/ipwatchd.compile:
	@$(call targetinfo)
	cd $(IPWATCHD_DIR)/src && $(IPWATCHD_MAKE_ENV) $(MAKE) $(IPWATCHD_MAKE_OPT)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ipwatchd.install:
	@$(call targetinfo)
	cd $(IPWATCHD_DIR)/src && $(IPWATCHD_MAKE_ENV) $(MAKE) $(IPWATCHD_MAKE_OPT)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ipwatchd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, ipwatchd)
	@$(call install_fixup, ipwatchd,PRIORITY,optional)
	@$(call install_fixup, ipwatchd,SECTION,base)
	@$(call install_fixup, ipwatchd,AUTHOR,"Jaroslav Imrich <jariq@jariq.sk>")
	@$(call install_fixup, ipwatchd,DESCRIPTION,missing)

	@$(call install_copy, ipwatchd, 0, 0, 0755, $(IPWATCHD_DIR)/src/ipwatchd, /usr/sbin/ipwatchd)
	@$(call install_copy, ipwatchd, 0, 0, 0755, $(IPWATCHD_DIR)/src/scripts/ipwatchd, /etc/init.d/ipwatchd)
	@$(call install_link, ipwatchd, ../init.d/ipwatchd, /etc/rc.d/S22_ipwatchd)
	@$(call install_copy, ipwatchd, 0, 0, 0755, $(IPWATCHD_DIR)/src/scripts/ipwatchd-script, /usr/sbin/ipwatchd-script)

	# The source package does not know of any etc directory. So make one for the config file.
	mkdir -p $(BUILDDIR)/$(IPWATCHD)/etc
	cp $(IPWATCHD_DIR)/src/ipwatchd.conf $(BUILDDIR)/$(IPWATCHD)/etc
	@$(call install_alternative, ipwatchd, 0, 0, 0644, /etc/ipwatchd.conf)

	@$(call install_finish, ipwatchd)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/ipwatchd.clean:
	@$(call targetinfo)
	@-cd $(IPWATCHD_DIR) && \
		$(IPWATCHD_MAKE_ENV) $(IPWATCHD_PATH) $(MAKE) clean
	@$(call clean_pkg, IPWATCHD)

# vim: syntax=make
