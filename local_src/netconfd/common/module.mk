#######################################################################################################################
# Module configuration

common_PROJECT_ROOT = $(PROJECT_ROOT)/common

#######################################################################################################################
# Build target configuration

INCLUDES += \
$(libcommon.a_INCLUDES)

MAIN_BUILDTARGETS += \
libcommon.a

TEST_BUILDTARGETS += \
common_tests.elf

# globally exclude files from coverage report
GCOVR_EXCLUDE += $(common_PROJECT_ROOT)/test-src
GCOVR_EXCLUDE += $(common_PROJECT_ROOT)/test-extern

#######################################################################################################################
# Settings for build target libcommon.a

libcommon.a_INCLUDES +=          \
$(NETCONFD_SHARED_INCLUDES)      \
-I$(common_PROJECT_ROOT)/extern  \
-I$(common_PROJECT_ROOT)/src     \
-I$(PROJECT_ROOT)/utility/extern 


libcommon.a_DISABLEDWARNINGS +=
libcommon.a_CXXDISABLEDWARNINGS += $(libcommon.a_DISABLEDWARNINGS) abi-tag
libcommon.a_CDISABLEDWARNINGS += $(libcommon.a_DISABLEDWARNINGS)
libcommon.a_DEFINES +=
libcommon.a_LIBS += boost_system
libcommon.a_PKG_CONFIGS =
libcommon.a_CPPFLAGS += $(call uniq, $(libcommon.a_INCLUDES))
libcommon.a_CPPFLAGS += $(call pkg_config_cppflags,$(libcommon.a_PKG_CONFIGS))
libcommon.a_CCXXFLAGS += $(OPTION_HIDE_SYMBOLS)
libcommon.a_CCXXFLAGS += $(OPTION_PIC)
libcommon.a_CFLAGS += $(call pkg_config_cflags,$(libcommon.a_PKG_CONFIGS))
libcommon.a_CFLAGS += $(call option_std,c99)
libcommon.a_CFLAGS += $(call option_disable_warning,$(libcommon.a_CDISABLEDWARNINGS))
libcommon.a_CFLAGS += $(libcommon.a_CCXXFLAGS)
libcommon.a_CXXFLAGS += $(call pkg_config_cxxflags,$(libcommon.a_PKG_CONFIGS))
libcommon.a_CXXFLAGS += $(call option_std,c++14)
libcommon.a_CXXFLAGS += $(call option_disable_warning,$(libcommon.a_CXXDISABLEDWARNINGS))
libcommon.a_CXXFLAGS += $(libcommon.a_CCXXFLAGS)
libcommon.a_SOURCES += $(call fglob_r,$(common_PROJECT_ROOT)/src,$(SOURCE_FILE_EXTENSIONS))
libcommon.a_SOURCES += $(netonfd_common_sources)
libcommon.a_CLANG_TIDY_RULESET = $(CLANG_TIDY_CHECKS)
libcommon.a_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-compat
libcommon.a_CLANG_TIDY_CHECKS += -google-runtime-references




#######################################################################################################################
# Settings for build target common_tests.elf

common_tests.elf_INCLUDES = \
$(libcommon.a_INCLUDES) \
-I$(PROJECT_ROOT)/clientapi/extern \
-I$(common_PROJECT_ROOT)/test-extern \
-I$(common_PROJECT_ROOT)/src \


common_tests.elf_STATICALLYLINKED += common gmock_main gmock gtest
common_tests.elf_LIBS += common gmock_main gmock gtest $(libcommon.a_LIBS)
common_tests.elf_PKG_CONFIGS += $(libcommon.a_PKG_CONFIGS)
common_tests.elf_PKG_CONFIG_LIBS += $(libcommon.a_PKG_CONFIG_LIBS)
common_tests.elf_DISABLEDWARNINGS += packed
common_tests.elf_CDISABLEDWARNINGS += $(common_tests.elf_DISABLEDWARNINGS)
common_tests.elf_CXXDISABLEDWARNINGS += $(common_tests.elf_DISABLEDWARNINGS) useless-cast suggest-override abi-tag
common_tests.elf_PREREQUISITES += $(call lib_buildtarget_raw,$(common_tests.elf_LIBS) $(common_tests.elf_PKG_CONFIG_LIBS),$(common_tests.elf_STATICALLYLINKED))
common_tests.elf_CPPFLAGS += $(call uniq, $(common_tests.elf_INCLUDES))
common_tests.elf_CPPFLAGS += $(call uniq, $(libcommon.a_DEFINES))
common_tests.elf_CPPFLAGS += $(call pkg_config_cppflags,$(common_tests.elf_PKG_CONFIGS))
common_tests.elf_CFLAGS += $(call option_std,gnu99)
common_tests.elf_CFLAGS += $(call option_disable_warning,$(common_tests.elf_CDISABLEDWARNINGS))
common_tests.elf_CXXFLAGS += $(call option_std,gnu++14)
common_tests.elf_CXXFLAGS += $(call option_disable_warning,$(common_tests.elf_CXXDISABLEDWARNINGS))
common_tests.elf_LDFLAGS += $(call option_lib,$(common_tests.elf_LIBS),common_tests.elf)
common_tests.elf_LDFLAGS += $(call pkg_config_ldflags,$(common_tests.elf_PKG_CONFIGS))
common_tests.elf_SOURCES += $(call fglob_r,$(common_PROJECT_ROOT)/test-src,$(SOURCE_FILE_EXTENSIONS))
common_tests.elf_DISABLE_CLANG_TIDY = T
common_tests.elf_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-compat
common_tests.elf_CLANG_TIDY_CHECKS += -google-runtime-references
# filter only source files in this sub-module
common_tests.elf_GCOVR_FILTER += $(common_PROJECT_ROOT)
# modules to include into this test's coverage report 
common_tests.elf_GCOVR_SEARCH_PATH += libcommon.a

#######################################################################################################################
# Custom install rules

