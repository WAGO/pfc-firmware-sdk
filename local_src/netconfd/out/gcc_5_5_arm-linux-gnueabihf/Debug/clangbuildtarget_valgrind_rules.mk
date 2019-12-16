### generated targets ###
.PHONY: valgrind-target
.PHONY: valgrind-target-netconfd.elf
.PHONY: valgrind-target-bridgeconfig_dsa.elf
.PHONY: valgrind-target-bal_tests.elf
.PHONY: valgrind-target-netconfd_tests.elf
.PHONY: valgrind-target-utility_tests.elf

### valgrind-target ###
valgrind-target: $(VG_LOCAL_VALGRIND)
valgrind-target: memcheck-target helgrind-target
valgrind-target-netconfd.elf: memcheck-target-netconfd.elf helgrind-target-netconfd.elf
valgrind-target-bridgeconfig_dsa.elf: memcheck-target-bridgeconfig_dsa.elf helgrind-target-bridgeconfig_dsa.elf
valgrind-target-bal_tests.elf: memcheck-target-bal_tests.elf helgrind-target-bal_tests.elf
valgrind-target-netconfd_tests.elf: memcheck-target-netconfd_tests.elf helgrind-target-netconfd_tests.elf
valgrind-target-utility_tests.elf: memcheck-target-utility_tests.elf helgrind-target-utility_tests.elf

### nulgrind-target ###
NULGRIND_TARGET_TARGETS += nulgrind-target-bal_tests.elf
NULGRIND_TARGET_TARGETS += nulgrind-target-netconfd_tests.elf
NULGRIND_TARGET_TARGETS += nulgrind-target-utility_tests.elf

