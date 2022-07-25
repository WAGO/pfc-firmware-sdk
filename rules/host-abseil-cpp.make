# -*-makefile-*-
#
# Copyright (C) 2020 by Alexander Riedel <alexander.riedel@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_ABSEIL_CPP) += host-abseil-cpp

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# cmake
#
HOST_ABSEIL_CPP_CONF_TOOL	:= cmake
HOST_ABSEIL_CPP_CONF_OPT	+= $(HOST_CMAKE_OPT)
HOST_ABSEIL_CPP_CONF_OPT	+= -DBUILD_SHARED_LIBS=ON
HOST_ABSEIL_CPP_CONF_OPT += -DCMAKE_CXX_STANDARD=11

# vim: syntax=make
