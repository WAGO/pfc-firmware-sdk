CPPFLAGS := $(CPPFLAGS)
CFLAGS := $(CFLAGS)
CXXFLAGS := $(CXXFLAGS)
LDFLAGS := $(LDFLAGS)
ARFLAGS := $(ARFLAGS)

dbus-wait.elf_SOURCES := $(dbus-wait.elf_SOURCES)
dbus-wait.elf_GENHEADERS := $(dbus-wait.elf_GENHEADERS)
dbus-wait.elf_CPPFLAGS := $(dbus-wait.elf_CPPFLAGS)
dbus-wait.elf_CFLAGS := $(dbus-wait.elf_CFLAGS)
dbus-wait.elf_CXXFLAGS := $(dbus-wait.elf_CXXFLAGS)
dbus-wait.elf_LDFLAGS := $(dbus-wait.elf_LDFLAGS)
dbus-wait.elf_ARFLAGS := $(dbus-wait.elf_ARFLAGS)

OBJECT_FILES := $(OBJECT_FILES)
DEP_FILES := $(DEP_FILES)
OUT_DIRS := $(call sort,$(OUT_DIR)/ $(dir $(OBJECT_FILES)) $(OUT_DIRS))
GEN_DIRS := $(call sort,$(BIN_DIR)/ $(OUT_DIRS) $(GEN_DIRS))

$(DEP_FILES) : | $(GEN_DIRS)

-include $(DEP_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(CLANG_TIDY_RULESET)),$(error CLANG_TIDY_RULESET has "$(CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(CLANG_TIDY_RULESET))"))

dbus-wait.elf_LINT_SOURCES ?= $(filter-out $(dbus-wait.elf_NOLINT_SOURCES),$(dbus-wait.elf_SOURCES))
OBJS_dbus-wait.elf := $(call objs,$(dbus-wait.elf_SOURCES),dbus-wait.elf/)
DEPS_dbus-wait.elf := $(call deps,$(dbus-wait.elf_SOURCES),dbus-wait.elf/)
LOBS_dbus-wait.elf := $(call lobs,$(dbus-wait.elf_LINT_SOURCES),dbus-wait.elf/)
TIDYS_dbus-wait.elf := $(call tidys,$(dbus-wait.elf_LINT_SOURCES),dbus-wait.elf/)
$(TIDYS_dbus-wait.elf): $(dbus-wait.elf_PRECLANG_FILES)
PLINTS_dbus-wait.elf := $(call plints,$(dbus-wait.elf_LINT_SOURCES),dbus-wait.elf/)
$(PLINTS_dbus-wait.elf): $(dbus-wait.elf_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(dbus-wait.elf_CLANG_TIDY_RULESET)),$(error dbus-wait.elf_CLANG_TIDY_RULESET has "$(dbus-wait.elf_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(dbus-wait.elf_CLANG_TIDY_RULESET))"))

dbus-wait.elf: $(BIN_DIR)/dbus-wait.elf

$(DEPS_dbus-wait.elf) $(OBJS_dbus-wait.elf) $(LOBS_dbus-wait.elf) $(TIDYS_dbus-wait.elf) $(PLINTS_dbus-wait.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/dbus-wait.elf/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(dbus-wait.elf_CPPFLAGS)

$(DEPS_dbus-wait.elf) $(OBJS_dbus-wait.elf) $(TIDYS_dbus-wait.elf) $(PLINTS_dbus-wait.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/dbus-wait.elf/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(dbus-wait.elf_CXXFLAGS)

$(DEPS_dbus-wait.elf) $(OBJS_dbus-wait.elf) $(TIDYS_dbus-wait.elf) $(PLINTS_dbus-wait.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/dbus-wait.elf/lint_mac.h: BUILDTARGET_CFLAGS:=$(dbus-wait.elf_CFLAGS)

lint-dbus-wait.elf: flexelint-dbus-wait.elf clang-tidy-dbus-wait.elf

$(LOBS_dbus-wait.elf): BUILDTARGET_LINTFLAGS:=$(dbus-wait.elf_LINTFLAGS)

flexelint-dbus-wait.elf: $($(dbus-wait.elf_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(dbus-wait.elf_DISABLE_LINT)$(DISABLE_LINT)LOBS_dbus-wait.elf)

$(TIDYS_dbus-wait.elf): BUILDTARGET_TIDYFLAGS:=$(dbus-wait.elf_TIDYFLAGS) -isystem $(OUT_DIR)/dbus-wait.elf -include lint_mac.h

$(TIDYS_dbus-wait.elf): dbus-wait.elf_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_dbus-wait.elf): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(dbus-wait.elf_CLANG_TIDY_RULESET) $(dbus-wait.elf_CLANG_TIDY_CHECKS)))

$(PLINTS_dbus-wait.elf): BUILDTARGET_PLINTFLAGS:=$(dbus-wait.elf_PLINTSFLAGS) -isystem $(OUT_DIR)/dbus-wait.elf -include lint_mac.h

clang-tidy-dbus-wait.elf: $($(dbus-wait.elf_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(dbus-wait.elf_DISABLE_LINT)$(DISABLE_LINT)TIDYS_dbus-wait.elf)

clean-clang-tidy-dbus-wait.elf:; $(SILENT)rm --force $(TIDYS_dbus-wait.elf)

clangsa-dbus-wait.elf: $(PLINTS_dbus-wait.elf)

info-txt-dbus-wait.elf: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,dbus-wait.elf,$<)

$(OUT_DIR)/dbus-wait.elf/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(dbus-wait.elf_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/dbus-wait.elf/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(dbus-wait.elf_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/dbus-wait.elf/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(dbus-wait.elf_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

dbus-wait.elf_PRECLANG_FILES=$(OUT_DIR)/dbus-wait.elf/lint_mac.h

$(OUT_DIR)/dbus-wait.elf/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(dbus-wait.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/dbus-wait.elf/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/dbus-wait.elf/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(dbus-wait.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/dbus-wait.elf/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



$(OUT_DIR)/dbus-wait.elf.dbg: $(OBJS_dbus-wait.elf) $(MK_FILES) $(dbus-wait.elf_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),dbus-wait.elf,$@,$(OBJS_dbus-wait.elf),$(LDFLAGS) $(dbus-wait.elf_LDFLAGS))

memcheck-dbus-wait.elf: $(BIN_DIR)/dbus-wait.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,dbus-wait.elf,./$(notdir $<),MEMCHECK)
helgrind-dbus-wait.elf: $(BIN_DIR)/dbus-wait.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,dbus-wait.elf,./$(notdir $<),HELGRIND)
callgrind-dbus-wait.elf: $(BIN_DIR)/dbus-wait.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,dbus-wait.elf,./$(notdir $<),CALLGRIND)
massif-dbus-wait.elf: $(BIN_DIR)/dbus-wait.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,dbus-wait.elf,./$(notdir $<),MASSIF)
nulgrind-dbus-wait.elf: $(BIN_DIR)/dbus-wait.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,dbus-wait.elf,./$(notdir $<),NULGRIND)
