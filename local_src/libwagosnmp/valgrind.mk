###############################################################################
#  Copyright (c) WAGO GmbH & Co. KG
#
#  PROPRIETARY RIGHTS are involved in the subject matter of this material.
#  All manufacturing, reproduction, use and sales rights pertaining to this
#  subject matter are governed by the license agreement. The recipient of this
#  software implicitly accepts the terms of the license.
###############################################################################

DEBUGGING_DIR ?= ../../wago_intern/debugging/scripts


VALGRIND_TARGET_FOLDER=bin/gcc_5_5_arm-linux-gnueabihf/Debug/
VALGRIND_TARGET_EXECUTABLE=alltests.elf
VALGRIND_TARGET_OPTIONS += --gtest_filter=*:-*DeathTest*

NULGRIND_OPTIONS += 
MEMCHECK_OPTIONS += 
HELGRIND_OPTIONS += 
VALGRIND_OPTIONS += 

include $(DEBUGGING_DIR)/valgrind.mk

