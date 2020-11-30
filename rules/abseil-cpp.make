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
PACKAGES-$(PTXCONF_ABSEIL_CPP) += abseil-cpp

#
# Paths and names
#
ABSEIL_CPP_VERSION	:= 20200225.2
ABSEIL_CPP_MD5		:= 73f2b6e72f1599a9139170c29482ddc4
ABSEIL_CPP		:= abseil-cpp-$(ABSEIL_CPP_VERSION)
ABSEIL_CPP_SUFFIX	:= tar.gz
ABSEIL_CPP_URL		:= https://github.com/abseil/abseil-cpp/archive/$(ABSEIL_CPP_VERSION).$(ABSEIL_CPP_SUFFIX)
ABSEIL_CPP_SOURCE	:= $(SRCDIR)/$(ABSEIL_CPP).$(ABSEIL_CPP_SUFFIX)
ABSEIL_CPP_DIR		:= $(BUILDDIR)/$(ABSEIL_CPP)
ABSEIL_CPP_LICENSE	:= Apache-2.0
ABSEIL_CPP_LICENSE_FILE := LICENSE


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# cmake
#
ABSEIL_CPP_CONF_TOOL	:= cmake
ABSEIL_CPP_CONF_OPT	:= $(CROSS_CMAKE_USR)
ABSEIL_CPP_CONF_OPT	+= -DBUILD_TESTING=OFF
ABSEIL_CPP_CONF_OPT	+= -DCMAKE_CXX_STANDARD=11
ABSEIL_CPP_CONF_OPT	+= -DABSL_ENABLE_INSTALL=TRUE
ABSEIL_CPP_CONF_OPT	+= -DBUILD_SHARED_LIBS=ON

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/abseil-cpp.targetinstall:
	@$(call targetinfo)

	@$(call install_init, abseil-cpp)
	@$(call install_fixup, abseil-cpp,PRIORITY,optional)
	@$(call install_fixup, abseil-cpp,SECTION,base)
	@$(call install_fixup, abseil-cpp,AUTHOR,"Alexander Riedel <alexander.riedel@wago.com>")
	@$(call install_fixup, abseil-cpp,DESCRIPTION,missing)

	@$(call install_lib, abseil-cpp, 0, 0, 0644, libabsl_bad_optional_access)
	@$(call install_lib, abseil-cpp, 0, 0, 0644, libabsl_base)
	@$(call install_lib, abseil-cpp, 0, 0, 0644, libabsl_civil_time)
	@$(call install_lib, abseil-cpp, 0, 0, 0644, libabsl_dynamic_annotations)
	@$(call install_lib, abseil-cpp, 0, 0, 0644, libabsl_int128)
	@$(call install_lib, abseil-cpp, 0, 0, 0644, libabsl_log_severity)
	@$(call install_lib, abseil-cpp, 0, 0, 0644, libabsl_raw_logging_internal)
	@$(call install_lib, abseil-cpp, 0, 0, 0644, libabsl_spinlock_wait)
	@$(call install_lib, abseil-cpp, 0, 0, 0644, libabsl_str_format_internal)
	@$(call install_lib, abseil-cpp, 0, 0, 0644, libabsl_strings)
	@$(call install_lib, abseil-cpp, 0, 0, 0644, libabsl_strings_internal)
	@$(call install_lib, abseil-cpp, 0, 0, 0644, libabsl_throw_delegate)
	@$(call install_lib, abseil-cpp, 0, 0, 0644, libabsl_time)
	@$(call install_lib, abseil-cpp, 0, 0, 0644, libabsl_time_zone)

	@$(call install_copy, abseil-cpp, 0, 0, 0644, $(ABSEIL_CPP_DIR)/$(ABSEIL_CPP_LICENSE_FILE), /usr/share/licenses/oss/license.abseil-cpp_$(ABSEIL_CPP_VERSION).txt)

	@$(call install_finish, abseil-cpp)

	@$(call touch)

# vim: syntax=make
