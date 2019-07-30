CPPFLAGS := $(CPPFLAGS)
CFLAGS := $(CFLAGS)
CXXFLAGS := $(CXXFLAGS)
LDFLAGS := $(LDFLAGS)
ARFLAGS := $(ARFLAGS)

libwagosnmp.so_SOURCES := $(libwagosnmp.so_SOURCES)
libwagosnmp.so_GENHEADERS := $(libwagosnmp.so_GENHEADERS)
libwagosnmp.so_CPPFLAGS := $(libwagosnmp.so_CPPFLAGS)
libwagosnmp.so_CFLAGS := $(libwagosnmp.so_CFLAGS)
libwagosnmp.so_CXXFLAGS := $(libwagosnmp.so_CXXFLAGS)
libwagosnmp.so_LDFLAGS := $(libwagosnmp.so_LDFLAGS)
libwagosnmp.so_ARFLAGS := $(libwagosnmp.so_ARFLAGS)

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

libwagosnmp.so_LINT_SOURCES ?= $(filter-out $(libwagosnmp.so_NOLINT_SOURCES),$(libwagosnmp.so_SOURCES))
OBJS_libwagosnmp.so := $(call objs,$(libwagosnmp.so_SOURCES),libwagosnmp.so/)
DEPS_libwagosnmp.so := $(call deps,$(libwagosnmp.so_SOURCES),libwagosnmp.so/)
LOBS_libwagosnmp.so := $(call lobs,$(libwagosnmp.so_LINT_SOURCES),libwagosnmp.so/)
TIDYS_libwagosnmp.so := $(call tidys,$(libwagosnmp.so_LINT_SOURCES),libwagosnmp.so/)
$(TIDYS_libwagosnmp.so): $(libwagosnmp.so_PRECLANG_FILES)
PLINTS_libwagosnmp.so := $(call plints,$(libwagosnmp.so_LINT_SOURCES),libwagosnmp.so/)
$(PLINTS_libwagosnmp.so): $(libwagosnmp.so_PRECLANG_FILES)

libwagosnmp.so: $(BIN_DIR)/libwagosnmp.so

$(DEPS_libwagosnmp.so) $(OBJS_libwagosnmp.so) $(LOBS_libwagosnmp.so) $(TIDYS_libwagosnmp.so) $(PLINTS_libwagosnmp.so) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libwagosnmp.so/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(libwagosnmp.so_CPPFLAGS)

$(DEPS_libwagosnmp.so) $(OBJS_libwagosnmp.so) $(TIDYS_libwagosnmp.so) $(PLINTS_libwagosnmp.so) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libwagosnmp.so/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(libwagosnmp.so_CXXFLAGS)

$(DEPS_libwagosnmp.so) $(OBJS_libwagosnmp.so) $(TIDYS_libwagosnmp.so) $(PLINTS_libwagosnmp.so) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libwagosnmp.so/lint_mac.h: BUILDTARGET_CFLAGS:=$(libwagosnmp.so_CFLAGS)

lint-libwagosnmp.so: flexelint-libwagosnmp.so clang-tidy-libwagosnmp.so

$(LOBS_libwagosnmp.so): BUILDTARGET_LINTFLAGS:=$(libwagosnmp.so_LINTFLAGS)

flexelint-libwagosnmp.so: $($(libwagosnmp.so_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)LOBS_libwagosnmp.so)

$(TIDYS_libwagosnmp.so): BUILDTARGET_TIDYFLAGS:=$(libwagosnmp.so_TIDYFLAGS) -isystem $(OUT_DIR)/libwagosnmp.so -include lint_mac.h

$(TIDYS_libwagosnmp.so): libwagosnmp.so_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_libwagosnmp.so): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(libwagosnmp.so_CLANG_TIDY_RULESET) $(libwagosnmp.so_CLANG_TIDY_CHECKS)))

$(PLINTS_libwagosnmp.so): BUILDTARGET_PLINTFLAGS:=$(libwagosnmp.so_PLINTSFLAGS) -isystem $(OUT_DIR)/libwagosnmp.so -include lint_mac.h

