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
 *  $Workfile: linux_memory.c $
 *
 * Last Modification:
 *  $Author: u010391 $
 *  $Modtime: 02.03.2009 $
 *  $Revision: 36787 $
 */


/****************************************************************************************/
/* includes */

#include "os_api.h"                         /* operating system */


/****************************************************************************************/
/* functionality */

/*
 * The @ref os_memory_alloc routine is described in header file os_api.h.
 */
handle_t
os_memory_alloc (uint_t size)
{
  return malloc(size);
}

/*
 * The @ref os_memory_dealloc routine is described in header file os_api.h.
 */
void
os_memory_dealloc (void* mem)
{
  free(mem);
}

/****************************************************************************************/
/* end of source */
