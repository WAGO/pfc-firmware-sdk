#######################################################################################################################
# Build target configuration

INCLUDES += \
$(libnetconfddbus.a_INCLUDES)

MAIN_BUILDTARGETS += \
libnetconfddbus.a 


libnetconfddbus_PROJECT_ROOT = $(PROJECT_ROOT)/dbus

#######################################################################################################################
# Settings for build target libnetconfddbus.a

libnetconfddbus.a_INCLUDES += \
-I$(libnetconfddbus_PROJECT_ROOT)/extern/ \
-I$(libnetconfddbus_PROJECT_ROOT)/src/ \
-isystem$(libnetconfddbus_PROJECT_ROOT)/generated/

libnetconfddbus.a_DISABLEDWARNINGS += packed sign-conversion conversion format vla stack-protector  missing-declarations cast-qual unused-parameter suggest-attribute=const
libnetconfddbus.a_CXXDISABLEDWARNINGS += $(libnetconfddbus.a_DISABLEDWARNINGS) useless-cast abi-tag
libnetconfddbus.a_CDISABLEDWARNINGS += $(libnetconfddbus.a_DISABLEDWARNINGS) missing-prototypes bad-function-cast
libnetconfddbus.a_PREREQUISITES +=
libnetconfddbus.a_DEFINES +=
libnetconfddbus.a_LIBS +=
libnetconfddbus.a_PKG_CONFIGS += dbus-glib-1 gio-unix-2.0
libnetconfddbus.a_CPPFLAGS += $(call pkg_config_cppflags,$(libnetconfddbus.a_PKG_CONFIGS))
libnetconfddbus.a_CPPFLAGS += $(libnetconfddbus.a_INCLUDES)
libnetconfddbus.a_CPPFLAGS += $(libnetconfddbus.a_INCLUDES_INTERNAL)
libnetconfddbus.a_CCXXFLAGS += $(OPTION_HIDE_SYMBOLS)
libnetconfddbus.a_CCXXFLAGS += $(OPTION_PIC)
libnetconfddbus.a_CFLAGS += $(call pkg_config_cflags,$(libnetconfddbus.a_PKG_CONFIGS))
libnetconfddbus.a_CFLAGS += $(call option_std,c99)
libnetconfddbus.a_CFLAGS += $(call option_disable_warning,$(libnetconfddbus.a_CDISABLEDWARNINGS))
libnetconfddbus.a_CFLAGS += $(libnetconfddbus.a_CCXXFLAGS)
libnetconfddbus.a_CXXFLAGS += $(call pkg_config_cxxflags,$(libnetconfddbus.a_PKG_CONFIGS))
libnetconfddbus.a_CXXFLAGS += $(call option_std,c++14)
libnetconfddbus.a_CXXFLAGS += $(call option_disable_warning,$(libnetconfddbus.a_CXXDISABLEDWARNINGS))
libnetconfddbus.a_CXXFLAGS += $(libnetconfddbus.a_CCXXFLAGS)
libnetconfddbus.a_SOURCES += $(call fglob_r,$(libnetconfddbus_PROJECT_ROOT)/src,$(SOURCE_FILE_EXTENSIONS))
libnetconfddbus.a_SOURCES += $(call fglob_r,$(libnetconfddbus_PROJECT_ROOT)/generated,$(SOURCE_FILE_EXTENSIONS))
libnetconfddbus.a_DISABLE_CLANG_TIDY = T


GDBUS_CODEGEN=$(PTXCONF_SYSROOT_HOST)/bin/gdbus-codegen
GDBUS_CODEGEN_OUTDIR=$(libnetconfddbus_PROJECT_ROOT)/generated
GDBUS_CODEGEN_OPTIONS= \
--interface-prefix de.wago.netconfd1 \
--generate-c-code netconfd_dbus \
--c-namespace netconfd \
--c-generate-object-manager \
--output-directory $(GDBUS_CODEGEN_OUTDIR)  


.PHONY: gdbus_generate
gdbus_generate: $(shell find $(libnetconfddbus_PROJECT_ROOT) -name *.xml) | $(GDBUS_CODEGEN_OUTDIR)
	@echo gdbus-codegen $(GDBUS_CODEGEN_OPTIONS) $<
	@rm -f $(GDBUS_CODEGEN_OUTDIR)/*
	@$(GDBUS_CODEGEN) $(GDBUS_CODEGEN_OPTIONS) $<


$(GDBUS_CODEGEN_OUTDIR):
	@mkdir -p $@