# -*-makefile-*-
#
# Copyright (C) 2010 by Marc Kleine-Budde <mkl@pengutronix.de>
# Copyright (C) 2010 by Robert Schwebel <r.schwebel@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_LIBCAP) += host-libcap

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_LIBCAP_MAKE_OPT := \
	prefix= \
	lib=lib \
	BUILD_GPERF=no \
	PAM_CAP=no \
	GOLANG=no \
	LIBATTR=no \
	DYNAMIC=yes

HOST_LIBCAP_INSTALL_OPT := $(HOST_LIBCAP_MAKE_OPT) RAISE_SETFCAP=no install

# vim: syntax=make
