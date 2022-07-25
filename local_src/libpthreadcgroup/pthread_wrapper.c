// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#include "pthread_wrapper.h"

#include <pthread.h>
#include <bits/local_lim.h>
#include <sys/prctl.h>
#include <signal.h>
#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <syscall.h>

#include "cgrules.h"
#include "helper.h"
#include "log.h"

typedef struct thread_info_list thread_info_list;
struct thread_info_list
{
    thread_info_list *next;
    struct thread_info thread_info;

};

#define LOAD_REAL_FUNC(funcname)                                               \
    do                                                                         \
    {                                                                          \
        typedef void (*fnDlSymbol)(void);                                      \
        union                                                                  \
        {                                                                      \
            fnDlSymbol func;                                                   \
            void *ptr;                                                         \
        } _lrf_func;                                                           \
        _lrf_func.ptr = dlsym(RTLD_NEXT, #funcname);                           \
        real_##funcname = (__typeof__(real_##funcname))_lrf_func.func;         \
    } while (0)

static int (*real_pthread_create)(pthread_t *__restrict __newthread,
                                  const pthread_attr_t *__restrict __attr,
                                  void *(*__start_routine)(void *),
                                  void *__restrict __arg);

static int (*real_pthread_setschedparam)(pthread_t __target_thread,
                                  int __policy,
                                  const struct sched_param *__param);


typedef struct thread_params
{
    void *(*func)(void *);
    void *arg;
} thread_params;

static thread_info_list first;

static __thread thread_info_list self;

static pthread_mutex_t thread_info_list_lock = PTHREAD_MUTEX_INITIALIZER;

__attribute__((constructor)) static void initialize()
{
    LOAD_REAL_FUNC(pthread_create);
    LOAD_REAL_FUNC(pthread_setschedparam);
    cgrules_init();
}

static bool add_to_thread_list(struct thread_info_list *thread_info_list)
{
	bool alreadyInList = false;

	struct thread_info thread_info = thread_info_list->thread_info;
	struct thread_info_list *current = &first;
	while (current->next != NULL)
	{
		if (current->next->thread_info.pthread == thread_info.pthread)
		{
			void *tmp = current->next;
			current->next = current->next->next;
			free(tmp);
			alreadyInList = true;
		}

		if(current->next)
			current = current->next;
	}

	current->next = thread_info_list;
	return alreadyInList;
}

static void register_thread()
{
    self.thread_info.thread_pid = syscall(SYS_gettid);
    self.thread_info.pthread = pthread_self();
    self.next = NULL;

    pthread_mutex_lock(&thread_info_list_lock);

    add_to_thread_list(&self);
    cgrules_move_to_default(self.thread_info.thread_pid);

	int policy;
	struct sched_param sched_param;
	pthread_getschedparam(pthread_self(), &policy, &sched_param);
	cgrules_apply_rule_by_prio(self.thread_info.thread_pid, sched_param.__sched_priority, policy);

    pthread_mutex_unlock(&thread_info_list_lock);
}

static void unregister_thread(void*  arg)
{
    (void)arg;
    //DBG("%s", __func__);
    pthread_mutex_lock(&thread_info_list_lock);

    thread_info_list *current = &first;
    while (current->next != &self)
        current = current->next;

    current->next = self.next;

    pthread_mutex_unlock(&thread_info_list_lock);
}

static void add_dummy_entry_to_list(pthread_t thread)
{
	pthread_mutex_lock(&thread_info_list_lock);

	struct thread_info_list *current = &first;

	while(current != NULL && current->thread_info.pthread != thread)
		current = current->next;

	if (current == NULL)
	{
		struct thread_info_list *thread_info_list = calloc(1, sizeof(*thread_info_list));
		thread_info_list->thread_info.pthread = thread;

		current = &first;
		while(current->next != NULL)
			current = current->next;

		current->next = thread_info_list;
	}

	pthread_mutex_unlock(&thread_info_list_lock);
}

static pid_t get_thread_pid(pthread_t thread)
{
	pthread_mutex_lock(&thread_info_list_lock);

	thread_info_list *current = &first;
	while (current != NULL)
		if (current->thread_info.pthread == thread)
			break;
		else
			current = current->next;

	pthread_mutex_unlock(&thread_info_list_lock);

	// is the thread not started yet?
	if (current == NULL)
	{
		add_dummy_entry_to_list(thread);
		return get_thread_pid(thread);
	}

	return current->thread_info.thread_pid;
}

static void *start_routine2(thread_params *params)
{
    void *result;
    pthread_cleanup_push(unregister_thread, NULL);
    register_thread();
    result = params->func(params->arg);
    pthread_cleanup_pop(1);
    return result;
}

static void *start_routine(void *arg)
{
    thread_params params = *(thread_params*)arg;
    free(arg);
    void *result = NULL;
    result = start_routine2(&params);
    pthread_exit(result);
}

static void pw_adjust_stacksize(pthread_attr_t * attr)
{
	size_t current_stacksize;
	if (pthread_attr_getstacksize(attr, &current_stacksize) == 0)
	{
		size_t adjusted_stacksize = current_stacksize;
		if (current_stacksize <= PW_STACK_MIN)
			adjusted_stacksize= PW_STACK_MIN;
		if (current_stacksize > PW_STACK_MAX)
			adjusted_stacksize = PW_STACK_MAX;

		if (current_stacksize != adjusted_stacksize)
        {
			pthread_attr_setstacksize(attr, adjusted_stacksize);
            DBG("Change stack size: %d -> %d KB", current_stacksize, adjusted_stacksize);
        }
	}
}

static void copy_pthread_attr(pthread_attr_t *__restrict dest, const pthread_attr_t *__restrict source)
{
	union {
		int detachstate;
		size_t guardsize;
		size_t stacksize;
		int schedpolicy;
		int inheritsched;
		int scope;
		struct sched_param schedparam;
	}u;

#define CP_ATTR(func) 									\
	do {												\
	if (pthread_attr_get##func(source, &u.func) == 0) 	\
		pthread_attr_set##func(dest, u.func);			\
	} while(0)

	CP_ATTR(detachstate);
	CP_ATTR(guardsize);
	CP_ATTR(inheritsched);
	CP_ATTR(schedpolicy);
	CP_ATTR(scope);
	CP_ATTR(stacksize);


#undef CP_ATTR
	if (pthread_attr_getschedparam(source, &u.schedparam) == 0)
		pthread_attr_setschedparam(dest, &u.schedparam);

//	void *stackaddr = NULL;
//	if (pthread_attr_getstack(source, &stackaddr, &u.stacksize) == 0 && stackaddr != NULL)
//		pthread_attr_setstack(dest, stackaddr, u.stacksize);
}

static int rts_pthread_create(pthread_t *__restrict __newthread,
                   	   	   	  const pthread_attr_t *__restrict __attr,
                   	   	   	  void *(*__start_routine)(void *),
                   	   	   	  void *__restrict __arg)
{
	pthread_attr_t attr;
	thread_params *params = calloc(1, sizeof(*params));

	params->func = __start_routine;
	params->arg = __arg;

	pthread_attr_init(&attr);

	if (__attr != NULL)
	{
		copy_pthread_attr(&attr, __attr);
	}

	pw_adjust_stacksize(&attr);

	int res = real_pthread_create(__newthread, &attr, start_routine, params);

	pthread_attr_destroy(&attr);


	if (!res)
		DBG("%s", "Thread created");

	if (res)
		free(params);

	return res;
}

int pthread_create(pthread_t *__restrict __newthread,
                   const pthread_attr_t *__restrict __attr,
                   void *(*__start_routine)(void *),
                   void *__restrict __arg)
{
	if (cgrules_is_rts_proc())
	{
		return rts_pthread_create(__newthread, __attr, __start_routine, __arg);
	}
	else
	{
		return real_pthread_create(__newthread, __attr, __start_routine, __arg);
	}
}

static int rts_pthread_setschedparam (pthread_t __target_thread,
										int __policy,
										const struct sched_param *__param)
{
	 int real_setschedparam_result
			= real_pthread_setschedparam(__target_thread, __policy, __param);

	if (real_setschedparam_result == 0)
	{
		pid_t tpid = get_thread_pid(__target_thread);
		if (tpid != 0)
			cgrules_apply_rule_by_prio(tpid, __param->__sched_priority, __policy);

		DBG("Param: %d policy: %d", __param->sched_priority, __policy);
	}

	return real_setschedparam_result;
}

int pthread_setschedparam(pthread_t __target_thread,
                          int __policy,
                          const struct sched_param *__param)
{
    if (cgrules_is_rts_proc())
    {
    	return rts_pthread_setschedparam(__target_thread, __policy, __param);
    }
    else
    {
    	return real_pthread_setschedparam(__target_thread, __policy, __param);
    }
}
