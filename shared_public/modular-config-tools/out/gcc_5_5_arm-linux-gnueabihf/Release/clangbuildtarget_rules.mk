CPPFLAGS := $(CPPFLAGS)
CFLAGS := $(CFLAGS)
CXXFLAGS := $(CXXFLAGS)
LDFLAGS := $(LDFLAGS)
ARFLAGS := $(ARFLAGS)

libconfigtoolutils.a_SOURCES := $(libconfigtoolutils.a_SOURCES)
libconfigtoolutils.a_GENHEADERS := $(libconfigtoolutils.a_GENHEADERS)
libconfigtoolutils.a_CPPFLAGS := $(libconfigtoolutils.a_CPPFLAGS)
libconfigtoolutils.a_CFLAGS := $(libconfigtoolutils.a_CFLAGS)
libconfigtoolutils.a_CXXFLAGS := $(libconfigtoolutils.a_CXXFLAGS)
libconfigtoolutils.a_LDFLAGS := $(libconfigtoolutils.a_LDFLAGS)
libconfigtoolutils.a_ARFLAGS := $(libconfigtoolutils.a_ARFLAGS)

libconfigtoolfail.a_SOURCES := $(libconfigtoolfail.a_SOURCES)
libconfigtoolfail.a_GENHEADERS := $(libconfigtoolfail.a_GENHEADERS)
libconfigtoolfail.a_CPPFLAGS := $(libconfigtoolfail.a_CPPFLAGS)
libconfigtoolfail.a_CFLAGS := $(libconfigtoolfail.a_CFLAGS)
libconfigtoolfail.a_CXXFLAGS := $(libconfigtoolfail.a_CXXFLAGS)
libconfigtoolfail.a_LDFLAGS := $(libconfigtoolfail.a_LDFLAGS)
libconfigtoolfail.a_ARFLAGS := $(libconfigtoolfail.a_ARFLAGS)

helloworld.elf_SOURCES := $(helloworld.elf_SOURCES)
helloworld.elf_GENHEADERS := $(helloworld.elf_GENHEADERS)
helloworld.elf_CPPFLAGS := $(helloworld.elf_CPPFLAGS)
helloworld.elf_CFLAGS := $(helloworld.elf_CFLAGS)
helloworld.elf_CXXFLAGS := $(helloworld.elf_CXXFLAGS)
helloworld.elf_LDFLAGS := $(helloworld.elf_LDFLAGS)
helloworld.elf_ARFLAGS := $(helloworld.elf_ARFLAGS)

config_linux_user.elf_SOURCES := $(config_linux_user.elf_SOURCES)
config_linux_user.elf_GENHEADERS := $(config_linux_user.elf_GENHEADERS)
config_linux_user.elf_CPPFLAGS := $(config_linux_user.elf_CPPFLAGS)
config_linux_user.elf_CFLAGS := $(config_linux_user.elf_CFLAGS)
config_linux_user.elf_CXXFLAGS := $(config_linux_user.elf_CXXFLAGS)
config_linux_user.elf_LDFLAGS := $(config_linux_user.elf_LDFLAGS)
config_linux_user.elf_ARFLAGS := $(config_linux_user.elf_ARFLAGS)

reserved_area.elf_SOURCES := $(reserved_area.elf_SOURCES)
reserved_area.elf_GENHEADERS := $(reserved_area.elf_GENHEADERS)
reserved_area.elf_CPPFLAGS := $(reserved_area.elf_CPPFLAGS)
reserved_area.elf_CFLAGS := $(reserved_area.elf_CFLAGS)
reserved_area.elf_CXXFLAGS := $(reserved_area.elf_CXXFLAGS)
reserved_area.elf_LDFLAGS := $(reserved_area.elf_LDFLAGS)
reserved_area.elf_ARFLAGS := $(reserved_area.elf_ARFLAGS)

libconfigtoolutils_tests.elf_SOURCES := $(libconfigtoolutils_tests.elf_SOURCES)
libconfigtoolutils_tests.elf_GENHEADERS := $(libconfigtoolutils_tests.elf_GENHEADERS)
libconfigtoolutils_tests.elf_CPPFLAGS := $(libconfigtoolutils_tests.elf_CPPFLAGS)
libconfigtoolutils_tests.elf_CFLAGS := $(libconfigtoolutils_tests.elf_CFLAGS)
libconfigtoolutils_tests.elf_CXXFLAGS := $(libconfigtoolutils_tests.elf_CXXFLAGS)
libconfigtoolutils_tests.elf_LDFLAGS := $(libconfigtoolutils_tests.elf_LDFLAGS)
libconfigtoolutils_tests.elf_ARFLAGS := $(libconfigtoolutils_tests.elf_ARFLAGS)

config_linux_user_tests.elf_SOURCES := $(config_linux_user_tests.elf_SOURCES)
config_linux_user_tests.elf_GENHEADERS := $(config_linux_user_tests.elf_GENHEADERS)
config_linux_user_tests.elf_CPPFLAGS := $(config_linux_user_tests.elf_CPPFLAGS)
config_linux_user_tests.elf_CFLAGS := $(config_linux_user_tests.elf_CFLAGS)
config_linux_user_tests.elf_CXXFLAGS := $(config_linux_user_tests.elf_CXXFLAGS)
config_linux_user_tests.elf_LDFLAGS := $(config_linux_user_tests.elf_LDFLAGS)
config_linux_user_tests.elf_ARFLAGS := $(config_linux_user_tests.elf_ARFLAGS)

reserved_area_tests.elf_SOURCES := $(reserved_area_tests.elf_SOURCES)
reserved_area_tests.elf_GENHEADERS := $(reserved_area_tests.elf_GENHEADERS)
reserved_area_tests.elf_CPPFLAGS := $(reserved_area_tests.elf_CPPFLAGS)
reserved_area_tests.elf_CFLAGS := $(reserved_area_tests.elf_CFLAGS)
reserved_area_tests.elf_CXXFLAGS := $(reserved_area_tests.elf_CXXFLAGS)
reserved_area_tests.elf_LDFLAGS := $(reserved_area_tests.elf_LDFLAGS)
reserved_area_tests.elf_ARFLAGS := $(reserved_area_tests.elf_ARFLAGS)

OBJECT_FILES := $(OBJECT_FILES)
DEP_FILES := $(DEP_FILES)
OUT_DIRS := $(call sort,$(OUT_DIR)/ $(dir $(OBJECT_FILES)) $(OUT_DIRS))
GEN_DIRS := $(call sort,$(BIN_DIR)/ $(OUT_DIRS) $(GEN_DIRS))

$(DEP_FILES) : | $(GEN_DIRS)

