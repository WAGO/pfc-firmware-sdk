// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#ifndef _PHTREADWRAPPER_H
#define _PHTREADWRAPPER_H 1

#include <sys/types.h>
#include <pthread.h>
#include <bits/local_lim.h>

#define PW_STACK_MIN (2*PTHREAD_STACK_MIN)
#define PW_STACK_MAX (16*PTHREAD_STACK_MIN)

typedef struct thread_info thread_info;
struct thread_info
{
	pid_t thread_pid;
	pthread_t pthread;
};

#endif
