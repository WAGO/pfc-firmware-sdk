CPPFLAGS := $(CPPFLAGS)
CFLAGS := $(CFLAGS)
CXXFLAGS := $(CXXFLAGS)
LDFLAGS := $(LDFLAGS)
ARFLAGS := $(ARFLAGS)

netconfd.elf_SOURCES := $(netconfd.elf_SOURCES)
netconfd.elf_GENHEADERS := $(netconfd.elf_GENHEADERS)
netconfd.elf_CPPFLAGS := $(netconfd.elf_CPPFLAGS)
netconfd.elf_CFLAGS := $(netconfd.elf_CFLAGS)
netconfd.elf_CXXFLAGS := $(netconfd.elf_CXXFLAGS)
netconfd.elf_LDFLAGS := $(netconfd.elf_LDFLAGS)
netconfd.elf_ARFLAGS := $(netconfd.elf_ARFLAGS)

libbal.a_SOURCES := $(libbal.a_SOURCES)
libbal.a_GENHEADERS := $(libbal.a_GENHEADERS)
libbal.a_CPPFLAGS := $(libbal.a_CPPFLAGS)
libbal.a_CFLAGS := $(libbal.a_CFLAGS)
libbal.a_CXXFLAGS := $(libbal.a_CXXFLAGS)
libbal.a_LDFLAGS := $(libbal.a_LDFLAGS)
libbal.a_ARFLAGS := $(libbal.a_ARFLAGS)

libbridge.a_SOURCES := $(libbridge.a_SOURCES)
libbridge.a_GENHEADERS := $(libbridge.a_GENHEADERS)
libbridge.a_CPPFLAGS := $(libbridge.a_CPPFLAGS)
libbridge.a_CFLAGS := $(libbridge.a_CFLAGS)
libbridge.a_CXXFLAGS := $(libbridge.a_CXXFLAGS)
libbridge.a_LDFLAGS := $(libbridge.a_LDFLAGS)
libbridge.a_ARFLAGS := $(libbridge.a_ARFLAGS)

libnetconfd.a_SOURCES := $(libnetconfd.a_SOURCES)
libnetconfd.a_GENHEADERS := $(libnetconfd.a_GENHEADERS)
libnetconfd.a_CPPFLAGS := $(libnetconfd.a_CPPFLAGS)
libnetconfd.a_CFLAGS := $(libnetconfd.a_CFLAGS)
libnetconfd.a_CXXFLAGS := $(libnetconfd.a_CXXFLAGS)
libnetconfd.a_LDFLAGS := $(libnetconfd.a_LDFLAGS)
libnetconfd.a_ARFLAGS := $(libnetconfd.a_ARFLAGS)

libnetconfddbus.a_SOURCES := $(libnetconfddbus.a_SOURCES)
libnetconfddbus.a_GENHEADERS := $(libnetconfddbus.a_GENHEADERS)
libnetconfddbus.a_CPPFLAGS := $(libnetconfddbus.a_CPPFLAGS)
libnetconfddbus.a_CFLAGS := $(libnetconfddbus.a_CFLAGS)
libnetconfddbus.a_CXXFLAGS := $(libnetconfddbus.a_CXXFLAGS)
libnetconfddbus.a_LDFLAGS := $(libnetconfddbus.a_LDFLAGS)
libnetconfddbus.a_ARFLAGS := $(libnetconfddbus.a_ARFLAGS)

bridgeconfig_dsa.elf_SOURCES := $(bridgeconfig_dsa.elf_SOURCES)
bridgeconfig_dsa.elf_GENHEADERS := $(bridgeconfig_dsa.elf_GENHEADERS)
bridgeconfig_dsa.elf_CPPFLAGS := $(bridgeconfig_dsa.elf_CPPFLAGS)
bridgeconfig_dsa.elf_CFLAGS := $(bridgeconfig_dsa.elf_CFLAGS)
bridgeconfig_dsa.elf_CXXFLAGS := $(bridgeconfig_dsa.elf_CXXFLAGS)
bridgeconfig_dsa.elf_LDFLAGS := $(bridgeconfig_dsa.elf_LDFLAGS)
bridgeconfig_dsa.elf_ARFLAGS := $(bridgeconfig_dsa.elf_ARFLAGS)

libutility.a_SOURCES := $(libutility.a_SOURCES)
libutility.a_GENHEADERS := $(libutility.a_GENHEADERS)
libutility.a_CPPFLAGS := $(libutility.a_CPPFLAGS)
libutility.a_CFLAGS := $(libutility.a_CFLAGS)
libutility.a_CXXFLAGS := $(libutility.a_CXXFLAGS)
libutility.a_LDFLAGS := $(libutility.a_LDFLAGS)
libutility.a_ARFLAGS := $(libutility.a_ARFLAGS)

bal_tests.elf_SOURCES := $(bal_tests.elf_SOURCES)
bal_tests.elf_GENHEADERS := $(bal_tests.elf_GENHEADERS)
bal_tests.elf_CPPFLAGS := $(bal_tests.elf_CPPFLAGS)
bal_tests.elf_CFLAGS := $(bal_tests.elf_CFLAGS)
bal_tests.elf_CXXFLAGS := $(bal_tests.elf_CXXFLAGS)
bal_tests.elf_LDFLAGS := $(bal_tests.elf_LDFLAGS)
bal_tests.elf_ARFLAGS := $(bal_tests.elf_ARFLAGS)

netconfd_tests.elf_SOURCES := $(netconfd_tests.elf_SOURCES)
netconfd_tests.elf_GENHEADERS := $(netconfd_tests.elf_GENHEADERS)
netconfd_tests.elf_CPPFLAGS := $(netconfd_tests.elf_CPPFLAGS)
netconfd_tests.elf_CFLAGS := $(netconfd_tests.elf_CFLAGS)
netconfd_tests.elf_CXXFLAGS := $(netconfd_tests.elf_CXXFLAGS)
netconfd_tests.elf_LDFLAGS := $(netconfd_tests.elf_LDFLAGS)
netconfd_tests.elf_ARFLAGS := $(netconfd_tests.elf_ARFLAGS)

utility_tests.elf_SOURCES := $(utility_tests.elf_SOURCES)
utility_tests.elf_GENHEADERS := $(utility_tests.elf_GENHEADERS)
utility_tests.elf_CPPFLAGS := $(utility_tests.elf_CPPFLAGS)
utility_tests.elf_CFLAGS := $(utility_tests.elf_CFLAGS)
utility_tests.elf_CXXFLAGS := $(utility_tests.elf_CXXFLAGS)
utility_tests.elf_LDFLAGS := $(utility_tests.elf_LDFLAGS)
utility_tests.elf_ARFLAGS := $(utility_tests.elf_ARFLAGS)

OBJECT_FILES := $(OBJECT_FILES)
DEP_FILES := $(DEP_FILES)
OUT_DIRS := $(call sort,$(OUT_DIR)/ $(dir $(OBJECT_FILES)) $(OUT_DIRS))
GEN_DIRS := $(call sort,$(BIN_DIR)/ $(OUT_DIRS) $(GEN_DIRS))

