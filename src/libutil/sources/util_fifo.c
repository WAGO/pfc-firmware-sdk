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
 *  $Workfile: util_fifo.c $
 *
 * Last Modification:
 *  $Author: WAGODOM\u010391 $
 *  $Modtime: 07.06.2011 $
 *  $Revision: 36864 $
 */


/****************************************************************************************/
/* includes */

#include "os_api.h"                         /* operating system */
#include "util_api.h"                       /* utility interface */


/****************************************************************************************/
/* functionality */

/**
 * The @ref util_fifo_initialize routine initializes a FIFO used by the application
 * to queue objects on a simple way. If there is only one source of the object, a
 * synchronization element such like a mutex is not required. If there are more than
 * one object sources, a synchronization element is required to avoid an inconsistent
 * FIFO.
 *
 * @param fifo Specifies the FIFO resource.
 * @param mem Specifies the pointer to memory required by the FIFO to store the objects.
 * @param fifo Specifies the maximum number of objects handled by the FIFO. Mmust be a power
 * of 2 (e.g. 2,4,8,16,32,64,...)
 */
void
util_fifo_initialize (util_fifo_t* fifo, void* mem, uint_t max)
{
  fifo->objects = (void**) mem;
  fifo->max = max;
  fifo->mask = max - 1;
  fifo->num = 0;
  fifo->ins = 0;
  fifo->rem = 0;
}

/**
 * The @ref util_fifo_async_insert routine inserts an object into the provided FIFO.
 * The object will always be referenced as the last object of the FIFO.
 *
 * @note
 * The operation is not synchronized. There is no guaranty, that the operation will not
 * be interrupted by another process or thread and another object will be inserted at
 * this time. In this case the FIFO information can be corrupt. Use this routine, if
 * there is only one object source or the call of this routine is synchronized.
 *
 * @param fifo Specifies the FIFO resource.
 * @param object Points to an object which have to be inserted into the FIFO.
 *
 * @return
 * The routine returns the number of stored objects minus one. If one object is stored,
 * the returned value is 0. In this case there is no previous event. The semaphore has
 * to be activated by the caller of this insert routine to process the stored object
 * asynchronously. If the returned value is greater than 0, the semaphore was previously
 * activated.
 *
 * @see
 * util_fifo_sync_insert util_fifo_get_object util_fifo_get_number
 */
uint_t
util_fifo_async_insert (util_fifo_t* fifo, void* object)
{
  *(fifo->objects + fifo->ins) = object;
  fifo->ins = (fifo->ins + 1) & fifo->mask;
  return (os_atomic_increment((uint_t *)&fifo->num) - 1);  /* returns the new value */
}

/**
 * The @ref util_fifo_sync_insert routine inserts an object into the provided FIFO.
 * The object will always be referenced as the last object of the FIFO.
 *
 * @note
 * The operation is not synchronized. There is no guaranty, that the operation will not
 * be interrupted by another process or thread and another object will be inserted at
 * this time. In this case the FIFO information can be corrupt. Use this routine, if
 * there is only one object source or the call of this routine is synchronized.
 *
 * @param section References the semaphore (lock mechanism).
 * @param fifo Specifies the FIFO resource.
 * @param object Points to an object which have to be inserted into the FIFO.
 *
 * @return
 * The routine returns the number of stored objects minus one. If one object is stored,
 * the returned value is 0. In this case there is no previous event. The semaphore has
 * to be activated by the caller of this insert routine to process the stored object
 * asynchronously. If the returned value is greater than 0, the semaphore was previously
 * activated.
 *
 * @see
 * util_fifo_async_insert util_fifo_get_object util_fifo_get_number
 */
uint_t
util_fifo_sync_insert (handle_t section, util_fifo_t* fifo, void* object)
{
  os_critical_section_lock(section);
  *(fifo->objects + fifo->ins) = object;
  fifo->ins = (fifo->ins + 1) & fifo->mask;
  os_critical_section_unlock(section);

  return (os_atomic_increment((uint_t *)&fifo->num) - 1);  /* returns the new value */
}

/**
 * The @user util_fifo_get_number routine returns the number of objects stored
 * in the FIFO. If objects are stored (number greater than 0), a call to @ref
 * util_fifo_get_object returns the first object. The number of calls depends
 * on the number of objects returned by this routine.
 *
 * If objects are not queued, the routine returns 0.
 *
 * @param fifo Specifies the FIFO resource.
 *
 * @return
 * The routine returns the number of stored objects.
 *
 * @see
 * util_fifo_async_insert util_fifo_sync_insert util_fifo_get_object
 */
uint_t
util_fifo_get_number (util_fifo_t* fifo)
{
  return os_atomic_exchange(&fifo->num, 0);
}

/**
 * The @user util_fifo_get_object routine returns the first object stored in
 * the provided queue.
 *
 * @note
 * The number of calls to get the next object depends on the number of objects
 * returned by a previous call to @ref util_fifo_get_number. There is no
 * guaranty for valid objects, if there are more calls to @ref util_fifo_get_object
 * than stored objects.
 *
 * @note
 * The routine is not synchronized. This is normally not needful, because there
 * is only one destination.
 *
 * @param fifo Specifies the FIFO resource.
 *
 * @return
 * The routine returns the first stored objects in FIFO. If there was a previous
 * call to @ref util_fifo_get_object, the former second object is the first
 * object now.
 *
 * @see
 * util_fifo_async_insert util_fifo_sync_insert util_fifo_get_number
 */
void*
util_fifo_get_object (util_fifo_t* fifo)
{
  void* object;

  object = *(fifo->objects + fifo->rem);
  fifo->rem = (fifo->rem + 1) & fifo->mask;

  return object;
}


/****************************************************************************************/
/* end of source */
