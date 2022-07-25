# -*-makefile-*-
#
# Copyright (C) 2021 by WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PHP_PAM) += php-pam

#
# Paths and names
#
PHP_PAM_VERSION	:= 2.2.3
PHP_PAM_MD5	:= 5adc8b8dea1e4f6897c42a5e752752e4
PHP_PAM		:= php-pam-$(PHP_PAM_VERSION)
PHP_PAM_SUFFIX	:= zip
PHP_PAM_URL	:= https://github.com/amishmm/php-pam/archive/refs/tags/v$(PHP_PAM_VERSION).$(PHP_PAM_SUFFIX)
PHP_PAM_SOURCE	:= $(SRCDIR)/$(PHP_PAM).$(PHP_PAM_SUFFIX)
PHP_PAM_DIR	:= $(BUILDDIR)/$(PHP_PAM)
PHP_PAM_LICENSE	:= PHP License
PHP_PAM_LICENSE_FILE := LICENSE
PHP_PAM_LICENSE_FILES	:= file://$(PHP_PAM_LICENSE_FILE);md5=7e571b888d585b31f9ef5edcc647fa30

#
# ...there must be a cleaner solution than this ugly sh**
#
PHP_PAM_CFLAGS += -I$(PTXDIST_SYSROOT_TARGET)/usr/include/php
PHP_PAM_CFLAGS += -I$(PTXDIST_SYSROOT_TARGET)/usr/include/php/main
PHP_PAM_CFLAGS += -I$(PTXDIST_SYSROOT_TARGET)/usr/include/php/TSRM
PHP_PAM_CFLAGS += -I$(PTXDIST_SYSROOT_TARGET)/usr/include/php/Zend

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(PHP_PAM_SOURCE):
#	@$(call targetinfo)
#	@$(call get, PHP_PAM)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

PHP_PAM_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
PHP_PAM_CONF_TOOL	:= autoconf
PHP_PAM_CONF_OPT	:= $(CROSS_AUTOCONF_USR) \
	--with-php-config=$(PTXDIST_SYSROOT_TARGET)/usr/bin/php-config \
	--with-pam=$(PTXDIST_SYSROOT_TARGET)/usr

$(STATEDIR)/php-pam.prepare:
	@$(call targetinfo)
	@$(call clean, $(PHP_PAM_DIR)/config.cache)
	cd $(PHP_PAM_DIR) && \
		$(PHP_PAM_CONF_ENV) \
		$(PTXDIST_SYSROOT_TARGET)/usr/bin/phpize
	@$(call world/prepare, PHP_PAM)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/php-pam.targetinstall:
	@$(call targetinfo)
	@$(call install_init, php-pam)
	@$(call install_fixup, php-pam,PRIORITY,optional)
	@$(call install_fixup, php-pam,SECTION,base)
	@$(call install_fixup, php-pam,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, php-pam,DESCRIPTION,missing)

	@$(call install_copy, php-pam, 0, 0, 0755, $(PHP_PAM_DIR)/modules/pam.so, /usr/lib/php/pam.so)
	@$(call install_copy, php-pam, 0, 0, 0644, $(PHP_PAM_DIR)/$(PHP_PAM_LICENSE_FILE), /usr/share/licenses/oss/license.php-pam_$(PHP_PAM_VERSION).txt)

	@$(call install_finish, php-pam)
	@$(call touch)


# vim: syntax=make
