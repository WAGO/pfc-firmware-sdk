
#######################################################################################################################
# Build target configuration

INCLUDES += \
$(libutility.a_INCLUDES)

MAIN_BUILDTARGETS += \
libutility.a

TEST_BUILDTARGETS += \
utility_tests.elf 

libutility_PROJECT_ROOT = $(PROJECT_ROOT)/modules/utility

# globally exclude files from coverage report
GCOVR_EXCLUDE += $(libutility_PROJECT_ROOT)/test-src
GCOVR_EXCLUDE += $(libutility_PROJECT_ROOT)/test-extern

#######################################################################################################################
# Settings for build target libutility.a

libutility.a_INCLUDES += \
$(NETCONFD_SHARED_INCLUDES)            \
-I$(libutility_PROJECT_ROOT)/extern   \
-I$(PROJECT_ROOT)/modules/netconfd/src/Logger 

libutility.a_VERSION := $(NETCONFD_VERSION)
libutility.a_DISABLEDWARNINGS += packed 
libutility.a_CXXDISABLEDWARNINGS += $(libutility.a_DISABLEDWARNINGS) useless-cast abi-tag
libutility.a_CDISABLEDWARNINGS += $(libutility.a_DISABLEDWARNINGS)
libutility.a_DEFINES +=
libutility.a_LIBS += common boost_system boost_filesystem
libutility.a_PKG_CONFIGS = libnl-route-3.0 libnl-3.0 libnl-genl-3.0 gio-unix-2.0 glib-2.0
libutility.a_CPPFLAGS += $(call uniq, $(libutility.a_INCLUDES) $(libbridge.a_INCLUDES)) 
libutility.a_CPPFLAGS += $(call uniq, $(libbridge.a_DEFINES))
libutility.a_CPPFLAGS += $(call pkg_config_cppflags,$(libutility.a_PKG_CONFIGS))
libutility.a_CCXXFLAGS += $(OPTION_HIDE_SYMBOLS)
libutility.a_CCXXFLAGS += $(OPTION_PIC)
libutility.a_CFLAGS += $(call pkg_config_cflags,$(libutility.a_PKG_CONFIGS))
libutility.a_CFLAGS += $(call option_std,c99)
libutility.a_CFLAGS += $(call option_disable_warning,$(libutility.a_CDISABLEDWARNINGS))
libutility.a_CFLAGS += $(libutility.a_CCXXFLAGS)
libutility.a_CXXFLAGS += $(call pkg_config_cxxflags,$(libutility.a_PKG_CONFIGS))
libutility.a_CXXFLAGS += $(call option_std,c++17)
libutility.a_CXXFLAGS += $(call option_disable_warning,$(libutility.a_CXXDISABLEDWARNINGS))
libutility.a_CXXFLAGS += $(libutility.a_CCXXFLAGS)
libutility.a_SOURCES += $(call fglob_r,$(libutility_PROJECT_ROOT)/src,$(SOURCE_FILE_EXTENSIONS))
libutility.a_CLANG_TIDY_RULESET = $(CLANG_TIDY_CHECKS)
libutility.a_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-compat
libutility.a_CLANG_TIDY_CHECKS += -google-runtime-references

#######################################################################################################################
# Settings for build target utility_tests.elf

utility_tests.elf_INCLUDES = \
$(libutility.a_INCLUDES) \
-I$(PROJECT_ROOT)/modules/utility/extern \
-I$(PROJECT_ROOT)/modules/utility/test-extern/mocks/ \
-I$(libutility_PROJECT_ROOT)/test-extern/ \
-I$(libutility_PROJECT_ROOT)/test-extern/mocks/ \
-I$(libutility_PROJECT_ROOT)/src/ \


utility_tests.elf_STATICALLYLINKED += utility gmock_main gmock gtest 
utility_tests.elf_LIBS += utility gmock_main gmock gtest $(libutility.a_LIBS)
utility_tests.elf_PKG_CONFIGS += $(libutility.a_PKG_CONFIGS)
utility_tests.elf_PKG_CONFIG_LIBS += $(libutility.a_PKG_CONFIG_LIBS)
utility_tests.elf_DISABLEDWARNINGS += packed 
utility_tests.elf_CDISABLEDWARNINGS += $(utility_tests.elf_DISABLEDWARNINGS)
utility_tests.elf_CXXDISABLEDWARNINGS += $(utility_tests.elf_DISABLEDWARNINGS) useless-cast suggest-override abi-tag
utility_tests.elf_PREREQUISITES += $(call lib_buildtarget_raw,$(utility_tests.elf_LIBS) $(utility_tests.elf_PKG_CONFIG_LIBS),$(utility_tests.elf_STATICALLYLINKED))
utility_tests.elf_CPPFLAGS += $(call uniq, $(utility_tests.elf_INCLUDES))
utility_tests.elf_CPPFLAGS += $(call uniq, $(libutility.a_DEFINES))
utility_tests.elf_CPPFLAGS += $(call pkg_config_cppflags,$(utility_tests.elf_PKG_CONFIGS))
utility_tests.elf_CFLAGS += $(call option_std,gnu99)
utility_tests.elf_CFLAGS += $(call option_disable_warning,$(utility_tests.elf_CDISABLEDWARNINGS))
utility_tests.elf_CXXFLAGS += $(call option_std,gnu++17) 
utility_tests.elf_CXXFLAGS += $(call option_disable_warning,$(utility_tests.elf_CXXDISABLEDWARNINGS))
utility_tests.elf_LDFLAGS += $(call option_lib,$(utility_tests.elf_LIBS),utility_tests.elf)
utility_tests.elf_LDFLAGS += $(call pkg_config_ldflags,$(utility_tests.elf_PKG_CONFIGS))
utility_tests.elf_SOURCES += $(call fglob_r,$(libutility_PROJECT_ROOT)/test-src,$(SOURCE_FILE_EXTENSIONS))
utility_tests.elf_DISABLE_CLANG_TIDY = T
utility_tests.elf_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-compat
utility_tests.elf_CLANG_TIDY_CHECKS += -google-runtime-references
# filter only source files in this sub-module
utility_tests.elf_GCOVR_FILTER += $(libutility_PROJECT_ROOT)
# modules to include into this test's coverage report 
utility_tests.elf_GCOVR_SEARCH_PATH += libutility.a
