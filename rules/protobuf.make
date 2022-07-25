# -*-makefile-*-
#
# Copyright (C) 2012 by Adrian Baumgarth <adrian.baumgarth@l-3com.com>
# Copyright (C) 2017 by Marvin Schmidt <Marvin.Schmidt@who-ing.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PROTOBUF) += protobuf

#
# Paths and names
#
PROTOBUF_VERSION	:= 3.19.4
PROTOBUF_MD5		:= ccedd5b7b09a9eda37e8654155baca5a
PROTOBUF		:= protobuf-cpp-$(PROTOBUF_VERSION)
PROTOBUF_SUFFIX		:= tar.gz
PROTOBUF_URL		:= https://github.com/protocolbuffers/protobuf/releases/download/v$(PROTOBUF_VERSION).$(PROTOBUF_SUFFIX)
PROTOBUF_SOURCE		:= $(SRCDIR)/$(PROTOBUF).$(PROTOBUF_SUFFIX)
PROTOBUF_DIR		:= $(BUILDDIR)/$(PROTOBUF)
PROTOBUF_LICENSE	:= BSD-3-Clause
PROTOBUF_LICENSE_FILE := LICENSE

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# cmake
#
PROTOBUF_CONF_TOOL	:= cmake

PROTOBUF_CONF_OPT	:= $(CROSS_CMAKE_USR)
PROTOBUF_CONF_OPT	+= -DCMAKE_BUILD_TYPE=MinSizeRel
PROTOBUF_CONF_OPT	+= -DCMAKE_C_FLAGS="-pipe -Os -flto"
PROTOBUF_CONF_OPT	+= -DCMAKE_CXX_FLAGS="-pipe -Os -flto"
PROTOBUF_CONF_OPT	+= -DBUILD_SHARED_LIBS=ON
PROTOBUF_CONF_OPT	+= -Dprotobuf_BUILD_TESTS=OFF
PROTOBUF_CONF_OPT	+= -Dprotobuf_BUILD_PROTOC_BINARIES=ON
PROTOBUF_CONF_OPT	+= -Dprotobuf_WITH_ZLIB=ON
PROTOBUF_CONF_OPT += -DWITH_PROTOC=$(PTXDIST_SYSROOT_HOST)/bin/protoc

PROTOBUF_SUBDIR := cmake

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/protobuf.targetinstall:
	@$(call targetinfo)

	@$(call install_init, protobuf)
	@$(call install_fixup, protobuf,PRIORITY,optional)
	@$(call install_fixup, protobuf,SECTION,base)
	@$(call install_fixup, protobuf,AUTHOR,"Adrian Baumgarth <adrian.baumgarth@l-3com.com>")
	@$(call install_fixup, protobuf,DESCRIPTION,missing)

ifdef PTXCONF_PROTOBUF_FULL
	@$(call install_lib, protobuf, 0, 0, 0644, libprotobuf)
endif

ifdef PTXCONF_PROTOBUF_LITE
	@$(call install_lib, protobuf, 0, 0, 0644, libprotobuf-lite)
endif

	@$(call install_copy, protobuf, 0, 0, 0644, $(PROTOBUF_DIR)/$(PROTOBUF_LICENSE_FILE), /usr/share/licenses/oss/license.protobuf_$(PROTOBUF_VERSION).txt)

	@$(call install_finish, protobuf)

	@$(call touch)

# vim: syntax=make
