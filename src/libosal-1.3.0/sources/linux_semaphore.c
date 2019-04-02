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
 *  $Workfile: linux_semaphore.c $
 *
 * Last Modification:
 *  $Author: u010391 $
 *  $Modtime: 03.03.2009 $
 *  $Revision: 89707 $
 */


/****************************************************************************************/
/* includes */

#include "os_api.h"                         /* operating system */
#include <semaphore.h>                      /* semaphores */


/****************************************************************************************/
/* functionality */

/*
 * The @ref os_semaphore_create routine is described in header file os_api.h.
 */
handle_t
os_semaphore_create (uint_t count_init, uint_t count_max)
{
  sem_t* sem;
  int_t status;

  sem = malloc(sizeof(*sem));
  if (NULL != sem)
  {
    status = sem_init(sem, 0, count_init);
    if (0 != status)
    {
      free(sem);
      sem = NULL;
    }
  }
  else
  {
    OS_ARGUMENT_USED(count_max);
  }

  return sem;
}

/*
 * The @ref os_semaphore_delete routine is described in header file os_api.h.
 */
void
os_semaphore_destroy (handle_t semaphore)
{
  if (NULL != semaphore)
  {
    sem_close(semaphore);
    free(semaphore);
  }
}

/*
 * The @ref os_semaphore_post routine is described in header file os_api.h.
 */
void
os_semaphore_post (handle_t semaphore)
{
  sem_post(semaphore);
}

/*
 * The @ref os_semaphore_wait routine is described in header file os_api.h.
 */
void
os_semaphore_wait (handle_t semaphore)
{
  sem_wait(semaphore);
}


/****************************************************************************************/
/* end of source */
