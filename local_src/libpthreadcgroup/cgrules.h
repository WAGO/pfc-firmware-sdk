// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG


#ifndef _CGRULES_H_
#define _CGRULES_H_

#include <sys/types.h>
#include <stdbool.h>

void cgrules_init();

bool cgrules_is_rts_proc(void);
void cgrules_move_to_default(pid_t pid);
void cgrules_apply_rule_by_name(const char* name, pid_t tpid);
void cgrules_apply_rule_by_prio(pid_t tpid, int prio, int policy);

#endif
