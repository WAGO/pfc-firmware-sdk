# -*-makefile-*-
#
# Copyright (C) 2017 by Marvin Schmidt <marvin.schmidt@who-ing.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_GRPC) += host-grpc

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# cmake
#
HOST_GRPC_CONF_TOOL	:= cmake

HOST_GRPC_CONF_OPT	:= $(HOST_CMAKE_OPT)
HOST_GRPC_CONF_OPT	+= -DCMAKE_VERBOSE_MAKEFILE=ON
HOST_GRPC_CONF_OPT  += -DgRPC_ABSL_PROVIDER=package
HOST_GRPC_CONF_OPT	+= -DgRPC_CARES_PROVIDER=package
HOST_GRPC_CONF_OPT	+= -DgRPC_PROTOBUF_PROVIDER=package
HOST_GRPC_CONF_OPT	+= -DgRPC_SSL_PROVIDER=package
HOST_GRPC_CONF_OPT	+= -DgRPC_ZLIB_PROVIDER=package
HOST_GRPC_CONF_OPT	+= -DgRPC_INSTALL=ON
HOST_GRPC_CONF_OPT	+= -DgRPC_PROTOBUF_PACKAGE_TYPE="CONFIG"
HOST_GRPC_CONF_OPT	+= -DgRPC_BUILD_GRPC_CSHARP_PLUGIN=OFF
HOST_GRPC_CONF_OPT	+= -DgRPC_BUILD_GRPC_NODE_PLUGIN=OFF
HOST_GRPC_CONF_OPT	+= -DgRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN=OFF
HOST_GRPC_CONF_OPT	+= -DgRPC_BUILD_GRPC_PHP_PLUGIN=OFF
HOST_GRPC_CONF_OPT	+= -DgRPC_BUILD_GRPC_PYTHON_PLUGIN=OFF
HOST_GRPC_CONF_OPT	+= -DgRPC_BUILD_GRPC_RUBY_PLUGIN=OFF

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------
# Host, image and cross packages don’t need to install anything in the target
# file system. Therefore, PTXdist only respects the targetinstall and
# targetinstall.post stages for packages whose name doesn’t start with host-,
# image-, or cross-.

# vim: syntax=make
