CPPFLAGS := $(CPPFLAGS)
CFLAGS := $(CFLAGS)
CXXFLAGS := $(CXXFLAGS)
LDFLAGS := $(LDFLAGS)
ARFLAGS := $(ARFLAGS)

mdmd.elf_SOURCES := $(mdmd.elf_SOURCES)
mdmd.elf_GENHEADERS := $(mdmd.elf_GENHEADERS)
mdmd.elf_CPPFLAGS := $(mdmd.elf_CPPFLAGS)
mdmd.elf_CFLAGS := $(mdmd.elf_CFLAGS)
mdmd.elf_CXXFLAGS := $(mdmd.elf_CXXFLAGS)
mdmd.elf_LDFLAGS := $(mdmd.elf_LDFLAGS)
mdmd.elf_ARFLAGS := $(mdmd.elf_ARFLAGS)

mdm_cuse.elf_SOURCES := $(mdm_cuse.elf_SOURCES)
mdm_cuse.elf_GENHEADERS := $(mdm_cuse.elf_GENHEADERS)
mdm_cuse.elf_CPPFLAGS := $(mdm_cuse.elf_CPPFLAGS)
mdm_cuse.elf_CFLAGS := $(mdm_cuse.elf_CFLAGS)
mdm_cuse.elf_CXXFLAGS := $(mdm_cuse.elf_CXXFLAGS)
mdm_cuse.elf_LDFLAGS := $(mdm_cuse.elf_LDFLAGS)
mdm_cuse.elf_ARFLAGS := $(mdm_cuse.elf_ARFLAGS)

alltests.elf_SOURCES := $(alltests.elf_SOURCES)
alltests.elf_GENHEADERS := $(alltests.elf_GENHEADERS)
alltests.elf_CPPFLAGS := $(alltests.elf_CPPFLAGS)
alltests.elf_CFLAGS := $(alltests.elf_CFLAGS)
alltests.elf_CXXFLAGS := $(alltests.elf_CXXFLAGS)
alltests.elf_LDFLAGS := $(alltests.elf_LDFLAGS)
alltests.elf_ARFLAGS := $(alltests.elf_ARFLAGS)

OBJECT_FILES := $(OBJECT_FILES)
DEP_FILES := $(DEP_FILES)
OUT_DIRS := $(call sort,$(OUT_DIR)/ $(dir $(OBJECT_FILES)) $(OUT_DIRS))
GEN_DIRS := $(call sort,$(BIN_DIR)/ $(OUT_DIRS) $(GEN_DIRS))

$(DEP_FILES) : | $(GEN_DIRS)

-include $(DEP_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(CLANG_TIDY_RULESET)),$(error CLANG_TIDY_RULESET has "$(CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(CLANG_TIDY_RULESET))"))

mdmd.elf_LINT_SOURCES ?= $(filter-out $(mdmd.elf_NOLINT_SOURCES),$(mdmd.elf_SOURCES))
OBJS_mdmd.elf := $(call objs,$(mdmd.elf_SOURCES),mdmd.elf/)
DEPS_mdmd.elf := $(call deps,$(mdmd.elf_SOURCES),mdmd.elf/)
LOBS_mdmd.elf := $(call lobs,$(mdmd.elf_LINT_SOURCES),mdmd.elf/)
TIDYS_mdmd.elf := $(call tidys,$(mdmd.elf_LINT_SOURCES),mdmd.elf/)
$(TIDYS_mdmd.elf): $(mdmd.elf_PRECLANG_FILES)
PLINTS_mdmd.elf := $(call plints,$(mdmd.elf_LINT_SOURCES),mdmd.elf/)
$(PLINTS_mdmd.elf): $(mdmd.elf_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(mdmd.elf_CLANG_TIDY_RULESET)),$(error mdmd.elf_CLANG_TIDY_RULESET has "$(mdmd.elf_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(mdmd.elf_CLANG_TIDY_RULESET))"))

mdmd.elf: $(BIN_DIR)/mdmd.elf

