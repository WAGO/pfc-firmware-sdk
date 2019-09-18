CPPFLAGS := $(CPPFLAGS)
CFLAGS := $(CFLAGS)
CXXFLAGS := $(CXXFLAGS)
LDFLAGS := $(LDFLAGS)
ARFLAGS := $(ARFLAGS)

libcgroupcpuload.so_SOURCES := $(libcgroupcpuload.so_SOURCES)
libcgroupcpuload.so_GENHEADERS := $(libcgroupcpuload.so_GENHEADERS)
libcgroupcpuload.so_CPPFLAGS := $(libcgroupcpuload.so_CPPFLAGS)
libcgroupcpuload.so_CFLAGS := $(libcgroupcpuload.so_CFLAGS)
libcgroupcpuload.so_CXXFLAGS := $(libcgroupcpuload.so_CXXFLAGS)
libcgroupcpuload.so_LDFLAGS := $(libcgroupcpuload.so_LDFLAGS)
libcgroupcpuload.so_ARFLAGS := $(libcgroupcpuload.so_ARFLAGS)

loadcompare.elf_SOURCES := $(loadcompare.elf_SOURCES)
loadcompare.elf_GENHEADERS := $(loadcompare.elf_GENHEADERS)
loadcompare.elf_CPPFLAGS := $(loadcompare.elf_CPPFLAGS)
loadcompare.elf_CFLAGS := $(loadcompare.elf_CFLAGS)
loadcompare.elf_CXXFLAGS := $(loadcompare.elf_CXXFLAGS)
loadcompare.elf_LDFLAGS := $(loadcompare.elf_LDFLAGS)
loadcompare.elf_ARFLAGS := $(loadcompare.elf_ARFLAGS)

checkcpu.elf_SOURCES := $(checkcpu.elf_SOURCES)
checkcpu.elf_GENHEADERS := $(checkcpu.elf_GENHEADERS)
checkcpu.elf_CPPFLAGS := $(checkcpu.elf_CPPFLAGS)
checkcpu.elf_CFLAGS := $(checkcpu.elf_CFLAGS)
checkcpu.elf_CXXFLAGS := $(checkcpu.elf_CXXFLAGS)
checkcpu.elf_LDFLAGS := $(checkcpu.elf_LDFLAGS)
checkcpu.elf_ARFLAGS := $(checkcpu.elf_ARFLAGS)

bellysmile.elf_SOURCES := $(bellysmile.elf_SOURCES)
bellysmile.elf_GENHEADERS := $(bellysmile.elf_GENHEADERS)
bellysmile.elf_CPPFLAGS := $(bellysmile.elf_CPPFLAGS)
bellysmile.elf_CFLAGS := $(bellysmile.elf_CFLAGS)
bellysmile.elf_CXXFLAGS := $(bellysmile.elf_CXXFLAGS)
bellysmile.elf_LDFLAGS := $(bellysmile.elf_LDFLAGS)
bellysmile.elf_ARFLAGS := $(bellysmile.elf_ARFLAGS)

testapp.elf_SOURCES := $(testapp.elf_SOURCES)
testapp.elf_GENHEADERS := $(testapp.elf_GENHEADERS)
testapp.elf_CPPFLAGS := $(testapp.elf_CPPFLAGS)
testapp.elf_CFLAGS := $(testapp.elf_CFLAGS)
testapp.elf_CXXFLAGS := $(testapp.elf_CXXFLAGS)
testapp.elf_LDFLAGS := $(testapp.elf_LDFLAGS)
testapp.elf_ARFLAGS := $(testapp.elf_ARFLAGS)

cpuloadmonitor.elf_SOURCES := $(cpuloadmonitor.elf_SOURCES)
cpuloadmonitor.elf_GENHEADERS := $(cpuloadmonitor.elf_GENHEADERS)
cpuloadmonitor.elf_CPPFLAGS := $(cpuloadmonitor.elf_CPPFLAGS)
cpuloadmonitor.elf_CFLAGS := $(cpuloadmonitor.elf_CFLAGS)
cpuloadmonitor.elf_CXXFLAGS := $(cpuloadmonitor.elf_CXXFLAGS)
cpuloadmonitor.elf_LDFLAGS := $(cpuloadmonitor.elf_LDFLAGS)
cpuloadmonitor.elf_ARFLAGS := $(cpuloadmonitor.elf_ARFLAGS)

OBJECT_FILES := $(OBJECT_FILES)
DEP_FILES := $(DEP_FILES)
OUT_DIRS := $(call sort,$(OUT_DIR)/ $(dir $(OBJECT_FILES)) $(OUT_DIRS))
GEN_DIRS := $(call sort,$(BIN_DIR)/ $(OUT_DIRS) $(GEN_DIRS))