nulgrind-target-netconfd.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(netconfd.elf_VALGRIND_FLAGS)        $(NULGRIND_FLAGS)        $(netconfd.elf_NULGRIND_FLAGS) $(call valgrind_xml_flags,netconfd.elf,NULGRIND)
nulgrind-target-netconfd.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(netconfd.elf_VALGRIND_TARGET_FLAGS) $(NULGRIND_TARGET_FLAGS) $(netconfd.elf_NULGRIND_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
nulgrind-target-netconfd.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
nulgrind-target-netconfd.elf: VALGRIND_TARGET_EXECUTABLE=netconfd.elf
nulgrind-target-netconfd.elf: VG_TOOL=nulgrind
nulgrind-target-netconfd.elf: $(BIN_DIR)/netconfd.elf $(VALGRIND)
nulgrind-target-netconfd.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
nulgrind-target-netconfd.elf:
	$(call vg-caller)

nulgrind-target-bridgeconfig_dsa.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(bridgeconfig_dsa.elf_VALGRIND_FLAGS)        $(NULGRIND_FLAGS)        $(bridgeconfig_dsa.elf_NULGRIND_FLAGS) $(call valgrind_xml_flags,bridgeconfig_dsa.elf,NULGRIND)
nulgrind-target-bridgeconfig_dsa.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(bridgeconfig_dsa.elf_VALGRIND_TARGET_FLAGS) $(NULGRIND_TARGET_FLAGS) $(bridgeconfig_dsa.elf_NULGRIND_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
nulgrind-target-bridgeconfig_dsa.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
nulgrind-target-bridgeconfig_dsa.elf: VALGRIND_TARGET_EXECUTABLE=bridgeconfig_dsa.elf
nulgrind-target-bridgeconfig_dsa.elf: VG_TOOL=nulgrind
nulgrind-target-bridgeconfig_dsa.elf: $(BIN_DIR)/bridgeconfig_dsa.elf $(VALGRIND)
nulgrind-target-bridgeconfig_dsa.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
nulgrind-target-bridgeconfig_dsa.elf:
	$(call vg-caller)

nulgrind-target-bal_tests.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(bal_tests.elf_VALGRIND_FLAGS)        $(NULGRIND_FLAGS)        $(bal_tests.elf_NULGRIND_FLAGS) $(call valgrind_xml_flags,bal_tests.elf,NULGRIND)
nulgrind-target-bal_tests.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(bal_tests.elf_VALGRIND_TARGET_FLAGS) $(NULGRIND_TARGET_FLAGS) $(bal_tests.elf_NULGRIND_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
nulgrind-target-bal_tests.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
nulgrind-target-bal_tests.elf: VALGRIND_TARGET_EXECUTABLE=bal_tests.elf
nulgrind-target-bal_tests.elf: VG_TOOL=nulgrind
nulgrind-target-bal_tests.elf: $(BIN_DIR)/bal_tests.elf $(VALGRIND)
nulgrind-target-bal_tests.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
nulgrind-target-bal_tests.elf:
	$(call vg-caller)

nulgrind-target-netconfd_tests.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(netconfd_tests.elf_VALGRIND_FLAGS)        $(NULGRIND_FLAGS)        $(netconfd_tests.elf_NULGRIND_FLAGS) $(call valgrind_xml_flags,netconfd_tests.elf,NULGRIND)
nulgrind-target-netconfd_tests.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(netconfd_tests.elf_VALGRIND_TARGET_FLAGS) $(NULGRIND_TARGET_FLAGS) $(netconfd_tests.elf_NULGRIND_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
nulgrind-target-netconfd_tests.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
nulgrind-target-netconfd_tests.elf: VALGRIND_TARGET_EXECUTABLE=netconfd_tests.elf
nulgrind-target-netconfd_tests.elf: VG_TOOL=nulgrind
nulgrind-target-netconfd_tests.elf: $(BIN_DIR)/netconfd_tests.elf $(VALGRIND)
nulgrind-target-netconfd_tests.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
nulgrind-target-netconfd_tests.elf:
	$(call vg-caller)

nulgrind-target-utility_tests.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(utility_tests.elf_VALGRIND_FLAGS)        $(NULGRIND_FLAGS)        $(utility_tests.elf_NULGRIND_FLAGS) $(call valgrind_xml_flags,utility_tests.elf,NULGRIND)
nulgrind-target-utility_tests.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(utility_tests.elf_VALGRIND_TARGET_FLAGS) $(NULGRIND_TARGET_FLAGS) $(utility_tests.elf_NULGRIND_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
nulgrind-target-utility_tests.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
nulgrind-target-utility_tests.elf: VALGRIND_TARGET_EXECUTABLE=utility_tests.elf
nulgrind-target-utility_tests.elf: VG_TOOL=nulgrind
nulgrind-target-utility_tests.elf: $(BIN_DIR)/utility_tests.elf $(VALGRIND)
nulgrind-target-utility_tests.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
nulgrind-target-utility_tests.elf:
	$(call vg-caller)


nulgrind-target: $(VG_LOCAL_VALGRIND) $(NULGRIND_TARGET_TARGETS)
$(NULGRIND_TARGET_TARGETS): $(SYNC_SETTINGS_MK_TMP)

### memcheck-target ###
MEMCHECK_TARGET_TARGETS += memcheck-target-bal_tests.elf
MEMCHECK_TARGET_TARGETS += memcheck-target-netconfd_tests.elf
MEMCHECK_TARGET_TARGETS += memcheck-target-utility_tests.elf

memcheck-target-netconfd.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(netconfd.elf_VALGRIND_FLAGS)        $(MEMCHECK_FLAGS)        $(netconfd.elf_MEMCHECK_FLAGS) $(call valgrind_xml_flags,netconfd.elf,MEMCHECK)
memcheck-target-netconfd.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(netconfd.elf_VALGRIND_TARGET_FLAGS) $(MEMCHECK_TARGET_FLAGS) $(netconfd.elf_MEMCHECK_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
memcheck-target-netconfd.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
memcheck-target-netconfd.elf: VALGRIND_TARGET_EXECUTABLE=netconfd.elf
memcheck-target-netconfd.elf: VG_TOOL=memcheck
memcheck-target-netconfd.elf: $(BIN_DIR)/netconfd.elf $(VALGRIND)
memcheck-target-netconfd.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
memcheck-target-netconfd.elf:
	$(call vg-caller)

memcheck-target-bridgeconfig_dsa.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(bridgeconfig_dsa.elf_VALGRIND_FLAGS)        $(MEMCHECK_FLAGS)        $(bridgeconfig_dsa.elf_MEMCHECK_FLAGS) $(call valgrind_xml_flags,bridgeconfig_dsa.elf,MEMCHECK)
memcheck-target-bridgeconfig_dsa.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(bridgeconfig_dsa.elf_VALGRIND_TARGET_FLAGS) $(MEMCHECK_TARGET_FLAGS) $(bridgeconfig_dsa.elf_MEMCHECK_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
memcheck-target-bridgeconfig_dsa.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
memcheck-target-bridgeconfig_dsa.elf: VALGRIND_TARGET_EXECUTABLE=bridgeconfig_dsa.elf
memcheck-target-bridgeconfig_dsa.elf: VG_TOOL=memcheck
memcheck-target-bridgeconfig_dsa.elf: $(BIN_DIR)/bridgeconfig_dsa.elf $(VALGRIND)
memcheck-target-bridgeconfig_dsa.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
memcheck-target-bridgeconfig_dsa.elf:
	$(call vg-caller)

memcheck-target-bal_tests.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(bal_tests.elf_VALGRIND_FLAGS)        $(MEMCHECK_FLAGS)        $(bal_tests.elf_MEMCHECK_FLAGS) $(call valgrind_xml_flags,bal_tests.elf,MEMCHECK)
memcheck-target-bal_tests.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(bal_tests.elf_VALGRIND_TARGET_FLAGS) $(MEMCHECK_TARGET_FLAGS) $(bal_tests.elf_MEMCHECK_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
memcheck-target-bal_tests.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
memcheck-target-bal_tests.elf: VALGRIND_TARGET_EXECUTABLE=bal_tests.elf
memcheck-target-bal_tests.elf: VG_TOOL=memcheck
memcheck-target-bal_tests.elf: $(BIN_DIR)/bal_tests.elf $(VALGRIND)
memcheck-target-bal_tests.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
memcheck-target-bal_tests.elf:
	$(call vg-caller)

memcheck-target-netconfd_tests.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(netconfd_tests.elf_VALGRIND_FLAGS)        $(MEMCHECK_FLAGS)        $(netconfd_tests.elf_MEMCHECK_FLAGS) $(call valgrind_xml_flags,netconfd_tests.elf,MEMCHECK)
memcheck-target-netconfd_tests.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(netconfd_tests.elf_VALGRIND_TARGET_FLAGS) $(MEMCHECK_TARGET_FLAGS) $(netconfd_tests.elf_MEMCHECK_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
memcheck-target-netconfd_tests.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
memcheck-target-netconfd_tests.elf: VALGRIND_TARGET_EXECUTABLE=netconfd_tests.elf
memcheck-target-netconfd_tests.elf: VG_TOOL=memcheck
memcheck-target-netconfd_tests.elf: $(BIN_DIR)/netconfd_tests.elf $(VALGRIND)
memcheck-target-netconfd_tests.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
memcheck-target-netconfd_tests.elf:
	$(call vg-caller)

memcheck-target-utility_tests.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(utility_tests.elf_VALGRIND_FLAGS)        $(MEMCHECK_FLAGS)        $(utility_tests.elf_MEMCHECK_FLAGS) $(call valgrind_xml_flags,utility_tests.elf,MEMCHECK)
memcheck-target-utility_tests.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(utility_tests.elf_VALGRIND_TARGET_FLAGS) $(MEMCHECK_TARGET_FLAGS) $(utility_tests.elf_MEMCHECK_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
memcheck-target-utility_tests.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
memcheck-target-utility_tests.elf: VALGRIND_TARGET_EXECUTABLE=utility_tests.elf
memcheck-target-utility_tests.elf: VG_TOOL=memcheck
memcheck-target-utility_tests.elf: $(BIN_DIR)/utility_tests.elf $(VALGRIND)
memcheck-target-utility_tests.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
memcheck-target-utility_tests.elf:
	$(call vg-caller)


memcheck-target: $(VG_LOCAL_VALGRIND) $(MEMCHECK_TARGET_TARGETS)
$(MEMCHECK_TARGET_TARGETS): $(SYNC_SETTINGS_MK_TMP)

### helgrind-target ###
HELGRIND_TARGET_TARGETS += helgrind-target-bal_tests.elf
HELGRIND_TARGET_TARGETS += helgrind-target-netconfd_tests.elf
HELGRIND_TARGET_TARGETS += helgrind-target-utility_tests.elf

helgrind-target-netconfd.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(netconfd.elf_VALGRIND_FLAGS)        $(HELGRIND_FLAGS)        $(netconfd.elf_HELGRIND_FLAGS) $(call valgrind_xml_flags,netconfd.elf,HELGRIND)
helgrind-target-netconfd.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(netconfd.elf_VALGRIND_TARGET_FLAGS) $(HELGRIND_TARGET_FLAGS) $(netconfd.elf_HELGRIND_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
helgrind-target-netconfd.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
helgrind-target-netconfd.elf: VALGRIND_TARGET_EXECUTABLE=netconfd.elf
helgrind-target-netconfd.elf: VG_TOOL=helgrind
helgrind-target-netconfd.elf: $(BIN_DIR)/netconfd.elf $(VALGRIND)
helgrind-target-netconfd.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
helgrind-target-netconfd.elf:
	$(call vg-caller)

helgrind-target-bridgeconfig_dsa.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(bridgeconfig_dsa.elf_VALGRIND_FLAGS)        $(HELGRIND_FLAGS)        $(bridgeconfig_dsa.elf_HELGRIND_FLAGS) $(call valgrind_xml_flags,bridgeconfig_dsa.elf,HELGRIND)
helgrind-target-bridgeconfig_dsa.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(bridgeconfig_dsa.elf_VALGRIND_TARGET_FLAGS) $(HELGRIND_TARGET_FLAGS) $(bridgeconfig_dsa.elf_HELGRIND_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
helgrind-target-bridgeconfig_dsa.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
helgrind-target-bridgeconfig_dsa.elf: VALGRIND_TARGET_EXECUTABLE=bridgeconfig_dsa.elf
helgrind-target-bridgeconfig_dsa.elf: VG_TOOL=helgrind
helgrind-target-bridgeconfig_dsa.elf: $(BIN_DIR)/bridgeconfig_dsa.elf $(VALGRIND)
helgrind-target-bridgeconfig_dsa.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
helgrind-target-bridgeconfig_dsa.elf:
	$(call vg-caller)

helgrind-target-bal_tests.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(bal_tests.elf_VALGRIND_FLAGS)        $(HELGRIND_FLAGS)        $(bal_tests.elf_HELGRIND_FLAGS) $(call valgrind_xml_flags,bal_tests.elf,HELGRIND)
helgrind-target-bal_tests.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(bal_tests.elf_VALGRIND_TARGET_FLAGS) $(HELGRIND_TARGET_FLAGS) $(bal_tests.elf_HELGRIND_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
helgrind-target-bal_tests.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
helgrind-target-bal_tests.elf: VALGRIND_TARGET_EXECUTABLE=bal_tests.elf
helgrind-target-bal_tests.elf: VG_TOOL=helgrind
helgrind-target-bal_tests.elf: $(BIN_DIR)/bal_tests.elf $(VALGRIND)
helgrind-target-bal_tests.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
helgrind-target-bal_tests.elf:
	$(call vg-caller)

helgrind-target-netconfd_tests.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(netconfd_tests.elf_VALGRIND_FLAGS)        $(HELGRIND_FLAGS)        $(netconfd_tests.elf_HELGRIND_FLAGS) $(call valgrind_xml_flags,netconfd_tests.elf,HELGRIND)
helgrind-target-netconfd_tests.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(netconfd_tests.elf_VALGRIND_TARGET_FLAGS) $(HELGRIND_TARGET_FLAGS) $(netconfd_tests.elf_HELGRIND_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
helgrind-target-netconfd_tests.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
helgrind-target-netconfd_tests.elf: VALGRIND_TARGET_EXECUTABLE=netconfd_tests.elf
helgrind-target-netconfd_tests.elf: VG_TOOL=helgrind
helgrind-target-netconfd_tests.elf: $(BIN_DIR)/netconfd_tests.elf $(VALGRIND)
helgrind-target-netconfd_tests.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
helgrind-target-netconfd_tests.elf:
	$(call vg-caller)

helgrind-target-utility_tests.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(utility_tests.elf_VALGRIND_FLAGS)        $(HELGRIND_FLAGS)        $(utility_tests.elf_HELGRIND_FLAGS) $(call valgrind_xml_flags,utility_tests.elf,HELGRIND)
helgrind-target-utility_tests.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(utility_tests.elf_VALGRIND_TARGET_FLAGS) $(HELGRIND_TARGET_FLAGS) $(utility_tests.elf_HELGRIND_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
helgrind-target-utility_tests.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
helgrind-target-utility_tests.elf: VALGRIND_TARGET_EXECUTABLE=utility_tests.elf
helgrind-target-utility_tests.elf: VG_TOOL=helgrind
helgrind-target-utility_tests.elf: $(BIN_DIR)/utility_tests.elf $(VALGRIND)
helgrind-target-utility_tests.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
helgrind-target-utility_tests.elf:
	$(call vg-caller)


helgrind-target: $(VG_LOCAL_VALGRIND) $(HELGRIND_TARGET_TARGETS)
$(HELGRIND_TARGET_TARGETS): $(SYNC_SETTINGS_MK_TMP)

### callgrind-target ###
CALLGRIND_TARGET_TARGETS += callgrind-target-bal_tests.elf
CALLGRIND_TARGET_TARGETS += callgrind-target-netconfd_tests.elf
CALLGRIND_TARGET_TARGETS += callgrind-target-utility_tests.elf

callgrind-target-netconfd.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(netconfd.elf_VALGRIND_FLAGS)        $(CALLGRIND_FLAGS)        $(netconfd.elf_CALLGRIND_FLAGS) $(call valgrind_xml_flags,netconfd.elf,CALLGRIND)
callgrind-target-netconfd.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(netconfd.elf_VALGRIND_TARGET_FLAGS) $(CALLGRIND_TARGET_FLAGS) $(netconfd.elf_CALLGRIND_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
callgrind-target-netconfd.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
callgrind-target-netconfd.elf: VALGRIND_TARGET_EXECUTABLE=netconfd.elf
callgrind-target-netconfd.elf: VG_TOOL=callgrind
callgrind-target-netconfd.elf: $(BIN_DIR)/netconfd.elf $(VALGRIND)
callgrind-target-netconfd.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
callgrind-target-netconfd.elf:
	$(call vg-caller)

callgrind-target-bridgeconfig_dsa.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(bridgeconfig_dsa.elf_VALGRIND_FLAGS)        $(CALLGRIND_FLAGS)        $(bridgeconfig_dsa.elf_CALLGRIND_FLAGS) $(call valgrind_xml_flags,bridgeconfig_dsa.elf,CALLGRIND)
callgrind-target-bridgeconfig_dsa.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(bridgeconfig_dsa.elf_VALGRIND_TARGET_FLAGS) $(CALLGRIND_TARGET_FLAGS) $(bridgeconfig_dsa.elf_CALLGRIND_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
callgrind-target-bridgeconfig_dsa.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
callgrind-target-bridgeconfig_dsa.elf: VALGRIND_TARGET_EXECUTABLE=bridgeconfig_dsa.elf
callgrind-target-bridgeconfig_dsa.elf: VG_TOOL=callgrind
callgrind-target-bridgeconfig_dsa.elf: $(BIN_DIR)/bridgeconfig_dsa.elf $(VALGRIND)
callgrind-target-bridgeconfig_dsa.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
callgrind-target-bridgeconfig_dsa.elf:
	$(call vg-caller)

callgrind-target-bal_tests.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(bal_tests.elf_VALGRIND_FLAGS)        $(CALLGRIND_FLAGS)        $(bal_tests.elf_CALLGRIND_FLAGS) $(call valgrind_xml_flags,bal_tests.elf,CALLGRIND)
callgrind-target-bal_tests.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(bal_tests.elf_VALGRIND_TARGET_FLAGS) $(CALLGRIND_TARGET_FLAGS) $(bal_tests.elf_CALLGRIND_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
callgrind-target-bal_tests.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
callgrind-target-bal_tests.elf: VALGRIND_TARGET_EXECUTABLE=bal_tests.elf
callgrind-target-bal_tests.elf: VG_TOOL=callgrind
callgrind-target-bal_tests.elf: $(BIN_DIR)/bal_tests.elf $(VALGRIND)
callgrind-target-bal_tests.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
callgrind-target-bal_tests.elf:
	$(call vg-caller)

callgrind-target-netconfd_tests.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(netconfd_tests.elf_VALGRIND_FLAGS)        $(CALLGRIND_FLAGS)        $(netconfd_tests.elf_CALLGRIND_FLAGS) $(call valgrind_xml_flags,netconfd_tests.elf,CALLGRIND)
callgrind-target-netconfd_tests.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(netconfd_tests.elf_VALGRIND_TARGET_FLAGS) $(CALLGRIND_TARGET_FLAGS) $(netconfd_tests.elf_CALLGRIND_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
callgrind-target-netconfd_tests.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
callgrind-target-netconfd_tests.elf: VALGRIND_TARGET_EXECUTABLE=netconfd_tests.elf
callgrind-target-netconfd_tests.elf: VG_TOOL=callgrind
callgrind-target-netconfd_tests.elf: $(BIN_DIR)/netconfd_tests.elf $(VALGRIND)
callgrind-target-netconfd_tests.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
callgrind-target-netconfd_tests.elf:
	$(call vg-caller)

callgrind-target-utility_tests.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(utility_tests.elf_VALGRIND_FLAGS)        $(CALLGRIND_FLAGS)        $(utility_tests.elf_CALLGRIND_FLAGS) $(call valgrind_xml_flags,utility_tests.elf,CALLGRIND)
callgrind-target-utility_tests.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(utility_tests.elf_VALGRIND_TARGET_FLAGS) $(CALLGRIND_TARGET_FLAGS) $(utility_tests.elf_CALLGRIND_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
callgrind-target-utility_tests.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
callgrind-target-utility_tests.elf: VALGRIND_TARGET_EXECUTABLE=utility_tests.elf
callgrind-target-utility_tests.elf: VG_TOOL=callgrind
callgrind-target-utility_tests.elf: $(BIN_DIR)/utility_tests.elf $(VALGRIND)
callgrind-target-utility_tests.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
callgrind-target-utility_tests.elf:
	$(call vg-caller)


callgrind-target: $(VG_LOCAL_VALGRIND) $(CALLGRIND_TARGET_TARGETS)
$(CALLGRIND_TARGET_TARGETS): $(SYNC_SETTINGS_MK_TMP)

### cachegrind-target ###
CACHEGRIND_TARGET_TARGETS += cachegrind-target-bal_tests.elf
CACHEGRIND_TARGET_TARGETS += cachegrind-target-netconfd_tests.elf
CACHEGRIND_TARGET_TARGETS += cachegrind-target-utility_tests.elf

cachegrind-target-netconfd.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(netconfd.elf_VALGRIND_FLAGS)        $(CACHEGRIND_FLAGS)        $(netconfd.elf_CACHEGRIND_FLAGS) $(call valgrind_xml_flags,netconfd.elf,CACHEGRIND)
cachegrind-target-netconfd.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(netconfd.elf_VALGRIND_TARGET_FLAGS) $(CACHEGRIND_TARGET_FLAGS) $(netconfd.elf_CACHEGRIND_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
cachegrind-target-netconfd.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
cachegrind-target-netconfd.elf: VALGRIND_TARGET_EXECUTABLE=netconfd.elf
cachegrind-target-netconfd.elf: VG_TOOL=cachegrind
cachegrind-target-netconfd.elf: $(BIN_DIR)/netconfd.elf $(VALGRIND)
cachegrind-target-netconfd.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
cachegrind-target-netconfd.elf:
	$(call vg-caller)

cachegrind-target-bridgeconfig_dsa.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(bridgeconfig_dsa.elf_VALGRIND_FLAGS)        $(CACHEGRIND_FLAGS)        $(bridgeconfig_dsa.elf_CACHEGRIND_FLAGS) $(call valgrind_xml_flags,bridgeconfig_dsa.elf,CACHEGRIND)
cachegrind-target-bridgeconfig_dsa.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(bridgeconfig_dsa.elf_VALGRIND_TARGET_FLAGS) $(CACHEGRIND_TARGET_FLAGS) $(bridgeconfig_dsa.elf_CACHEGRIND_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
cachegrind-target-bridgeconfig_dsa.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
cachegrind-target-bridgeconfig_dsa.elf: VALGRIND_TARGET_EXECUTABLE=bridgeconfig_dsa.elf
cachegrind-target-bridgeconfig_dsa.elf: VG_TOOL=cachegrind
cachegrind-target-bridgeconfig_dsa.elf: $(BIN_DIR)/bridgeconfig_dsa.elf $(VALGRIND)
cachegrind-target-bridgeconfig_dsa.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
cachegrind-target-bridgeconfig_dsa.elf:
	$(call vg-caller)

cachegrind-target-bal_tests.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(bal_tests.elf_VALGRIND_FLAGS)        $(CACHEGRIND_FLAGS)        $(bal_tests.elf_CACHEGRIND_FLAGS) $(call valgrind_xml_flags,bal_tests.elf,CACHEGRIND)
cachegrind-target-bal_tests.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(bal_tests.elf_VALGRIND_TARGET_FLAGS) $(CACHEGRIND_TARGET_FLAGS) $(bal_tests.elf_CACHEGRIND_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
cachegrind-target-bal_tests.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
cachegrind-target-bal_tests.elf: VALGRIND_TARGET_EXECUTABLE=bal_tests.elf
cachegrind-target-bal_tests.elf: VG_TOOL=cachegrind
cachegrind-target-bal_tests.elf: $(BIN_DIR)/bal_tests.elf $(VALGRIND)
cachegrind-target-bal_tests.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
cachegrind-target-bal_tests.elf:
	$(call vg-caller)

cachegrind-target-netconfd_tests.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(netconfd_tests.elf_VALGRIND_FLAGS)        $(CACHEGRIND_FLAGS)        $(netconfd_tests.elf_CACHEGRIND_FLAGS) $(call valgrind_xml_flags,netconfd_tests.elf,CACHEGRIND)
cachegrind-target-netconfd_tests.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(netconfd_tests.elf_VALGRIND_TARGET_FLAGS) $(CACHEGRIND_TARGET_FLAGS) $(netconfd_tests.elf_CACHEGRIND_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
cachegrind-target-netconfd_tests.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
cachegrind-target-netconfd_tests.elf: VALGRIND_TARGET_EXECUTABLE=netconfd_tests.elf
cachegrind-target-netconfd_tests.elf: VG_TOOL=cachegrind
cachegrind-target-netconfd_tests.elf: $(BIN_DIR)/netconfd_tests.elf $(VALGRIND)
cachegrind-target-netconfd_tests.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
cachegrind-target-netconfd_tests.elf:
	$(call vg-caller)

cachegrind-target-utility_tests.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(utility_tests.elf_VALGRIND_FLAGS)        $(CACHEGRIND_FLAGS)        $(utility_tests.elf_CACHEGRIND_FLAGS) $(call valgrind_xml_flags,utility_tests.elf,CACHEGRIND)
cachegrind-target-utility_tests.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(utility_tests.elf_VALGRIND_TARGET_FLAGS) $(CACHEGRIND_TARGET_FLAGS) $(utility_tests.elf_CACHEGRIND_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
cachegrind-target-utility_tests.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
cachegrind-target-utility_tests.elf: VALGRIND_TARGET_EXECUTABLE=utility_tests.elf
cachegrind-target-utility_tests.elf: VG_TOOL=cachegrind
cachegrind-target-utility_tests.elf: $(BIN_DIR)/utility_tests.elf $(VALGRIND)
cachegrind-target-utility_tests.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
cachegrind-target-utility_tests.elf:
	$(call vg-caller)


cachegrind-target: $(VG_LOCAL_VALGRIND) $(CACHEGRIND_TARGET_TARGETS)
$(CACHEGRIND_TARGET_TARGETS): $(SYNC_SETTINGS_MK_TMP)

### massif-target ###
MASSIF_TARGET_TARGETS += massif-target-bal_tests.elf
MASSIF_TARGET_TARGETS += massif-target-netconfd_tests.elf
MASSIF_TARGET_TARGETS += massif-target-utility_tests.elf

massif-target-netconfd.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(netconfd.elf_VALGRIND_FLAGS)        $(MASSIF_FLAGS)        $(netconfd.elf_MASSIF_FLAGS) $(call valgrind_xml_flags,netconfd.elf,MASSIF)
massif-target-netconfd.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(netconfd.elf_VALGRIND_TARGET_FLAGS) $(MASSIF_TARGET_FLAGS) $(netconfd.elf_MASSIF_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
massif-target-netconfd.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
massif-target-netconfd.elf: VALGRIND_TARGET_EXECUTABLE=netconfd.elf
massif-target-netconfd.elf: VG_TOOL=massif
massif-target-netconfd.elf: $(BIN_DIR)/netconfd.elf $(VALGRIND)
massif-target-netconfd.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
massif-target-netconfd.elf:
	$(call vg-caller)

massif-target-bridgeconfig_dsa.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(bridgeconfig_dsa.elf_VALGRIND_FLAGS)        $(MASSIF_FLAGS)        $(bridgeconfig_dsa.elf_MASSIF_FLAGS) $(call valgrind_xml_flags,bridgeconfig_dsa.elf,MASSIF)
massif-target-bridgeconfig_dsa.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(bridgeconfig_dsa.elf_VALGRIND_TARGET_FLAGS) $(MASSIF_TARGET_FLAGS) $(bridgeconfig_dsa.elf_MASSIF_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
massif-target-bridgeconfig_dsa.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
massif-target-bridgeconfig_dsa.elf: VALGRIND_TARGET_EXECUTABLE=bridgeconfig_dsa.elf
massif-target-bridgeconfig_dsa.elf: VG_TOOL=massif
massif-target-bridgeconfig_dsa.elf: $(BIN_DIR)/bridgeconfig_dsa.elf $(VALGRIND)
massif-target-bridgeconfig_dsa.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
massif-target-bridgeconfig_dsa.elf:
	$(call vg-caller)

massif-target-bal_tests.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(bal_tests.elf_VALGRIND_FLAGS)        $(MASSIF_FLAGS)        $(bal_tests.elf_MASSIF_FLAGS) $(call valgrind_xml_flags,bal_tests.elf,MASSIF)
massif-target-bal_tests.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(bal_tests.elf_VALGRIND_TARGET_FLAGS) $(MASSIF_TARGET_FLAGS) $(bal_tests.elf_MASSIF_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
massif-target-bal_tests.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
massif-target-bal_tests.elf: VALGRIND_TARGET_EXECUTABLE=bal_tests.elf
massif-target-bal_tests.elf: VG_TOOL=massif
massif-target-bal_tests.elf: $(BIN_DIR)/bal_tests.elf $(VALGRIND)
massif-target-bal_tests.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
massif-target-bal_tests.elf:
	$(call vg-caller)

massif-target-netconfd_tests.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(netconfd_tests.elf_VALGRIND_FLAGS)        $(MASSIF_FLAGS)        $(netconfd_tests.elf_MASSIF_FLAGS) $(call valgrind_xml_flags,netconfd_tests.elf,MASSIF)
massif-target-netconfd_tests.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(netconfd_tests.elf_VALGRIND_TARGET_FLAGS) $(MASSIF_TARGET_FLAGS) $(netconfd_tests.elf_MASSIF_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
massif-target-netconfd_tests.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
massif-target-netconfd_tests.elf: VALGRIND_TARGET_EXECUTABLE=netconfd_tests.elf
massif-target-netconfd_tests.elf: VG_TOOL=massif
massif-target-netconfd_tests.elf: $(BIN_DIR)/netconfd_tests.elf $(VALGRIND)
massif-target-netconfd_tests.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
massif-target-netconfd_tests.elf:
	$(call vg-caller)

massif-target-utility_tests.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(utility_tests.elf_VALGRIND_FLAGS)        $(MASSIF_FLAGS)        $(utility_tests.elf_MASSIF_FLAGS) $(call valgrind_xml_flags,utility_tests.elf,MASSIF)
massif-target-utility_tests.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(utility_tests.elf_VALGRIND_TARGET_FLAGS) $(MASSIF_TARGET_FLAGS) $(utility_tests.elf_MASSIF_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
massif-target-utility_tests.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
massif-target-utility_tests.elf: VALGRIND_TARGET_EXECUTABLE=utility_tests.elf
massif-target-utility_tests.elf: VG_TOOL=massif
massif-target-utility_tests.elf: $(BIN_DIR)/utility_tests.elf $(VALGRIND)
massif-target-utility_tests.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
massif-target-utility_tests.elf:
	$(call vg-caller)


massif-target: $(VG_LOCAL_VALGRIND) $(MASSIF_TARGET_TARGETS)
$(MASSIF_TARGET_TARGETS): $(SYNC_SETTINGS_MK_TMP)

### drd-target ###
DRD_TARGET_TARGETS += drd-target-bal_tests.elf
DRD_TARGET_TARGETS += drd-target-netconfd_tests.elf
DRD_TARGET_TARGETS += drd-target-utility_tests.elf

drd-target-netconfd.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(netconfd.elf_VALGRIND_FLAGS)        $(DRD_FLAGS)        $(netconfd.elf_DRD_FLAGS) $(call valgrind_xml_flags,netconfd.elf,DRD)
drd-target-netconfd.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(netconfd.elf_VALGRIND_TARGET_FLAGS) $(DRD_TARGET_FLAGS) $(netconfd.elf_DRD_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
drd-target-netconfd.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
drd-target-netconfd.elf: VALGRIND_TARGET_EXECUTABLE=netconfd.elf
drd-target-netconfd.elf: VG_TOOL=drd
drd-target-netconfd.elf: $(BIN_DIR)/netconfd.elf $(VALGRIND)
drd-target-netconfd.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
drd-target-netconfd.elf:
	$(call vg-caller)

drd-target-bridgeconfig_dsa.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(bridgeconfig_dsa.elf_VALGRIND_FLAGS)        $(DRD_FLAGS)        $(bridgeconfig_dsa.elf_DRD_FLAGS) $(call valgrind_xml_flags,bridgeconfig_dsa.elf,DRD)
drd-target-bridgeconfig_dsa.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(bridgeconfig_dsa.elf_VALGRIND_TARGET_FLAGS) $(DRD_TARGET_FLAGS) $(bridgeconfig_dsa.elf_DRD_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
drd-target-bridgeconfig_dsa.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
drd-target-bridgeconfig_dsa.elf: VALGRIND_TARGET_EXECUTABLE=bridgeconfig_dsa.elf
drd-target-bridgeconfig_dsa.elf: VG_TOOL=drd
drd-target-bridgeconfig_dsa.elf: $(BIN_DIR)/bridgeconfig_dsa.elf $(VALGRIND)
drd-target-bridgeconfig_dsa.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
drd-target-bridgeconfig_dsa.elf:
	$(call vg-caller)

drd-target-bal_tests.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(bal_tests.elf_VALGRIND_FLAGS)        $(DRD_FLAGS)        $(bal_tests.elf_DRD_FLAGS) $(call valgrind_xml_flags,bal_tests.elf,DRD)
drd-target-bal_tests.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(bal_tests.elf_VALGRIND_TARGET_FLAGS) $(DRD_TARGET_FLAGS) $(bal_tests.elf_DRD_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
drd-target-bal_tests.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
drd-target-bal_tests.elf: VALGRIND_TARGET_EXECUTABLE=bal_tests.elf
drd-target-bal_tests.elf: VG_TOOL=drd
drd-target-bal_tests.elf: $(BIN_DIR)/bal_tests.elf $(VALGRIND)
drd-target-bal_tests.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
drd-target-bal_tests.elf:
	$(call vg-caller)

drd-target-netconfd_tests.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(netconfd_tests.elf_VALGRIND_FLAGS)        $(DRD_FLAGS)        $(netconfd_tests.elf_DRD_FLAGS) $(call valgrind_xml_flags,netconfd_tests.elf,DRD)
drd-target-netconfd_tests.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(netconfd_tests.elf_VALGRIND_TARGET_FLAGS) $(DRD_TARGET_FLAGS) $(netconfd_tests.elf_DRD_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
drd-target-netconfd_tests.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
drd-target-netconfd_tests.elf: VALGRIND_TARGET_EXECUTABLE=netconfd_tests.elf
drd-target-netconfd_tests.elf: VG_TOOL=drd
drd-target-netconfd_tests.elf: $(BIN_DIR)/netconfd_tests.elf $(VALGRIND)
drd-target-netconfd_tests.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
drd-target-netconfd_tests.elf:
	$(call vg-caller)

drd-target-utility_tests.elf: VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL = $(VALGRIND_FLAGS)        $(utility_tests.elf_VALGRIND_FLAGS)        $(DRD_FLAGS)        $(utility_tests.elf_DRD_FLAGS) $(call valgrind_xml_flags,utility_tests.elf,DRD)
drd-target-utility_tests.elf: VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL = $(VALGRIND_TARGET_FLAGS) $(utility_tests.elf_VALGRIND_TARGET_FLAGS) $(DRD_TARGET_FLAGS) $(utility_tests.elf_DRD_TARGET_FLAGS) $(VALGRIND_GTEST_FILTER_FLAGS)
drd-target-utility_tests.elf: VALGRIND_GTEST_FILTER?=*:-*_Target*.*:*.*_Target*
drd-target-utility_tests.elf: VALGRIND_TARGET_EXECUTABLE=utility_tests.elf
drd-target-utility_tests.elf: VG_TOOL=drd
drd-target-utility_tests.elf: $(BIN_DIR)/utility_tests.elf $(VALGRIND)
drd-target-utility_tests.elf: VALGRIND_ARGS=$(VALGRIND_BUILDTARGET_TOOL_FLAGS_CALL) $(VG_ABS_DUT) $(VALGRIND_BUILDTARGET_TOOL_TARGET_FLAGS_CALL)
drd-target-utility_tests.elf:
	$(call vg-caller)


drd-target: $(VG_LOCAL_VALGRIND) $(DRD_TARGET_TARGETS)
$(DRD_TARGET_TARGETS): $(SYNC_SETTINGS_MK_TMP)