-include $(DEP_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(CLANG_TIDY_RULESET)),$(error CLANG_TIDY_RULESET has "$(CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(CLANG_TIDY_RULESET))"))

libconfigtoolutils.a_LINT_SOURCES ?= $(filter-out $(libconfigtoolutils.a_NOLINT_SOURCES),$(libconfigtoolutils.a_SOURCES))
OBJS_libconfigtoolutils.a := $(call objs,$(libconfigtoolutils.a_SOURCES),libconfigtoolutils.a/)
DEPS_libconfigtoolutils.a := $(call deps,$(libconfigtoolutils.a_SOURCES),libconfigtoolutils.a/)
LOBS_libconfigtoolutils.a := $(call lobs,$(libconfigtoolutils.a_LINT_SOURCES),libconfigtoolutils.a/)
TIDYS_libconfigtoolutils.a := $(call tidys,$(libconfigtoolutils.a_LINT_SOURCES),libconfigtoolutils.a/)
$(TIDYS_libconfigtoolutils.a): $(libconfigtoolutils.a_PRECLANG_FILES)
PLINTS_libconfigtoolutils.a := $(call plints,$(libconfigtoolutils.a_LINT_SOURCES),libconfigtoolutils.a/)
$(PLINTS_libconfigtoolutils.a): $(libconfigtoolutils.a_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(libconfigtoolutils.a_CLANG_TIDY_RULESET)),$(error libconfigtoolutils.a_CLANG_TIDY_RULESET has "$(libconfigtoolutils.a_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(libconfigtoolutils.a_CLANG_TIDY_RULESET))"))

libconfigtoolutils.a: $(BIN_DIR)/libconfigtoolutils.a

$(DEPS_libconfigtoolutils.a) $(OBJS_libconfigtoolutils.a) $(LOBS_libconfigtoolutils.a) $(TIDYS_libconfigtoolutils.a) $(PLINTS_libconfigtoolutils.a) ./out/gcc_5_5_arm-linux-gnueabihf/Release/libconfigtoolutils.a/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(libconfigtoolutils.a_CPPFLAGS)

$(DEPS_libconfigtoolutils.a) $(OBJS_libconfigtoolutils.a) $(TIDYS_libconfigtoolutils.a) $(PLINTS_libconfigtoolutils.a) ./out/gcc_5_5_arm-linux-gnueabihf/Release/libconfigtoolutils.a/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(libconfigtoolutils.a_CXXFLAGS)

$(DEPS_libconfigtoolutils.a) $(OBJS_libconfigtoolutils.a) $(TIDYS_libconfigtoolutils.a) $(PLINTS_libconfigtoolutils.a) ./out/gcc_5_5_arm-linux-gnueabihf/Release/libconfigtoolutils.a/lint_mac.h: BUILDTARGET_CFLAGS:=$(libconfigtoolutils.a_CFLAGS)

lint-libconfigtoolutils.a: flexelint-libconfigtoolutils.a clang-tidy-libconfigtoolutils.a

$(LOBS_libconfigtoolutils.a): BUILDTARGET_LINTFLAGS:=$(libconfigtoolutils.a_LINTFLAGS)

flexelint-libconfigtoolutils.a: $($(libconfigtoolutils.a_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(libconfigtoolutils.a_DISABLE_LINT)$(DISABLE_LINT)LOBS_libconfigtoolutils.a)

$(TIDYS_libconfigtoolutils.a): BUILDTARGET_TIDYFLAGS:=$(libconfigtoolutils.a_TIDYFLAGS) -isystem $(OUT_DIR)/libconfigtoolutils.a -include lint_mac.h

$(TIDYS_libconfigtoolutils.a): libconfigtoolutils.a_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_libconfigtoolutils.a): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(libconfigtoolutils.a_CLANG_TIDY_RULESET) $(libconfigtoolutils.a_CLANG_TIDY_CHECKS)))

$(PLINTS_libconfigtoolutils.a): BUILDTARGET_PLINTFLAGS:=$(libconfigtoolutils.a_PLINTSFLAGS) -isystem $(OUT_DIR)/libconfigtoolutils.a -include lint_mac.h

clang-tidy-libconfigtoolutils.a: $($(libconfigtoolutils.a_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(libconfigtoolutils.a_DISABLE_LINT)$(DISABLE_LINT)TIDYS_libconfigtoolutils.a)

clean-clang-tidy-libconfigtoolutils.a:; $(SILENT)rm --force $(TIDYS_libconfigtoolutils.a)

clangsa-libconfigtoolutils.a: $(PLINTS_libconfigtoolutils.a)

info-txt-libconfigtoolutils.a: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,libconfigtoolutils.a,$<)

