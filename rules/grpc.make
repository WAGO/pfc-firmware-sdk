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
PACKAGES-$(PTXCONF_GRPC) += grpc

#
# Paths and names
#
GRPC_VERSION  := 1.30.0
GRPC_MD5    := 316b30c89b48b8ae0ad005bb12ac566a
GRPC			:= grpc-$(GRPC_VERSION)
GRPC_SUFFIX		:= tar.gz
GRPC_URL		:= https://github.com/grpc/grpc/archive/v$(GRPC_VERSION).$(GRPC_SUFFIX)
GRPC_SOURCE		:= $(SRCDIR)/$(GRPC).$(GRPC_SUFFIX)
GRPC_DIR		:= $(BUILDDIR)/$(GRPC)
GRPC_LICENSE	:= Apache-2.0
GRPC_LICENSE_FILE := LICENSE

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# cmake
#
GRPC_CONF_TOOL	:= cmake

GRPC_CONF_OPT	:= $(CROSS_CMAKE_USR)
GRPC_CONF_OPT	+= -DCMAKE_VERBOSE_MAKEFILE=ON
GRPC_CONF_OPT	+= -DBUILD_SHARED_LIBS=ON
GRPC_CONF_OPT	+= -DCMAKE_BUILD_TYPE=MinSizeRel
GRPC_CONF_OPT	+= -DCMAKE_C_FLAGS="-pipe -Os -flto"
GRPC_CONF_OPT	+= -DCMAKE_CXX_FLAGS="-pipe -Os -flto"
GRPC_CONF_OPT	+= -DgRPC_ABSL_PROVIDER=package
GRPC_CONF_OPT	+= -DgRPC_CARES_PROVIDER=package
GRPC_CONF_OPT	+= -DgRPC_PROTOBUF_PROVIDER=package
GRPC_CONF_OPT	+= -DgRPC_SSL_PROVIDER=package
GRPC_CONF_OPT	+= -DgRPC_ZLIB_PROVIDER=package
GRPC_CONF_OPT	+= -DgRPC_INSTALL=ON
GRPC_CONF_OPT	+= -DgRPC_USE_PROTO_LITE=ON
GRPC_CONF_OPT	+= -DgRPC_PROTOBUF_PACKAGE_TYPE="CONFIG"
GRPC_CONF_OPT	+= -DPROTOBUF_PROTOC_EXECUTABLE:PATH="$(PTXCONF_SYSROOT_HOST)/bin/protoc"
GRPC_CONF_OPT	+= -DgRPC_BUILD_GRPC_CSHARP_PLUGIN=OFF
GRPC_CONF_OPT	+= -DgRPC_BUILD_GRPC_NODE_PLUGIN=OFF
GRPC_CONF_OPT	+= -DgRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN=OFF
GRPC_CONF_OPT	+= -DgRPC_BUILD_GRPC_PHP_PLUGIN=OFF
GRPC_CONF_OPT	+= -DgRPC_BUILD_GRPC_PYTHON_PLUGIN=OFF
GRPC_CONF_OPT	+= -DgRPC_BUILD_GRPC_RUBY_PLUGIN=OFF

# ----------------------------------------------------------------------------
# Configure
# ----------------------------------------------------------------------------

$(STATEDIR)/grpc.prepare:
	@$(call targetinfo)
	@export PATH=$(PTXCONF_SYSROOT_HOST)/bin:${PATH}
	@$(call world/prepare, GRPC)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/grpc.compile:
	@$(call targetinfo)
	# We need the grpc_cpp_plugin for the build host in order to execute it
	# Sneaky workaround: build the plugins, replace the plugin, continue build
	cd $(GRPC_DIR)-build \
			&& make grpc_cpp_plugin \
			&& mv grpc_cpp_plugin{,.bkp} && cp $(PTXCONF_SYSROOT_HOST)/bin/grpc_cpp_plugin . \
			&& make all \
			&& rm grpc_cpp_plugin && mv grpc_cpp_plugin{.bkp,}
	@$(call world/compile, GRPC)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/grpc.targetinstall:
	@$(call targetinfo)

	@$(call install_init, grpc)
	@$(call install_fixup, grpc,PRIORITY,optional)
	@$(call install_fixup, grpc,SECTION,base)
	@$(call install_fixup, grpc,AUTHOR,"Marvin Schmidt <marvin.schmidt@who-ing.de>")
	@$(call install_fixup, grpc,DESCRIPTION,missing)

	@$(call install_lib, grpc, 0, 0, 0644, libaddress_sorting)
	@$(call install_lib, grpc, 0, 0, 0644, libgpr)
	@$(call install_lib, grpc, 0, 0, 0644, libgrpc)
	@$(call install_lib, grpc, 0, 0, 0644, libgrpc++)
	@$(call install_lib, grpc, 0, 0, 0644, libupb)
	# @$(call install_lib, grpc, 0, 0, 0644, libgrpc++_cronet)
	# @$(call install_lib, grpc, 0, 0, 0644, libgrpc++_error_details)
	# @$(call install_lib, grpc, 0, 0, 0644, libgrpc++_reflection)
	# @$(call install_lib, grpc, 0, 0, 0644, libgrpc++_unsecure)
	# @$(call install_lib, grpc, 0, 0, 0644, libgrpc_cronet)
	# @$(call install_lib, grpc, 0, 0, 0644, libgrpc_csharp_ext)
	# @$(call install_lib, grpc, 0, 0, 0644, libgrpc_plugin_support)
	# @$(call install_lib, grpc, 0, 0, 0644, libgrpc_unsecure)
	
	@$(call install_copy, grpc, 0, 0, 0644, $(GRPC_DIR)/$(GRPC_LICENSE_FILE), /usr/share/licenses/oss/license.grpc_$(GRPC_VERSION).txt)

	@$(call install_finish, grpc)

	@$(call touch)

# vim: syntax=make
