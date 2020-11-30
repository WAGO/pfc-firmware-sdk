# -*-makefile-*-
#
# Copyright (C) 2018 by Marvin Schmidt <marvin.schmidt@who-ing.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_MPARK_VARIANT) += mpark-variant

#
# Paths and names
#
MPARK_VARIANT_VERSION	:= 1.3.0
MPARK_VARIANT_MD5		:= 368b7d6f1a07bd6ee26ff518258dc71c
MPARK_VARIANT		:= mpark-variant-$(MPARK_VARIANT_VERSION)
MPARK_VARIANT_SUFFIX	:= tar.gz
MPARK_VARIANT_URL		:= https://github.com/mpark/variant/archive/v$(MPARK_VARIANT_VERSION).$(MPARK_VARIANT_SUFFIX)
MPARK_VARIANT_SOURCE	:= $(SRCDIR)/$(MPARK_VARIANT).$(MPARK_VARIANT_SUFFIX)
MPARK_VARIANT_DIR		:= $(BUILDDIR)/$(MPARK_VARIANT)
MPARK_VARIANT_LICENSE	:= Boost-1.0

#
# cmake
#
MPARK_VARIANT_CONF_TOOL	:= cmake
MPARK_VARIANT_CONF_OPT	:= $(CROSS_CMAKE_USR)

# vim: syntax=make
