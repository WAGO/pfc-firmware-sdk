# -*-makefile-*-
#
# Copyright (C) 2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LSOF) += lsof

#
# Paths and names
#
LSOF_BASE	:= 4.91
LSOF_VERSION	:= $(LSOF_BASE)+dfsg
LSOF_MD5	:= 0df626776ed32b6deae052d1c613ee3a
LSOF_SUFFIX	:= tar.gz
LSOF		:= lsof-$(LSOF_VERSION)
LSOF_TARBALL	:= lsof_$(LSOF_VERSION).orig.$(LSOF_SUFFIX)
LSOF_URL	:= http://snapshot.debian.org/archive/debian/20190226T105528Z/pool/main/l/lsof/$(LSOF_TARBALL)
LSOF_SOURCE	:= $(SRCDIR)/$(LSOF_TARBALL)
LSOF_DIR	:= $(BUILDDIR)/$(LSOF)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LSOF_PATH	:= PATH=$(CROSS_PATH)

LSOF_ENV 	:= \
	$(CROSS_ENV) \
	LINUX_HASSELINUX=N \
	LSOF_AR="$(CROSS_AR) cr"

LSOF_MAKEVARS	:= \
	$(CROSS_ENV_CC) \
	LSOF_USER=none \
	DEBUG=-O2 \
	RANLIB="$(CROSS_RANLIB) liblsof.a"

#
# autoconf
#
LSOF_AUTOCONF := -n linux

$(STATEDIR)/lsof.prepare:
	@$(call targetinfo)
	cd $(LSOF_DIR) && \
		$(LSOF_PATH) $(LSOF_ENV) \
		./Configure $(LSOF_AUTOCONF)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/lsof.install:
	@$(call targetinfo)
	install -D -m 755 "$(LSOF_DIR)/lsof" "$(LSOF_PKGDIR)/usr/bin/lsof"
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/lsof.targetinstall:
	@$(call targetinfo)

	@$(call install_init, lsof)
	@$(call install_fixup, lsof,PRIORITY,optional)
	@$(call install_fixup, lsof,SECTION,base)
	@$(call install_fixup, lsof,AUTHOR,"Marc Kleine-Budde <mkl@pengutronix.de>")
	@$(call install_fixup, lsof,DESCRIPTION,missing)

	@$(call install_copy, lsof, 0, 0, 0755, -, /usr/bin/lsof)

	@$(call install_finish, lsof)

	@$(call touch)

# vim: syntax=make
