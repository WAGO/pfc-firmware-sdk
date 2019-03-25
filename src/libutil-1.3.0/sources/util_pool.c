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
 *  $Workfile: util_pool.c $
 *
 * Last Modification:
 *  $Author: u010391 $
 *  $Modtime: 02.03.2009 $
 *  $Revision: 81893 $
 */


/****************************************************************************************/
/* includes */

#include "os_api.h"                         /* operating system */
#include "util_api.h"                       /* utility interface */


/****************************************************************************************/
/* functionality */

/**
 * The @user util_pool_create creates a pool used to handle unused objects. On the
 * startup phase all objects can be loaded (e.g. allocation). If the objects are not
 * in use, the pool manages these objects at this time. Allocating and deallocating memory
 * can be avoid during runtime.
 *
 * NOTE:
 * The size of an object requires at least the size of a void pointer.
 *
 * @param pool Specifies the pool resource.
 * @param mem Points the buffer contains all the objects.
 * @param max Specifies the maximum number of objects have to be managed.
 * @param size Specifies the size in bytes of an object.
 * @see
 * util_pool_destroy
 */
void
util_pool_create (util_pool_t* pool, void* mem, uint_t max, uint_t size)
{
  OS_ASSERT(NULL != pool);
  OS_ASSERT(NULL != mem);
  OS_ASSERT(0 != max);
  OS_ASSERT(0 != max);
  OS_ASSERT(size >= sizeof(pool->mem));
  OS_ASSERT(0 == ((sizeof(pool->mem) - 1) & size));

  pool->next_obj = NULL;
  pool->mem = mem;
  pool->mem_obj = mem;
  pool->max = max;
  pool->num = max;
  pool->size = size/sizeof(pool->mem);
}

/**
 *
 *
 * @param pool
 * @param object
 */
/**
 * The @user util_pool_async_insert routine inserts an object into the pool. This pool manages
 * the inserted objects until an application queries for an unused object by a call to
 * @user util_pool_async_query.
 *
 * NOTE:
 * The operation is not synchronized. There is no guaranty, that the operation will not
 * be interrupted by another process or thread and another object will be inserted at this time.
 * In this case the queue information can be corrupt.
 *
 * NOTE:
 * The size of object has to be inserted is at least the size of a void pointer.
 *
 * @param pool Specifies the pool resource.
 * @param object Points to an object which have to be inserted.
 * @see
 * util_pool_async_query
 */
void
util_pool_async_insert (util_pool_t* pool, void* object)
{
  util_list_single_t* single;

  single = (util_list_single_t*) object;

  single->next = pool->next_obj;
  pool->next_obj = single;
}

/**
 * The @user util_pool_async_query routine can be used by an application to query for
 * an unused object (@user util_pool_async_insert). If the pool contains at least one object,
 * the object will be removed from the management list and returned by leaving this routine.
 *
 * NOTE:
 * The operation is not synchronized. There is no guaranty, that the operation will not
 * be interrupted by another process or thread and another object will be inserted at this time.
 * In this case the queue information can be corrupt.
 *
 * @param pool Specifies the pool resource.
 * @return
 * A reference to an object indicates success. The value @e NULL indicates an empty queue.
 * @see
 * util_pool_async_insert
 */
void*
util_pool_async_query (util_pool_t* pool)
{
  util_list_single_t* single;

  single = pool->next_obj;
  if (NULL != single)
  {
    pool->next_obj = single->next;
  }
  else if (0 != pool->num)
  {
    single = (util_list_single_t*) pool->mem_obj;
    pool->mem_obj = (uint_t*) (pool->mem_obj + pool->size);
    pool->num--;
  }

  return single;
}

/**
 * The @user util_pool_sync_insert routine inserts an object into the pool synchronous.
 * Inserting an object is blocked for other processes or threads at this time.
 *
 * NOTE:
 * The size of object has to be inserted is at least the size of a void pointer.
 *
 * @param section References the semaphore (lock mechanism).
 * @param pool Specifies the pool resource.
 * @param object Points to an object which have to be inserted.
 * @see
 * util_pool_sync_query
 */
void
util_pool_sync_insert (handle_t section, util_pool_t* pool, void* object)
{
  util_list_single_t* single;

  single = (util_list_single_t*) object;

  os_critical_section_lock (section);
  single->next = pool->next_obj;
  pool->next_obj = single;
  os_critical_section_unlock (section);
}

/**
 * The @user util_pool_sync_query routine can be used by an application to query synchronously for
 * a previously inserted object (@user util_pool_sync_insert). Checking the pool for an object is
 * blocked for other processes or threads at this time. If the pool contains at least one object,
 * the object will be removed from the management list and returned by leaving this routine.
 *
 * @param section References the semaphore (lock mechanism).
 * @param pool Specifies the pool resource.
 * @return
 * A reference to an object indicates success. The value @e NULL indicates an empty queue.
 * @see
 * util_pool_sync_insert
 */
void*
util_pool_sync_query (handle_t section, util_pool_t* pool)
{
  util_list_single_t* single;

  pool = (util_pool_t*) pool;

  os_critical_section_lock (section);
  single = pool->next_obj;
  if (NULL != single)
  {
    pool->next_obj = single->next;
  }
  else if (0 != pool->num)
  {
    single = (util_list_single_t*) pool->mem_obj;
    pool->mem_obj = (uint_t*) (pool->mem_obj + pool->size);
    pool->num--;
  }
  os_critical_section_unlock (section);

  return single;
}


/****************************************************************************************/
/* end of source */
