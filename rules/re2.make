# -*-makefile-*-
#
# Copyright (C) 2020 by Jobst Wellensiek <jobst.wellensiek@wago.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_RE2) += re2

#
# Paths and names
#
RE2_VERSION	:= 2022-02-01
RE2_MD5		:= c050bafdf8ca452036a17aae42dae71d
RE2		:= re2-$(RE2_VERSION)
RE2_SUFFIX	:= tar.gz
RE2_URL		:= https://github.com/google/re2/archive/refs/tags/$(RE2_VERSION).$(RE2_SUFFIX)
RE2_SOURCE	:= $(SRCDIR)/$(RE2).$(RE2_SUFFIX)
RE2_DIR		:= $(BUILDDIR)/$(RE2)
RE2_LICENSE	:= BSD-style
RE2_LICENSE_FILE := LICENSE
RE2_IPK_VERSION := $(shell echo $(RE2_VERSION) | sed "s/-/+/g")


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# cmake
#
RE2_CONF_TOOL	:= cmake
RE2_CONF_OPT	:= $(CROSS_CMAKE_USR)
RE2_CONF_OPT += -DBUILD_TESTING=OFF
RE2_CONF_OPT += -DCMAKE_CXX_STANDARD=11
RE2_CONF_OPT += -DABSL_ENABLE_INSTALL=TRUE
RE2_CONF_OPT += -DBUILD_SHARED_LIBS=ON

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/re2.targetinstall:
	@$(call targetinfo)

	@$(call install_init, re2)
	@$(call install_fixup, re2,PRIORITY,optional)
	@$(call install_fixup, re2,SECTION,base)
	@$(call install_fixup, re2,AUTHOR,"Jobst Wellensiek <jobst.wellensiek@wago.com>")
	@$(call install_fixup, re2,DESCRIPTION,missing)

	@$(call install_lib, re2, 0, 0, 0644, libre2) 

	@$(call install_copy, re2, 0, 0, 0644, $(RE2_DIR)/$(RE2_LICENSE_FILE), /usr/share/licenses/oss/license.re2_$(RE2_IPK_VERSION).txt)

	@$(call install_finish, re2)

	@$(call touch)

# vim: syntax=make
