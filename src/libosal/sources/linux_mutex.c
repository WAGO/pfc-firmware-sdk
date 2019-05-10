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
 *  $Workfile: linux_mutex.c $
 *
 * Last Modification:
 *  $Author: u010391 $
 *  $Modtime: 03.03.2009 $
 *  $Revision: 36787 $
 */


/****************************************************************************************/
/* includes */

#include "os_api.h"                         /* operating system */
#include <malloc.h>
#include <pthread.h>


/****************************************************************************************/
/* functionality */

/*
 * The @ref os_mutex_create routine is described in header file os_api.h.
 */
handle_t
os_mutex_create (void)
{
  pthread_mutex_t* mutex;
  int_t ret;

  mutex = malloc(sizeof(*mutex));
  if (NULL != mutex)
  {
    ret = pthread_mutex_init(mutex, NULL);
    if (0 > ret)
    {
      free(mutex);
      mutex = NULL;
    }
  }

  return mutex;
}

/*
 * The @ref os_mutex_destroy routine is described in header file os_api.h.
 */
void
os_mutex_destroy (handle_t mutex)
{
  if (NULL != mutex)
  {
    pthread_mutex_destroy(mutex);
    free(mutex);
  }
}

/*
 * The @ref os_mutex_lock routine is described in header file os_api.h.
 */
void
os_mutex_lock (handle_t mutex)
{
  pthread_mutex_lock(mutex);
}

/*
 * The @ref os_mutex_unlock routine is described in header file os_api.h.
 */
void
os_mutex_unlock (handle_t mutex)
{
  pthread_mutex_unlock(mutex);
}


/****************************************************************************************/
/* end of source */
