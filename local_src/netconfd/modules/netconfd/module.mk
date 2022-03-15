
#######################################################################################################################
# Build target configuration

INCLUDES += \
$(libnetconfd.a_INCLUDES)

MAIN_BUILDTARGETS += \
libnetconfd.a \
netconfd.elf

TEST_BUILDTARGETS += \
netconfd_tests.elf

INSTALL_TARGETS += \
$(DESTDIR)/usr/bin/netconfd.elf

libnetconfd_PROJECT_ROOT = $(PROJECT_ROOT)/modules/netconfd

# globally exclude files from coverage report
GCOVR_EXCLUDE += $(libnetconfd_PROJECT_ROOT)/test-src
GCOVR_EXCLUDE += $(libnetconfd_PROJECT_ROOT)/test-extern

#######################################################################################################################
# Settings for build target libnetconfd.a

libnetconfd.a_INCLUDES += \
$(NETCONFD_SHARED_INCLUDES)	\
-I$(libnetconfd_PROJECT_ROOT)/extern \
-I$(libnetconfd_PROJECT_ROOT)/src \
-I$(libnetconfd_PROJECT_ROOT)/src/InterfaceManager \
-I$(libnetconfd_PROJECT_ROOT)/src/DevicePropertiesProvider \
-I$(libnetconfd_PROJECT_ROOT)/src/PersistenceProvider \
-I$(libnetconfd_PROJECT_ROOT)/src/Logger \
-I$(libnetconfd_PROJECT_ROOT)/src/EventManager \
-I$(libnetconfd_PROJECT_ROOT)/src/IPManager \
-I$(libnetconfd_PROJECT_ROOT)/src/IPManager/DynamicIPClient \
-I$(libnetconfd_PROJECT_ROOT)/src/IPManager/GratuitousArp \
-I$(libnetconfd_PROJECT_ROOT)/src/NetDevs \
-I$(libnetconfd_PROJECT_ROOT)/src/Hostname \
-I$(libnetconfd_PROJECT_ROOT)/src/MacDistribution/ \


libnetconfd.a_DISABLEDWARNINGS += packed 
libnetconfd.a_CXXDISABLEDWARNINGS += $(libnetconfd.a_DISABLEDWARNINGS) useless-cast abi-tag
libnetconfd.a_CDISABLEDWARNINGS += $(libnetconfd.a_DISABLEDWARNINGS)
libnetconfd.a_DEFINES += -DBOOST_LOG_DYN_LINK
libnetconfd.a_STATICALLYLINKED += bridge
libnetconfd.a_LIBS += boost_log boost_thread boost_system boost_serialization boost_interprocess rt
libnetconfd.a_PKG_CONFIGS += glib-2.0 libbridge
libnetconfd.a_PREREQUISITES += $(call lib_buildtarget_raw,$(netconfd.a_LIBS) $(netconfd.a_PKG_CONFIG_LIBS),$(netconfd.a_STATICALLYLINKED))
libnetconfd.a_CPPFLAGS += $(call uniq, $(libnetconfd.a_INCLUDES) $(libutility.a_INCLUDES) $(libnetconfddbus.a_INCLUDES)) 
libnetconfd.a_CPPFLAGS += $(call uniq, $(libnetconfd.a_DEFINES)  $(libutility.a_DEFINES))
libnetconfd.a_CPPFLAGS += $(call pkg_config_cppflags,$(libnetconfd.a_PKG_CONFIGS) $(libnetconfddbus.a_PKG_CONFIGS))
libnetconfd.a_CCXXFLAGS += $(OPTION_HIDE_SYMBOLS)
libnetconfd.a_CCXXFLAGS += $(OPTION_PIC)
libnetconfd.a_CFLAGS += $(call pkg_config_cflags,$(libnetconfd.a_PKG_CONFIGS))
libnetconfd.a_CFLAGS += $(call option_std,c99)
libnetconfd.a_CFLAGS += $(call option_disable_warning,$(libnetconfd.a_CDISABLEDWARNINGS))
libnetconfd.a_CFLAGS += $(libnetconfd.a_CCXXFLAGS)
libnetconfd.a_CXXFLAGS += $(call pkg_config_cxxflags,$(libnetconfd.a_PKG_CONFIGS))
libnetconfd.a_CXXFLAGS += $(call option_std,c++17)
libnetconfd.a_CXXFLAGS += $(call option_disable_warning,$(libnetconfd.a_CXXDISABLEDWARNINGS))
libnetconfd.a_CXXFLAGS += $(libnetconfd.a_CCXXFLAGS)
libnetconfd.a_SOURCES += $(call fglob_r,$(libnetconfd_PROJECT_ROOT)/src,$(SOURCE_FILE_EXTENSIONS))
libnetconfd.a_SOURCES += $(netonfd_common_sources)
libnetconfd.a_CLANG_TIDY_RULESET = $(CLANG_TIDY_CHECKS)
libnetconfd.a_CLANG_TIDY_CHECKS += $(SHARED_CLANG_TIDY_CHECKS)


#######################################################################################################################
# Settings for build target netconfd

netconfd.elf_INCLUDES =           \
  $(NETCONFD_SHARED_INCLUDES)     \
-I$(PROJECT_ROOT)/modules/main    \
-I$(PROJECT_ROOT)/modules/utility/extern \
-I$(PROJECT_ROOT)/modules/netconfd/extern