$(OUT_DIR)/libconfigtoolutils.a/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(libconfigtoolutils.a_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/libconfigtoolutils.a/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(libconfigtoolutils.a_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/libconfigtoolutils.a/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(libconfigtoolutils.a_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

libconfigtoolutils.a_PRECLANG_FILES=$(OUT_DIR)/libconfigtoolutils.a/lint_mac.h

$(OUT_DIR)/libconfigtoolutils.a/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(libconfigtoolutils.a_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/libconfigtoolutils.a/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/libconfigtoolutils.a/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(libconfigtoolutils.a_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/libconfigtoolutils.a/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



libconfigtoolfail.a_LINT_SOURCES ?= $(filter-out $(libconfigtoolfail.a_NOLINT_SOURCES),$(libconfigtoolfail.a_SOURCES))
OBJS_libconfigtoolfail.a := $(call objs,$(libconfigtoolfail.a_SOURCES),libconfigtoolfail.a/)
DEPS_libconfigtoolfail.a := $(call deps,$(libconfigtoolfail.a_SOURCES),libconfigtoolfail.a/)
LOBS_libconfigtoolfail.a := $(call lobs,$(libconfigtoolfail.a_LINT_SOURCES),libconfigtoolfail.a/)
TIDYS_libconfigtoolfail.a := $(call tidys,$(libconfigtoolfail.a_LINT_SOURCES),libconfigtoolfail.a/)
$(TIDYS_libconfigtoolfail.a): $(libconfigtoolfail.a_PRECLANG_FILES)
PLINTS_libconfigtoolfail.a := $(call plints,$(libconfigtoolfail.a_LINT_SOURCES),libconfigtoolfail.a/)
$(PLINTS_libconfigtoolfail.a): $(libconfigtoolfail.a_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(libconfigtoolfail.a_CLANG_TIDY_RULESET)),$(error libconfigtoolfail.a_CLANG_TIDY_RULESET has "$(libconfigtoolfail.a_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(libconfigtoolfail.a_CLANG_TIDY_RULESET))"))

libconfigtoolfail.a: $(BIN_DIR)/libconfigtoolfail.a

$(DEPS_libconfigtoolfail.a) $(OBJS_libconfigtoolfail.a) $(LOBS_libconfigtoolfail.a) $(TIDYS_libconfigtoolfail.a) $(PLINTS_libconfigtoolfail.a) ./out/gcc_5_5_arm-linux-gnueabihf/Release/libconfigtoolfail.a/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(libconfigtoolfail.a_CPPFLAGS)

$(DEPS_libconfigtoolfail.a) $(OBJS_libconfigtoolfail.a) $(TIDYS_libconfigtoolfail.a) $(PLINTS_libconfigtoolfail.a) ./out/gcc_5_5_arm-linux-gnueabihf/Release/libconfigtoolfail.a/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(libconfigtoolfail.a_CXXFLAGS)

$(DEPS_libconfigtoolfail.a) $(OBJS_libconfigtoolfail.a) $(TIDYS_libconfigtoolfail.a) $(PLINTS_libconfigtoolfail.a) ./out/gcc_5_5_arm-linux-gnueabihf/Release/libconfigtoolfail.a/lint_mac.h: BUILDTARGET_CFLAGS:=$(libconfigtoolfail.a_CFLAGS)

lint-libconfigtoolfail.a: flexelint-libconfigtoolfail.a clang-tidy-libconfigtoolfail.a

$(LOBS_libconfigtoolfail.a): BUILDTARGET_LINTFLAGS:=$(libconfigtoolfail.a_LINTFLAGS)

flexelint-libconfigtoolfail.a: $($(libconfigtoolfail.a_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(libconfigtoolfail.a_DISABLE_LINT)$(DISABLE_LINT)LOBS_libconfigtoolfail.a)

$(TIDYS_libconfigtoolfail.a): BUILDTARGET_TIDYFLAGS:=$(libconfigtoolfail.a_TIDYFLAGS) -isystem $(OUT_DIR)/libconfigtoolfail.a -include lint_mac.h

$(TIDYS_libconfigtoolfail.a): libconfigtoolfail.a_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_libconfigtoolfail.a): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(libconfigtoolfail.a_CLANG_TIDY_RULESET) $(libconfigtoolfail.a_CLANG_TIDY_CHECKS)))

$(PLINTS_libconfigtoolfail.a): BUILDTARGET_PLINTFLAGS:=$(libconfigtoolfail.a_PLINTSFLAGS) -isystem $(OUT_DIR)/libconfigtoolfail.a -include lint_mac.h

clang-tidy-libconfigtoolfail.a: $($(libconfigtoolfail.a_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(libconfigtoolfail.a_DISABLE_LINT)$(DISABLE_LINT)TIDYS_libconfigtoolfail.a)

clean-clang-tidy-libconfigtoolfail.a:; $(SILENT)rm --force $(TIDYS_libconfigtoolfail.a)

clangsa-libconfigtoolfail.a: $(PLINTS_libconfigtoolfail.a)

info-txt-libconfigtoolfail.a: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,libconfigtoolfail.a,$<)

$(OUT_DIR)/libconfigtoolfail.a/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(libconfigtoolfail.a_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/libconfigtoolfail.a/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(libconfigtoolfail.a_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/libconfigtoolfail.a/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(libconfigtoolfail.a_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

libconfigtoolfail.a_PRECLANG_FILES=$(OUT_DIR)/libconfigtoolfail.a/lint_mac.h

$(OUT_DIR)/libconfigtoolfail.a/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(libconfigtoolfail.a_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/libconfigtoolfail.a/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/libconfigtoolfail.a/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(libconfigtoolfail.a_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/libconfigtoolfail.a/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



helloworld.elf_LINT_SOURCES ?= $(filter-out $(helloworld.elf_NOLINT_SOURCES),$(helloworld.elf_SOURCES))
OBJS_helloworld.elf := $(call objs,$(helloworld.elf_SOURCES),helloworld.elf/)
DEPS_helloworld.elf := $(call deps,$(helloworld.elf_SOURCES),helloworld.elf/)
LOBS_helloworld.elf := $(call lobs,$(helloworld.elf_LINT_SOURCES),helloworld.elf/)
TIDYS_helloworld.elf := $(call tidys,$(helloworld.elf_LINT_SOURCES),helloworld.elf/)
$(TIDYS_helloworld.elf): $(helloworld.elf_PRECLANG_FILES)
PLINTS_helloworld.elf := $(call plints,$(helloworld.elf_LINT_SOURCES),helloworld.elf/)
$(PLINTS_helloworld.elf): $(helloworld.elf_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(helloworld.elf_CLANG_TIDY_RULESET)),$(error helloworld.elf_CLANG_TIDY_RULESET has "$(helloworld.elf_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(helloworld.elf_CLANG_TIDY_RULESET))"))

helloworld.elf: $(BIN_DIR)/helloworld.elf

$(DEPS_helloworld.elf) $(OBJS_helloworld.elf) $(LOBS_helloworld.elf) $(TIDYS_helloworld.elf) $(PLINTS_helloworld.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/helloworld.elf/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(helloworld.elf_CPPFLAGS)

$(DEPS_helloworld.elf) $(OBJS_helloworld.elf) $(TIDYS_helloworld.elf) $(PLINTS_helloworld.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/helloworld.elf/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(helloworld.elf_CXXFLAGS)

$(DEPS_helloworld.elf) $(OBJS_helloworld.elf) $(TIDYS_helloworld.elf) $(PLINTS_helloworld.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/helloworld.elf/lint_mac.h: BUILDTARGET_CFLAGS:=$(helloworld.elf_CFLAGS)

lint-helloworld.elf: flexelint-helloworld.elf clang-tidy-helloworld.elf

$(LOBS_helloworld.elf): BUILDTARGET_LINTFLAGS:=$(helloworld.elf_LINTFLAGS)

flexelint-helloworld.elf: $($(helloworld.elf_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(helloworld.elf_DISABLE_LINT)$(DISABLE_LINT)LOBS_helloworld.elf)

$(TIDYS_helloworld.elf): BUILDTARGET_TIDYFLAGS:=$(helloworld.elf_TIDYFLAGS) -isystem $(OUT_DIR)/helloworld.elf -include lint_mac.h

$(TIDYS_helloworld.elf): helloworld.elf_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_helloworld.elf): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(helloworld.elf_CLANG_TIDY_RULESET) $(helloworld.elf_CLANG_TIDY_CHECKS)))

$(PLINTS_helloworld.elf): BUILDTARGET_PLINTFLAGS:=$(helloworld.elf_PLINTSFLAGS) -isystem $(OUT_DIR)/helloworld.elf -include lint_mac.h

clang-tidy-helloworld.elf: $($(helloworld.elf_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(helloworld.elf_DISABLE_LINT)$(DISABLE_LINT)TIDYS_helloworld.elf)

clean-clang-tidy-helloworld.elf:; $(SILENT)rm --force $(TIDYS_helloworld.elf)

clangsa-helloworld.elf: $(PLINTS_helloworld.elf)

info-txt-helloworld.elf: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,helloworld.elf,$<)

$(OUT_DIR)/helloworld.elf/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(helloworld.elf_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/helloworld.elf/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(helloworld.elf_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/helloworld.elf/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(helloworld.elf_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

helloworld.elf_PRECLANG_FILES=$(OUT_DIR)/helloworld.elf/lint_mac.h

$(OUT_DIR)/helloworld.elf/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(helloworld.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/helloworld.elf/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/helloworld.elf/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(helloworld.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/helloworld.elf/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



config_linux_user.elf_LINT_SOURCES ?= $(filter-out $(config_linux_user.elf_NOLINT_SOURCES),$(config_linux_user.elf_SOURCES))
OBJS_config_linux_user.elf := $(call objs,$(config_linux_user.elf_SOURCES),config_linux_user.elf/)
DEPS_config_linux_user.elf := $(call deps,$(config_linux_user.elf_SOURCES),config_linux_user.elf/)
LOBS_config_linux_user.elf := $(call lobs,$(config_linux_user.elf_LINT_SOURCES),config_linux_user.elf/)
TIDYS_config_linux_user.elf := $(call tidys,$(config_linux_user.elf_LINT_SOURCES),config_linux_user.elf/)
$(TIDYS_config_linux_user.elf): $(config_linux_user.elf_PRECLANG_FILES)
PLINTS_config_linux_user.elf := $(call plints,$(config_linux_user.elf_LINT_SOURCES),config_linux_user.elf/)
$(PLINTS_config_linux_user.elf): $(config_linux_user.elf_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(config_linux_user.elf_CLANG_TIDY_RULESET)),$(error config_linux_user.elf_CLANG_TIDY_RULESET has "$(config_linux_user.elf_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(config_linux_user.elf_CLANG_TIDY_RULESET))"))

config_linux_user.elf: $(BIN_DIR)/config_linux_user.elf

$(DEPS_config_linux_user.elf) $(OBJS_config_linux_user.elf) $(LOBS_config_linux_user.elf) $(TIDYS_config_linux_user.elf) $(PLINTS_config_linux_user.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/config_linux_user.elf/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(config_linux_user.elf_CPPFLAGS)

$(DEPS_config_linux_user.elf) $(OBJS_config_linux_user.elf) $(TIDYS_config_linux_user.elf) $(PLINTS_config_linux_user.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/config_linux_user.elf/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(config_linux_user.elf_CXXFLAGS)

$(DEPS_config_linux_user.elf) $(OBJS_config_linux_user.elf) $(TIDYS_config_linux_user.elf) $(PLINTS_config_linux_user.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/config_linux_user.elf/lint_mac.h: BUILDTARGET_CFLAGS:=$(config_linux_user.elf_CFLAGS)

lint-config_linux_user.elf: flexelint-config_linux_user.elf clang-tidy-config_linux_user.elf

$(LOBS_config_linux_user.elf): BUILDTARGET_LINTFLAGS:=$(config_linux_user.elf_LINTFLAGS)

flexelint-config_linux_user.elf: $($(config_linux_user.elf_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(config_linux_user.elf_DISABLE_LINT)$(DISABLE_LINT)LOBS_config_linux_user.elf)

$(TIDYS_config_linux_user.elf): BUILDTARGET_TIDYFLAGS:=$(config_linux_user.elf_TIDYFLAGS) -isystem $(OUT_DIR)/config_linux_user.elf -include lint_mac.h

$(TIDYS_config_linux_user.elf): config_linux_user.elf_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_config_linux_user.elf): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(config_linux_user.elf_CLANG_TIDY_RULESET) $(config_linux_user.elf_CLANG_TIDY_CHECKS)))

$(PLINTS_config_linux_user.elf): BUILDTARGET_PLINTFLAGS:=$(config_linux_user.elf_PLINTSFLAGS) -isystem $(OUT_DIR)/config_linux_user.elf -include lint_mac.h

clang-tidy-config_linux_user.elf: $($(config_linux_user.elf_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(config_linux_user.elf_DISABLE_LINT)$(DISABLE_LINT)TIDYS_config_linux_user.elf)

clean-clang-tidy-config_linux_user.elf:; $(SILENT)rm --force $(TIDYS_config_linux_user.elf)

clangsa-config_linux_user.elf: $(PLINTS_config_linux_user.elf)

info-txt-config_linux_user.elf: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,config_linux_user.elf,$<)

$(OUT_DIR)/config_linux_user.elf/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(config_linux_user.elf_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/config_linux_user.elf/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(config_linux_user.elf_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/config_linux_user.elf/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(config_linux_user.elf_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

config_linux_user.elf_PRECLANG_FILES=$(OUT_DIR)/config_linux_user.elf/lint_mac.h

$(OUT_DIR)/config_linux_user.elf/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(config_linux_user.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/config_linux_user.elf/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/config_linux_user.elf/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(config_linux_user.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/config_linux_user.elf/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



reserved_area.elf_LINT_SOURCES ?= $(filter-out $(reserved_area.elf_NOLINT_SOURCES),$(reserved_area.elf_SOURCES))
OBJS_reserved_area.elf := $(call objs,$(reserved_area.elf_SOURCES),reserved_area.elf/)
DEPS_reserved_area.elf := $(call deps,$(reserved_area.elf_SOURCES),reserved_area.elf/)
LOBS_reserved_area.elf := $(call lobs,$(reserved_area.elf_LINT_SOURCES),reserved_area.elf/)
TIDYS_reserved_area.elf := $(call tidys,$(reserved_area.elf_LINT_SOURCES),reserved_area.elf/)
$(TIDYS_reserved_area.elf): $(reserved_area.elf_PRECLANG_FILES)
PLINTS_reserved_area.elf := $(call plints,$(reserved_area.elf_LINT_SOURCES),reserved_area.elf/)
$(PLINTS_reserved_area.elf): $(reserved_area.elf_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(reserved_area.elf_CLANG_TIDY_RULESET)),$(error reserved_area.elf_CLANG_TIDY_RULESET has "$(reserved_area.elf_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(reserved_area.elf_CLANG_TIDY_RULESET))"))

reserved_area.elf: $(BIN_DIR)/reserved_area.elf

$(DEPS_reserved_area.elf) $(OBJS_reserved_area.elf) $(LOBS_reserved_area.elf) $(TIDYS_reserved_area.elf) $(PLINTS_reserved_area.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/reserved_area.elf/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(reserved_area.elf_CPPFLAGS)

$(DEPS_reserved_area.elf) $(OBJS_reserved_area.elf) $(TIDYS_reserved_area.elf) $(PLINTS_reserved_area.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/reserved_area.elf/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(reserved_area.elf_CXXFLAGS)

$(DEPS_reserved_area.elf) $(OBJS_reserved_area.elf) $(TIDYS_reserved_area.elf) $(PLINTS_reserved_area.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/reserved_area.elf/lint_mac.h: BUILDTARGET_CFLAGS:=$(reserved_area.elf_CFLAGS)

lint-reserved_area.elf: flexelint-reserved_area.elf clang-tidy-reserved_area.elf

$(LOBS_reserved_area.elf): BUILDTARGET_LINTFLAGS:=$(reserved_area.elf_LINTFLAGS)

flexelint-reserved_area.elf: $($(reserved_area.elf_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(reserved_area.elf_DISABLE_LINT)$(DISABLE_LINT)LOBS_reserved_area.elf)

$(TIDYS_reserved_area.elf): BUILDTARGET_TIDYFLAGS:=$(reserved_area.elf_TIDYFLAGS) -isystem $(OUT_DIR)/reserved_area.elf -include lint_mac.h

$(TIDYS_reserved_area.elf): reserved_area.elf_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_reserved_area.elf): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(reserved_area.elf_CLANG_TIDY_RULESET) $(reserved_area.elf_CLANG_TIDY_CHECKS)))

$(PLINTS_reserved_area.elf): BUILDTARGET_PLINTFLAGS:=$(reserved_area.elf_PLINTSFLAGS) -isystem $(OUT_DIR)/reserved_area.elf -include lint_mac.h

clang-tidy-reserved_area.elf: $($(reserved_area.elf_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(reserved_area.elf_DISABLE_LINT)$(DISABLE_LINT)TIDYS_reserved_area.elf)

clean-clang-tidy-reserved_area.elf:; $(SILENT)rm --force $(TIDYS_reserved_area.elf)

clangsa-reserved_area.elf: $(PLINTS_reserved_area.elf)

info-txt-reserved_area.elf: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,reserved_area.elf,$<)

$(OUT_DIR)/reserved_area.elf/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(reserved_area.elf_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/reserved_area.elf/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(reserved_area.elf_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/reserved_area.elf/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(reserved_area.elf_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

reserved_area.elf_PRECLANG_FILES=$(OUT_DIR)/reserved_area.elf/lint_mac.h

$(OUT_DIR)/reserved_area.elf/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(reserved_area.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/reserved_area.elf/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/reserved_area.elf/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(reserved_area.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/reserved_area.elf/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



libconfigtoolutils_tests.elf_LINT_SOURCES ?= $(filter-out $(libconfigtoolutils_tests.elf_NOLINT_SOURCES),$(libconfigtoolutils_tests.elf_SOURCES))
OBJS_libconfigtoolutils_tests.elf := $(call objs,$(libconfigtoolutils_tests.elf_SOURCES),libconfigtoolutils_tests.elf/)
DEPS_libconfigtoolutils_tests.elf := $(call deps,$(libconfigtoolutils_tests.elf_SOURCES),libconfigtoolutils_tests.elf/)
LOBS_libconfigtoolutils_tests.elf := $(call lobs,$(libconfigtoolutils_tests.elf_LINT_SOURCES),libconfigtoolutils_tests.elf/)
TIDYS_libconfigtoolutils_tests.elf := $(call tidys,$(libconfigtoolutils_tests.elf_LINT_SOURCES),libconfigtoolutils_tests.elf/)
$(TIDYS_libconfigtoolutils_tests.elf): $(libconfigtoolutils_tests.elf_PRECLANG_FILES)
PLINTS_libconfigtoolutils_tests.elf := $(call plints,$(libconfigtoolutils_tests.elf_LINT_SOURCES),libconfigtoolutils_tests.elf/)
$(PLINTS_libconfigtoolutils_tests.elf): $(libconfigtoolutils_tests.elf_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(libconfigtoolutils_tests.elf_CLANG_TIDY_RULESET)),$(error libconfigtoolutils_tests.elf_CLANG_TIDY_RULESET has "$(libconfigtoolutils_tests.elf_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(libconfigtoolutils_tests.elf_CLANG_TIDY_RULESET))"))

libconfigtoolutils_tests.elf: $(BIN_DIR)/libconfigtoolutils_tests.elf

$(DEPS_libconfigtoolutils_tests.elf) $(OBJS_libconfigtoolutils_tests.elf) $(LOBS_libconfigtoolutils_tests.elf) $(TIDYS_libconfigtoolutils_tests.elf) $(PLINTS_libconfigtoolutils_tests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/libconfigtoolutils_tests.elf/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(libconfigtoolutils_tests.elf_CPPFLAGS)

$(DEPS_libconfigtoolutils_tests.elf) $(OBJS_libconfigtoolutils_tests.elf) $(TIDYS_libconfigtoolutils_tests.elf) $(PLINTS_libconfigtoolutils_tests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/libconfigtoolutils_tests.elf/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(libconfigtoolutils_tests.elf_CXXFLAGS)

$(DEPS_libconfigtoolutils_tests.elf) $(OBJS_libconfigtoolutils_tests.elf) $(TIDYS_libconfigtoolutils_tests.elf) $(PLINTS_libconfigtoolutils_tests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/libconfigtoolutils_tests.elf/lint_mac.h: BUILDTARGET_CFLAGS:=$(libconfigtoolutils_tests.elf_CFLAGS)

lint-libconfigtoolutils_tests.elf: flexelint-libconfigtoolutils_tests.elf clang-tidy-libconfigtoolutils_tests.elf

$(LOBS_libconfigtoolutils_tests.elf): BUILDTARGET_LINTFLAGS:=$(libconfigtoolutils_tests.elf_LINTFLAGS)

flexelint-libconfigtoolutils_tests.elf: $($(libconfigtoolutils_tests.elf_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(libconfigtoolutils_tests.elf_DISABLE_LINT)$(DISABLE_LINT)LOBS_libconfigtoolutils_tests.elf)

$(TIDYS_libconfigtoolutils_tests.elf): BUILDTARGET_TIDYFLAGS:=$(libconfigtoolutils_tests.elf_TIDYFLAGS) -isystem $(OUT_DIR)/libconfigtoolutils_tests.elf -include lint_mac.h

$(TIDYS_libconfigtoolutils_tests.elf): libconfigtoolutils_tests.elf_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_libconfigtoolutils_tests.elf): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(libconfigtoolutils_tests.elf_CLANG_TIDY_RULESET) $(libconfigtoolutils_tests.elf_CLANG_TIDY_CHECKS)))

$(PLINTS_libconfigtoolutils_tests.elf): BUILDTARGET_PLINTFLAGS:=$(libconfigtoolutils_tests.elf_PLINTSFLAGS) -isystem $(OUT_DIR)/libconfigtoolutils_tests.elf -include lint_mac.h

clang-tidy-libconfigtoolutils_tests.elf: $($(libconfigtoolutils_tests.elf_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(libconfigtoolutils_tests.elf_DISABLE_LINT)$(DISABLE_LINT)TIDYS_libconfigtoolutils_tests.elf)

clean-clang-tidy-libconfigtoolutils_tests.elf:; $(SILENT)rm --force $(TIDYS_libconfigtoolutils_tests.elf)

clangsa-libconfigtoolutils_tests.elf: $(PLINTS_libconfigtoolutils_tests.elf)

info-txt-libconfigtoolutils_tests.elf: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,libconfigtoolutils_tests.elf,$<)

$(OUT_DIR)/libconfigtoolutils_tests.elf/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(libconfigtoolutils_tests.elf_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/libconfigtoolutils_tests.elf/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(libconfigtoolutils_tests.elf_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/libconfigtoolutils_tests.elf/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(libconfigtoolutils_tests.elf_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

libconfigtoolutils_tests.elf_PRECLANG_FILES=$(OUT_DIR)/libconfigtoolutils_tests.elf/lint_mac.h

$(OUT_DIR)/libconfigtoolutils_tests.elf/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(libconfigtoolutils_tests.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/libconfigtoolutils_tests.elf/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/libconfigtoolutils_tests.elf/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(libconfigtoolutils_tests.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/libconfigtoolutils_tests.elf/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



config_linux_user_tests.elf_LINT_SOURCES ?= $(filter-out $(config_linux_user_tests.elf_NOLINT_SOURCES),$(config_linux_user_tests.elf_SOURCES))
OBJS_config_linux_user_tests.elf := $(call objs,$(config_linux_user_tests.elf_SOURCES),config_linux_user_tests.elf/)
DEPS_config_linux_user_tests.elf := $(call deps,$(config_linux_user_tests.elf_SOURCES),config_linux_user_tests.elf/)
LOBS_config_linux_user_tests.elf := $(call lobs,$(config_linux_user_tests.elf_LINT_SOURCES),config_linux_user_tests.elf/)
TIDYS_config_linux_user_tests.elf := $(call tidys,$(config_linux_user_tests.elf_LINT_SOURCES),config_linux_user_tests.elf/)
$(TIDYS_config_linux_user_tests.elf): $(config_linux_user_tests.elf_PRECLANG_FILES)
PLINTS_config_linux_user_tests.elf := $(call plints,$(config_linux_user_tests.elf_LINT_SOURCES),config_linux_user_tests.elf/)
$(PLINTS_config_linux_user_tests.elf): $(config_linux_user_tests.elf_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(config_linux_user_tests.elf_CLANG_TIDY_RULESET)),$(error config_linux_user_tests.elf_CLANG_TIDY_RULESET has "$(config_linux_user_tests.elf_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(config_linux_user_tests.elf_CLANG_TIDY_RULESET))"))

config_linux_user_tests.elf: $(BIN_DIR)/config_linux_user_tests.elf

$(DEPS_config_linux_user_tests.elf) $(OBJS_config_linux_user_tests.elf) $(LOBS_config_linux_user_tests.elf) $(TIDYS_config_linux_user_tests.elf) $(PLINTS_config_linux_user_tests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/config_linux_user_tests.elf/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(config_linux_user_tests.elf_CPPFLAGS)

$(DEPS_config_linux_user_tests.elf) $(OBJS_config_linux_user_tests.elf) $(TIDYS_config_linux_user_tests.elf) $(PLINTS_config_linux_user_tests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/config_linux_user_tests.elf/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(config_linux_user_tests.elf_CXXFLAGS)

$(DEPS_config_linux_user_tests.elf) $(OBJS_config_linux_user_tests.elf) $(TIDYS_config_linux_user_tests.elf) $(PLINTS_config_linux_user_tests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/config_linux_user_tests.elf/lint_mac.h: BUILDTARGET_CFLAGS:=$(config_linux_user_tests.elf_CFLAGS)

lint-config_linux_user_tests.elf: flexelint-config_linux_user_tests.elf clang-tidy-config_linux_user_tests.elf

$(LOBS_config_linux_user_tests.elf): BUILDTARGET_LINTFLAGS:=$(config_linux_user_tests.elf_LINTFLAGS)

flexelint-config_linux_user_tests.elf: $($(config_linux_user_tests.elf_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(config_linux_user_tests.elf_DISABLE_LINT)$(DISABLE_LINT)LOBS_config_linux_user_tests.elf)

$(TIDYS_config_linux_user_tests.elf): BUILDTARGET_TIDYFLAGS:=$(config_linux_user_tests.elf_TIDYFLAGS) -isystem $(OUT_DIR)/config_linux_user_tests.elf -include lint_mac.h

$(TIDYS_config_linux_user_tests.elf): config_linux_user_tests.elf_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_config_linux_user_tests.elf): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(config_linux_user_tests.elf_CLANG_TIDY_RULESET) $(config_linux_user_tests.elf_CLANG_TIDY_CHECKS)))

$(PLINTS_config_linux_user_tests.elf): BUILDTARGET_PLINTFLAGS:=$(config_linux_user_tests.elf_PLINTSFLAGS) -isystem $(OUT_DIR)/config_linux_user_tests.elf -include lint_mac.h

clang-tidy-config_linux_user_tests.elf: $($(config_linux_user_tests.elf_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(config_linux_user_tests.elf_DISABLE_LINT)$(DISABLE_LINT)TIDYS_config_linux_user_tests.elf)

clean-clang-tidy-config_linux_user_tests.elf:; $(SILENT)rm --force $(TIDYS_config_linux_user_tests.elf)

clangsa-config_linux_user_tests.elf: $(PLINTS_config_linux_user_tests.elf)

info-txt-config_linux_user_tests.elf: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,config_linux_user_tests.elf,$<)

$(OUT_DIR)/config_linux_user_tests.elf/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(config_linux_user_tests.elf_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/config_linux_user_tests.elf/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(config_linux_user_tests.elf_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/config_linux_user_tests.elf/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(config_linux_user_tests.elf_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

config_linux_user_tests.elf_PRECLANG_FILES=$(OUT_DIR)/config_linux_user_tests.elf/lint_mac.h

$(OUT_DIR)/config_linux_user_tests.elf/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(config_linux_user_tests.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/config_linux_user_tests.elf/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/config_linux_user_tests.elf/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(config_linux_user_tests.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/config_linux_user_tests.elf/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



reserved_area_tests.elf_LINT_SOURCES ?= $(filter-out $(reserved_area_tests.elf_NOLINT_SOURCES),$(reserved_area_tests.elf_SOURCES))
OBJS_reserved_area_tests.elf := $(call objs,$(reserved_area_tests.elf_SOURCES),reserved_area_tests.elf/)
DEPS_reserved_area_tests.elf := $(call deps,$(reserved_area_tests.elf_SOURCES),reserved_area_tests.elf/)
LOBS_reserved_area_tests.elf := $(call lobs,$(reserved_area_tests.elf_LINT_SOURCES),reserved_area_tests.elf/)
TIDYS_reserved_area_tests.elf := $(call tidys,$(reserved_area_tests.elf_LINT_SOURCES),reserved_area_tests.elf/)
$(TIDYS_reserved_area_tests.elf): $(reserved_area_tests.elf_PRECLANG_FILES)
PLINTS_reserved_area_tests.elf := $(call plints,$(reserved_area_tests.elf_LINT_SOURCES),reserved_area_tests.elf/)
$(PLINTS_reserved_area_tests.elf): $(reserved_area_tests.elf_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(reserved_area_tests.elf_CLANG_TIDY_RULESET)),$(error reserved_area_tests.elf_CLANG_TIDY_RULESET has "$(reserved_area_tests.elf_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(reserved_area_tests.elf_CLANG_TIDY_RULESET))"))

reserved_area_tests.elf: $(BIN_DIR)/reserved_area_tests.elf

$(DEPS_reserved_area_tests.elf) $(OBJS_reserved_area_tests.elf) $(LOBS_reserved_area_tests.elf) $(TIDYS_reserved_area_tests.elf) $(PLINTS_reserved_area_tests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/reserved_area_tests.elf/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(reserved_area_tests.elf_CPPFLAGS)

$(DEPS_reserved_area_tests.elf) $(OBJS_reserved_area_tests.elf) $(TIDYS_reserved_area_tests.elf) $(PLINTS_reserved_area_tests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/reserved_area_tests.elf/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(reserved_area_tests.elf_CXXFLAGS)

$(DEPS_reserved_area_tests.elf) $(OBJS_reserved_area_tests.elf) $(TIDYS_reserved_area_tests.elf) $(PLINTS_reserved_area_tests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/reserved_area_tests.elf/lint_mac.h: BUILDTARGET_CFLAGS:=$(reserved_area_tests.elf_CFLAGS)

lint-reserved_area_tests.elf: flexelint-reserved_area_tests.elf clang-tidy-reserved_area_tests.elf

$(LOBS_reserved_area_tests.elf): BUILDTARGET_LINTFLAGS:=$(reserved_area_tests.elf_LINTFLAGS)

flexelint-reserved_area_tests.elf: $($(reserved_area_tests.elf_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(reserved_area_tests.elf_DISABLE_LINT)$(DISABLE_LINT)LOBS_reserved_area_tests.elf)

$(TIDYS_reserved_area_tests.elf): BUILDTARGET_TIDYFLAGS:=$(reserved_area_tests.elf_TIDYFLAGS) -isystem $(OUT_DIR)/reserved_area_tests.elf -include lint_mac.h

$(TIDYS_reserved_area_tests.elf): reserved_area_tests.elf_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_reserved_area_tests.elf): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(reserved_area_tests.elf_CLANG_TIDY_RULESET) $(reserved_area_tests.elf_CLANG_TIDY_CHECKS)))

$(PLINTS_reserved_area_tests.elf): BUILDTARGET_PLINTFLAGS:=$(reserved_area_tests.elf_PLINTSFLAGS) -isystem $(OUT_DIR)/reserved_area_tests.elf -include lint_mac.h

clang-tidy-reserved_area_tests.elf: $($(reserved_area_tests.elf_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(reserved_area_tests.elf_DISABLE_LINT)$(DISABLE_LINT)TIDYS_reserved_area_tests.elf)

clean-clang-tidy-reserved_area_tests.elf:; $(SILENT)rm --force $(TIDYS_reserved_area_tests.elf)

clangsa-reserved_area_tests.elf: $(PLINTS_reserved_area_tests.elf)

info-txt-reserved_area_tests.elf: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,reserved_area_tests.elf,$<)

$(OUT_DIR)/reserved_area_tests.elf/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(reserved_area_tests.elf_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/reserved_area_tests.elf/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(reserved_area_tests.elf_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/reserved_area_tests.elf/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(reserved_area_tests.elf_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

reserved_area_tests.elf_PRECLANG_FILES=$(OUT_DIR)/reserved_area_tests.elf/lint_mac.h

$(OUT_DIR)/reserved_area_tests.elf/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(reserved_area_tests.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/reserved_area_tests.elf/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/reserved_area_tests.elf/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(reserved_area_tests.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/reserved_area_tests.elf/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



$(OUT_DIR)/helloworld.elf.dbg: $(OBJS_helloworld.elf) $(MK_FILES) $(helloworld.elf_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),helloworld.elf,$@,$(OBJS_helloworld.elf),$(LDFLAGS) $(helloworld.elf_LDFLAGS))

$(OUT_DIR)/config_linux_user.elf.dbg: $(OBJS_config_linux_user.elf) $(MK_FILES) $(config_linux_user.elf_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),config_linux_user.elf,$@,$(OBJS_config_linux_user.elf),$(LDFLAGS) $(config_linux_user.elf_LDFLAGS))

$(OUT_DIR)/reserved_area.elf.dbg: $(OBJS_reserved_area.elf) $(MK_FILES) $(reserved_area.elf_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),reserved_area.elf,$@,$(OBJS_reserved_area.elf),$(LDFLAGS) $(reserved_area.elf_LDFLAGS))

$(OUT_DIR)/libconfigtoolutils_tests.elf.dbg: $(OBJS_libconfigtoolutils_tests.elf) $(MK_FILES) $(libconfigtoolutils_tests.elf_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),libconfigtoolutils_tests.elf,$@,$(OBJS_libconfigtoolutils_tests.elf),$(LDFLAGS) $(libconfigtoolutils_tests.elf_LDFLAGS))

$(OUT_DIR)/config_linux_user_tests.elf.dbg: $(OBJS_config_linux_user_tests.elf) $(MK_FILES) $(config_linux_user_tests.elf_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),config_linux_user_tests.elf,$@,$(OBJS_config_linux_user_tests.elf),$(LDFLAGS) $(config_linux_user_tests.elf_LDFLAGS))

$(OUT_DIR)/reserved_area_tests.elf.dbg: $(OBJS_reserved_area_tests.elf) $(MK_FILES) $(reserved_area_tests.elf_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),reserved_area_tests.elf,$@,$(OBJS_reserved_area_tests.elf),$(LDFLAGS) $(reserved_area_tests.elf_LDFLAGS))

$(BIN_DIR)/libconfigtoolutils.a: $(OBJS_libconfigtoolutils.a) $(MK_FILES) $(libconfigtoolutils.a_PREREQUISITES) $(AR_SUPPORT_FILES); $(SILENT) rm --force $@; $(call ar,libconfigtoolutils.a,$@,$(OBJS_libconfigtoolutils.a),$(ARFLAGS) $(libconfigtoolutils.a_ARFLAGS))

$(BIN_DIR)/libconfigtoolfail.a: $(OBJS_libconfigtoolfail.a) $(MK_FILES) $(libconfigtoolfail.a_PREREQUISITES) $(AR_SUPPORT_FILES); $(SILENT) rm --force $@; $(call ar,libconfigtoolfail.a,$@,$(OBJS_libconfigtoolfail.a),$(ARFLAGS) $(libconfigtoolfail.a_ARFLAGS))

check-libconfigtoolutils_tests.elf: $(BIN_DIR)/libconfigtoolutils_tests.elf; $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_check,libconfigtoolutils_tests.elf,./$(notdir $<))

check-config_linux_user_tests.elf: $(BIN_DIR)/config_linux_user_tests.elf; $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_check,config_linux_user_tests.elf,./$(notdir $<))

check-reserved_area_tests.elf: $(BIN_DIR)/reserved_area_tests.elf; $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_check,reserved_area_tests.elf,./$(notdir $<))

memcheck-helloworld.elf: $(BIN_DIR)/helloworld.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,helloworld.elf,./$(notdir $<),MEMCHECK)
helgrind-helloworld.elf: $(BIN_DIR)/helloworld.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,helloworld.elf,./$(notdir $<),HELGRIND)
callgrind-helloworld.elf: $(BIN_DIR)/helloworld.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,helloworld.elf,./$(notdir $<),CALLGRIND)
massif-helloworld.elf: $(BIN_DIR)/helloworld.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,helloworld.elf,./$(notdir $<),MASSIF)
nulgrind-helloworld.elf: $(BIN_DIR)/helloworld.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,helloworld.elf,./$(notdir $<),NULGRIND)

memcheck-config_linux_user.elf: $(BIN_DIR)/config_linux_user.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,config_linux_user.elf,./$(notdir $<),MEMCHECK)
helgrind-config_linux_user.elf: $(BIN_DIR)/config_linux_user.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,config_linux_user.elf,./$(notdir $<),HELGRIND)
callgrind-config_linux_user.elf: $(BIN_DIR)/config_linux_user.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,config_linux_user.elf,./$(notdir $<),CALLGRIND)
massif-config_linux_user.elf: $(BIN_DIR)/config_linux_user.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,config_linux_user.elf,./$(notdir $<),MASSIF)
nulgrind-config_linux_user.elf: $(BIN_DIR)/config_linux_user.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,config_linux_user.elf,./$(notdir $<),NULGRIND)

memcheck-reserved_area.elf: $(BIN_DIR)/reserved_area.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,reserved_area.elf,./$(notdir $<),MEMCHECK)
helgrind-reserved_area.elf: $(BIN_DIR)/reserved_area.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,reserved_area.elf,./$(notdir $<),HELGRIND)
callgrind-reserved_area.elf: $(BIN_DIR)/reserved_area.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,reserved_area.elf,./$(notdir $<),CALLGRIND)
massif-reserved_area.elf: $(BIN_DIR)/reserved_area.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,reserved_area.elf,./$(notdir $<),MASSIF)
nulgrind-reserved_area.elf: $(BIN_DIR)/reserved_area.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,reserved_area.elf,./$(notdir $<),NULGRIND)

memcheck-libconfigtoolutils_tests.elf: $(BIN_DIR)/libconfigtoolutils_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,libconfigtoolutils_tests.elf,./$(notdir $<),MEMCHECK)
helgrind-libconfigtoolutils_tests.elf: $(BIN_DIR)/libconfigtoolutils_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,libconfigtoolutils_tests.elf,./$(notdir $<),HELGRIND)
callgrind-libconfigtoolutils_tests.elf: $(BIN_DIR)/libconfigtoolutils_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,libconfigtoolutils_tests.elf,./$(notdir $<),CALLGRIND)
massif-libconfigtoolutils_tests.elf: $(BIN_DIR)/libconfigtoolutils_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,libconfigtoolutils_tests.elf,./$(notdir $<),MASSIF)
nulgrind-libconfigtoolutils_tests.elf: $(BIN_DIR)/libconfigtoolutils_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,libconfigtoolutils_tests.elf,./$(notdir $<),NULGRIND)

memcheck-config_linux_user_tests.elf: $(BIN_DIR)/config_linux_user_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,config_linux_user_tests.elf,./$(notdir $<),MEMCHECK)
helgrind-config_linux_user_tests.elf: $(BIN_DIR)/config_linux_user_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,config_linux_user_tests.elf,./$(notdir $<),HELGRIND)
callgrind-config_linux_user_tests.elf: $(BIN_DIR)/config_linux_user_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,config_linux_user_tests.elf,./$(notdir $<),CALLGRIND)
massif-config_linux_user_tests.elf: $(BIN_DIR)/config_linux_user_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,config_linux_user_tests.elf,./$(notdir $<),MASSIF)
nulgrind-config_linux_user_tests.elf: $(BIN_DIR)/config_linux_user_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,config_linux_user_tests.elf,./$(notdir $<),NULGRIND)

memcheck-reserved_area_tests.elf: $(BIN_DIR)/reserved_area_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,reserved_area_tests.elf,./$(notdir $<),MEMCHECK)
helgrind-reserved_area_tests.elf: $(BIN_DIR)/reserved_area_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,reserved_area_tests.elf,./$(notdir $<),HELGRIND)
callgrind-reserved_area_tests.elf: $(BIN_DIR)/reserved_area_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,reserved_area_tests.elf,./$(notdir $<),CALLGRIND)
massif-reserved_area_tests.elf: $(BIN_DIR)/reserved_area_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,reserved_area_tests.elf,./$(notdir $<),MASSIF)
nulgrind-reserved_area_tests.elf: $(BIN_DIR)/reserved_area_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,reserved_area_tests.elf,./$(notdir $<),NULGRIND)
