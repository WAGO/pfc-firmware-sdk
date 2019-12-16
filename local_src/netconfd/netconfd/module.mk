
#######################################################################################################################
# Build target configuration

INCLUDES += \
$(libnetconfd.a_INCLUDES)

MAIN_BUILDTARGETS += \
libnetconfd.a 

TEST_BUILDTARGETS += \
netconfd_tests.elf

libnetconfd_PROJECT_ROOT = $(PROJECT_ROOT)/netconfd

#######################################################################################################################
# Settings for build target libnetconfd.a

libnetconfd.a_INCLUDES += \
-I$(PROJECT_ROOT)/extern/ \
-I$(PROJECT_ROOT)/bal/extern/ \
-isystem$(PROJECT_ROOT)/gsl/include/ \
-I$(libnetconfd_PROJECT_ROOT)/extern/ \
-I$(libnetconfd_PROJECT_ROOT)/src/ \
-I$(libnetconfd_PROJECT_ROOT)/src/InterfaceManager/ \
-I$(libnetconfd_PROJECT_ROOT)/src/DevicePropertiesProvider/ \
-I$(libnetconfd_PROJECT_ROOT)/src/PersistenceProvider/ \
-I$(libnetconfd_PROJECT_ROOT)/src/JsonConfigConverter/ \
-I$(libnetconfd_PROJECT_ROOT)/src/Logger/ \
-I$(libnetconfd_PROJECT_ROOT)/src/EventManager/ \
-I$(libnetconfd_PROJECT_ROOT)/src/IPManager/ \

libnetconfd.a_DISABLEDWARNINGS += packed 
libnetconfd.a_CXXDISABLEDWARNINGS += $(libnetconfd.a_DISABLEDWARNINGS) useless-cast abi-tag
libnetconfd.a_CDISABLEDWARNINGS += $(libnetconfd.a_DISABLEDWARNINGS)
libnetconfd.a_DEFINES += -DBOOST_LOG_DYN_LINK
libnetconfd.a_STATICALLYLINKED += bal 
libnetconfd.a_LIBS += bal boost_log boost_thread boost_system boost_serialization
libnetconfd.a_PKG_CONFIGS += glib-2.0
libnetconfd.a_PREREQUISITES += $(call lib_buildtarget_raw,$(netconfd.a_LIBS) $(netconfd.a_PKG_CONFIG_LIBS),$(netconfd.a_STATICALLYLINKED))
libnetconfd.a_CPPFLAGS += $(call uniq, $(libnetconfd.a_INCLUDES) $(libbal.a_INCLUDES) $(libnetconfddbus.a_INCLUDES)) 
libnetconfd.a_CPPFLAGS += $(call uniq, $(libnetconfd.a_DEFINES)  $(libbal.a_DEFINES))
libnetconfd.a_CPPFLAGS += $(call pkg_config_cppflags,$(libnetconfd.a_PKG_CONFIGS) $(libnetconfddbus.a_PKG_CONFIGS))
libnetconfd.a_CCXXFLAGS += $(OPTION_HIDE_SYMBOLS)
libnetconfd.a_CCXXFLAGS += $(OPTION_PIC)
libnetconfd.a_CFLAGS += $(call pkg_config_cflags,$(libnetconfd.a_PKG_CONFIGS))
libnetconfd.a_CFLAGS += $(call option_std,c99)
libnetconfd.a_CFLAGS += $(call option_disable_warning,$(libnetconfd.a_CDISABLEDWARNINGS))
libnetconfd.a_CFLAGS += $(libnetconfd.a_CCXXFLAGS)
libnetconfd.a_CXXFLAGS += $(call pkg_config_cxxflags,$(libnetconfd.a_PKG_CONFIGS))
libnetconfd.a_CXXFLAGS += $(call option_std,c++14)
libnetconfd.a_CXXFLAGS += $(call option_disable_warning,$(libnetconfd.a_CXXDISABLEDWARNINGS))
libnetconfd.a_CXXFLAGS += $(libnetconfd.a_CCXXFLAGS)
libnetconfd.a_SOURCES += $(call fglob_r,$(libnetconfd_PROJECT_ROOT)/src,$(SOURCE_FILE_EXTENSIONS))
libnetconfd.a_CLANG_TIDY_RULESET = $(CLANG_TIDY_CHECKS)
libnetconfd.a_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-compat
libnetconfd.a_CLANG_TIDY_CHECKS += -google-runtime-references





