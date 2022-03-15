# -*-makefile-*-
#
# Copyright (C) 2016 by Juergen Borleis <jbe@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
ifndef PTXCONF_LIBAIO_SKIP_TARGETINSTALL
PACKAGES-$(PTXCONF_LIBAIO) += libaio
else
LAZY_PACKAGES-$(PTXCONF_LIBAIO) += libaio
endif

#
# Paths and names
#
LIBAIO_VERSION	:= 0.3.110
LIBAIO_MD5	:= 2a35602e43778383e2f4907a4ca39ab8
LIBAIO		:= libaio-$(LIBAIO_VERSION)
LIBAIO_SUFFIX	:= tar.gz
LIBAIO_URL	:= http://pkgs.fedoraproject.org/repo/pkgs/libaio/libaio-0.3.110.tar.gz/2a35602e43778383e2f4907a4ca39ab8/$(LIBAIO).$(LIBAIO_SUFFIX)
LIBAIO_SOURCE	:= $(SRCDIR)/$(LIBAIO).$(LIBAIO_SUFFIX)
LIBAIO_DIR	:= $(BUILDDIR)/$(LIBAIO)
LIBAIO_LICENSE	:= LGPL-2.1-only

# ----------------------------------------------------------------------------
# Prepare + Compile
# ----------------------------------------------------------------------------

#
# just a Makefile made by a creative Linux kernel hacker...
#
LIBAIO_CONF_TOOL	:= NO
LIBAIO_MAKE_ENV		:= $(CROSS_ENV)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libaio.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libaio)
	@$(call install_fixup, libaio,PRIORITY,optional)
	@$(call install_fixup, libaio,SECTION,base)
	@$(call install_fixup, libaio,AUTHOR,"Juergen Borleis <jbe@pengutronix.de>")
	@$(call install_fixup, libaio,DESCRIPTION,"asynchronous I/O access library")

	@$(call install_lib, libaio, 0, 0, 0644, libaio)

	@$(call install_finish, libaio)

	@$(call touch)

# vim: syntax=make
