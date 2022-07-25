# -*-makefile-*-
#
# Copyright (C) 2017 Jobst Wellensiek <jobst.wellensiek@wago.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_RE2) += host-re2

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# cmake
#
HOST_RE2_CONF_TOOL      := cmake
HOST_RE2_CONF_OPT += $(HOST_CMAKE_OPT)
HOST_RE2_CONF_OPT += -DBUILD_SHARED_LIBS=ON
HOST_RE2_CONF_OPT += -DCMAKE_CXX_STANDARD=11

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------
# Host, image and cross packages don’t need to install anything in the target
# file system. Therefore, PTXdist only respects the targetinstall and
# targetinstall.post stages for packages whose name doesn’t start with host-,
# image-, or cross-.

# vim: syntax=make
