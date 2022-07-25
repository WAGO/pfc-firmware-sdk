# Copyright (c) WAGO GmbH & Co. KG
#
# PROPRIETARY RIGHTS are involved in the subject matter of this material. All
# manufacturing, reproduction, use, and sales rights pertaining to this
# subject matter are governed by the license agreement. The recipient of this
# software implicitly accepts the terms of the license.

#---------
#
# MakefileWorker.mk
#
# Include this helper file in your makefile
# It makes a static library and a shared library using libtool
#
#
#----------
# Inputs - these variables describe what to build
#
#   INCLUDE_DIRS - Directories used to search for include files.
#                   This generates a -I for each directory
#	SRC_DIRS - Directories containing source file to built into the library
#   SRC_FILES - Specific source files to build into library. Helpful when not all code 
#				in a directory can be built for test (hopefully a temporary situation)
#----------
# You can adjust these variables to influence how to build the target
# and where to put and name outputs
# See below to determine defaults
#   COMPONENT_NAME - the name of the thing being built
#   MK_OBJS_DIR - a directory where o and d files go
#   MK_LIB_DIR - a directory where libs go
#   MK_ENABLE_DEBUG - build for debug
#   MK_USE_STD_CPP_LIB - Set to N to keep the standard C++ library out
#		of the test harness
#   MK_USE_GCOV - Turn on coverage analysis
#		Clean then build with this flag set to Y, then 'make gcov'
#   MK_MAPFILE - generate a map file
#   MK_WARNINGFLAGS - overly picky by default
#	OTHER_MAKEFILE_TO_INCLUDE - a hook to use this makefile to make 
#		other targets. Like CSlim, which is part of fitnesse
#	MK_USE_VPATH - Use Make's VPATH functionality to support user 
#		specification of source files and directories that aren't below 
#		the user's Makefile in the directory tree, like:
#			SRC_DIRS += ../../lib/foo
#		It defaults to N, and shouldn't be necessary except in the above case.
#----------
#
#  Other flags users can initialize to sneak in their settings
#	MK_CXXFLAGS - flags for the C++ compiler
#	MK_CPPFLAGS - flags for the C++ AND C compiler
#	MK_CFLAGS - C complier
#	MK_LDFLAGS - Linker flags
#----------

# Debug on by default
ifndef MK_ENABLE_DEBUG
	MK_ENABLE_DEBUG = Y
endif

# Use the standard C++ library
ifndef MK_USE_STD_CPP_LIB
	MK_USE_STD_CPP_LIB = Y
endif

# Default warnings
ifndef MK_WARNINGFLAGS
	MK_WARNINGFLAGS = -pedantic-errors -Wall -Wextra -Werror -Wshadow -Wswitch-default -Wswitch-enum -Wconversion
endif

# Default dir for temporary files (d, o)
ifndef MK_OBJS_DIR
    MK_OBJS_DIR = objs
endif

ifndef MK_LIB_DIR
	MK_LIB_DIR = lib
endif

# No map by default
ifndef MK_MAP_FILE
	MK_MAP_FILE = N
endif

ifdef SILENCE
	#LIBTOOL_SILENCE= --quiet
endif

ifndef LIBTOOL
	LIBTOOL=libtool
endif


# No VPATH is default
ifndef MK_USE_VPATH
	MK_USE_VPATH := N
endif
# Make empty, instead of 'N', for usage in $(if ) conditionals
ifneq ($(MK_USE_VPATH), Y)
	MK_USE_VPATH := 
endif

# --------------------------------------
# derived flags in the following area
# --------------------------------------
ifeq ($(MK_ENABLE_DEBUG), Y)
	MK_CPPFLAGS += -g
endif

ifeq ($(MK_USE_STD_CPP_LIB), N)
	MK_CPPFLAGS += -DMK_STD_CPP_LIB_DISABLED
	MK_CXXFLAGS += -nostdinc++
endif


MK_CPPFLAGS += $(MK_WARNINGFLAGS)

