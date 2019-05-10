# -*-makefile-*-
#
# Copyright (C) 2017 by Wago Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_AIDE) += aide

#
# Paths and names
#
AIDE_VERSION	:= 0.16
AIDE_MD5	:= 25c616f67c667acd4088747ae7f6a9a3
AIDE		:= aide-$(AIDE_VERSION)
AIDE_SUFFIX	:= tar.gz
AIDE_URL	:= https://sourceforge.net/projects/aide/files/aide/$(AIDE_VERSION)/$(AIDE).$(AIDE_SUFFIX)
AIDE_SOURCE	:= $(SRCDIR)/$(AIDE).$(AIDE_SUFFIX)
AIDE_DIR	:= $(BUILDDIR)/$(AIDE)
AIDE_LICENSE	:= GPLv2

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(AIDE_SOURCE):
#	@$(call targetinfo)
#	@$(call get, AIDE)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

AIDE_CONF_TOOL := autoconf

AIDE_CONF_OPT	:= $(CROSS_AUTOCONF_USR)\
			--with-gcrypt	\
			--disable-lfs \
			--disable-static \
			--disable-forced-configmd \
			--disable-forced-dbmd

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/aide.compile:
	@$(call targetinfo)
	@$(call world/compile, AIDE)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/aide.install:
#	@$(call targetinfo)
#	@$(call world/install, AIDE)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/aide.targetinstall:
	@$(call targetinfo)

	@$(call install_init, aide)
	@$(call install_fixup, aide,PRIORITY,optional)
	@$(call install_fixup, aide,SECTION,base)
	@$(call install_fixup, aide,AUTHOR,"Wago Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, aide,DESCRIPTION,missing)

	@$(call install_copy, aide, 0, 0, 0750, -, /usr/bin/aide)
	@$(call install_copy, aide, 0, 0, 0750, /var/lib/aide/)

	@$(call install_alternative, aide, 0, 0, 0640, /etc/aide.conf)

	@$(call install_alternative, aide, 0, 0, 0750, /etc/config-tools/backup-restore/backup_aide_check)

#	#
#	# busybox init: aide check script
#	#
ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_AIDE_CHECK_STARTSCRIPT
	@$(call install_alternative, aide, 0, 0, 0755, /etc/init.d/aide_check)

ifneq ($(call remove_quotes, $(PTXCONF_AIDE_CHECK_BBINIT_LINK)),)
	@$(call install_link, aide, \
		/etc/init.d/aide_check, \
		/etc/rc.d/$(PTXCONF_AIDE_CHECK_BBINIT_LINK))
endif
endif # PTXCONF_AIDE_CHECK_STARTSCRIPT
endif # PTXCONF_INITMETHOD_BBINIT

	@$(call install_finish, aide)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/aide.clean:
	@$(call targetinfo)
	@$(call clean_pkg, AIDE)

# vim: syntax=make
