# -*-makefile-*-
#
# Copyright (C) 2007 by KOAN sas, by Marco Cavallini <m.cavallini@koansoftware.com>
#               2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_SED) += sed

#
# Paths and names
#
SED_VERSION	:= 4.7
SED_MD5		:= 777ddfd9d71dd06711fe91f0925e1573
SED		:= sed-$(SED_VERSION)
SED_SUFFIX	:= tar.xz
SED_URL		:= $(call ptx/mirror, GNU, sed/$(SED).$(SED_SUFFIX))
SED_SOURCE	:= $(SRCDIR)/$(SED).$(SED_SUFFIX)
SED_DIR		:= $(BUILDDIR)/$(SED)
SED_LICENSE	:= GPLv3, GNU Free Documentation License
SED_LICENSE_FILES := \
	file://COPYING;md5=f27defe1e96c2e1ecd4e0c9be8967949 \
	file://COPYING.DOC;md5=10b9de612d532fdeeb7fe8fcd1435cc6

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

SED_PATH	:= PATH=$(CROSS_PATH)
SED_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
SED_AUTOCONF := \
	$(CROSS_AUTOCONF_USR)

# sed has a silent dependency to libacl! if libacl was build and
# "disable-acl" is not set in configure, then sed will have a
# runtime dependency to libacl which might not be installed within
# targetimage, e.g. libacl is build as module
ifneq ($(PTXCONF_ACL),y)
	SED_AUTOCONF	+= --disable-acl
endif

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/sed.targetinstall:
	@$(call targetinfo)

	@$(call install_init, sed)
	@$(call install_fixup, sed,PRIORITY,optional)
	@$(call install_fixup, sed,SECTION,base)
	@$(call install_fixup, sed,AUTHOR,"Marco Cavallini <m.cavallini@koansoftware.com>")
	@$(call install_fixup, sed,DESCRIPTION,missing)

	@$(call install_copy, sed, 0, 0, 0755, -, /usr/bin/sed)

	@$(call install_finish, sed)

	@$(call touch)

# vim: syntax=make
