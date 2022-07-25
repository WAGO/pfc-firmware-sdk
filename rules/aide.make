# -*-makefile-*-
#
# Copyright (C) 2017 by WAGO GmbH & Co. KG
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
AIDE_VERSION	:= 0.17.4
AIDE_MD5	:= adac3bc719b923d49059edb992eb95c9
AIDE		:= aide-$(AIDE_VERSION)
AIDE_SUFFIX	:= tar.gz
AIDE_URL	:= https://github.com/aide/aide/releases/download/v$(AIDE_VERSION)/$(AIDE).$(AIDE_SUFFIX)
AIDE_SOURCE	:= $(SRCDIR)/$(AIDE).$(AIDE_SUFFIX)
AIDE_DIR	:= $(BUILDDIR)/$(AIDE)
AIDE_LICENSE		:= GPLv2
AIDE_LICENSE_FILE	:= COPYING
AIDE_LICENSE_FILES	:= file://$(AIDE_LICENSE_FILE);md5=b234ee4d69f5fce4486a80fdaf4a4263

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

AIDE_CONF_OPT	:= $(CROSS_AUTOCONF_USR) \
			--disable-static \
			--without-curl \
			--without-mhash \
			--with-gcrypt	\
			--with-zlib

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
	@$(call install_fixup, aide,AUTHOR,"WAGO GmbH \& Co. KG")
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