clang-tidy-libwagosnmp.so: $($(libwagosnmp.so_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)TIDYS_libwagosnmp.so)

clean-clang-tidy-libwagosnmp.so:; $(SILENT)rm --force $(TIDYS_libwagosnmp.so)

clangsa-libwagosnmp.so: $(PLINTS_libwagosnmp.so)

info-txt-libwagosnmp.so: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,libwagosnmp.so,$<)

$(OUT_DIR)/libwagosnmp.so/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(libwagosnmp.so_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/libwagosnmp.so/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(libwagosnmp.so_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/libwagosnmp.so/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(libwagosnmp.so_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

libwagosnmp.so_PRECLANG_FILES=$(OUT_DIR)/libwagosnmp.so/lint_mac.h

$(OUT_DIR)/libwagosnmp.so/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(libwagosnmp.so_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libwagosnmp.so/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/libwagosnmp.so/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(libwagosnmp.so_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/libwagosnmp.so/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



alltests.elf_LINT_SOURCES ?= $(filter-out $(alltests.elf_NOLINT_SOURCES),$(alltests.elf_SOURCES))
OBJS_alltests.elf := $(call objs,$(alltests.elf_SOURCES),alltests.elf/)
DEPS_alltests.elf := $(call deps,$(alltests.elf_SOURCES),alltests.elf/)
LOBS_alltests.elf := $(call lobs,$(alltests.elf_LINT_SOURCES),alltests.elf/)
TIDYS_alltests.elf := $(call tidys,$(alltests.elf_LINT_SOURCES),alltests.elf/)
$(TIDYS_alltests.elf): $(alltests.elf_PRECLANG_FILES)
PLINTS_alltests.elf := $(call plints,$(alltests.elf_LINT_SOURCES),alltests.elf/)
$(PLINTS_alltests.elf): $(alltests.elf_PRECLANG_FILES)

alltests.elf: $(BIN_DIR)/alltests.elf

$(DEPS_alltests.elf) $(OBJS_alltests.elf) $(LOBS_alltests.elf) $(TIDYS_alltests.elf) $(PLINTS_alltests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/alltests.elf/lint_mac.h: BUILDTARGET_CPPFLAGS:=$(alltests.elf_CPPFLAGS)

$(DEPS_alltests.elf) $(OBJS_alltests.elf) $(TIDYS_alltests.elf) $(PLINTS_alltests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/alltests.elf/lint_mac.h: BUILDTARGET_CXXFLAGS:=$(alltests.elf_CXXFLAGS)

$(DEPS_alltests.elf) $(OBJS_alltests.elf) $(TIDYS_alltests.elf) $(PLINTS_alltests.elf) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/alltests.elf/lint_mac.h: BUILDTARGET_CFLAGS:=$(alltests.elf_CFLAGS)

lint-alltests.elf: flexelint-alltests.elf clang-tidy-alltests.elf

$(LOBS_alltests.elf): BUILDTARGET_LINTFLAGS:=$(alltests.elf_LINTFLAGS)

flexelint-alltests.elf: $($(alltests.elf_DISABLE_FLEXELINT)$(DISABLE_FLEXELINT)LOBS_alltests.elf)

$(TIDYS_alltests.elf): BUILDTARGET_TIDYFLAGS:=$(alltests.elf_TIDYFLAGS) -isystem $(OUT_DIR)/alltests.elf -include lint_mac.h

$(TIDYS_alltests.elf): alltests.elf_CLANG_TIDY_RULESET ?= $(CLANG_TIDY_RULESET)

$(TIDYS_alltests.elf): CLANG_TIDY_CHECKS_OPTION:=$(subst $( ),$(,),$(strip $(alltests.elf_CLANG_TIDY_RULESET) $(alltests.elf_CLANG_TIDY_CHECKS)))

$(PLINTS_alltests.elf): BUILDTARGET_PLINTFLAGS:=$(alltests.elf_PLINTSFLAGS) -isystem $(OUT_DIR)/alltests.elf -include lint_mac.h

clang-tidy-alltests.elf: $($(alltests.elf_DISABLE_CLANG_TIDY)$(DISABLE_CLANG_TIDY)TIDYS_alltests.elf)

clean-clang-tidy-alltests.elf:; $(SILENT)rm --force $(TIDYS_alltests.elf)

clangsa-alltests.elf: $(PLINTS_alltests.elf)

info-txt-alltests.elf: $(SCRIPT_DIR)/internal/clang/info_txt.template $(MK_FILES); $(SILENT)$(call clangbuild_subst_info_buildtarget,alltests.elf,$<)

$(OUT_DIR)/alltests.elf/%.o.d: $(PROJECT_ROOT)/% $(MK_FILES) $(MAKEDEP_SUPPORT_FILES) | $(alltests.elf_GENHEADERS); $(SILENT)$(call makedep,$@,$<,$(FLAGS))

$(OUT_DIR)/alltests.elf/%.o: $(PROJECT_ROOT)/% $(MK_FILES) $(CC_SUPPORT_FILES) | $(alltests.elf_GENHEADERS); $(SILENT)$(call cc,$@,$<,$(FLAGS))

$(OUT_DIR)/alltests.elf/%.lob: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) | $(alltests.elf_GENHEADERS); $(SILENT)$(call lint,$@,$<,$(FLAGS))

alltests.elf_PRECLANG_FILES=$(OUT_DIR)/alltests.elf/lint_mac.h

$(OUT_DIR)/alltests.elf/%.tidy: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(alltests.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/alltests.elf/%.o.d $(wildcard .clang-tidy); $(SILENT)$(call clang-tidy,$@,$<,$(FLAGS))

$(OUT_DIR)/alltests.elf/%.plint: $(PROJECT_ROOT)/% $(MK_FILES) $(LINT_SUPPORT_FILES) $(alltests.elf_PRECLANG_FILES) ./out/gcc_5_5_arm-linux-gnueabihf/Debug/alltests.elf/%.o.d; $(SILENT)$(call clang-sa,$@,$<,$(FLAGS))



$(DESTDIR)/usr/%/libwagosnmp.so.0.0.2: $(OUT_DIR)/libwagosnmp.so.dbg | $(DESTDIR)/usr/%; $(SILENT)$(call echo_if_silent,installing libwagosnmp.so to $@) && ln -Tsf libwagosnmp.so.0.0.2 $(DESTDIR)/usr/$*/libwagosnmp.so.0 && ln -Tsf libwagosnmp.so.0.0.2 $(DESTDIR)/usr/$*/libwagosnmp.so && cp $< $@

$(OUT_DIR)/libwagosnmp.so.dbg: $(OBJS_libwagosnmp.so) $(MK_FILES) $(libwagosnmp.so_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),libwagosnmp.so,$@,$(OBJS_libwagosnmp.so),$(LDFLAGS) $(libwagosnmp.so_LDFLAGS))

$(OUT_DIR)/alltests.elf.dbg: $(OBJS_alltests.elf) $(MK_FILES) $(alltests.elf_PREREQUISITES) $(LD_SUPPORT_FILES); $(SILENT)$(call ld$(ARTIFACT),alltests.elf,$@,$(OBJS_alltests.elf),$(LDFLAGS) $(alltests.elf_LDFLAGS))

check-alltests.elf: $(BIN_DIR)/alltests.elf; $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_check,alltests.elf,./$(notdir $<))

memcheck-alltests.elf: $(BIN_DIR)/alltests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,alltests.elf,./$(notdir $<),MEMCHECK)
helgrind-alltests.elf: $(BIN_DIR)/alltests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,alltests.elf,./$(notdir $<),HELGRIND)
callgrind-alltests.elf: $(BIN_DIR)/alltests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,alltests.elf,./$(notdir $<),CALLGRIND)
massif-alltests.elf: $(BIN_DIR)/alltests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,alltests.elf,./$(notdir $<),MASSIF)
nulgrind-alltests.elf: $(BIN_DIR)/alltests.elf $(VALGRIND); $(CHECK_IGNORE_ERROR)$(SILENT) $(call echo_colored,PURPLE,$<) && cd $(dir $<) && $(call clangbuild_valgrind,alltests.elf,./$(notdir $<),NULGRIND)
