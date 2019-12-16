
#######################################################################################################################
# Build target configuration

INCLUDES += \
$(libbridge.a_INCLUDES)

MAIN_BUILDTARGETS += \
libbridge.a 


libbridge_PROJECT_ROOT = $(PROJECT_ROOT)/libbridge

#######################################################################################################################
# Settings for build target libbridge.a

libbridge.a_INCLUDES += \
-I$(libbridge_PROJECT_ROOT)/extern/ \
-I$(libbridge_PROJECT_ROOT)/src/


libbridge.a_DISABLEDWARNINGS += packed sign-conversion conversion format vla stack-protector missing-prototypes missing-declarations
libbridge.a_CXXDISABLEDWARNINGS += $(libbridge.a_DISABLEDWARNINGS) useless-cast abi-tag
libbridge.a_CDISABLEDWARNINGS += $(libbridge.a_DISABLEDWARNINGS)
libbridge.a_DEFINES +=
libbridge.a_LIBS +=
libbridge.a_CPPFLAGS += $(call pkg_config_cppflags,$(libbridge.a_PKG_CONFIGS))
libbridge.a_CPPFLAGS += $(libbridge.a_INCLUDES)
libbridge.a_CCXXFLAGS += $(OPTION_HIDE_SYMBOLS)
libbridge.a_CCXXFLAGS += $(OPTION_PIC)
libbridge.a_CFLAGS += $(call pkg_config_cflags,$(libbridge.a_PKG_CONFIGS))
libbridge.a_CFLAGS += $(call option_std,c99)
libbridge.a_CFLAGS += $(call option_disable_warning,$(libbridge.a_CDISABLEDWARNINGS))
libbridge.a_CFLAGS += $(libbridge.a_CCXXFLAGS)
libbridge.a_CXXFLAGS += $(call pkg_config_cxxflags,$(libbridge.a_PKG_CONFIGS))
libbridge.a_CXXFLAGS += $(call option_std,c++14)
libbridge.a_CXXFLAGS += $(call option_disable_warning,$(libbridge.a_CXXDISABLEDWARNINGS))
libbridge.a_CXXFLAGS += $(libbridge.a_CCXXFLAGS)
libbridge.a_SOURCES += $(call fglob_r,$(libbridge_PROJECT_ROOT)/src,$(SOURCE_FILE_EXTENSIONS))
libbridge.a_DISABLE_CLANG_TIDY = T