netconfd.elf_VERSION = $(NETCONFD_VERSION)
netconfd.elf_DISABLEDWARNINGS += packed
netconfd.elf_CDISABLEDWARNINGS += $(netconfd.so_DISABLEDWARNINGS)  
netconfd.elf_CXXDISABLEDWARNINGS += $(netconfd.so_DISABLEDWARNINGS) abi-tag useless-cast
netconfd.elf_STATICALLYLINKED += netconfd common utility netconfddbus
netconfd.elf_LIBS += netconfd common boost_log utility boost_thread boost_system boost_filesystem boost_serialization netconfddbus rt
netconfd.elf_PKG_CONFIGS += $(call uniq, $(libnetconfd.a_PKG_CONFIGS) $(libnetconfddbus.a_PKG_CONFIGS) $(libutility.a_PKG_CONFIGS))
netconfd.elf_PKG_CONFIG_LIBS += $(call uniq, $(libnetconfd.a_PKG_CONFIGS_LIBS))
netconfd.elf_PREREQUISITES += $(call lib_buildtarget_raw, $(netconfd.elf_LIBS) $(netconfd.elf_PKG_CONFIG_LIBS),$(netconfd.elf_STATICALLYLINKED))
netconfd.elf_INCLUDES += $(call uniq,$(libnetconfd.a_INCLUDES) $(libbal.a_INCLUDES) $(libbridge.a_INCLUDES) $(libnetconfddbus.a_INCLUDES))
netconfd.elf_DEFINES += $(call uniq,$(libbal.a_DEFINES)) -DBOOST_LOG_DYN_LINK -DNETCONFD_VERSION=$(netconfd.elf_VERSION)
netconfd.elf_CPPFLAGS += $(netconfd.elf_INCLUDES)
netconfd.elf_CPPFLAGS += $(netconfd.elf_DEFINES)
netconfd.elf_CPPFLAGS += $(call pkg_config_cppflags,$(netconfd.elf_PKG_CONFIGS))
netconfd.elf_CCXXFLAGS += $(OPTION_HIDE_SYMBOLS)
netconfd.elf_CCXXFLAGS += $(OPTION_PIC)
netconfd.elf_CCXXFLAGS += $(netconfd.elf_DEFINES)
netconfd.elf_CXXFLAGS += $(call option_disable_warning,$(netconfd.elf_CXXDISABLEDWARNINGS))
netconfd.elf_CXXFLAGS += $(call pkg_config_cxxflags,$(netconfd.elf_PKG_CONFIGS))
netconfd.elf_CXXFLAGS += $(netconfd.elf_CCXXFLAGS)
netconfd.elf_CFLAGS += $(netconfd.elf_CCXXFLAGS)
netconfd.elf_CFLAGS += $(call option_disable_warning,$(netconfd.elf_CDISABLEDWARNINGS))
netconfd.elf_LDFLAGS += $(OPTION_PIC)
netconfd.elf_LDFLAGS += -Wl,--no-undefined
netconfd.elf_LDFLAGS += -Wl,--exclude-libs=ALL
netconfd.elf_LDFLAGS += $(call option_lib,$(netconfd.elf_LIBS),netconfd.elf)
netconfd.elf_LDFLAGS += $(call pkg_config_ldflags,$(netconfd.elf_PKG_CONFIGS))
netconfd.elf_SOURCES += $(call fglob_r,$(libnetconfd_PROJECT_ROOT)/main,$(SOURCE_FILE_EXTENSIONS))
netconfd.elf_CLANG_TIDY_RULESET = $(CLANG_TIDY_CHECKS)
netconfd.elf_CLANG_TIDY_CHECKS += $(SHARED_CLANG_TIDY_CHECKS)


#######################################################################################################################
# Settings for build target netconfd_tests.elf

netconfd_tests.elf_INCLUDES = \
$(libnetconfd.a_INCLUDES) \
-I$(PROJECT_ROOT)/modules/utility/extern \
-I$(PROJECT_ROOT)/modules/utility/test-extern/mocks \
-I$(PROJECT_ROOT)/modules/common/test-extern/ \
-I$(libnetconfd_PROJECT_ROOT)/test-extern \
-I$(libnetconfd_PROJECT_ROOT)/test-extern/mocks \
-I$(libnetconfd_PROJECT_ROOT)/src \

netconfd_tests.elf_STATICALLYLINKED += netconfd common utility gmock_main gmock gtest 
netconfd_tests.elf_LIBS += netconfd common bridge utility gmock_main gmock gtest boost_log boost_thread boost_system boost_filesystem boost_serialization
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
netconfd_tests.elf_CXXFLAGS += $(call option_std,gnu++17) 
netconfd_tests.elf_CXXFLAGS += $(call option_disable_warning,$(netconfd_tests.elf_CXXDISABLEDWARNINGS))
netconfd_tests.elf_LDFLAGS += $(call option_lib,$(netconfd_tests.elf_LIBS),netconfd_tests.elf)
netconfd_tests.elf_LDFLAGS += $(call pkg_config_ldflags,$(netconfd_tests.elf_PKG_CONFIGS))
netconfd_tests.elf_SOURCES += $(call fglob_r,$(libnetconfd_PROJECT_ROOT)/test-src,$(SOURCE_FILE_EXTENSIONS))
netconfd_tests.elf_DISABLE_CLANG_TIDY = T
# filter only source files in this sub-module
netconfd_tests.elf_GCOVR_FILTER += $(libnetconfd_PROJECT_ROOT)
# modules to include into this test's coverage report 
netconfd_tests.elf_GCOVR_SEARCH_PATH += libnetconfd.a