TARGET_MAP = $(TARGET).map.txt
ifeq ($(MK_MAP_FILE), Y)
	MK_LDFLAGS += -Wl,-Map,$(TARGET_MAP)
endif

#LD_LIBRARIES += -lstdc++ -lgcov

TARGET_BASE = $(MK_LIB_DIR)/lib$(COMPONENT_NAME)

TARGET_LIB = $(TARGET_BASE).la


#Helper Functions
get_src_from_dir  = $(wildcard $1/*.cpp) $(wildcard $1/*.c)
get_files_from_dir = $(wildcard $(dir $1).libs/*.*)
get_dirs_from_dirspec  = $(wildcard $1)
get_src_from_dir_list = $(foreach dir, $1, $(call get_src_from_dir,$(dir)))
get_files_from_dir_list = $(foreach dir, $1, $(call get_files_from_dir,$(dir)))
__src_to = $(subst .c,$1, $(subst .cpp,$1,$(if $(MK_USE_VPATH),$(notdir $2),$2)))
src_to = $(addprefix $(MK_OBJS_DIR)/,$(call __src_to,$1,$2))
src_to_o = $(call src_to,.o,$1)
src_to_lo = $(call src_to,.lo,$1)
src_to_d = $(call src_to,.d,$1)
src_to_gcda = $(call src_to,.gcda,$1)
src_to_gcno = $(call src_to,.gcno,$1)
time = $(shell date +%s)
delta_t = $(eval minus, $1, $2)
debug_print_list = $(foreach word,$1,echo "  $(word)";) echo;

#Derived
STUFF_TO_CLEAN += $(TARGET) $(TARGET).exe $(TARGET_LIB) $(TARGET_MAP)

SRC += $(call get_src_from_dir_list, $(SRC_DIRS)) $(SRC_FILES)			        
OBJ = $(call src_to_o,$(SRC))
LOBJ = $(call src_to_lo,$(SRC))

LIBTOOL_LIB = $(MK_LIB_DIR)/.libs/* 
LIBTOOL_BUILD = $(call get_files_from_dir_list, $(LOBJ))

STUFF_TO_CLEAN += $(OBJ)
STUFF_TO_CLEAN += $(LOBJ)
STUFF_TO_CLEAN += $(LIBTOOL_LIB)
STUFF_TO_CLEAN += $(LIBTOOL_BUILD)

ALL_SRC = $(SRC)

# If we're using VPATH
ifeq ($(MK_USE_VPATH), Y)
	# gather all the source directories and add them
	VPATH += $(sort $(dir $(ALL_SRC)))
	# Add the component name to the objs dir path, to differentiate between same-name objects
	MK_OBJS_DIR := $(addsuffix /$(TARGET),$(MK_OBJS_DIR))
endif

MK_CPPFLAGS +=  $(INCLUDES) 

INCLUDES_DIRS_EXPANDED = $(call get_dirs_from_dirspec, $(INCLUDE_DIRS))
INCLUDES += $(foreach dir, $(INCLUDES_DIRS_EXPANDED), -I$(dir))
MOCK_DIRS_EXPANDED = $(call get_dirs_from_dirspec, $(MOCKS_SRC_DIRS))
INCLUDES += $(foreach dir, $(MOCK_DIRS_EXPANDED), -I$(dir))


DEP_FILES = $(call src_to_d, $(ALL_SRC))
STUFF_TO_CLEAN += $(DEP_FILES) $(PRODUCTION_CODE_START) $(PRODUCTION_CODE_END)
STUFF_TO_CLEAN += $(STDLIB_CODE_START) $(MAP_FILE)

# We'll use the MK_CFLAGS etc so that you can override AND add to the CppUTest flags
CFLAGS = $(MK_CFLAGS) $(MK_ADDITIONAL_CFLAGS)
CPPFLAGS = $(MK_CPPFLAGS) $(MK_ADDITIONAL_CPPFLAGS)
CXXFLAGS = $(MK_CXXFLAGS) $(MK_ADDITIONAL_CXXFLAGS)
LDFLAGS = $(MK_LDFLAGS) $(MK_ADDITIONAL_LDFLAGS)

# Targets

.PHONY: all
all: $(TARGET_LIB) $(TARGET_SHAREDLIB)

.PHONY: flags
flags: 
	@echo
	@echo "Compile C and C++ source with CPPFLAGS:"
	@$(call debug_print_list,$(CPPFLAGS))
	@echo "Compile C++ source with CXXFLAGS:"
	@$(call debug_print_list,$(CXXFLAGS))
	@echo "Compile C source with CFLAGS:"
	@$(call debug_print_list,$(CFLAGS))
	@echo "Link with LDFLAGS:"
	@$(call debug_print_list,$(LDFLAGS))
	@echo "Link with LD_LIBRARIES:"
	@$(call debug_print_list,$(LD_LIBRARIES))
	@echo "Create libraries with ARFLAGS:"
	@$(call debug_print_list,$(ARFLAGS))
	
$(TARGET_LIB): $(OBJ)
	$(SILENCE)mkdir -p $(dir $@)
	$(SILENCE)$(LIBTOOL) $(LIBTOOL_SILENCE) --tag=CC --mode=link $(COMPILE.c) -o $@ $(LOBJ) $(MK_LIB) $(LDFLAGS)
	
#$(TARGET_SHAREDLIB): 
	
	
#$(TARGET): $(PRODUCTION_CODE_START) $(OBJ) $(USER_LIBS) $(PRODUCTION_CODE_END) $(MK_LIB) $(STDLIB_CODE_START)
#	$(SILENCE)echo Linking $@
#	$(SILENCE)$(LINK.o) -o $@ $^ $(LD_LIBRARIES)

$(MK_OBJS_DIR)/%.o: %.cpp
	@echo compiling $(notdir $<)
	$(SILENCE)mkdir -p $(dir $@)
	$(SILENCE)$(COMPILE.cpp) -MMD -MP $(OUTPUT_OPTION) $<

#$(MK_OBJS_DIR)/%.o: %.c
#	@echo compiling $(notdir $<)
#	$(SILENCE)mkdir -p $(dir $@)
#	$(SILENCE)$(COMPILE.c) -MMD -MP  $(OUTPUT_OPTION) $<
	
$(MK_OBJS_DIR)/%.o: %.c
	@echo compiling $(notdir $<)
	$(SILENCE)mkdir -p $(dir $@)
	$(SILENCE)$(LIBTOOL)  $(LIBTOOL_SILENCE) --tag=CC --mode=compile $(COMPILE.c) -MMD -MP $(OUTPUT_OPTION) $<
	
ifneq "$(MAKECMDGOALS)" "clean"
-include $(DEP_FILES)
endif

.PHONY: clean
clean:
	$(SILENCE)echo Making clean
	$(SILENCE)$(RM) $(STUFF_TO_CLEAN)
	
#realclean gets rid of all gcov, o and d files in the directory tree
#not just the ones made by this makefile
.PHONY: realclean
realclean: clean
	$(SILENCE)rm -rf gcov
	$(SILENCE)find . -name "*.gdcno" | xargs rm -f
	$(SILENCE)find . -name "*.[do]" | xargs rm -f	

debug:
	@echo
	@echo "Target Lib:"
	@echo $(TARGET_LIB)
	@echo "Target Source files:"
	@$(call debug_print_list,$(SRC))
	@echo "Target Object files:"
	@$(call debug_print_list,$(OBJ))
	@echo "Target LinkObject files:"
	@$(call debug_print_list,$(LOBJ))
	@echo "All Input Dependency files:"
	@$(call debug_print_list,$(DEP_FILES))
	@echo Stuff to clean:
	@$(call debug_print_list,$(STUFF_TO_CLEAN))
	@echo Includes:
	@$(call debug_print_list,$(INCLUDES))


ifneq "$(OTHER_MAKEFILE_TO_INCLUDE)" ""
-include $(OTHER_MAKEFILE_TO_INCLUDE)
endif