$(DEP_FILES) : | $(GEN_DIRS)

-include $(DEP_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(CLANG_TIDY_RULESET)),$(error CLANG_TIDY_RULESET has "$(CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(CLANG_TIDY_RULESET))"))

libcgroupcpuload.so_LINT_SOURCES ?= $(filter-out $(libcgroupcpuload.so_NOLINT_SOURCES),$(libcgroupcpuload.so_SOURCES))
OBJS_libcgroupcpuload.so := $(call objs,$(libcgroupcpuload.so_SOURCES),libcgroupcpuload.so/)
DEPS_libcgroupcpuload.so := $(call deps,$(libcgroupcpuload.so_SOURCES),libcgroupcpuload.so/)
LOBS_libcgroupcpuload.so := $(call lobs,$(libcgroupcpuload.so_LINT_SOURCES),libcgroupcpuload.so/)
TIDYS_libcgroupcpuload.so := $(call tidys,$(libcgroupcpuload.so_LINT_SOURCES),libcgroupcpuload.so/)
$(TIDYS_libcgroupcpuload.so): $(libcgroupcpuload.so_PRECLANG_FILES)
PLINTS_libcgroupcpuload.so := $(call plints,$(libcgroupcpuload.so_LINT_SOURCES),libcgroupcpuload.so/)
$(PLINTS_libcgroupcpuload.so): $(libcgroupcpuload.so_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(libcgroupcpuload.so_CLANG_TIDY_RULESET)),$(error libcgroupcpuload.so_CLANG_TIDY_RULESET has "$(libcgroupcpuload.so_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(libcgroupcpuload.so_CLANG_TIDY_RULESET))"))

libcgroupcpuload.so: $(BIN_DIR)/libcgroupcpuload.so

$(DEPS_libcgroupcpuload.so) $(OBJS_libcgroupcpuload.so) $(LOBS_libcgroupcpuload.so) $(TIDYS_libcgroupcpuload.so) $(PLINTS_libcgroupcpuload.so) ./out/gcc_5_5_arm-linux-gnueabihf/Release/libcgroupcpuload.so/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(libcgroupcpuload.so_CPPFLAGS)

$(DEPS_libcgroupcpuload.so) $(OBJS_libcgroupcpuload.so) $(TIDYS_libcgroupcpuload.so) $(PLINTS_libcgroupcpuload.so) ./out/gcc_5_5_arm-linux-gnueabihf/Release/libcgroupcpuload.so/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(libcgroupcpuload.so_CXXFLAGS)

$(DEPS_libcgroupcpuload.so) $(OBJS_libcgroupcpuload.so) $(TIDYS_libcgroupcpuload.so) $(PLINTS_libcgroupcpuload.so) ./out/gcc_5_5_arm-linux-gnueabihf/Release/libcgroupcpuload.so/lint_mac.h: BUILDTARGET_CFLAGS:=$(libcgroupcpuload.so_CFLAGS)

lint-libcgroupcpuload.so: flexelint-libcgroupcpuload.so clang-tidy-libcgroupcpuload.so

$(LOBS_libcgroupcpuload.so): BUILDTARGET_LINTFLAGS:=$(libcgroupcpuload.so_LINTFLAGS)

flexelint-libcgroupcpuload.so: $($(libcgroupcpuload.so_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(libcgroupcpuload.so_DISABLE_LINT)$(DISABLE_LINT)LOBS_libcgroupcpuload.so)

$(TIDYS_libcgroupcpuload.so): BUILDTARGET_TIDYFLAGS:=$(libcgroupcpuload.so_TIDYFLAGS) -isystem $(OUT_DIR)/libcgroupcpuload.so -include lint_mac.h

$(TIDYS_libcgroupcpuload.so): libcgroupcpuload.so_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_libcgroupcpuload.so): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(libcgroupcpuload.so_CLANG_TIDY_RULESET) $(libcgroupcpuload.so_CLANG_TIDY_CHECKS)))

$(PLINTS_libcgroupcpuload.so): BUILDTARGET_PLINTFLAGS:=$(libcgroupcpuload.so_PLINTSFLAGS) -isystem $(OUT_DIR)/libcgroupcpuload.so -include lint_mac.h

clang-tidy-libcgroupcpuload.so: $($(libcgroupcpuload.so_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(libcgroupcpuload.so_DISABLE_LINT)$(DISABLE_LINT)TIDYS_libcgroupcpuload.so)

clean-clang-tidy-libcgroupcpuload.so:; $(SILENT)rm --force $(TIDYS_libcgroupcpuload.so)

clangsa-libcgroupcpuload.so: $(PLINTS_libcgroupcpuload.so)

info-txt-libcgroupcpuload.so: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,libcgroupcpuload.so,$<)

