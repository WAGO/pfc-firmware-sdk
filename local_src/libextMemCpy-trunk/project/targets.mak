##----------------------------------------------------------------------------------------------------------------------
### Copyright (c) WAGO GmbH & Co. KG
###
### PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
### the subject matter of this material. All manufacturing, reproduction,
### use, and sales rights pertaining to this subject matter are governed
### by the license agreement. The recipient of this software implicitly
### accepts the terms of the license.
###
##----------------------------------------------------------------------------------------------------------------------
##----------------------------------------------------------------------------------------------------------------------
###
###  \file     targets.mak
###
###  \version  $Id:
###
###  \brief    This is the common makefile to build the targets.
###
###  \author   Wauer : WAGO GmbH & Co. KG
##----------------------------------------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------------------------------------
# Common compiler definitions
#-----------------------------------------------------------------------------------------------------------------------

# Debug
ifeq ($(DEBUG),n)
CFLAGS += -DNDEBUG
endif

#-----------------------------------------------------------------------------------------------------------------------
# Common build targets
#-----------------------------------------------------------------------------------------------------------------------

all: .start_info .compile_info $(CC_OUT_OBJ) $(CC_CAPITAL_OUT_OBJ) .lib_info $(TARGET_STATIC_LIB) $(TARGET_SHARED_OBJ) .link_info $(TARGET_EXEC)

static_lib: .start_info .compile_info $(CC_OUT_OBJ) $(CC_CAPITAL_OUT_OBJ) .lib_info $(TARGET_STATIC_LIB)

shared_lib: .start_info .compile_info $(CC_OUT_OBJ) $(CC_CAPITAL_OUT_OBJ) .lib_info $(TARGET_SHARED_OBJ)

executable: .start_info .compile_info $(CC_OUT_OBJ) $(CC_CAPITAL_OUT_OBJ) .link_info $(TARGET_EXEC)

.PHONY: lint
lint: 
	@echo "Lint <$(LINT_RES)>"
	@lint-nt.exe +v -u $(LINT_CFG) $(LINT_RES)  | tr '\\' / | sed 's/Z://' 

# compile the sources with the preefix '.c'
$(CC_OUT_OBJ): $(OUT_DIR)/%.o: $(PATH_TO_PROJECT_ROOT)/%.c
	@echo "CC $<"
	@mkdir -p $(dir $@)
	@$(CC) -c $(CFLAGS) $< -o $@ -MMD

# compile the sources with the preefix '.C'
$(CC_CAPITAL_OUT_OBJ): $(OUT_DIR)/%.o: $(PATH_TO_PROJECT_ROOT)/%.C
	@echo "CC $<"
	@mkdir -p $(dir $@)
	@$(CC) -x c -c $(CFLAGS) $< -o $@ -MMD

# generate the static library
$(TARGET_STATIC_LIB): $(STATIC_LIB_OUT_OBJ)
	@echo "AR $(TARGET_STATIC_LIB)"
	@$(AR) rcs $(OUT_DIR)/$(TARGET_STATIC_LIB) $(STATIC_LIB_OUT_OBJ)

# generate the shared object
$(TARGET_SHARED_OBJ): $(SHARED_LIB_OUT_OBJ)
	@echo "CC $(TARGET_SHARED_OBJ).$(VERSION)"
	@$(CC) -shared -Wl,-soname,$(TARGET_SHARED_OBJ) -o $(OUT_DIR)/$(TARGET_SHARED_OBJ).$(VERSION) $(SHARED_LIB_OUT_OBJ) $(LDDFLAGS)

# link the executable
$(TARGET_EXEC): $(EXEC_OUT_OBJ)
	@echo "LD $(TARGET_EXEC)"
	@$(CC) $(EXEC_OUT_OBJ) $(LDDFLAGS) -o $(OUT_DIR)/$@

# clean all generated files
clean: .start_info .clean_info
	@echo "RM *.o"
	@find $(OUT_DIR) -name '*.o' | xargs rm -f
	@echo "RM *.d"
	@find $(OUT_DIR) -name '*.d' | xargs rm -f
	@echo "RM $(TARGET_STATIC_LIB)"
	@rm -f $(OUT_DIR)/$(TARGET_STATIC_LIB)
	@echo "RM $(TARGET_SHARED_OBJ).$(VERSION)"
	@rm -f $(OUT_DIR)/$(TARGET_SHARED_OBJ).$(VERSION)
	@echo "RM $(TARGET_EXEC)"
	@rm -f $(OUT_DIR)/$(TARGET_EXEC)

# target info texts
.start_info:
	@echo
	@echo "---------- Processing of <$(TARGET)> for target platform <$(TARGET_PLATFORM)> started ----------"

.clean_info:
	@echo
	@echo "Removing the generated files..."

.compile_info:
	@echo
	@echo "Compiling the sources..."
	
.lib_info:
	@echo
	@echo "Gernerating the library..."

.link_info:
	@echo
	@echo "Linking the executable..."


# include the dependency information of the source files (dependency filenames are generated out of the object files)
-include $(subst .o,.d,$(CC_OUT_OBJ))
-include $(subst .o,.d,$(CC_CAPITAL_OUT_OBJ))