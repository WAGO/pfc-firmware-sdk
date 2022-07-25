/*
 * Copyright (c) 2000 - 2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
 * the subject matter of this material. All manufacturing, reproduction,
 * use, and sales rights pertaining to this subject matter are governed
 * by the license agreement. The recipient of this software implicitly
 * accepts the terms of the license.
 *
 * Filename:
 *  $Workfile: linux_csection.c $
 *
 * Last Modification:
 *  $Author: u010391 $
 *  $Modtime: 03.03.2009 $
 *  $Revision: 65684 $
 */


/****************************************************************************************/
/* includes */

#include "os_api.h"                         /* operating system */
#include <malloc.h>
#include <pthread.h>


/****************************************************************************************/
/* functionality */

/*
 * The @ref os_critical_section_create routine is described in header file os_api.h.
 */
handle_t
os_critical_section_create (void)
{
  pthread_mutex_t* section;
  int_t ret;

  section = malloc(sizeof(*section));
  if (NULL != section)
  {
    ret = pthread_mutex_init(section, NULL);
    if (0 > ret)
    {
      free(section);
      section = NULL;
    }
  }

  return section;
}

/*
 * The @ref os_critical_section_destroy routine is described in header file os_api.h.
 */
void
os_critical_section_destroy (handle_t section)
{
  if (NULL != section)
  {
    pthread_mutex_destroy(section);
    free(section);
  }
}

/**
 * The @ref os_critical_section_lock routine is described in header file os_api.h.
 */
void
os_critical_section_lock (handle_t section)
{
  pthread_mutex_lock(section);
}

/*
 * The @ref os_critical_section_unlock routine is described in header file os_api.h.
 */
void
os_critical_section_unlock (handle_t section)
{
  pthread_mutex_unlock(section);
}


/****************************************************************************************/
/* end of source */