#######################################################################################################################
# Settings for build target netconfd_tests.elf

netconfd_tests.elf_INCLUDES = \
$(libnetconfd.a_INCLUDES) \
-I$(PROJECT_ROOT)/bal/test-extern/mocks/ \
-I$(PROJECT_ROOT)/utility/extern \
-I$(PROJECT_ROOT)/utility/test-extern/mocks/ \
-I$(libnetconfd_PROJECT_ROOT)/test-extern/ \
-I$(libnetconfd_PROJECT_ROOT)/test-extern/mocks/ \
-I$(libnetconfd_PROJECT_ROOT)/src/ \

netconfd_tests.elf_STATICALLYLINKED += netconfd bal utility gmock_main gmock gtest 
netconfd_tests.elf_LIBS += netconfd bal bridge utility gmock_main gmock gtest boost_log boost_thread boost_system boost_filesystem boost_serialization
netconfd_tests.elf_PKG_CONFIGS += $(libnetconfd.a_PKG_CONFIGS)
netconfd_tests.elf_PKG_CONFIG_LIBS += $(libnetconfd.a_PKG_CONFIG_LIBS)
netconfd_tests.elf_DISABLEDWARNINGS += packed 
netconfd_tests.elf_CDISABLEDWARNINGS += $(netconfd_tests.elf_DISABLEDWARNINGS)
netconfd_tests.elf_CXXDISABLEDWARNINGS += $(netconfd_tests.elf_DISABLEDWARNINGS) useless-cast suggest-override abi-tag
netconfd_tests.elf_PREREQUISITES += $(call lib_buildtarget_raw,$(netconfd_tests.elf_LIBS) $(netconfd_tests.elf_PKG_CONFIG_LIBS),$(netconfd_tests.elf_STATICALLYLINKED))
netconfd_tests.elf_CPPFLAGS += $(call uniq, $(netconfd_tests.elf_INCLUDES))
netconfd_tests.elf_CPPFLAGS += $(call uniq, $(libnetconfd.a_DEFINES))
netconfd_tests.elf_CPPFLAGS += $(call pkg_config_cppflags,$(netconfd_tests.elf_PKG_CONFIGS))
netconfd_tests.elf_CFLAGS += $(call option_std,gnu99)
netconfd_tests.elf_CFLAGS += $(call option_disable_warning,$(netconfd_tests.elf_CDISABLEDWARNINGS))
netconfd_tests.elf_CXXFLAGS += $(call option_std,gnu++14) 
netconfd_tests.elf_CXXFLAGS += $(call option_disable_warning,$(netconfd_tests.elf_CXXDISABLEDWARNINGS))
netconfd_tests.elf_LDFLAGS += $(call option_lib,$(netconfd_tests.elf_LIBS),netconfd_tests.elf)
netconfd_tests.elf_LDFLAGS += $(call pkg_config_ldflags,$(netconfd_tests.elf_PKG_CONFIGS))
netconfd_tests.elf_SOURCES += $(call fglob_r,$(libnetconfd_PROJECT_ROOT)/test-src,$(SOURCE_FILE_EXTENSIONS))
netconfd_tests.elf_DISABLE_CLANG_TIDY = T
netconfd_tests.a_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-compat
netconfd_tests.a_CLANG_TIDY_CHECKS += -google-runtime-references



