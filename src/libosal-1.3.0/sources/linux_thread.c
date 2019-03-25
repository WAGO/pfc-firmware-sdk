/*
 * Copyright (c) 2000 - 2009 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
 * the subject matter of this material. All manufacturing, reproduction,
 * use, and sales rights pertaining to this subject matter are governed
 * by the license agreement. The recipient of this software implicitly
 * accepts the terms of the license.
 *
 * Filename:
 *  $Workfile: linux_thread.c $
 *
 * Last Modification:
 *  $Author: u010391 $
 *  $Modtime: 03.03.2009 $
 *  $Revision: 100884 $
 */

/****************************************************************************************/
/* includes */

#include "os_api.h"                         /* operating system */
#include <pthread.h>                        /* threads */
#include <limits.h>                         /* threads */


/****************************************************************************************/
/* functionality */

/*
 * The @ref os_thread_create routine is described in header file os_api.h.
 */
handle_t
os_thread_create (uint_t size, uint_t prio, os_thread_routine_t routine, void* user)
{
  struct sched_param sched;
  pthread_attr_t attr;
  pthread_t thread = 0;
  int status;

  thread = 0;
  status = pthread_attr_init(&attr);
  if (0 == status)
  {
    sched.__sched_priority = (int)prio;

    if (size < PTHREAD_STACK_MIN)
    {
      size = PTHREAD_STACK_MIN;
    }

    status |= pthread_attr_setstacksize(&attr, size);
    status |= pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    status |= pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    status |= pthread_attr_setschedparam(&attr, &sched);

    if (0 == status)
    {
      (void) pthread_create(&thread, &attr, (void*) routine, user);
    }

    pthread_attr_destroy(&attr);
  }

  return (handle_t) thread;
}

/*
 * The @ref os_thread_create_name routine is described in header file os_api.h.
 */
handle_t
os_thread_create_name (uint_t size, uint_t prio, os_thread_routine_t routine, void* user, char *name)
{
  handle_t thread = NULL;
  int status;

  thread = os_thread_create (size, prio, routine, user);

  if (NULL != thread)
  {
    status = pthread_setname_np((pthread_t) thread, name);
  }

  return thread;
}

/*
 * The @ref os_thread_destroy routine is described in header file os_api.h.
 */
void
os_thread_destroy (handle_t thread)
{
  if (NULL != thread)
  {
    pthread_join((pthread_t) thread, NULL);
  }
}

/*
 * The @ref os_thread_exit routine is described in header file os_api.h.
 */
void
os_thread_exit (uint_t exit_code)
{
  if (0 != exit_code)
  {

  }

  pthread_exit(NULL);
}


/****************************************************************************************/
/* end of source */
