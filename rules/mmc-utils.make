# -*-makefile-*-
#
# Copyright (C) 2016 by Matthias Klein <matthias.klein@optimeas.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_MMC_UTILS) += mmc-utils

#
# Paths and names
#
# No tags: use a fake descriptive commit-ish to include the date
MMC_UTILS_VERSION	:= 2018-03-27-gb4fe0c8c0e57
MMC_UTILS_MD5		:= 1018cdca94530054cc11e4bcb97d4b11
MMC_UTILS		:= mmc-utils-$(MMC_UTILS_VERSION)
MMC_UTILS_SUFFIX	:= tar.gz
MMC_UTILS_URL		:= https://git.kernel.org/pub/scm/linux/kernel/git/cjb/mmc-utils.git;tag=$(MMC_UTILS_VERSION)
MMC_UTILS_SOURCE	:= $(SRCDIR)/$(MMC_UTILS).$(MMC_UTILS_SUFFIX)
MMC_UTILS_DIR		:= $(BUILDDIR)/$(MMC_UTILS)
MMC_UTILS_LICENSE	:= GPL-2.0-only AND BSD-3-Clause

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

MMC_UTILS_CONF_TOOL	:= NO
MMC_UTILS_MAKE_ENV	:= $(CROSS_ENV) prefix=/usr

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/mmc-utils.targetinstall:
	@$(call targetinfo)

	@$(call install_init, mmc-utils)
	@$(call install_fixup, mmc-utils,PRIORITY,optional)
	@$(call install_fixup, mmc-utils,SECTION,base)
	@$(call install_fixup, mmc-utils,AUTHOR,"Matthias Klein <matthias.klein@optimeas.de>")
	@$(call install_fixup, mmc-utils,DESCRIPTION,missing)

	@$(call install_copy, mmc-utils, 0, 0, 0755, -, /usr/bin/mmc)

	@$(call install_finish, mmc-utils)

	@$(call touch)

# vim: syntax=make
