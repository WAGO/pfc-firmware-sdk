
#######################################################################################################################
# Build target configuration

INCLUDES += \
$(libbal.a_INCLUDES)

MAIN_BUILDTARGETS += \
libbal.a 

TEST_BUILDTARGETS += \
bal_tests.elf

libbal_PROJECT_ROOT = $(PROJECT_ROOT)/bal

# globally exclude files from coverage report
GCOVR_EXCLUDE += $(libbal_PROJECT_ROOT)/test-src
GCOVR_EXCLUDE += $(libbal_PROJECT_ROOT)/test-extern

#######################################################################################################################
# Settings for build target libbal.a

libbal.a_INCLUDES +=\
  $(NETCONFD_SHARED_INCLUDES) \
-I$(PROJECT_ROOT)/utility/extern \
-I$(PROJECT_ROOT)/netconfd/extern \
-I$(libbal_PROJECT_ROOT)/extern


libbal.a_DISABLEDWARNINGS += packed 
libbal.a_CXXDISABLEDWARNINGS += $(libbal.a_DISABLEDWARNINGS) useless-cast abi-tag
libbal.a_CDISABLEDWARNINGS += $(libbal.a_DISABLEDWARNINGS)
libbal.a_DEFINES +=
libbal.a_PKG_CONFIGS += libbridge
libbal.a_LIBS += utility bridge
libbal.a_CPPFLAGS += $(call uniq, $(libbal.a_INCLUDES) $(libbridge.a_INCLUDES)) 
libbal.a_CPPFLAGS += $(call uniq, $(libbridge.a_DEFINES))
libbal.a_CPPFLAGS += $(call pkg_config_cppflags,$(libbal.a_PKG_CONFIGS))
libbal.a_CCXXFLAGS += $(OPTION_HIDE_SYMBOLS)
libbal.a_CCXXFLAGS += $(OPTION_PIC)
libbal.a_CFLAGS += $(call pkg_config_cflags,$(libbal.a_PKG_CONFIGS))
libbal.a_CFLAGS += $(call option_std,c99)
libbal.a_CFLAGS += $(call option_disable_warning,$(libbal.a_CDISABLEDWARNINGS))
libbal.a_CFLAGS += $(libbal.a_CCXXFLAGS)
libbal.a_CXXFLAGS += $(call pkg_config_cxxflags,$(libbal.a_PKG_CONFIGS))
libbal.a_CXXFLAGS += $(call option_std,c++17)
libbal.a_CXXFLAGS += $(call option_disable_warning,$(libbal.a_CXXDISABLEDWARNINGS))
libbal.a_CXXFLAGS += $(libbal.a_CCXXFLAGS)
libbal.a_SOURCES += $(call fglob_r,$(libbal_PROJECT_ROOT)/src,$(SOURCE_FILE_EXTENSIONS))
libbal.a_CLANG_TIDY_RULESET = $(CLANG_TIDY_CHECKS)
libbal.a_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-compat
libbal.a_CLANG_TIDY_CHECKS += -google-runtime-references



#######################################################################################################################
# Settings for build target bal_tests.elf

bal_tests.elf_INCLUDES = \
$(libbal.a_INCLUDES) \
-I$(libbal_PROJECT_ROOT)/test-extern/ \
-I$(libbal_PROJECT_ROOT)/test-extern/mocks/ \
-I$(PROJECT_ROOT)/netconfd/test-extern/mocks/ \
-I$(PROJECT_ROOT)/utility/extern

bal_tests.elf_STATICALLYLINKED += bal utility gmock_main gmock gtest 
bal_tests.elf_LIBS += bal utility gmock_main gmock gtest boost_filesystem
bal_tests.elf_PKG_CONFIGS += $(libbal.a_PKG_CONFIGS)
bal_tests.elf_PKG_CONFIG_LIBS += $(libbal.a_PKG_CONFIG_LIBS)
bal_tests.elf_DISABLEDWARNINGS += packed 
bal_tests.elf_CDISABLEDWARNINGS += $(bal_tests.elf_DISABLEDWARNINGS)
bal_tests.elf_CXXDISABLEDWARNINGS += $(bal_tests.elf_DISABLEDWARNINGS) useless-cast suggest-override abi-tag
bal_tests.elf_PREREQUISITES += $(call lib_buildtarget_raw,$(bal_tests.elf_LIBS) $(bal_tests.elf_PKG_CONFIG_LIBS),$(bal_tests.elf_STATICALLYLINKED))
bal_tests.elf_CPPFLAGS += $(call uniq, $(bal_tests.elf_INCLUDES))
bal_tests.elf_CPPFLAGS += $(call uniq, $(libbal.a_DEFINES))
bal_tests.elf_CPPFLAGS += $(call pkg_config_cppflags,$(bal_tests.elf_PKG_CONFIGS))
bal_tests.elf_CFLAGS += $(call option_std,gnu99)
bal_tests.elf_CFLAGS += $(call option_disable_warning,$(bal_tests.elf_CDISABLEDWARNINGS))
bal_tests.elf_CXXFLAGS += $(call option_std,gnu++17) 
bal_tests.elf_CXXFLAGS += $(call option_disable_warning,$(bal_tests.elf_CXXDISABLEDWARNINGS))
bal_tests.elf_LDFLAGS += $(call option_lib,$(bal_tests.elf_LIBS),bal_tests.elf)
bal_tests.elf_LDFLAGS += $(call pkg_config_ldflags,$(bal_tests.elf_PKG_CONFIGS))
bal_tests.elf_SOURCES += $(call fglob_r,$(libbal_PROJECT_ROOT)/test-src,$(SOURCE_FILE_EXTENSIONS))
bal_tests.elf_DISABLE_CLANG_TIDY = T
bal_tests.a_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-compat
bal_tests.a_CLANG_TIDY_CHECKS += -google-runtime-references
# filter only source files in this sub-module
bal_tests.elf_GCOVR_FILTER += $(libbal_PROJECT_ROOT)
# modules to include into this test's coverage report 
bal_tests.elf_GCOVR_SEARCH_PATH += libbal.a
