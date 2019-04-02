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
 *  $Workfile: util_queue.c $
 *
 * Last Modification:
 *  $Author: u010391 $
 *  $Modtime: 02.03.2009 $
 *  $Revision: 78096 $
 */


/****************************************************************************************/
/* includes */

#include "os_api.h"                         /* operating system */
#include "util_api.h"                       /* utility interface */


/****************************************************************************************/
/* functionality */

/**
 * The @user util_queue_create routine creates a queue used by application to list
 * several objects such like messages.
 *
 * NOTE:
 * All objects which have to be queued requires a simple void pointer (void*). This
 * pointer is the first element of the object.
 *
 * @param queue Specifies the queue resource.
 *
 * @return
 * A handle to the created queue indicates success. NULL indicates failure.
 * @see
 * util_queue_destroy
 */
void
util_queue_create (util_queue_t* queue)
{
  queue->root.next = NULL;
  queue->last = &queue->root;
}

/**
 * The @user util_queue_async_insert routine inserts an object into the queue. The object
 * will always be referenced as the last object of the queue (FIFO).
 *
 * NOTE:
 * All objects which have to be queued requires a simple void pointer (void*). This
 * pointer is the first element of the object and will be used by the queue to handle
 * the references to the next object of the queue (referenced list).
 *
 * NOTE:
 * The operation is not synchronized. There is no guaranty, that the operation will not
 * be interrupted by another process or thread and another object will be inserted at this time.
 * In this case the queue information can be corrupt.
 *
 * @param queue Specifies the queue resource.
 * @param object Points to an object which have to be inserted.
 */
void
util_queue_async_insert (util_queue_t* queue, void* object)
{
  util_list_single_t* single;

  single = (util_list_single_t*) object;

  single->next = NULL;
  queue->last->next = single;
  queue->last = single;
}

/**
 * The @user util_queue_async_query routine can be used by an application to query for
 * a previously inserted object (@user util_queue_async_insert). If the queue contains at
 * least one object, the object will be removed from the queue list and returned by leaving this
 * routine.
 *
 * NOTE:
 * The routine returns always the first object inserted in the queue.
 *
 * NOTE:
 * The operation is not synchronized. There is no guaranty, that the operation will not
 * be interrupted by another process or thread and another object will be inserted at this time.
 * In this case the queue information can be corrupt.
 *
 * @param queue Specifies the queue resource.
 * @return
 * A reference to an object indicates success. NULL indicates an empty queue.
 * @see
 * util_queue_async_insert
 */
void*
util_queue_async_query (util_queue_t* queue)
{
  util_list_single_t* single;

  single = queue->root.next;
  if (NULL != single)
  {
    queue->root.next = single->next;
    if (NULL == queue->root.next)
    {
      queue->last = &queue->root;
    }
  }

  return single;
}

/**
 * The @user util_queue_sync_insert routine inserts an object into the queue synchronous.
 * Inserting an object is blocked for other processes or threads at this time.
 * The object will always be referenced as the last object of the queue (FIFO).
 *
 * NOTE:
 * All objects which have to be queued requires a simple void pointer (void*). This
 * pointer is the first element of the object and will be used by the queue to handle
 * the references to the next object of the queue (referenced list).
 *
 * @param section References the semaphore (lock mechanism).
 * @param queue Specifies the queue resource.
 * @param object Points to an object which have to be inserted.
* @see
 * util_queue_sync_query
 */
void
util_queue_sync_insert (handle_t section, util_queue_t* queue, void* object)
{
  util_list_single_t* single;

  single = (util_list_single_t*) object;

  os_critical_section_lock (section);
  single->next = NULL;
  queue->last->next = single;
  queue->last = single;
  os_critical_section_unlock (section);
}

/**
 * The @user util_queue_sync_query routine can be used by an application to query synchronously for
 * a previously inserted object (@user util_queue_sync_insert). Checking the queue for an object is
 * blocked for other processes or threads at this time. If the queue contains at least one object,
 * the object will be removed from the queue list and returned by leaving this routine.
 *
 * NOTE:
 * The routine returns always the first object inserted in the queue.
 *
 * @param section References the semaphore (lock mechanism).
 * @param queue Specifies the queue resource.
 * @return
 * A reference to an object indicates success. NULL indicates an empty queue.
 * @see
 * util_queue_sync_insert
 */
void*
util_queue_sync_query (handle_t section, util_queue_t* queue)
{
  util_list_single_t* single;

  os_critical_section_lock (section);
  single = queue->root.next;
  if (NULL != single)
  {
    queue->root.next = single->next;
    if (NULL == queue->root.next)
    {
      queue->last = &queue->root;
    }
  }
  os_critical_section_unlock (section);

  return single;
}


/****************************************************************************************/
/* end of source */
