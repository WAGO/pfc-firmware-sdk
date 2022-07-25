#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of project common-header (PTXdist package libcommonheader).
#
# Copyright (c) 2017-2022 WAGO GmbH & Co. KG
#######################################################################################################################
SCRIPT_DIR ?= ../../wago_intern/debugging/scripts


VALGRIND_TARGET_FOLDER=bin/gcc_5_5_arm-linux-gnueabihf/Debug/
VALGRIND_TARGET_EXECUTABLE=alltests.elf
VALGRIND_TARGET_OPTIONS += --gtest_filter=*:-*DeathTest*

NULGRIND_OPTIONS += 
MEMCHECK_OPTIONS += 
HELGRIND_OPTIONS += 
VALGRIND_OPTIONS += 

-include $(SCRIPT_DIR)/valgrind.mk