$(OUT_DIR)/libcgroupcpuload.so/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(libcgroupcpuload.so_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/libcgroupcpuload.so/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(libcgroupcpuload.so_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/libcgroupcpuload.so/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(libcgroupcpuload.so_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

libcgroupcpuload.so_PRECLANG_FILES=$(OUT_DIR)/libcgroupcpuload.so/lint_mac.h

$(OUT_DIR)/libcgroupcpuload.so/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(libcgroupcpuload.so_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/libcgroupcpuload.so/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/libcgroupcpuload.so/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(libcgroupcpuload.so_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/libcgroupcpuload.so/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



loadcompare.elf_LINT_SOURCES ?= $(filter-out $(loadcompare.elf_NOLINT_SOURCES),$(loadcompare.elf_SOURCES))
OBJS_loadcompare.elf := $(call objs,$(loadcompare.elf_SOURCES),loadcompare.elf/)
DEPS_loadcompare.elf := $(call deps,$(loadcompare.elf_SOURCES),loadcompare.elf/)
LOBS_loadcompare.elf := $(call lobs,$(loadcompare.elf_LINT_SOURCES),loadcompare.elf/)
TIDYS_loadcompare.elf := $(call tidys,$(loadcompare.elf_LINT_SOURCES),loadcompare.elf/)
$(TIDYS_loadcompare.elf): $(loadcompare.elf_PRECLANG_FILES)
PLINTS_loadcompare.elf := $(call plints,$(loadcompare.elf_LINT_SOURCES),loadcompare.elf/)
$(PLINTS_loadcompare.elf): $(loadcompare.elf_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(loadcompare.elf_CLANG_TIDY_RULESET)),$(error loadcompare.elf_CLANG_TIDY_RULESET has "$(loadcompare.elf_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(loadcompare.elf_CLANG_TIDY_RULESET))"))

loadcompare.elf: $(BIN_DIR)/loadcompare.elf

$(DEPS_loadcompare.elf) $(OBJS_loadcompare.elf) $(LOBS_loadcompare.elf) $(TIDYS_loadcompare.elf) $(PLINTS_loadcompare.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/loadcompare.elf/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(loadcompare.elf_CPPFLAGS)

$(DEPS_loadcompare.elf) $(OBJS_loadcompare.elf) $(TIDYS_loadcompare.elf) $(PLINTS_loadcompare.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/loadcompare.elf/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(loadcompare.elf_CXXFLAGS)

$(DEPS_loadcompare.elf) $(OBJS_loadcompare.elf) $(TIDYS_loadcompare.elf) $(PLINTS_loadcompare.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/loadcompare.elf/lint_mac.h: BUILDTARGET_CFLAGS:=$(loadcompare.elf_CFLAGS)

lint-loadcompare.elf: flexelint-loadcompare.elf clang-tidy-loadcompare.elf

$(LOBS_loadcompare.elf): BUILDTARGET_LINTFLAGS:=$(loadcompare.elf_LINTFLAGS)

flexelint-loadcompare.elf: $($(loadcompare.elf_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(loadcompare.elf_DISABLE_LINT)$(DISABLE_LINT)LOBS_loadcompare.elf)

$(TIDYS_loadcompare.elf): BUILDTARGET_TIDYFLAGS:=$(loadcompare.elf_TIDYFLAGS) -isystem $(OUT_DIR)/loadcompare.elf -include lint_mac.h

$(TIDYS_loadcompare.elf): loadcompare.elf_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_loadcompare.elf): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(loadcompare.elf_CLANG_TIDY_RULESET) $(loadcompare.elf_CLANG_TIDY_CHECKS)))

$(PLINTS_loadcompare.elf): BUILDTARGET_PLINTFLAGS:=$(loadcompare.elf_PLINTSFLAGS) -isystem $(OUT_DIR)/loadcompare.elf -include lint_mac.h

clang-tidy-loadcompare.elf: $($(loadcompare.elf_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(loadcompare.elf_DISABLE_LINT)$(DISABLE_LINT)TIDYS_loadcompare.elf)

clean-clang-tidy-loadcompare.elf:; $(SILENT)rm --force $(TIDYS_loadcompare.elf)

clangsa-loadcompare.elf: $(PLINTS_loadcompare.elf)

info-txt-loadcompare.elf: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,loadcompare.elf,$<)

$(OUT_DIR)/loadcompare.elf/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(loadcompare.elf_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/loadcompare.elf/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(loadcompare.elf_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/loadcompare.elf/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(loadcompare.elf_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

loadcompare.elf_PRECLANG_FILES=$(OUT_DIR)/loadcompare.elf/lint_mac.h

$(OUT_DIR)/loadcompare.elf/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(loadcompare.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/loadcompare.elf/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/loadcompare.elf/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(loadcompare.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/loadcompare.elf/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



checkcpu.elf_LINT_SOURCES ?= $(filter-out $(checkcpu.elf_NOLINT_SOURCES),$(checkcpu.elf_SOURCES))
OBJS_checkcpu.elf := $(call objs,$(checkcpu.elf_SOURCES),checkcpu.elf/)
DEPS_checkcpu.elf := $(call deps,$(checkcpu.elf_SOURCES),checkcpu.elf/)
LOBS_checkcpu.elf := $(call lobs,$(checkcpu.elf_LINT_SOURCES),checkcpu.elf/)
TIDYS_checkcpu.elf := $(call tidys,$(checkcpu.elf_LINT_SOURCES),checkcpu.elf/)
$(TIDYS_checkcpu.elf): $(checkcpu.elf_PRECLANG_FILES)
PLINTS_checkcpu.elf := $(call plints,$(checkcpu.elf_LINT_SOURCES),checkcpu.elf/)
$(PLINTS_checkcpu.elf): $(checkcpu.elf_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(checkcpu.elf_CLANG_TIDY_RULESET)),$(error checkcpu.elf_CLANG_TIDY_RULESET has "$(checkcpu.elf_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(checkcpu.elf_CLANG_TIDY_RULESET))"))

checkcpu.elf: $(BIN_DIR)/checkcpu.elf

$(DEPS_checkcpu.elf) $(OBJS_checkcpu.elf) $(LOBS_checkcpu.elf) $(TIDYS_checkcpu.elf) $(PLINTS_checkcpu.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/checkcpu.elf/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(checkcpu.elf_CPPFLAGS)

$(DEPS_checkcpu.elf) $(OBJS_checkcpu.elf) $(TIDYS_checkcpu.elf) $(PLINTS_checkcpu.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/checkcpu.elf/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(checkcpu.elf_CXXFLAGS)

$(DEPS_checkcpu.elf) $(OBJS_checkcpu.elf) $(TIDYS_checkcpu.elf) $(PLINTS_checkcpu.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/checkcpu.elf/lint_mac.h: BUILDTARGET_CFLAGS:=$(checkcpu.elf_CFLAGS)

lint-checkcpu.elf: flexelint-checkcpu.elf clang-tidy-checkcpu.elf

$(LOBS_checkcpu.elf): BUILDTARGET_LINTFLAGS:=$(checkcpu.elf_LINTFLAGS)

flexelint-checkcpu.elf: $($(checkcpu.elf_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(checkcpu.elf_DISABLE_LINT)$(DISABLE_LINT)LOBS_checkcpu.elf)

$(TIDYS_checkcpu.elf): BUILDTARGET_TIDYFLAGS:=$(checkcpu.elf_TIDYFLAGS) -isystem $(OUT_DIR)/checkcpu.elf -include lint_mac.h

$(TIDYS_checkcpu.elf): checkcpu.elf_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_checkcpu.elf): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(checkcpu.elf_CLANG_TIDY_RULESET) $(checkcpu.elf_CLANG_TIDY_CHECKS)))

$(PLINTS_checkcpu.elf): BUILDTARGET_PLINTFLAGS:=$(checkcpu.elf_PLINTSFLAGS) -isystem $(OUT_DIR)/checkcpu.elf -include lint_mac.h

clang-tidy-checkcpu.elf: $($(checkcpu.elf_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(checkcpu.elf_DISABLE_LINT)$(DISABLE_LINT)TIDYS_checkcpu.elf)

clean-clang-tidy-checkcpu.elf:; $(SILENT)rm --force $(TIDYS_checkcpu.elf)

clangsa-checkcpu.elf: $(PLINTS_checkcpu.elf)

info-txt-checkcpu.elf: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,checkcpu.elf,$<)

$(OUT_DIR)/checkcpu.elf/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(checkcpu.elf_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/checkcpu.elf/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(checkcpu.elf_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/checkcpu.elf/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(checkcpu.elf_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

checkcpu.elf_PRECLANG_FILES=$(OUT_DIR)/checkcpu.elf/lint_mac.h

$(OUT_DIR)/checkcpu.elf/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(checkcpu.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/checkcpu.elf/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/checkcpu.elf/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(checkcpu.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/checkcpu.elf/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



bellysmile.elf_LINT_SOURCES ?= $(filter-out $(bellysmile.elf_NOLINT_SOURCES),$(bellysmile.elf_SOURCES))
OBJS_bellysmile.elf := $(call objs,$(bellysmile.elf_SOURCES),bellysmile.elf/)
DEPS_bellysmile.elf := $(call deps,$(bellysmile.elf_SOURCES),bellysmile.elf/)
LOBS_bellysmile.elf := $(call lobs,$(bellysmile.elf_LINT_SOURCES),bellysmile.elf/)
TIDYS_bellysmile.elf := $(call tidys,$(bellysmile.elf_LINT_SOURCES),bellysmile.elf/)
$(TIDYS_bellysmile.elf): $(bellysmile.elf_PRECLANG_FILES)
PLINTS_bellysmile.elf := $(call plints,$(bellysmile.elf_LINT_SOURCES),bellysmile.elf/)
$(PLINTS_bellysmile.elf): $(bellysmile.elf_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(bellysmile.elf_CLANG_TIDY_RULESET)),$(error bellysmile.elf_CLANG_TIDY_RULESET has "$(bellysmile.elf_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(bellysmile.elf_CLANG_TIDY_RULESET))"))

bellysmile.elf: $(BIN_DIR)/bellysmile.elf

$(DEPS_bellysmile.elf) $(OBJS_bellysmile.elf) $(LOBS_bellysmile.elf) $(TIDYS_bellysmile.elf) $(PLINTS_bellysmile.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/bellysmile.elf/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(bellysmile.elf_CPPFLAGS)

$(DEPS_bellysmile.elf) $(OBJS_bellysmile.elf) $(TIDYS_bellysmile.elf) $(PLINTS_bellysmile.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/bellysmile.elf/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(bellysmile.elf_CXXFLAGS)

$(DEPS_bellysmile.elf) $(OBJS_bellysmile.elf) $(TIDYS_bellysmile.elf) $(PLINTS_bellysmile.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/bellysmile.elf/lint_mac.h: BUILDTARGET_CFLAGS:=$(bellysmile.elf_CFLAGS)

lint-bellysmile.elf: flexelint-bellysmile.elf clang-tidy-bellysmile.elf

$(LOBS_bellysmile.elf): BUILDTARGET_LINTFLAGS:=$(bellysmile.elf_LINTFLAGS)

flexelint-bellysmile.elf: $($(bellysmile.elf_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(bellysmile.elf_DISABLE_LINT)$(DISABLE_LINT)LOBS_bellysmile.elf)

$(TIDYS_bellysmile.elf): BUILDTARGET_TIDYFLAGS:=$(bellysmile.elf_TIDYFLAGS) -isystem $(OUT_DIR)/bellysmile.elf -include lint_mac.h

$(TIDYS_bellysmile.elf): bellysmile.elf_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_bellysmile.elf): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(bellysmile.elf_CLANG_TIDY_RULESET) $(bellysmile.elf_CLANG_TIDY_CHECKS)))

$(PLINTS_bellysmile.elf): BUILDTARGET_PLINTFLAGS:=$(bellysmile.elf_PLINTSFLAGS) -isystem $(OUT_DIR)/bellysmile.elf -include lint_mac.h

clang-tidy-bellysmile.elf: $($(bellysmile.elf_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(bellysmile.elf_DISABLE_LINT)$(DISABLE_LINT)TIDYS_bellysmile.elf)

clean-clang-tidy-bellysmile.elf:; $(SILENT)rm --force $(TIDYS_bellysmile.elf)

clangsa-bellysmile.elf: $(PLINTS_bellysmile.elf)

info-txt-bellysmile.elf: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,bellysmile.elf,$<)

$(OUT_DIR)/bellysmile.elf/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(bellysmile.elf_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/bellysmile.elf/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(bellysmile.elf_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/bellysmile.elf/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(bellysmile.elf_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

bellysmile.elf_PRECLANG_FILES=$(OUT_DIR)/bellysmile.elf/lint_mac.h

$(OUT_DIR)/bellysmile.elf/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(bellysmile.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/bellysmile.elf/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/bellysmile.elf/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(bellysmile.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/bellysmile.elf/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



testapp.elf_LINT_SOURCES ?= $(filter-out $(testapp.elf_NOLINT_SOURCES),$(testapp.elf_SOURCES))
OBJS_testapp.elf := $(call objs,$(testapp.elf_SOURCES),testapp.elf/)
DEPS_testapp.elf := $(call deps,$(testapp.elf_SOURCES),testapp.elf/)
LOBS_testapp.elf := $(call lobs,$(testapp.elf_LINT_SOURCES),testapp.elf/)
TIDYS_testapp.elf := $(call tidys,$(testapp.elf_LINT_SOURCES),testapp.elf/)
$(TIDYS_testapp.elf): $(testapp.elf_PRECLANG_FILES)
PLINTS_testapp.elf := $(call plints,$(testapp.elf_LINT_SOURCES),testapp.elf/)
$(PLINTS_testapp.elf): $(testapp.elf_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(testapp.elf_CLANG_TIDY_RULESET)),$(error testapp.elf_CLANG_TIDY_RULESET has "$(testapp.elf_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(testapp.elf_CLANG_TIDY_RULESET))"))

testapp.elf: $(BIN_DIR)/testapp.elf

$(DEPS_testapp.elf) $(OBJS_testapp.elf) $(LOBS_testapp.elf) $(TIDYS_testapp.elf) $(PLINTS_testapp.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/testapp.elf/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(testapp.elf_CPPFLAGS)

$(DEPS_testapp.elf) $(OBJS_testapp.elf) $(TIDYS_testapp.elf) $(PLINTS_testapp.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/testapp.elf/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(testapp.elf_CXXFLAGS)

$(DEPS_testapp.elf) $(OBJS_testapp.elf) $(TIDYS_testapp.elf) $(PLINTS_testapp.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/testapp.elf/lint_mac.h: BUILDTARGET_CFLAGS:=$(testapp.elf_CFLAGS)

lint-testapp.elf: flexelint-testapp.elf clang-tidy-testapp.elf

$(LOBS_testapp.elf): BUILDTARGET_LINTFLAGS:=$(testapp.elf_LINTFLAGS)

flexelint-testapp.elf: $($(testapp.elf_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(testapp.elf_DISABLE_LINT)$(DISABLE_LINT)LOBS_testapp.elf)

$(TIDYS_testapp.elf): BUILDTARGET_TIDYFLAGS:=$(testapp.elf_TIDYFLAGS) -isystem $(OUT_DIR)/testapp.elf -include lint_mac.h

$(TIDYS_testapp.elf): testapp.elf_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_testapp.elf): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(testapp.elf_CLANG_TIDY_RULESET) $(testapp.elf_CLANG_TIDY_CHECKS)))

$(PLINTS_testapp.elf): BUILDTARGET_PLINTFLAGS:=$(testapp.elf_PLINTSFLAGS) -isystem $(OUT_DIR)/testapp.elf -include lint_mac.h

clang-tidy-testapp.elf: $($(testapp.elf_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(testapp.elf_DISABLE_LINT)$(DISABLE_LINT)TIDYS_testapp.elf)

clean-clang-tidy-testapp.elf:; $(SILENT)rm --force $(TIDYS_testapp.elf)

clangsa-testapp.elf: $(PLINTS_testapp.elf)

info-txt-testapp.elf: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,testapp.elf,$<)

$(OUT_DIR)/testapp.elf/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(testapp.elf_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/testapp.elf/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(testapp.elf_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/testapp.elf/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(testapp.elf_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

testapp.elf_PRECLANG_FILES=$(OUT_DIR)/testapp.elf/lint_mac.h

$(OUT_DIR)/testapp.elf/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(testapp.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/testapp.elf/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/testapp.elf/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(testapp.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/testapp.elf/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



cpuloadmonitor.elf_LINT_SOURCES ?= $(filter-out $(cpuloadmonitor.elf_NOLINT_SOURCES),$(cpuloadmonitor.elf_SOURCES))
OBJS_cpuloadmonitor.elf := $(call objs,$(cpuloadmonitor.elf_SOURCES),cpuloadmonitor.elf/)
DEPS_cpuloadmonitor.elf := $(call deps,$(cpuloadmonitor.elf_SOURCES),cpuloadmonitor.elf/)
LOBS_cpuloadmonitor.elf := $(call lobs,$(cpuloadmonitor.elf_LINT_SOURCES),cpuloadmonitor.elf/)
TIDYS_cpuloadmonitor.elf := $(call tidys,$(cpuloadmonitor.elf_LINT_SOURCES),cpuloadmonitor.elf/)
$(TIDYS_cpuloadmonitor.elf): $(cpuloadmonitor.elf_PRECLANG_FILES)
PLINTS_cpuloadmonitor.elf := $(call plints,$(cpuloadmonitor.elf_LINT_SOURCES),cpuloadmonitor.elf/)
$(PLINTS_cpuloadmonitor.elf): $(cpuloadmonitor.elf_PRECLANG_FILES)

$(if $(filter CLANG_TIDY_BASE_CHECKS CLANG_TIDY_CHECKS,$(cpuloadmonitor.elf_CLANG_TIDY_RULESET)),$(error cpuloadmonitor.elf_CLANG_TIDY_RULESET has "$(cpuloadmonitor.elf_CLANG_TIDY_RULESET)" assigned to it. Instead, assign "$$($(cpuloadmonitor.elf_CLANG_TIDY_RULESET))"))

cpuloadmonitor.elf: $(BIN_DIR)/cpuloadmonitor.elf

$(DEPS_cpuloadmonitor.elf) $(OBJS_cpuloadmonitor.elf) $(LOBS_cpuloadmonitor.elf) $(TIDYS_cpuloadmonitor.elf) $(PLINTS_cpuloadmonitor.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/cpuloadmonitor.elf/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(cpuloadmonitor.elf_CPPFLAGS)

$(DEPS_cpuloadmonitor.elf) $(OBJS_cpuloadmonitor.elf) $(TIDYS_cpuloadmonitor.elf) $(PLINTS_cpuloadmonitor.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/cpuloadmonitor.elf/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(cpuloadmonitor.elf_CXXFLAGS)

$(DEPS_cpuloadmonitor.elf) $(OBJS_cpuloadmonitor.elf) $(TIDYS_cpuloadmonitor.elf) $(PLINTS_cpuloadmonitor.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Release/cpuloadmonitor.elf/lint_mac.h: BUILDTARGET_CFLAGS:=$(cpuloadmonitor.elf_CFLAGS)

lint-cpuloadmonitor.elf: flexelint-cpuloadmonitor.elf clang-tidy-cpuloadmonitor.elf

$(LOBS_cpuloadmonitor.elf): BUILDTARGET_LINTFLAGS:=$(cpuloadmonitor.elf_LINTFLAGS)

flexelint-cpuloadmonitor.elf: $($(cpuloadmonitor.elf_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)$(cpuloadmonitor.elf_DISABLE_LINT)$(DISABLE_LINT)LOBS_cpuloadmonitor.elf)

$(TIDYS_cpuloadmonitor.elf): BUILDTARGET_TIDYFLAGS:=$(cpuloadmonitor.elf_TIDYFLAGS) -isystem $(OUT_DIR)/cpuloadmonitor.elf -include lint_mac.h

$(TIDYS_cpuloadmonitor.elf): cpuloadmonitor.elf_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_cpuloadmonitor.elf): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(cpuloadmonitor.elf_CLANG_TIDY_RULESET) $(cpuloadmonitor.elf_CLANG_TIDY_CHECKS)))

$(PLINTS_cpuloadmonitor.elf): BUILDTARGET_PLINTFLAGS:=$(cpuloadmonitor.elf_PLINTSFLAGS) -isystem $(OUT_DIR)/cpuloadmonitor.elf -include lint_mac.h

clang-tidy-cpuloadmonitor.elf: $($(cpuloadmonitor.elf_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)$(cpuloadmonitor.elf_DISABLE_LINT)$(DISABLE_LINT)TIDYS_cpuloadmonitor.elf)

clean-clang-tidy-cpuloadmonitor.elf:; $(SILENT)rm --force $(TIDYS_cpuloadmonitor.elf)

clangsa-cpuloadmonitor.elf: $(PLINTS_cpuloadmonitor.elf)

info-txt-cpuloadmonitor.elf: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,cpuloadmonitor.elf,$<)

$(OUT_DIR)/cpuloadmonitor.elf/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(cpuloadmonitor.elf_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/cpuloadmonitor.elf/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(cpuloadmonitor.elf_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/cpuloadmonitor.elf/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(cpuloadmonitor.elf_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

cpuloadmonitor.elf_PRECLANG_FILES=$(OUT_DIR)/cpuloadmonitor.elf/lint_mac.h

$(OUT_DIR)/cpuloadmonitor.elf/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(cpuloadmonitor.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/cpuloadmonitor.elf/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/cpuloadmonitor.elf/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(cpuloadmonitor.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Release/cpuloadmonitor.elf/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



$(DESTDIR)/usr/%/libcgroupcpuload.so.0.0.1: $(OUT_DIR)/libcgroupcpuload.so.dbg | $(DESTDIR)/usr/%; $(SILENT)$(call echo_if_silent,installing libcgroupcpuload.so to $@) && ln -Tsf libcgroupcpuload.so.0.0.1 $(DESTDIR)/usr/$*/libcgroupcpuload.so.0 && ln -Tsf libcgroupcpuload.so.0.0.1 $(DESTDIR)/usr/$*/libcgroupcpuload.so && cp $< $@

$(OUT_DIR)/libcgroupcpuload.so.dbg: $(OBJS_libcgroupcpuload.so) $(MK_FILES) $(libcgroupcpuload.so_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),libcgroupcpuload.so,$@,$(OBJS_libcgroupcpuload.so),$(LDFLAGS) $(libcgroupcpuload.so_LDFLAGS))

$(OUT_DIR)/loadcompare.elf.dbg: $(OBJS_loadcompare.elf) $(MK_FILES) $(loadcompare.elf_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),loadcompare.elf,$@,$(OBJS_loadcompare.elf),$(LDFLAGS) $(loadcompare.elf_LDFLAGS))

$(OUT_DIR)/checkcpu.elf.dbg: $(OBJS_checkcpu.elf) $(MK_FILES) $(checkcpu.elf_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),checkcpu.elf,$@,$(OBJS_checkcpu.elf),$(LDFLAGS) $(checkcpu.elf_LDFLAGS))

$(OUT_DIR)/bellysmile.elf.dbg: $(OBJS_bellysmile.elf) $(MK_FILES) $(bellysmile.elf_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),bellysmile.elf,$@,$(OBJS_bellysmile.elf),$(LDFLAGS) $(bellysmile.elf_LDFLAGS))

$(OUT_DIR)/testapp.elf.dbg: $(OBJS_testapp.elf) $(MK_FILES) $(testapp.elf_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),testapp.elf,$@,$(OBJS_testapp.elf),$(LDFLAGS) $(testapp.elf_LDFLAGS))

$(OUT_DIR)/cpuloadmonitor.elf.dbg: $(OBJS_cpuloadmonitor.elf) $(MK_FILES) $(cpuloadmonitor.elf_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),cpuloadmonitor.elf,$@,$(OBJS_cpuloadmonitor.elf),$(LDFLAGS) $(cpuloadmonitor.elf_LDFLAGS))

memcheck-loadcompare.elf: $(BIN_DIR)/loadcompare.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,loadcompare.elf,./$(notdir $<),MEMCHECK)
helgrind-loadcompare.elf: $(BIN_DIR)/loadcompare.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,loadcompare.elf,./$(notdir $<),HELGRIND)
callgrind-loadcompare.elf: $(BIN_DIR)/loadcompare.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,loadcompare.elf,./$(notdir $<),CALLGRIND)
massif-loadcompare.elf: $(BIN_DIR)/loadcompare.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,loadcompare.elf,./$(notdir $<),MASSIF)
nulgrind-loadcompare.elf: $(BIN_DIR)/loadcompare.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,loadcompare.elf,./$(notdir $<),NULGRIND)

memcheck-checkcpu.elf: $(BIN_DIR)/checkcpu.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,checkcpu.elf,./$(notdir $<),MEMCHECK)
helgrind-checkcpu.elf: $(BIN_DIR)/checkcpu.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,checkcpu.elf,./$(notdir $<),HELGRIND)
callgrind-checkcpu.elf: $(BIN_DIR)/checkcpu.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,checkcpu.elf,./$(notdir $<),CALLGRIND)
massif-checkcpu.elf: $(BIN_DIR)/checkcpu.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,checkcpu.elf,./$(notdir $<),MASSIF)
nulgrind-checkcpu.elf: $(BIN_DIR)/checkcpu.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,checkcpu.elf,./$(notdir $<),NULGRIND)

memcheck-bellysmile.elf: $(BIN_DIR)/bellysmile.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,bellysmile.elf,./$(notdir $<),MEMCHECK)
helgrind-bellysmile.elf: $(BIN_DIR)/bellysmile.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,bellysmile.elf,./$(notdir $<),HELGRIND)
callgrind-bellysmile.elf: $(BIN_DIR)/bellysmile.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,bellysmile.elf,./$(notdir $<),CALLGRIND)
massif-bellysmile.elf: $(BIN_DIR)/bellysmile.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,bellysmile.elf,./$(notdir $<),MASSIF)
nulgrind-bellysmile.elf: $(BIN_DIR)/bellysmile.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,bellysmile.elf,./$(notdir $<),NULGRIND)

memcheck-testapp.elf: $(BIN_DIR)/testapp.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,testapp.elf,./$(notdir $<),MEMCHECK)
helgrind-testapp.elf: $(BIN_DIR)/testapp.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,testapp.elf,./$(notdir $<),HELGRIND)
callgrind-testapp.elf: $(BIN_DIR)/testapp.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,testapp.elf,./$(notdir $<),CALLGRIND)
massif-testapp.elf: $(BIN_DIR)/testapp.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,testapp.elf,./$(notdir $<),MASSIF)
nulgrind-testapp.elf: $(BIN_DIR)/testapp.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,testapp.elf,./$(notdir $<),NULGRIND)

memcheck-cpuloadmonitor.elf: $(BIN_DIR)/cpuloadmonitor.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,cpuloadmonitor.elf,./$(notdir $<),MEMCHECK)
helgrind-cpuloadmonitor.elf: $(BIN_DIR)/cpuloadmonitor.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,cpuloadmonitor.elf,./$(notdir $<),HELGRIND)
callgrind-cpuloadmonitor.elf: $(BIN_DIR)/cpuloadmonitor.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,cpuloadmonitor.elf,./$(notdir $<),CALLGRIND)
massif-cpuloadmonitor.elf: $(BIN_DIR)/cpuloadmonitor.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,cpuloadmonitor.elf,./$(notdir $<),MASSIF)
nulgrind-cpuloadmonitor.elf: $(BIN_DIR)/cpuloadmonitor.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,cpuloadmonitor.elf,./$(notdir $<),NULGRIND)
