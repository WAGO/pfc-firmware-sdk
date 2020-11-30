# -*-makefile-*-
#
# Copyright (C) 2012 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_PROTOBUF) += host-protobuf

HOST_PROTOBUF		= $(PROTOBUF)
HOST_PROTOBUF_DIR	= $(HOST_BUILDDIR)/$(HOST_PROTOBUF)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_PROTOBUF_CONF_TOOL	:= cmake

HOST_PROTOBUF_CONF_OPT	:= $(HOST_CMAKE_OPT)
HOST_PROTOBUF_CONF_OPT	+= -Dprotobuf_BUILD_TESTS=OFF
HOST_PROTOBUF_CONF_OPT	+= -Dprotobuf_BUILD_PROTOC_BINARIES=ON

HOST_PROTOBUF_SUBDIR := cmake

$(STATEDIR)/host-protobuf.install:
	@$(call targetinfo)
	@$(call world/install, HOST_PROTOBUF)
	# install js_embed binary, we need it to build protobuf for the target
	#@cp $(HOST_PROTOBUF_DIR)-build/js_embed $(PTXDIST_SYSROOT_HOST)/bin/js_embed
	@$(call touch)

# vim: syntax=make
