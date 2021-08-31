# -*-makefile-*-
#
# Copyright (C) 2017 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_MESON) += host-meson

#
# Paths and names
#
HOST_MESON_VERSION	:= 0.53.1
HOST_MESON_MD5		:= 9bf73f7b5a2426a7c8674a809bb8cae2
HOST_MESON		:= meson-$(HOST_MESON_VERSION)
HOST_MESON_SUFFIX	:= tar.gz
HOST_MESON_URL		:= https://github.com/mesonbuild/meson/releases/download/$(HOST_MESON_VERSION)/$(HOST_MESON).$(HOST_MESON_SUFFIX)
HOST_MESON_SOURCE	:= $(SRCDIR)/$(HOST_MESON).$(HOST_MESON_SUFFIX)
HOST_MESON_DIR		:= $(HOST_BUILDDIR)/$(HOST_MESON)
HOST_MESON_LICENSE	:= Apache-2.0
HOST_MESON_LICENSE_FILES	:= file://COPYING;md5=3b83ef96387f14655fc854ddc3c6bd57

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_MESON_CONF_TOOL	:= NO

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/host-meson.compile:
	@$(call targetinfo)
	@$(call world/execute, HOST_MESON, $(SYSTEMPYTHON3) setup.py build)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

# Special dirs to avoid collisions with host-python3
HOST_MESON_INSTALL_OPT	:= \
	install \
	--prefix=/ \
	--install-lib=/lib/meson \
	--install-scripts=/lib/meson \
	--root=$(HOST_MESON_PKGDIR)

$(STATEDIR)/host-meson.install:
	@$(call targetinfo)
ifeq ($(PTXCONF_CONFIGFILE_VERSION), "2020.08.0")
	@$(call world/execute, HOST_MESON, \
		$(SYSTEMPYTHON3) setup.py $(HOST_MESON_INSTALL_OPT))
else
	@cd $(HOST_MESON_DIR) && \
		$(SYSTEMPYTHON3) setup.py $(HOST_MESON_INSTALL_OPT)
endif
	@mkdir -vp $(HOST_MESON_PKGDIR)/bin
	@ln -svf ../lib/meson/meson $(HOST_MESON_PKGDIR)/bin/meson
	@$(call touch)

$(STATEDIR)/host-meson.install.post: $(PTXDIST_MESON_CROSS_FILE)

# vim: syntax=make
