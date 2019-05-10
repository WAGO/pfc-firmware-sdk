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
 *  $Workfile: linux_timer.c $
 *
 * Last Modification:
 *  $Author: u010391 $
 *  $Modtime: 29.04.2009 $
 *  $Revision: 36787 $
 */


/****************************************************************************************/
/* includes */

#include "os_api.h"                         /* operating system */

#include <pthread.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <errno.h>


/****************************************************************************************/
/* static declaration */

typedef struct timer_ref_tag    timer_ref_t;

struct timer_ref_tag
{
  timer_t id;
  handle_t user;
  uint_t timeout;
  os_timeout_routine_t routine;
};



/****************************************************************************************/
/* functionality */

/**
 * The @ref os_timer_notify_handler routine handles the timeout of the timer referenced
 * by the signal event of the operating system. The application specific callback routine
 * will be called to notify the event.
 *
 * @param sigval Specifies the signal event of the timeout.
 */
static void
os_timer_notify_handler (sigval_t sigval)
{
  timer_ref_t* timer;

  timer = (timer_ref_t*) sigval.sival_ptr;
  timer->routine(timer->user);
}

/*
 * The @ref os_timer_activate_periodical routine is described in header file os_api.h.
 */
handle_t
os_timer_activate_periodical (handle_t user, uint_t timeout, os_timeout_routine_t routine)
{
  struct itimerspec ts;
  struct sigevent se;
  timer_ref_t* timer;
  int_t status;

  timer = malloc(sizeof(*timer));
  if (NULL != timer)
  {
    ts.it_value.tv_sec = (__time_t)(timeout / 1000);
    ts.it_value.tv_nsec = (__suseconds_t)(timeout % 1000) * 1000000;
    ts.it_interval.tv_sec = (__time_t)ts.it_value.tv_sec;
    ts.it_interval.tv_nsec = (__suseconds_t)ts.it_value.tv_nsec;

    memset(&se, 0, sizeof(struct sigevent));
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = timer;
    se._sigev_un._sigev_thread._function = os_timer_notify_handler;
    se._sigev_un._sigev_thread._attribute = NULL;

    status = timer_create(CLOCK_REALTIME, &se, &timer->id);
    if (0 == status)
    {
      status = timer_settime(timer->id, 0, &ts, NULL);
      if (0 != status)
      {
        timer_delete(timer->id);
      }
    }

    if (0 == status)
    {
      timer->user = user;
      timer->timeout = timeout;
      timer->routine = routine;
    }
    else
    {
      free(timer);
      timer = NULL;
    }
  }

  return timer;
}

/*
 * The @ref os_timer_activate_onetime routine is described in header file os_api.h.
 */
handle_t
os_timer_activate_onetime (handle_t user, uint_t timeout, os_timeout_routine_t routine)
{
  struct itimerspec ts;
  struct sigevent se;
  timer_ref_t* timer;
  int_t status;

  timer = malloc(sizeof(*timer));
  if (NULL != timer)
  {
    ts.it_value.tv_sec = (__time_t)(timeout / 1000);
    ts.it_value.tv_nsec = (__suseconds_t)(timeout % 1000) * 1000000;
    ts.it_interval.tv_sec = (__time_t)0;
    ts.it_interval.tv_nsec = (__suseconds_t)0;

    memset(&se, 0, sizeof(struct sigevent));
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = timer;
    se._sigev_un._sigev_thread._function = os_timer_notify_handler;
    se._sigev_un._sigev_thread._attribute = NULL;

    status = timer_create(CLOCK_REALTIME, &se, &timer->id);
    if (0 == status)
    {
      status = timer_settime(timer->id, 0, &ts, NULL);
      if (0 != status)
      {
        timer_delete(timer->id);
      }
    }

    if (0 == status)
    {
      timer->user = user;
      timer->timeout = timeout;
      timer->routine = routine;
    }
    else
    {
      free(timer);
      timer = NULL;
    }
  }

  return timer;
}

/*
 * The @ref os_timer_deactivate routine is described in header file os_api.h.
 */
void
os_timer_deactivate (handle_t ref)
{
  timer_ref_t* timer;

  timer = (timer_ref_t*) ref;
  if (NULL != timer)
  {
    timer_delete(timer->id);
    free(timer);
  }
}

/*
 * The @ref os_timer_stop routine is described in header file os_api.h.
 */
void
os_timer_stop (handle_t ref)
{
  struct itimerspec ts;
  timer_ref_t* timer;

  timer = (timer_ref_t*) ref;
  if (NULL != timer)
  {
    timer->timeout = 0;

    ts.it_value.tv_sec = (__time_t)0;
    ts.it_value.tv_nsec = (__suseconds_t)0;
    ts.it_interval.tv_sec = (__time_t)0;
    ts.it_interval.tv_nsec = (__suseconds_t)0;

    timer_settime(timer->id, 0, &ts, NULL);
  }
}

/*
 * The @ref os_timer_restart_onetime routine is described in header file os_api.h.
 */
handle_t
os_timer_restart_periodical (handle_t ref, uint_t timeout)
{
  struct itimerspec ts;
  timer_ref_t* timer;
  int_t status;

  timer = (timer_ref_t*) ref;
  if (NULL != timer)
  {
    timer->timeout = timeout;

    ts.it_value.tv_sec = (__time_t)(timeout / 1000);
    ts.it_value.tv_nsec = (__suseconds_t)(timeout % 1000) * 1000000;
    ts.it_interval.tv_sec = (__time_t)ts.it_value.tv_sec;
    ts.it_interval.tv_nsec = (__suseconds_t)ts.it_value.tv_nsec;

    status = timer_settime(timer->id, 0, &ts, NULL);
    if (0 != status)
    {
      timer = NULL;
    }
  }

  return timer;
}

/*
 * The @ref os_timer_restart_onetime routine is described in header file os_api.h.
 */
handle_t
os_timer_restart_onetime (handle_t ref, uint_t timeout)
{
  struct itimerspec ts;
  timer_ref_t* timer;
  int_t status;

  timer = (timer_ref_t*) ref;
  if (NULL != timer)
  {
    timer->timeout = timeout;

    ts.it_value.tv_sec = (__time_t)(timeout / 1000);
    ts.it_value.tv_nsec = (__suseconds_t)(timeout % 1000) * 1000000;
    ts.it_interval.tv_sec = (__time_t)0;
    ts.it_interval.tv_nsec = (__suseconds_t)0;

    status = timer_settime(timer->id, 0, &ts, NULL);
    if (0 != status)
    {
      timer = NULL;
    }
  }

  return timer;
}


/****************************************************************************************/
/* end of source */