$(DEPS_mdmd.elf) $(OBJS_mdmd.elf) $(LOBS_mdmd.elf) $(TIDYS_mdmd.elf) $(PLINTS_mdmd.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/mdmd.elf/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(mdmd.elf_CPPFLAGS)

$(DEPS_mdmd.elf) $(OBJS_mdmd.elf) $(TIDYS_mdmd.elf) $(PLINTS_mdmd.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/mdmd.elf/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(mdmd.elf_CXXFLAGS)

$(DEPS_mdmd.elf) $(OBJS_mdmd.elf) $(TIDYS_mdmd.elf) $(PLINTS_mdmd.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/mdmd.elf/lint_mac.h: BUILDTARGET_CFLAGS:=$(mdmd.elf_CFLAGS)

lint-mdmd.elf: flexelint-mdmd.elf clang-tidy-mdmd.elf

$(LOBS_mdmd.elf): BUILDTARGET_LINTFLAGS:=$(mdmd.elf_LINTFLAGS)

flexelint-mdmd.elf: $($(mdmd.elf_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(mdmd.elf_DISABLE_LINT)$(DISABLE_LINT)LOBS_mdmd.elf)

$(TIDYS_mdmd.elf): BUILDTARGET_TIDYFLAGS:=$(mdmd.elf_TIDYFLAGS) -isystem $(OUT_DIR)/mdmd.elf -include lint_mac.h

$(TIDYS_mdmd.elf): mdmd.elf_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_mdmd.elf): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(mdmd.elf_CLANG_TIDY_RULESET) $(mdmd.elf_CLANG_TIDY_CHECKS)))

$(PLINTS_mdmd.elf): BUILDTARGET_PLINTFLAGS:=$(mdmd.elf_PLINTSFLAGS) -isystem $(OUT_DIR)/mdmd.elf -include lint_mac.h

clang-tidy-mdmd.elf: $($(mdmd.elf_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(mdmd.elf_DISABLE_LINT)$(DISABLE_LINT)TIDYS_mdmd.elf)

clean-clang-tidy-mdmd.elf:; $(SILENT)rm --force $(TIDYS_mdmd.elf)

clangsa-mdmd.elf: $(PLINTS_mdmd.elf)

info-txt-mdmd.elf: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,mdmd.elf,$<)

$(OUT_DIR)/mdmd.elf/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(mdmd.elf_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/mdmd.elf/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(mdmd.elf_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/mdmd.elf/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(mdmd.elf_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

mdmd.elf_PRECLANG_FILES=$(OUT_DIR)/mdmd.elf/lint_mac.h

$(OUT_DIR)/mdmd.elf/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(mdmd.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/mdmd.elf/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/mdmd.elf/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(mdmd.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/mdmd.elf/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



mdm_cuse.elf_LINT_SOURCES ?= $(filter-out $(mdm_cuse.elf_NOLINT_SOURCES),$(mdm_cuse.elf_SOURCES))
OBJS_mdm_cuse.elf := $(call objs,$(mdm_cuse.elf_SOURCES),mdm_cuse.elf/)
DEPS_mdm_cuse.elf := $(call deps,$(mdm_cuse.elf_SOURCES),mdm_cuse.elf/)
LOBS_mdm_cuse.elf := $(call lobs,$(mdm_cuse.elf_LINT_SOURCES),mdm_cuse.elf/)
TIDYS_mdm_cuse.elf := $(call tidys,$(mdm_cuse.elf_LINT_SOURCES),mdm_cuse.elf/)
$(TIDYS_mdm_cuse.elf): $(mdm_cuse.elf_PRECLANG_FILES)
PLINTS_mdm_cuse.elf := $(call plints,$(mdm_cuse.elf_LINT_SOURCES),mdm_cuse.elf/)
$(PLINTS_mdm_cuse.elf): $(mdm_cuse.elf_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(mdm_cuse.elf_CLANG_TIDY_RULESET)),$(error mdm_cuse.elf_CLANG_TIDY_RULESET has "$(mdm_cuse.elf_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(mdm_cuse.elf_CLANG_TIDY_RULESET))"))

mdm_cuse.elf: $(BIN_DIR)/mdm_cuse.elf

$(DEPS_mdm_cuse.elf) $(OBJS_mdm_cuse.elf) $(LOBS_mdm_cuse.elf) $(TIDYS_mdm_cuse.elf) $(PLINTS_mdm_cuse.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/mdm_cuse.elf/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(mdm_cuse.elf_CPPFLAGS)

$(DEPS_mdm_cuse.elf) $(OBJS_mdm_cuse.elf) $(TIDYS_mdm_cuse.elf) $(PLINTS_mdm_cuse.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/mdm_cuse.elf/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(mdm_cuse.elf_CXXFLAGS)

$(DEPS_mdm_cuse.elf) $(OBJS_mdm_cuse.elf) $(TIDYS_mdm_cuse.elf) $(PLINTS_mdm_cuse.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/mdm_cuse.elf/lint_mac.h: BUILDTARGET_CFLAGS:=$(mdm_cuse.elf_CFLAGS)

lint-mdm_cuse.elf: flexelint-mdm_cuse.elf clang-tidy-mdm_cuse.elf

$(LOBS_mdm_cuse.elf): BUILDTARGET_LINTFLAGS:=$(mdm_cuse.elf_LINTFLAGS)

flexelint-mdm_cuse.elf: $($(mdm_cuse.elf_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(mdm_cuse.elf_DISABLE_LINT)$(DISABLE_LINT)LOBS_mdm_cuse.elf)

$(TIDYS_mdm_cuse.elf): BUILDTARGET_TIDYFLAGS:=$(mdm_cuse.elf_TIDYFLAGS) -isystem $(OUT_DIR)/mdm_cuse.elf -include lint_mac.h

$(TIDYS_mdm_cuse.elf): mdm_cuse.elf_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_mdm_cuse.elf): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(mdm_cuse.elf_CLANG_TIDY_RULESET) $(mdm_cuse.elf_CLANG_TIDY_CHECKS)))

$(PLINTS_mdm_cuse.elf): BUILDTARGET_PLINTFLAGS:=$(mdm_cuse.elf_PLINTSFLAGS) -isystem $(OUT_DIR)/mdm_cuse.elf -include lint_mac.h

clang-tidy-mdm_cuse.elf: $($(mdm_cuse.elf_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(mdm_cuse.elf_DISABLE_LINT)$(DISABLE_LINT)TIDYS_mdm_cuse.elf)

clean-clang-tidy-mdm_cuse.elf:; $(SILENT)rm --force $(TIDYS_mdm_cuse.elf)

clangsa-mdm_cuse.elf: $(PLINTS_mdm_cuse.elf)

info-txt-mdm_cuse.elf: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,mdm_cuse.elf,$<)

$(OUT_DIR)/mdm_cuse.elf/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(mdm_cuse.elf_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/mdm_cuse.elf/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(mdm_cuse.elf_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/mdm_cuse.elf/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(mdm_cuse.elf_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

mdm_cuse.elf_PRECLANG_FILES=$(OUT_DIR)/mdm_cuse.elf/lint_mac.h

$(OUT_DIR)/mdm_cuse.elf/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(mdm_cuse.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/mdm_cuse.elf/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/mdm_cuse.elf/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(mdm_cuse.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/mdm_cuse.elf/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



alltests.elf_LINT_SOURCES ?= $(filter-out $(alltests.elf_NOLINT_SOURCES),$(alltests.elf_SOURCES))
OBJS_alltests.elf := $(call objs,$(alltests.elf_SOURCES),alltests.elf/)
DEPS_alltests.elf := $(call deps,$(alltests.elf_SOURCES),alltests.elf/)
LOBS_alltests.elf := $(call lobs,$(alltests.elf_LINT_SOURCES),alltests.elf/)
TIDYS_alltests.elf := $(call tidys,$(alltests.elf_LINT_SOURCES),alltests.elf/)
$(TIDYS_alltests.elf): $(alltests.elf_PRECLANG_FILES)
PLINTS_alltests.elf := $(call plints,$(alltests.elf_LINT_SOURCES),alltests.elf/)
$(PLINTS_alltests.elf): $(alltests.elf_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(alltests.elf_CLANG_TIDY_RULESET)),$(error alltests.elf_CLANG_TIDY_RULESET has "$(alltests.elf_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(alltests.elf_CLANG_TIDY_RULESET))"))

alltests.elf: $(BIN_DIR)/alltests.elf

$(DEPS_alltests.elf) $(OBJS_alltests.elf) $(LOBS_alltests.elf) $(TIDYS_alltests.elf) $(PLINTS_alltests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/alltests.elf/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(alltests.elf_CPPFLAGS)

$(DEPS_alltests.elf) $(OBJS_alltests.elf) $(TIDYS_alltests.elf) $(PLINTS_alltests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/alltests.elf/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(alltests.elf_CXXFLAGS)

$(DEPS_alltests.elf) $(OBJS_alltests.elf) $(TIDYS_alltests.elf) $(PLINTS_alltests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/alltests.elf/lint_mac.h: BUILDTARGET_CFLAGS:=$(alltests.elf_CFLAGS)

lint-alltests.elf: flexelint-alltests.elf clang-tidy-alltests.elf

$(LOBS_alltests.elf): BUILDTARGET_LINTFLAGS:=$(alltests.elf_LINTFLAGS)

flexelint-alltests.elf: $($(alltests.elf_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(alltests.elf_DISABLE_LINT)$(DISABLE_LINT)LOBS_alltests.elf)

$(TIDYS_alltests.elf): BUILDTARGET_TIDYFLAGS:=$(alltests.elf_TIDYFLAGS) -isystem $(OUT_DIR)/alltests.elf -include lint_mac.h

$(TIDYS_alltests.elf): alltests.elf_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_alltests.elf): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(alltests.elf_CLANG_TIDY_RULESET) $(alltests.elf_CLANG_TIDY_CHECKS)))

$(PLINTS_alltests.elf): BUILDTARGET_PLINTFLAGS:=$(alltests.elf_PLINTSFLAGS) -isystem $(OUT_DIR)/alltests.elf -include lint_mac.h

clang-tidy-alltests.elf: $($(alltests.elf_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(alltests.elf_DISABLE_LINT)$(DISABLE_LINT)TIDYS_alltests.elf)

clean-clang-tidy-alltests.elf:; $(SILENT)rm --force $(TIDYS_alltests.elf)

clangsa-alltests.elf: $(PLINTS_alltests.elf)

info-txt-alltests.elf: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,alltests.elf,$<)

$(OUT_DIR)/alltests.elf/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(alltests.elf_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/alltests.elf/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(alltests.elf_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/alltests.elf/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(alltests.elf_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

alltests.elf_PRECLANG_FILES=$(OUT_DIR)/alltests.elf/lint_mac.h

$(OUT_DIR)/alltests.elf/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(alltests.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/alltests.elf/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/alltests.elf/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(alltests.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/alltests.elf/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



$(OUT_DIR)/mdmd.elf.dbg: $(OBJS_mdmd.elf) $(MK_FILES) $(mdmd.elf_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),mdmd.elf,$@,$(OBJS_mdmd.elf),$(LDFLAGS) $(mdmd.elf_LDFLAGS))

$(OUT_DIR)/mdm_cuse.elf.dbg: $(OBJS_mdm_cuse.elf) $(MK_FILES) $(mdm_cuse.elf_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),mdm_cuse.elf,$@,$(OBJS_mdm_cuse.elf),$(LDFLAGS) $(mdm_cuse.elf_LDFLAGS))

$(OUT_DIR)/alltests.elf.dbg: $(OBJS_alltests.elf) $(MK_FILES) $(alltests.elf_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),alltests.elf,$@,$(OBJS_alltests.elf),$(LDFLAGS) $(alltests.elf_LDFLAGS))

check-alltests.elf: $(BIN_DIR)/alltests.elf; $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_check,alltests.elf,./$(notdir $<))

memcheck-mdmd.elf: $(BIN_DIR)/mdmd.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,mdmd.elf,./$(notdir $<),MEMCHECK)
helgrind-mdmd.elf: $(BIN_DIR)/mdmd.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,mdmd.elf,./$(notdir $<),HELGRIND)
callgrind-mdmd.elf: $(BIN_DIR)/mdmd.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,mdmd.elf,./$(notdir $<),CALLGRIND)
massif-mdmd.elf: $(BIN_DIR)/mdmd.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,mdmd.elf,./$(notdir $<),MASSIF)
nulgrind-mdmd.elf: $(BIN_DIR)/mdmd.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,mdmd.elf,./$(notdir $<),NULGRIND)

memcheck-mdm_cuse.elf: $(BIN_DIR)/mdm_cuse.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,mdm_cuse.elf,./$(notdir $<),MEMCHECK)
helgrind-mdm_cuse.elf: $(BIN_DIR)/mdm_cuse.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,mdm_cuse.elf,./$(notdir $<),HELGRIND)
callgrind-mdm_cuse.elf: $(BIN_DIR)/mdm_cuse.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,mdm_cuse.elf,./$(notdir $<),CALLGRIND)
massif-mdm_cuse.elf: $(BIN_DIR)/mdm_cuse.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,mdm_cuse.elf,./$(notdir $<),MASSIF)
nulgrind-mdm_cuse.elf: $(BIN_DIR)/mdm_cuse.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,mdm_cuse.elf,./$(notdir $<),NULGRIND)

memcheck-alltests.elf: $(BIN_DIR)/alltests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,alltests.elf,./$(notdir $<),MEMCHECK)
helgrind-alltests.elf: $(BIN_DIR)/alltests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,alltests.elf,./$(notdir $<),HELGRIND)
callgrind-alltests.elf: $(BIN_DIR)/alltests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,alltests.elf,./$(notdir $<),CALLGRIND)
massif-alltests.elf: $(BIN_DIR)/alltests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,alltests.elf,./$(notdir $<),MASSIF)
nulgrind-alltests.elf: $(BIN_DIR)/alltests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,alltests.elf,./$(notdir $<),NULGRIND)