$(DEP_FILES) : | $(GEN_DIRS)

-include $(DEP_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(CLANG_TIDY_RULESET)),$(error CLANG_TIDY_RULESET has "$(CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(CLANG_TIDY_RULESET))"))

netconfd.elf_LINT_SOURCES ?= $(filter-out $(netconfd.elf_NOLINT_SOURCES),$(netconfd.elf_SOURCES))
OBJS_netconfd.elf := $(call objs,$(netconfd.elf_SOURCES),netconfd.elf/)
DEPS_netconfd.elf := $(call deps,$(netconfd.elf_SOURCES),netconfd.elf/)
LOBS_netconfd.elf := $(call lobs,$(netconfd.elf_LINT_SOURCES),netconfd.elf/)
TIDYS_netconfd.elf := $(call tidys,$(netconfd.elf_LINT_SOURCES),netconfd.elf/)
$(TIDYS_netconfd.elf): $(netconfd.elf_PRECLANG_FILES)
PLINTS_netconfd.elf := $(call plints,$(netconfd.elf_LINT_SOURCES),netconfd.elf/)
$(PLINTS_netconfd.elf): $(netconfd.elf_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(netconfd.elf_CLANG_TIDY_RULESET)),$(error netconfd.elf_CLANG_TIDY_RULESET has "$(netconfd.elf_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(netconfd.elf_CLANG_TIDY_RULESET))"))

netconfd.elf: $(BIN_DIR)/netconfd.elf

