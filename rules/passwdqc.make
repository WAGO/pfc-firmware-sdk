# -*-makefile-*-
#
# Copyright (C) 2021 by WAGO Kontakttechnik GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PASSWDQC) += passwdqc

#
# Paths and names
#
PASSWDQC_VERSION	:= 2_0_2
PASSWDQC_VERSION_LIC	:= $(shell echo "${PASSWDQC_VERSION}" | tr _ .)
PASSWDQC_MD5		:= f30955b2397642c6ea20a8f696421150
PASSWDQC		:= PASSWDQC_$(PASSWDQC_VERSION)
PASSWDQC_SUFFIX		:= tar.gz
PASSWDQC_URL		:= https://github.com/openwall/passwdqc/archive/refs/tags/$(PASSWDQC).$(PASSWDQC_SUFFIX)
PASSWDQC_SOURCE		:= $(SRCDIR)/$(PASSWDQC).$(PASSWDQC_SUFFIX)
PASSWDQC_DIR		:= $(BUILDDIR)/$(PASSWDQC)
PASSWDQC_LICENSE	:= BSD, BSD-3-Clause, GPL-2.0
PASSWDQC_LICENSE	:= file://LICENSE;md5=ac99c8678577a1c2f9f04cccee411d5d

PASSWDQC_CONF_TOOL	:= NO
PASSWDQC_MAKE_ENV	:= $(CROSS_ENV)
PASSWDQC_MAKE_OPT	:= \
	$(CROSS_ENV_CFLAGS) \
	$(CROSS_ENV_CPPFLAGS) \
	$(CROSS_ENV_LDFLAGS) \
	$(CROSS_ENV_CC)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(PASSWDQC_SOURCE):
#	@$(call targetinfo)
#	@$(call get, PASSWDQC)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#PASSWDQC_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
#PASSWDQC_CONF_TOOL	:= autoconf
#PASSWDQC_CONF_OPT	:= $(CROSS_AUTOCONF_USR)

#$(STATEDIR)/passwdqc.prepare:
#	@$(call targetinfo)
#	@$(call clean, $(PASSWDQC_DIR)/config.cache)
#	cd $(PASSWDQC_DIR) && \
#		$(PASSWDQC_PATH) $(PASSWDQC_ENV) \
#		./configure $(PASSWDQC_CONF_OPT)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/passwdqc.compile:
#	@$(call targetinfo)
#	@$(call world/compile, PASSWDQC)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/passwdqc.install:
#	@$(call targetinfo)
#	@$(call world/install, PASSWDQC)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

PASSWDQ_TOOLS-$(PTXCONF_PASSWDQC_PWQCHECK)	+= pwqcheck
PASSWDQ_TOOLS-$(PTXCONF_PASSWDQC_PWQFILTER)	+= pwqfilter
PASSWDQ_TOOLS-$(PTXCONF_PASSWDQC_PWQGEN)	+= pwqgen

$(STATEDIR)/passwdqc.targetinstall:
	@$(call targetinfo)

	@$(call install_init, passwdqc)
	@$(call install_fixup, passwdqc,PRIORITY,optional)
	@$(call install_fixup, passwdqc,SECTION,base)
	@$(call install_fixup, passwdqc,AUTHOR,"WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, passwdqc,DESCRIPTION,missing)


	#
	# install configuration file
	#
ifdef PTXCONF_PASSWDQC_PWDQC_CONF
	@$(call install_alternative, passwdqc, 0, 0, 0755, /etc/passwdqc.conf)
endif

	#
	# install library
	#
	@$(call install_lib, passwdqc, 0, 0, 0644, libpasswdqc)

	#
	# install tools
	#
	@$(foreach tool, $(PASSWDQ_TOOLS-y), \
		$(call install_copy, passwdqc, 0, 0, 0755, -, /usr/bin/$(tool));)

	#
	# install pam module
	#
ifdef PTXCONF_PASSWDQC_PAM_PASSWDQC
	@$(call install_copy, passwdqc, 0, 0, 0755, -, /lib/security/pam_passwdqc.so)
endif

	#
	# install oss license
	#
ifdef PTXCONF_PASSWDQC_PAM_PASSWDQC
	@$(call install_copy, passwdqc, 0, 0, 0644, $(PASSWDQC_DIR)/LICENSE, /usr/share/licenses/oss/license.passwdqc_$(PASSWDQC_VERSION_LIC).txt)
endif

	@$(call install_finish, passwdqc)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/passwdqc.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, PASSWDQC)

# vim: syntax=make
