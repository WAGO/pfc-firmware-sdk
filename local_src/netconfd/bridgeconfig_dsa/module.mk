
INCLUDES += \
$(bridgeconfig_dsa.a_INCLUDES)

MAIN_BUILDTARGETS += bridgeconfig_dsa.elf


INSTALL_TARGETS += \
$(DESTDIR)/usr/bin/bridgeconfig_dsa.elf \


bridgeconfig_dsa_PROJECT_ROOT = $(PROJECT_ROOT)/bridgeconfig_dsa

bridgeconfig_dsa.elf_INCLUDES += \
-I$(PROJECT_ROOT)/utility/extern/ \
-I$(bridgeconfig_dsa_PROJECT_ROOT)/src/ \

bridgeconfig_dsa.elf_VERSION = $(bridgeconfig_dsa_VERSION)
bridgeconfig_dsa.elf_SOURCEDIR += $(PROJECT_ROOT)/bridgeconfig_dsa
bridgeconfig_dsa.elf_DISABLEDWARNINGS += packed
bridgeconfig_dsa.elf_CDISABLEDWARNINGS += $(bridgeconfig_dsa.so_DISABLEDWARNINGS)  
bridgeconfig_dsa.elf_CXXDISABLEDWARNINGS += $(bridgeconfig_dsa.so_DISABLEDWARNINGS) abi-tag useless-cast
bridgeconfig_dsa.elf_STATICALLYLINKED +=
bridgeconfig_dsa.elf_LIBS += netconfd utility boost_system boost_filesystem boost_regex
bridgeconfig_dsa.elf_LIBS += $(libnetconfd.a_LIBS) $(libutility.a_LIBS)
bridgeconfig_dsa.elf_INCLUDES += $(libnetconfd.a_INCLUDES) $(libutility.a_INCLUDES)
bridgeconfig_dsa.elf_PKG_CONFIGS += 
bridgeconfig_dsa.elf_PKG_CONFIG_LIBS += 
bridgeconfig_dsa.elf_DEFINES += 
bridgeconfig_dsa.elf_INCLUDES += $(call uniq,$(libbridgeconfig_dsa.a_INCLUDES))
bridgeconfig_dsa.elf_PREREQUISITES += $(call lib_buildtarget_raw, $(bridgeconfig_dsa.elf_LIBS) $(bridgeconfig_dsa.elf_PKG_CONFIG_LIBS),$(bridgeconfig_dsa.elf_STATICALLYLINKED))
bridgeconfig_dsa.elf_CPPFLAGS += $(bridgeconfig_dsa.elf_INCLUDES)
bridgeconfig_dsa.elf_CPPFLAGS += $(bridgeconfig_dsa.elf_DEFINES)
bridgeconfig_dsa.elf_CPPFLAGS += $(call pkg_config_cppflags,$(bridgeconfig_dsa.elf_PKG_CONFIGS))
bridgeconfig_dsa.elf_CCXXFLAGS += $(OPTION_HIDE_SYMBOLS)
bridgeconfig_dsa.elf_CCXXFLAGS += $(OPTION_PIC)
bridgeconfig_dsa.elf_CCXXFLAGS += $(bridgeconfig_dsa.elf_DEFINES)
bridgeconfig_dsa.elf_CXXFLAGS += $(call option_disable_warning,$(bridgeconfig_dsa.elf_CXXDISABLEDWARNINGS))
bridgeconfig_dsa.elf_CXXFLAGS += $(call pkg_config_cxxflags,$(bridgeconfig_dsa.elf_PKG_CONFIGS))
bridgeconfig_dsa.elf_CXXFLAGS += $(bridgeconfig_dsa.elf_CCXXFLAGS)
bridgeconfig_dsa.elf_CFLAGS += $(bridgeconfig_dsa.elf_CCXXFLAGS)
bridgeconfig_dsa.elf_CFLAGS += $(call option_disable_warning,$(bridgeconfig_dsa.elf_CDISABLEDWARNINGS))
bridgeconfig_dsa.elf_LDFLAGS += $(OPTION_PIC)
bridgeconfig_dsa.elf_LDFLAGS += -Wl,--no-undefined
bridgeconfig_dsa.elf_LDFLAGS += -Wl,--exclude-libs=ALL
bridgeconfig_dsa.elf_LDFLAGS += $(call option_lib,$(bridgeconfig_dsa.elf_LIBS),bridgeconfig_dsa.elf)
bridgeconfig_dsa.elf_LDFLAGS += $(call pkg_config_ldflags,$(bridgeconfig_dsa.elf_PKG_CONFIGS))
bridgeconfig_dsa.elf_SOURCES += $(call fglob_r,$(bridgeconfig_dsa.elf_SOURCEDIR),$(SOURCE_FILE_EXTENSIONS))
bridgeconfig_dsa.elf_CLANG_TIDY_RULESET = $(CLANG_TIDY_CHECKS)
bridgeconfig_dsa.elf_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-compat
bridgeconfig_dsa.elf_CLANG_TIDY_CHECKS += -google-runtime-references