$(DEPS_netconfd.elf) $(OBJS_netconfd.elf) $(LOBS_netconfd.elf) $(TIDYS_netconfd.elf) $(PLINTS_netconfd.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/netconfd.elf/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(netconfd.elf_CPPFLAGS)

$(DEPS_netconfd.elf) $(OBJS_netconfd.elf) $(TIDYS_netconfd.elf) $(PLINTS_netconfd.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/netconfd.elf/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(netconfd.elf_CXXFLAGS)

$(DEPS_netconfd.elf) $(OBJS_netconfd.elf) $(TIDYS_netconfd.elf) $(PLINTS_netconfd.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/netconfd.elf/lint_mac.h: BUILDTARGET_CFLAGS:=$(netconfd.elf_CFLAGS)

lint-netconfd.elf: flexelint-netconfd.elf clang-tidy-netconfd.elf

$(LOBS_netconfd.elf): BUILDTARGET_LINTFLAGS:=$(netconfd.elf_LINTFLAGS)

flexelint-netconfd.elf: $($(netconfd.elf_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(netconfd.elf_DISABLE_LINT)$(DISABLE_LINT)LOBS_netconfd.elf)

$(TIDYS_netconfd.elf): BUILDTARGET_TIDYFLAGS:=$(netconfd.elf_TIDYFLAGS) -isystem $(OUT_DIR)/netconfd.elf -include lint_mac.h

$(TIDYS_netconfd.elf): netconfd.elf_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_netconfd.elf): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(netconfd.elf_CLANG_TIDY_RULESET) $(netconfd.elf_CLANG_TIDY_CHECKS)))

$(PLINTS_netconfd.elf): BUILDTARGET_PLINTFLAGS:=$(netconfd.elf_PLINTSFLAGS) -isystem $(OUT_DIR)/netconfd.elf -include lint_mac.h

clang-tidy-netconfd.elf: $($(netconfd.elf_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(netconfd.elf_DISABLE_LINT)$(DISABLE_LINT)TIDYS_netconfd.elf)

clean-clang-tidy-netconfd.elf:; $(SILENT)rm --force $(TIDYS_netconfd.elf)

clangsa-netconfd.elf: $(PLINTS_netconfd.elf)

info-txt-netconfd.elf: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,netconfd.elf,$<)

$(OUT_DIR)/netconfd.elf/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(netconfd.elf_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/netconfd.elf/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(netconfd.elf_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/netconfd.elf/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(netconfd.elf_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

netconfd.elf_PRECLANG_FILES=$(OUT_DIR)/netconfd.elf/lint_mac.h

$(OUT_DIR)/netconfd.elf/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(netconfd.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/netconfd.elf/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/netconfd.elf/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(netconfd.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/netconfd.elf/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



libbal.a_LINT_SOURCES ?= $(filter-out $(libbal.a_NOLINT_SOURCES),$(libbal.a_SOURCES))
OBJS_libbal.a := $(call objs,$(libbal.a_SOURCES),libbal.a/)
DEPS_libbal.a := $(call deps,$(libbal.a_SOURCES),libbal.a/)
LOBS_libbal.a := $(call lobs,$(libbal.a_LINT_SOURCES),libbal.a/)
TIDYS_libbal.a := $(call tidys,$(libbal.a_LINT_SOURCES),libbal.a/)
$(TIDYS_libbal.a): $(libbal.a_PRECLANG_FILES)
PLINTS_libbal.a := $(call plints,$(libbal.a_LINT_SOURCES),libbal.a/)
$(PLINTS_libbal.a): $(libbal.a_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(libbal.a_CLANG_TIDY_RULESET)),$(error libbal.a_CLANG_TIDY_RULESET has "$(libbal.a_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(libbal.a_CLANG_TIDY_RULESET))"))

libbal.a: $(BIN_DIR)/libbal.a

$(DEPS_libbal.a) $(OBJS_libbal.a) $(LOBS_libbal.a) $(TIDYS_libbal.a) $(PLINTS_libbal.a) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libbal.a/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(libbal.a_CPPFLAGS)

$(DEPS_libbal.a) $(OBJS_libbal.a) $(TIDYS_libbal.a) $(PLINTS_libbal.a) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libbal.a/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(libbal.a_CXXFLAGS)

$(DEPS_libbal.a) $(OBJS_libbal.a) $(TIDYS_libbal.a) $(PLINTS_libbal.a) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libbal.a/lint_mac.h: BUILDTARGET_CFLAGS:=$(libbal.a_CFLAGS)

lint-libbal.a: flexelint-libbal.a clang-tidy-libbal.a

$(LOBS_libbal.a): BUILDTARGET_LINTFLAGS:=$(libbal.a_LINTFLAGS)

flexelint-libbal.a: $($(libbal.a_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(libbal.a_DISABLE_LINT)$(DISABLE_LINT)LOBS_libbal.a)

$(TIDYS_libbal.a): BUILDTARGET_TIDYFLAGS:=$(libbal.a_TIDYFLAGS) -isystem $(OUT_DIR)/libbal.a -include lint_mac.h

$(TIDYS_libbal.a): libbal.a_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_libbal.a): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(libbal.a_CLANG_TIDY_RULESET) $(libbal.a_CLANG_TIDY_CHECKS)))

$(PLINTS_libbal.a): BUILDTARGET_PLINTFLAGS:=$(libbal.a_PLINTSFLAGS) -isystem $(OUT_DIR)/libbal.a -include lint_mac.h

clang-tidy-libbal.a: $($(libbal.a_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(libbal.a_DISABLE_LINT)$(DISABLE_LINT)TIDYS_libbal.a)

clean-clang-tidy-libbal.a:; $(SILENT)rm --force $(TIDYS_libbal.a)

clangsa-libbal.a: $(PLINTS_libbal.a)

info-txt-libbal.a: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,libbal.a,$<)

$(OUT_DIR)/libbal.a/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(libbal.a_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/libbal.a/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(libbal.a_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/libbal.a/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(libbal.a_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

libbal.a_PRECLANG_FILES=$(OUT_DIR)/libbal.a/lint_mac.h

$(OUT_DIR)/libbal.a/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(libbal.a_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libbal.a/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/libbal.a/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(libbal.a_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libbal.a/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



libbridge.a_LINT_SOURCES ?= $(filter-out $(libbridge.a_NOLINT_SOURCES),$(libbridge.a_SOURCES))
OBJS_libbridge.a := $(call objs,$(libbridge.a_SOURCES),libbridge.a/)
DEPS_libbridge.a := $(call deps,$(libbridge.a_SOURCES),libbridge.a/)
LOBS_libbridge.a := $(call lobs,$(libbridge.a_LINT_SOURCES),libbridge.a/)
TIDYS_libbridge.a := $(call tidys,$(libbridge.a_LINT_SOURCES),libbridge.a/)
$(TIDYS_libbridge.a): $(libbridge.a_PRECLANG_FILES)
PLINTS_libbridge.a := $(call plints,$(libbridge.a_LINT_SOURCES),libbridge.a/)
$(PLINTS_libbridge.a): $(libbridge.a_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(libbridge.a_CLANG_TIDY_RULESET)),$(error libbridge.a_CLANG_TIDY_RULESET has "$(libbridge.a_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(libbridge.a_CLANG_TIDY_RULESET))"))

libbridge.a: $(BIN_DIR)/libbridge.a

$(DEPS_libbridge.a) $(OBJS_libbridge.a) $(LOBS_libbridge.a) $(TIDYS_libbridge.a) $(PLINTS_libbridge.a) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libbridge.a/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(libbridge.a_CPPFLAGS)

$(DEPS_libbridge.a) $(OBJS_libbridge.a) $(TIDYS_libbridge.a) $(PLINTS_libbridge.a) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libbridge.a/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(libbridge.a_CXXFLAGS)

$(DEPS_libbridge.a) $(OBJS_libbridge.a) $(TIDYS_libbridge.a) $(PLINTS_libbridge.a) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libbridge.a/lint_mac.h: BUILDTARGET_CFLAGS:=$(libbridge.a_CFLAGS)

lint-libbridge.a: flexelint-libbridge.a clang-tidy-libbridge.a

$(LOBS_libbridge.a): BUILDTARGET_LINTFLAGS:=$(libbridge.a_LINTFLAGS)

flexelint-libbridge.a: $($(libbridge.a_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(libbridge.a_DISABLE_LINT)$(DISABLE_LINT)LOBS_libbridge.a)

$(TIDYS_libbridge.a): BUILDTARGET_TIDYFLAGS:=$(libbridge.a_TIDYFLAGS) -isystem $(OUT_DIR)/libbridge.a -include lint_mac.h

$(TIDYS_libbridge.a): libbridge.a_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_libbridge.a): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(libbridge.a_CLANG_TIDY_RULESET) $(libbridge.a_CLANG_TIDY_CHECKS)))

$(PLINTS_libbridge.a): BUILDTARGET_PLINTFLAGS:=$(libbridge.a_PLINTSFLAGS) -isystem $(OUT_DIR)/libbridge.a -include lint_mac.h

clang-tidy-libbridge.a: $($(libbridge.a_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(libbridge.a_DISABLE_LINT)$(DISABLE_LINT)TIDYS_libbridge.a)

clean-clang-tidy-libbridge.a:; $(SILENT)rm --force $(TIDYS_libbridge.a)

clangsa-libbridge.a: $(PLINTS_libbridge.a)

info-txt-libbridge.a: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,libbridge.a,$<)

$(OUT_DIR)/libbridge.a/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(libbridge.a_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/libbridge.a/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(libbridge.a_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/libbridge.a/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(libbridge.a_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

libbridge.a_PRECLANG_FILES=$(OUT_DIR)/libbridge.a/lint_mac.h

$(OUT_DIR)/libbridge.a/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(libbridge.a_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libbridge.a/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/libbridge.a/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(libbridge.a_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libbridge.a/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



libnetconfd.a_LINT_SOURCES ?= $(filter-out $(libnetconfd.a_NOLINT_SOURCES),$(libnetconfd.a_SOURCES))
OBJS_libnetconfd.a := $(call objs,$(libnetconfd.a_SOURCES),libnetconfd.a/)
DEPS_libnetconfd.a := $(call deps,$(libnetconfd.a_SOURCES),libnetconfd.a/)
LOBS_libnetconfd.a := $(call lobs,$(libnetconfd.a_LINT_SOURCES),libnetconfd.a/)
TIDYS_libnetconfd.a := $(call tidys,$(libnetconfd.a_LINT_SOURCES),libnetconfd.a/)
$(TIDYS_libnetconfd.a): $(libnetconfd.a_PRECLANG_FILES)
PLINTS_libnetconfd.a := $(call plints,$(libnetconfd.a_LINT_SOURCES),libnetconfd.a/)
$(PLINTS_libnetconfd.a): $(libnetconfd.a_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(libnetconfd.a_CLANG_TIDY_RULESET)),$(error libnetconfd.a_CLANG_TIDY_RULESET has "$(libnetconfd.a_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(libnetconfd.a_CLANG_TIDY_RULESET))"))

libnetconfd.a: $(BIN_DIR)/libnetconfd.a

$(DEPS_libnetconfd.a) $(OBJS_libnetconfd.a) $(LOBS_libnetconfd.a) $(TIDYS_libnetconfd.a) $(PLINTS_libnetconfd.a) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libnetconfd.a/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(libnetconfd.a_CPPFLAGS)

$(DEPS_libnetconfd.a) $(OBJS_libnetconfd.a) $(TIDYS_libnetconfd.a) $(PLINTS_libnetconfd.a) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libnetconfd.a/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(libnetconfd.a_CXXFLAGS)

$(DEPS_libnetconfd.a) $(OBJS_libnetconfd.a) $(TIDYS_libnetconfd.a) $(PLINTS_libnetconfd.a) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libnetconfd.a/lint_mac.h: BUILDTARGET_CFLAGS:=$(libnetconfd.a_CFLAGS)

lint-libnetconfd.a: flexelint-libnetconfd.a clang-tidy-libnetconfd.a

$(LOBS_libnetconfd.a): BUILDTARGET_LINTFLAGS:=$(libnetconfd.a_LINTFLAGS)

flexelint-libnetconfd.a: $($(libnetconfd.a_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(libnetconfd.a_DISABLE_LINT)$(DISABLE_LINT)LOBS_libnetconfd.a)

$(TIDYS_libnetconfd.a): BUILDTARGET_TIDYFLAGS:=$(libnetconfd.a_TIDYFLAGS) -isystem $(OUT_DIR)/libnetconfd.a -include lint_mac.h

$(TIDYS_libnetconfd.a): libnetconfd.a_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_libnetconfd.a): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(libnetconfd.a_CLANG_TIDY_RULESET) $(libnetconfd.a_CLANG_TIDY_CHECKS)))

$(PLINTS_libnetconfd.a): BUILDTARGET_PLINTFLAGS:=$(libnetconfd.a_PLINTSFLAGS) -isystem $(OUT_DIR)/libnetconfd.a -include lint_mac.h

clang-tidy-libnetconfd.a: $($(libnetconfd.a_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(libnetconfd.a_DISABLE_LINT)$(DISABLE_LINT)TIDYS_libnetconfd.a)

clean-clang-tidy-libnetconfd.a:; $(SILENT)rm --force $(TIDYS_libnetconfd.a)

clangsa-libnetconfd.a: $(PLINTS_libnetconfd.a)

info-txt-libnetconfd.a: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,libnetconfd.a,$<)

$(OUT_DIR)/libnetconfd.a/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(libnetconfd.a_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/libnetconfd.a/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(libnetconfd.a_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/libnetconfd.a/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(libnetconfd.a_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

libnetconfd.a_PRECLANG_FILES=$(OUT_DIR)/libnetconfd.a/lint_mac.h

$(OUT_DIR)/libnetconfd.a/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(libnetconfd.a_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libnetconfd.a/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/libnetconfd.a/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(libnetconfd.a_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libnetconfd.a/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



libnetconfddbus.a_LINT_SOURCES ?= $(filter-out $(libnetconfddbus.a_NOLINT_SOURCES),$(libnetconfddbus.a_SOURCES))
OBJS_libnetconfddbus.a := $(call objs,$(libnetconfddbus.a_SOURCES),libnetconfddbus.a/)
DEPS_libnetconfddbus.a := $(call deps,$(libnetconfddbus.a_SOURCES),libnetconfddbus.a/)
LOBS_libnetconfddbus.a := $(call lobs,$(libnetconfddbus.a_LINT_SOURCES),libnetconfddbus.a/)
TIDYS_libnetconfddbus.a := $(call tidys,$(libnetconfddbus.a_LINT_SOURCES),libnetconfddbus.a/)
$(TIDYS_libnetconfddbus.a): $(libnetconfddbus.a_PRECLANG_FILES)
PLINTS_libnetconfddbus.a := $(call plints,$(libnetconfddbus.a_LINT_SOURCES),libnetconfddbus.a/)
$(PLINTS_libnetconfddbus.a): $(libnetconfddbus.a_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(libnetconfddbus.a_CLANG_TIDY_RULESET)),$(error libnetconfddbus.a_CLANG_TIDY_RULESET has "$(libnetconfddbus.a_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(libnetconfddbus.a_CLANG_TIDY_RULESET))"))

libnetconfddbus.a: $(BIN_DIR)/libnetconfddbus.a

$(DEPS_libnetconfddbus.a) $(OBJS_libnetconfddbus.a) $(LOBS_libnetconfddbus.a) $(TIDYS_libnetconfddbus.a) $(PLINTS_libnetconfddbus.a) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libnetconfddbus.a/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(libnetconfddbus.a_CPPFLAGS)

$(DEPS_libnetconfddbus.a) $(OBJS_libnetconfddbus.a) $(TIDYS_libnetconfddbus.a) $(PLINTS_libnetconfddbus.a) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libnetconfddbus.a/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(libnetconfddbus.a_CXXFLAGS)

$(DEPS_libnetconfddbus.a) $(OBJS_libnetconfddbus.a) $(TIDYS_libnetconfddbus.a) $(PLINTS_libnetconfddbus.a) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libnetconfddbus.a/lint_mac.h: BUILDTARGET_CFLAGS:=$(libnetconfddbus.a_CFLAGS)

lint-libnetconfddbus.a: flexelint-libnetconfddbus.a clang-tidy-libnetconfddbus.a

$(LOBS_libnetconfddbus.a): BUILDTARGET_LINTFLAGS:=$(libnetconfddbus.a_LINTFLAGS)

flexelint-libnetconfddbus.a: $($(libnetconfddbus.a_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(libnetconfddbus.a_DISABLE_LINT)$(DISABLE_LINT)LOBS_libnetconfddbus.a)

$(TIDYS_libnetconfddbus.a): BUILDTARGET_TIDYFLAGS:=$(libnetconfddbus.a_TIDYFLAGS) -isystem $(OUT_DIR)/libnetconfddbus.a -include lint_mac.h

$(TIDYS_libnetconfddbus.a): libnetconfddbus.a_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_libnetconfddbus.a): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(libnetconfddbus.a_CLANG_TIDY_RULESET) $(libnetconfddbus.a_CLANG_TIDY_CHECKS)))

$(PLINTS_libnetconfddbus.a): BUILDTARGET_PLINTFLAGS:=$(libnetconfddbus.a_PLINTSFLAGS) -isystem $(OUT_DIR)/libnetconfddbus.a -include lint_mac.h

clang-tidy-libnetconfddbus.a: $($(libnetconfddbus.a_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(libnetconfddbus.a_DISABLE_LINT)$(DISABLE_LINT)TIDYS_libnetconfddbus.a)

clean-clang-tidy-libnetconfddbus.a:; $(SILENT)rm --force $(TIDYS_libnetconfddbus.a)

clangsa-libnetconfddbus.a: $(PLINTS_libnetconfddbus.a)

info-txt-libnetconfddbus.a: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,libnetconfddbus.a,$<)

$(OUT_DIR)/libnetconfddbus.a/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(libnetconfddbus.a_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/libnetconfddbus.a/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(libnetconfddbus.a_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/libnetconfddbus.a/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(libnetconfddbus.a_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

libnetconfddbus.a_PRECLANG_FILES=$(OUT_DIR)/libnetconfddbus.a/lint_mac.h

$(OUT_DIR)/libnetconfddbus.a/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(libnetconfddbus.a_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libnetconfddbus.a/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/libnetconfddbus.a/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(libnetconfddbus.a_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libnetconfddbus.a/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



bridgeconfig_dsa.elf_LINT_SOURCES ?= $(filter-out $(bridgeconfig_dsa.elf_NOLINT_SOURCES),$(bridgeconfig_dsa.elf_SOURCES))
OBJS_bridgeconfig_dsa.elf := $(call objs,$(bridgeconfig_dsa.elf_SOURCES),bridgeconfig_dsa.elf/)
DEPS_bridgeconfig_dsa.elf := $(call deps,$(bridgeconfig_dsa.elf_SOURCES),bridgeconfig_dsa.elf/)
LOBS_bridgeconfig_dsa.elf := $(call lobs,$(bridgeconfig_dsa.elf_LINT_SOURCES),bridgeconfig_dsa.elf/)
TIDYS_bridgeconfig_dsa.elf := $(call tidys,$(bridgeconfig_dsa.elf_LINT_SOURCES),bridgeconfig_dsa.elf/)
$(TIDYS_bridgeconfig_dsa.elf): $(bridgeconfig_dsa.elf_PRECLANG_FILES)
PLINTS_bridgeconfig_dsa.elf := $(call plints,$(bridgeconfig_dsa.elf_LINT_SOURCES),bridgeconfig_dsa.elf/)
$(PLINTS_bridgeconfig_dsa.elf): $(bridgeconfig_dsa.elf_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(bridgeconfig_dsa.elf_CLANG_TIDY_RULESET)),$(error bridgeconfig_dsa.elf_CLANG_TIDY_RULESET has "$(bridgeconfig_dsa.elf_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(bridgeconfig_dsa.elf_CLANG_TIDY_RULESET))"))

bridgeconfig_dsa.elf: $(BIN_DIR)/bridgeconfig_dsa.elf

$(DEPS_bridgeconfig_dsa.elf) $(OBJS_bridgeconfig_dsa.elf) $(LOBS_bridgeconfig_dsa.elf) $(TIDYS_bridgeconfig_dsa.elf) $(PLINTS_bridgeconfig_dsa.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/bridgeconfig_dsa.elf/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(bridgeconfig_dsa.elf_CPPFLAGS)

$(DEPS_bridgeconfig_dsa.elf) $(OBJS_bridgeconfig_dsa.elf) $(TIDYS_bridgeconfig_dsa.elf) $(PLINTS_bridgeconfig_dsa.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/bridgeconfig_dsa.elf/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(bridgeconfig_dsa.elf_CXXFLAGS)

$(DEPS_bridgeconfig_dsa.elf) $(OBJS_bridgeconfig_dsa.elf) $(TIDYS_bridgeconfig_dsa.elf) $(PLINTS_bridgeconfig_dsa.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/bridgeconfig_dsa.elf/lint_mac.h: BUILDTARGET_CFLAGS:=$(bridgeconfig_dsa.elf_CFLAGS)

lint-bridgeconfig_dsa.elf: flexelint-bridgeconfig_dsa.elf clang-tidy-bridgeconfig_dsa.elf

$(LOBS_bridgeconfig_dsa.elf): BUILDTARGET_LINTFLAGS:=$(bridgeconfig_dsa.elf_LINTFLAGS)

flexelint-bridgeconfig_dsa.elf: $($(bridgeconfig_dsa.elf_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(bridgeconfig_dsa.elf_DISABLE_LINT)$(DISABLE_LINT)LOBS_bridgeconfig_dsa.elf)

$(TIDYS_bridgeconfig_dsa.elf): BUILDTARGET_TIDYFLAGS:=$(bridgeconfig_dsa.elf_TIDYFLAGS) -isystem $(OUT_DIR)/bridgeconfig_dsa.elf -include lint_mac.h

$(TIDYS_bridgeconfig_dsa.elf): bridgeconfig_dsa.elf_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_bridgeconfig_dsa.elf): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(bridgeconfig_dsa.elf_CLANG_TIDY_RULESET) $(bridgeconfig_dsa.elf_CLANG_TIDY_CHECKS)))

$(PLINTS_bridgeconfig_dsa.elf): BUILDTARGET_PLINTFLAGS:=$(bridgeconfig_dsa.elf_PLINTSFLAGS) -isystem $(OUT_DIR)/bridgeconfig_dsa.elf -include lint_mac.h

clang-tidy-bridgeconfig_dsa.elf: $($(bridgeconfig_dsa.elf_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(bridgeconfig_dsa.elf_DISABLE_LINT)$(DISABLE_LINT)TIDYS_bridgeconfig_dsa.elf)

clean-clang-tidy-bridgeconfig_dsa.elf:; $(SILENT)rm --force $(TIDYS_bridgeconfig_dsa.elf)

clangsa-bridgeconfig_dsa.elf: $(PLINTS_bridgeconfig_dsa.elf)

info-txt-bridgeconfig_dsa.elf: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,bridgeconfig_dsa.elf,$<)

$(OUT_DIR)/bridgeconfig_dsa.elf/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(bridgeconfig_dsa.elf_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/bridgeconfig_dsa.elf/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(bridgeconfig_dsa.elf_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/bridgeconfig_dsa.elf/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(bridgeconfig_dsa.elf_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

bridgeconfig_dsa.elf_PRECLANG_FILES=$(OUT_DIR)/bridgeconfig_dsa.elf/lint_mac.h

$(OUT_DIR)/bridgeconfig_dsa.elf/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(bridgeconfig_dsa.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/bridgeconfig_dsa.elf/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/bridgeconfig_dsa.elf/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(bridgeconfig_dsa.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/bridgeconfig_dsa.elf/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



libutility.a_LINT_SOURCES ?= $(filter-out $(libutility.a_NOLINT_SOURCES),$(libutility.a_SOURCES))
OBJS_libutility.a := $(call objs,$(libutility.a_SOURCES),libutility.a/)
DEPS_libutility.a := $(call deps,$(libutility.a_SOURCES),libutility.a/)
LOBS_libutility.a := $(call lobs,$(libutility.a_LINT_SOURCES),libutility.a/)
TIDYS_libutility.a := $(call tidys,$(libutility.a_LINT_SOURCES),libutility.a/)
$(TIDYS_libutility.a): $(libutility.a_PRECLANG_FILES)
PLINTS_libutility.a := $(call plints,$(libutility.a_LINT_SOURCES),libutility.a/)
$(PLINTS_libutility.a): $(libutility.a_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(libutility.a_CLANG_TIDY_RULESET)),$(error libutility.a_CLANG_TIDY_RULESET has "$(libutility.a_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(libutility.a_CLANG_TIDY_RULESET))"))

libutility.a: $(BIN_DIR)/libutility.a

$(DEPS_libutility.a) $(OBJS_libutility.a) $(LOBS_libutility.a) $(TIDYS_libutility.a) $(PLINTS_libutility.a) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libutility.a/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(libutility.a_CPPFLAGS)

$(DEPS_libutility.a) $(OBJS_libutility.a) $(TIDYS_libutility.a) $(PLINTS_libutility.a) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libutility.a/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(libutility.a_CXXFLAGS)

$(DEPS_libutility.a) $(OBJS_libutility.a) $(TIDYS_libutility.a) $(PLINTS_libutility.a) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libutility.a/lint_mac.h: BUILDTARGET_CFLAGS:=$(libutility.a_CFLAGS)

lint-libutility.a: flexelint-libutility.a clang-tidy-libutility.a

$(LOBS_libutility.a): BUILDTARGET_LINTFLAGS:=$(libutility.a_LINTFLAGS)

flexelint-libutility.a: $($(libutility.a_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(libutility.a_DISABLE_LINT)$(DISABLE_LINT)LOBS_libutility.a)

$(TIDYS_libutility.a): BUILDTARGET_TIDYFLAGS:=$(libutility.a_TIDYFLAGS) -isystem $(OUT_DIR)/libutility.a -include lint_mac.h

$(TIDYS_libutility.a): libutility.a_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_libutility.a): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(libutility.a_CLANG_TIDY_RULESET) $(libutility.a_CLANG_TIDY_CHECKS)))

$(PLINTS_libutility.a): BUILDTARGET_PLINTFLAGS:=$(libutility.a_PLINTSFLAGS) -isystem $(OUT_DIR)/libutility.a -include lint_mac.h

clang-tidy-libutility.a: $($(libutility.a_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(libutility.a_DISABLE_LINT)$(DISABLE_LINT)TIDYS_libutility.a)

clean-clang-tidy-libutility.a:; $(SILENT)rm --force $(TIDYS_libutility.a)

clangsa-libutility.a: $(PLINTS_libutility.a)

info-txt-libutility.a: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,libutility.a,$<)

$(OUT_DIR)/libutility.a/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(libutility.a_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/libutility.a/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(libutility.a_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/libutility.a/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(libutility.a_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

libutility.a_PRECLANG_FILES=$(OUT_DIR)/libutility.a/lint_mac.h

$(OUT_DIR)/libutility.a/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(libutility.a_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libutility.a/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/libutility.a/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(libutility.a_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libutility.a/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



bal_tests.elf_LINT_SOURCES ?= $(filter-out $(bal_tests.elf_NOLINT_SOURCES),$(bal_tests.elf_SOURCES))
OBJS_bal_tests.elf := $(call objs,$(bal_tests.elf_SOURCES),bal_tests.elf/)
DEPS_bal_tests.elf := $(call deps,$(bal_tests.elf_SOURCES),bal_tests.elf/)
LOBS_bal_tests.elf := $(call lobs,$(bal_tests.elf_LINT_SOURCES),bal_tests.elf/)
TIDYS_bal_tests.elf := $(call tidys,$(bal_tests.elf_LINT_SOURCES),bal_tests.elf/)
$(TIDYS_bal_tests.elf): $(bal_tests.elf_PRECLANG_FILES)
PLINTS_bal_tests.elf := $(call plints,$(bal_tests.elf_LINT_SOURCES),bal_tests.elf/)
$(PLINTS_bal_tests.elf): $(bal_tests.elf_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(bal_tests.elf_CLANG_TIDY_RULESET)),$(error bal_tests.elf_CLANG_TIDY_RULESET has "$(bal_tests.elf_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(bal_tests.elf_CLANG_TIDY_RULESET))"))

bal_tests.elf: $(BIN_DIR)/bal_tests.elf

$(DEPS_bal_tests.elf) $(OBJS_bal_tests.elf) $(LOBS_bal_tests.elf) $(TIDYS_bal_tests.elf) $(PLINTS_bal_tests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/bal_tests.elf/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(bal_tests.elf_CPPFLAGS)

$(DEPS_bal_tests.elf) $(OBJS_bal_tests.elf) $(TIDYS_bal_tests.elf) $(PLINTS_bal_tests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/bal_tests.elf/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(bal_tests.elf_CXXFLAGS)

$(DEPS_bal_tests.elf) $(OBJS_bal_tests.elf) $(TIDYS_bal_tests.elf) $(PLINTS_bal_tests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/bal_tests.elf/lint_mac.h: BUILDTARGET_CFLAGS:=$(bal_tests.elf_CFLAGS)

lint-bal_tests.elf: flexelint-bal_tests.elf clang-tidy-bal_tests.elf

$(LOBS_bal_tests.elf): BUILDTARGET_LINTFLAGS:=$(bal_tests.elf_LINTFLAGS)

flexelint-bal_tests.elf: $($(bal_tests.elf_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(bal_tests.elf_DISABLE_LINT)$(DISABLE_LINT)LOBS_bal_tests.elf)

$(TIDYS_bal_tests.elf): BUILDTARGET_TIDYFLAGS:=$(bal_tests.elf_TIDYFLAGS) -isystem $(OUT_DIR)/bal_tests.elf -include lint_mac.h

$(TIDYS_bal_tests.elf): bal_tests.elf_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_bal_tests.elf): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(bal_tests.elf_CLANG_TIDY_RULESET) $(bal_tests.elf_CLANG_TIDY_CHECKS)))

$(PLINTS_bal_tests.elf): BUILDTARGET_PLINTFLAGS:=$(bal_tests.elf_PLINTSFLAGS) -isystem $(OUT_DIR)/bal_tests.elf -include lint_mac.h

clang-tidy-bal_tests.elf: $($(bal_tests.elf_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(bal_tests.elf_DISABLE_LINT)$(DISABLE_LINT)TIDYS_bal_tests.elf)

clean-clang-tidy-bal_tests.elf:; $(SILENT)rm --force $(TIDYS_bal_tests.elf)

clangsa-bal_tests.elf: $(PLINTS_bal_tests.elf)

info-txt-bal_tests.elf: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,bal_tests.elf,$<)

$(OUT_DIR)/bal_tests.elf/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(bal_tests.elf_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/bal_tests.elf/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(bal_tests.elf_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/bal_tests.elf/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(bal_tests.elf_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

bal_tests.elf_PRECLANG_FILES=$(OUT_DIR)/bal_tests.elf/lint_mac.h

$(OUT_DIR)/bal_tests.elf/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(bal_tests.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/bal_tests.elf/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/bal_tests.elf/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(bal_tests.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/bal_tests.elf/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



netconfd_tests.elf_LINT_SOURCES ?= $(filter-out $(netconfd_tests.elf_NOLINT_SOURCES),$(netconfd_tests.elf_SOURCES))
OBJS_netconfd_tests.elf := $(call objs,$(netconfd_tests.elf_SOURCES),netconfd_tests.elf/)
DEPS_netconfd_tests.elf := $(call deps,$(netconfd_tests.elf_SOURCES),netconfd_tests.elf/)
LOBS_netconfd_tests.elf := $(call lobs,$(netconfd_tests.elf_LINT_SOURCES),netconfd_tests.elf/)
TIDYS_netconfd_tests.elf := $(call tidys,$(netconfd_tests.elf_LINT_SOURCES),netconfd_tests.elf/)
$(TIDYS_netconfd_tests.elf): $(netconfd_tests.elf_PRECLANG_FILES)
PLINTS_netconfd_tests.elf := $(call plints,$(netconfd_tests.elf_LINT_SOURCES),netconfd_tests.elf/)
$(PLINTS_netconfd_tests.elf): $(netconfd_tests.elf_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(netconfd_tests.elf_CLANG_TIDY_RULESET)),$(error netconfd_tests.elf_CLANG_TIDY_RULESET has "$(netconfd_tests.elf_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(netconfd_tests.elf_CLANG_TIDY_RULESET))"))

netconfd_tests.elf: $(BIN_DIR)/netconfd_tests.elf

$(DEPS_netconfd_tests.elf) $(OBJS_netconfd_tests.elf) $(LOBS_netconfd_tests.elf) $(TIDYS_netconfd_tests.elf) $(PLINTS_netconfd_tests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/netconfd_tests.elf/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(netconfd_tests.elf_CPPFLAGS)

$(DEPS_netconfd_tests.elf) $(OBJS_netconfd_tests.elf) $(TIDYS_netconfd_tests.elf) $(PLINTS_netconfd_tests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/netconfd_tests.elf/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(netconfd_tests.elf_CXXFLAGS)

$(DEPS_netconfd_tests.elf) $(OBJS_netconfd_tests.elf) $(TIDYS_netconfd_tests.elf) $(PLINTS_netconfd_tests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/netconfd_tests.elf/lint_mac.h: BUILDTARGET_CFLAGS:=$(netconfd_tests.elf_CFLAGS)

lint-netconfd_tests.elf: flexelint-netconfd_tests.elf clang-tidy-netconfd_tests.elf

$(LOBS_netconfd_tests.elf): BUILDTARGET_LINTFLAGS:=$(netconfd_tests.elf_LINTFLAGS)

flexelint-netconfd_tests.elf: $($(netconfd_tests.elf_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(netconfd_tests.elf_DISABLE_LINT)$(DISABLE_LINT)LOBS_netconfd_tests.elf)

$(TIDYS_netconfd_tests.elf): BUILDTARGET_TIDYFLAGS:=$(netconfd_tests.elf_TIDYFLAGS) -isystem $(OUT_DIR)/netconfd_tests.elf -include lint_mac.h

$(TIDYS_netconfd_tests.elf): netconfd_tests.elf_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_netconfd_tests.elf): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(netconfd_tests.elf_CLANG_TIDY_RULESET) $(netconfd_tests.elf_CLANG_TIDY_CHECKS)))

$(PLINTS_netconfd_tests.elf): BUILDTARGET_PLINTFLAGS:=$(netconfd_tests.elf_PLINTSFLAGS) -isystem $(OUT_DIR)/netconfd_tests.elf -include lint_mac.h

clang-tidy-netconfd_tests.elf: $($(netconfd_tests.elf_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(netconfd_tests.elf_DISABLE_LINT)$(DISABLE_LINT)TIDYS_netconfd_tests.elf)

clean-clang-tidy-netconfd_tests.elf:; $(SILENT)rm --force $(TIDYS_netconfd_tests.elf)

clangsa-netconfd_tests.elf: $(PLINTS_netconfd_tests.elf)

info-txt-netconfd_tests.elf: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,netconfd_tests.elf,$<)

$(OUT_DIR)/netconfd_tests.elf/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(netconfd_tests.elf_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/netconfd_tests.elf/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(netconfd_tests.elf_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/netconfd_tests.elf/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(netconfd_tests.elf_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

netconfd_tests.elf_PRECLANG_FILES=$(OUT_DIR)/netconfd_tests.elf/lint_mac.h

$(OUT_DIR)/netconfd_tests.elf/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(netconfd_tests.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/netconfd_tests.elf/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/netconfd_tests.elf/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(netconfd_tests.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/netconfd_tests.elf/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



utility_tests.elf_LINT_SOURCES ?= $(filter-out $(utility_tests.elf_NOLINT_SOURCES),$(utility_tests.elf_SOURCES))
OBJS_utility_tests.elf := $(call objs,$(utility_tests.elf_SOURCES),utility_tests.elf/)
DEPS_utility_tests.elf := $(call deps,$(utility_tests.elf_SOURCES),utility_tests.elf/)
LOBS_utility_tests.elf := $(call lobs,$(utility_tests.elf_LINT_SOURCES),utility_tests.elf/)
TIDYS_utility_tests.elf := $(call tidys,$(utility_tests.elf_LINT_SOURCES),utility_tests.elf/)
$(TIDYS_utility_tests.elf): $(utility_tests.elf_PRECLANG_FILES)
PLINTS_utility_tests.elf := $(call plints,$(utility_tests.elf_LINT_SOURCES),utility_tests.elf/)
$(PLINTS_utility_tests.elf): $(utility_tests.elf_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(utility_tests.elf_CLANG_TIDY_RULESET)),$(error utility_tests.elf_CLANG_TIDY_RULESET has "$(utility_tests.elf_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(utility_tests.elf_CLANG_TIDY_RULESET))"))

utility_tests.elf: $(BIN_DIR)/utility_tests.elf

$(DEPS_utility_tests.elf) $(OBJS_utility_tests.elf) $(LOBS_utility_tests.elf) $(TIDYS_utility_tests.elf) $(PLINTS_utility_tests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/utility_tests.elf/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(utility_tests.elf_CPPFLAGS)

$(DEPS_utility_tests.elf) $(OBJS_utility_tests.elf) $(TIDYS_utility_tests.elf) $(PLINTS_utility_tests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/utility_tests.elf/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(utility_tests.elf_CXXFLAGS)

$(DEPS_utility_tests.elf) $(OBJS_utility_tests.elf) $(TIDYS_utility_tests.elf) $(PLINTS_utility_tests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/utility_tests.elf/lint_mac.h: BUILDTARGET_CFLAGS:=$(utility_tests.elf_CFLAGS)

lint-utility_tests.elf: flexelint-utility_tests.elf clang-tidy-utility_tests.elf

$(LOBS_utility_tests.elf): BUILDTARGET_LINTFLAGS:=$(utility_tests.elf_LINTFLAGS)

flexelint-utility_tests.elf: $($(utility_tests.elf_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(utility_tests.elf_DISABLE_LINT)$(DISABLE_LINT)LOBS_utility_tests.elf)

$(TIDYS_utility_tests.elf): BUILDTARGET_TIDYFLAGS:=$(utility_tests.elf_TIDYFLAGS) -isystem $(OUT_DIR)/utility_tests.elf -include lint_mac.h

$(TIDYS_utility_tests.elf): utility_tests.elf_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_utility_tests.elf): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(utility_tests.elf_CLANG_TIDY_RULESET) $(utility_tests.elf_CLANG_TIDY_CHECKS)))

$(PLINTS_utility_tests.elf): BUILDTARGET_PLINTFLAGS:=$(utility_tests.elf_PLINTSFLAGS) -isystem $(OUT_DIR)/utility_tests.elf -include lint_mac.h

clang-tidy-utility_tests.elf: $($(utility_tests.elf_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(utility_tests.elf_DISABLE_LINT)$(DISABLE_LINT)TIDYS_utility_tests.elf)

clean-clang-tidy-utility_tests.elf:; $(SILENT)rm --force $(TIDYS_utility_tests.elf)

clangsa-utility_tests.elf: $(PLINTS_utility_tests.elf)

info-txt-utility_tests.elf: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,utility_tests.elf,$<)

$(OUT_DIR)/utility_tests.elf/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(utility_tests.elf_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/utility_tests.elf/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(utility_tests.elf_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/utility_tests.elf/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(utility_tests.elf_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

utility_tests.elf_PRECLANG_FILES=$(OUT_DIR)/utility_tests.elf/lint_mac.h

$(OUT_DIR)/utility_tests.elf/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(utility_tests.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/utility_tests.elf/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/utility_tests.elf/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(utility_tests.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/utility_tests.elf/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



$(OUT_DIR)/netconfd.elf.dbg: $(OBJS_netconfd.elf) $(MK_FILES) $(netconfd.elf_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),netconfd.elf,$@,$(OBJS_netconfd.elf),$(LDFLAGS) $(netconfd.elf_LDFLAGS))

$(OUT_DIR)/bridgeconfig_dsa.elf.dbg: $(OBJS_bridgeconfig_dsa.elf) $(MK_FILES) $(bridgeconfig_dsa.elf_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),bridgeconfig_dsa.elf,$@,$(OBJS_bridgeconfig_dsa.elf),$(LDFLAGS) $(bridgeconfig_dsa.elf_LDFLAGS))

$(OUT_DIR)/bal_tests.elf.dbg: $(OBJS_bal_tests.elf) $(MK_FILES) $(bal_tests.elf_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),bal_tests.elf,$@,$(OBJS_bal_tests.elf),$(LDFLAGS) $(bal_tests.elf_LDFLAGS))

$(OUT_DIR)/netconfd_tests.elf.dbg: $(OBJS_netconfd_tests.elf) $(MK_FILES) $(netconfd_tests.elf_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),netconfd_tests.elf,$@,$(OBJS_netconfd_tests.elf),$(LDFLAGS) $(netconfd_tests.elf_LDFLAGS))

$(OUT_DIR)/utility_tests.elf.dbg: $(OBJS_utility_tests.elf) $(MK_FILES) $(utility_tests.elf_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),utility_tests.elf,$@,$(OBJS_utility_tests.elf),$(LDFLAGS) $(utility_tests.elf_LDFLAGS))

$(BIN_DIR)/libbal.a: $(OBJS_libbal.a) $(MK_FILES) $(libbal.a_PREREQUISITES) $(AR_SUPPORT_FILES); $(SILENT) rm --force $@; $(call ar,libbal.a,$@,$(OBJS_libbal.a),$(ARFLAGS) $(libbal.a_ARFLAGS))

$(BIN_DIR)/libbridge.a: $(OBJS_libbridge.a) $(MK_FILES) $(libbridge.a_PREREQUISITES) $(AR_SUPPORT_FILES); $(SILENT) rm --force $@; $(call ar,libbridge.a,$@,$(OBJS_libbridge.a),$(ARFLAGS) $(libbridge.a_ARFLAGS))

$(BIN_DIR)/libnetconfd.a: $(OBJS_libnetconfd.a) $(MK_FILES) $(libnetconfd.a_PREREQUISITES) $(AR_SUPPORT_FILES); $(SILENT) rm --force $@; $(call ar,libnetconfd.a,$@,$(OBJS_libnetconfd.a),$(ARFLAGS) $(libnetconfd.a_ARFLAGS))

$(BIN_DIR)/libnetconfddbus.a: $(OBJS_libnetconfddbus.a) $(MK_FILES) $(libnetconfddbus.a_PREREQUISITES) $(AR_SUPPORT_FILES); $(SILENT) rm --force $@; $(call ar,libnetconfddbus.a,$@,$(OBJS_libnetconfddbus.a),$(ARFLAGS) $(libnetconfddbus.a_ARFLAGS))

$(BIN_DIR)/libutility.a: $(OBJS_libutility.a) $(MK_FILES) $(libutility.a_PREREQUISITES) $(AR_SUPPORT_FILES); $(SILENT) rm --force $@; $(call ar,libutility.a,$@,$(OBJS_libutility.a),$(ARFLAGS) $(libutility.a_ARFLAGS))

check-bal_tests.elf: $(BIN_DIR)/bal_tests.elf; $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_check,bal_tests.elf,./$(notdir $<))

check-netconfd_tests.elf: $(BIN_DIR)/netconfd_tests.elf; $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_check,netconfd_tests.elf,./$(notdir $<))

check-utility_tests.elf: $(BIN_DIR)/utility_tests.elf; $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_check,utility_tests.elf,./$(notdir $<))

memcheck-netconfd.elf: $(BIN_DIR)/netconfd.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,netconfd.elf,./$(notdir $<),MEMCHECK)
helgrind-netconfd.elf: $(BIN_DIR)/netconfd.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,netconfd.elf,./$(notdir $<),HELGRIND)
callgrind-netconfd.elf: $(BIN_DIR)/netconfd.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,netconfd.elf,./$(notdir $<),CALLGRIND)
massif-netconfd.elf: $(BIN_DIR)/netconfd.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,netconfd.elf,./$(notdir $<),MASSIF)
nulgrind-netconfd.elf: $(BIN_DIR)/netconfd.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,netconfd.elf,./$(notdir $<),NULGRIND)

memcheck-bridgeconfig_dsa.elf: $(BIN_DIR)/bridgeconfig_dsa.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,bridgeconfig_dsa.elf,./$(notdir $<),MEMCHECK)
helgrind-bridgeconfig_dsa.elf: $(BIN_DIR)/bridgeconfig_dsa.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,bridgeconfig_dsa.elf,./$(notdir $<),HELGRIND)
callgrind-bridgeconfig_dsa.elf: $(BIN_DIR)/bridgeconfig_dsa.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,bridgeconfig_dsa.elf,./$(notdir $<),CALLGRIND)
massif-bridgeconfig_dsa.elf: $(BIN_DIR)/bridgeconfig_dsa.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,bridgeconfig_dsa.elf,./$(notdir $<),MASSIF)
nulgrind-bridgeconfig_dsa.elf: $(BIN_DIR)/bridgeconfig_dsa.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,bridgeconfig_dsa.elf,./$(notdir $<),NULGRIND)

memcheck-bal_tests.elf: $(BIN_DIR)/bal_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,bal_tests.elf,./$(notdir $<),MEMCHECK)
helgrind-bal_tests.elf: $(BIN_DIR)/bal_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,bal_tests.elf,./$(notdir $<),HELGRIND)
callgrind-bal_tests.elf: $(BIN_DIR)/bal_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,bal_tests.elf,./$(notdir $<),CALLGRIND)
massif-bal_tests.elf: $(BIN_DIR)/bal_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,bal_tests.elf,./$(notdir $<),MASSIF)
nulgrind-bal_tests.elf: $(BIN_DIR)/bal_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,bal_tests.elf,./$(notdir $<),NULGRIND)

memcheck-netconfd_tests.elf: $(BIN_DIR)/netconfd_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,netconfd_tests.elf,./$(notdir $<),MEMCHECK)
helgrind-netconfd_tests.elf: $(BIN_DIR)/netconfd_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,netconfd_tests.elf,./$(notdir $<),HELGRIND)
callgrind-netconfd_tests.elf: $(BIN_DIR)/netconfd_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,netconfd_tests.elf,./$(notdir $<),CALLGRIND)
massif-netconfd_tests.elf: $(BIN_DIR)/netconfd_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,netconfd_tests.elf,./$(notdir $<),MASSIF)
nulgrind-netconfd_tests.elf: $(BIN_DIR)/netconfd_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,netconfd_tests.elf,./$(notdir $<),NULGRIND)

memcheck-utility_tests.elf: $(BIN_DIR)/utility_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,utility_tests.elf,./$(notdir $<),MEMCHECK)
helgrind-utility_tests.elf: $(BIN_DIR)/utility_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,utility_tests.elf,./$(notdir $<),HELGRIND)
callgrind-utility_tests.elf: $(BIN_DIR)/utility_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,utility_tests.elf,./$(notdir $<),CALLGRIND)
massif-utility_tests.elf: $(BIN_DIR)/utility_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,utility_tests.elf,./$(notdir $<),MASSIF)
nulgrind-utility_tests.elf: $(BIN_DIR)/utility_tests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,utility_tests.elf,./$(notdir $<),NULGRIND)
