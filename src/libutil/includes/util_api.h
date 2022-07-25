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
 *  $Workfile: util_api.h $
 *
 * Last Modification:
 *  $Author: u010391 $
 *  $Modtime: 02.03.2009 $
 *  $Revision: 65684 $
 */
#ifndef UTIL_API_H_
#define UTIL_API_H_

/****************************************************************************************/
/* includes */

#include <stdbool.h>

/****************************************************************************************/
/* utility types */

typedef struct util_list_single_tag                   util_list_single_t;
typedef struct util_queue_tag                         util_queue_t;
typedef struct util_pool_tag                          util_pool_t;
typedef struct util_fifo_tag                          util_fifo_t;
typedef struct util_linked_list_tag                   util_linked_list_t;
typedef struct util_linked_list_obj_tag               util_linked_list_obj_t;

/**
 * @ingroup util_list_single_tag
 *
 * The @ref util_list_single_tag structure is defined to handle single lists according
 * a queue or a pool.
 */
struct util_list_single_tag
{
  /** Points to the next object in list. */
  util_list_single_t*   next;
};

/**
 * @ingroup util_queue_tag
 *
 * The @ref util_queue_tag describes the variables required to process a queue.
 */
struct util_queue_tag
{
  /** Specifies the queue root. */
  util_list_single_t    root;
  /** Points to the last object in queue. */
  util_list_single_t*   last;
};

/**
 * @ingroup util_queue_tag
 *
 * The @ref util_queue_tag describes the variables required to process a queue.
 */
struct util_pool_tag
{
  /** Point to the next unused object in pool. */
  util_list_single_t*   next_obj;
  /** Points to the memory used for the pool objects. */
  uint_t*               mem;
  /** Points to the next unused object memory. */
  uint_t*               mem_obj;
  /** Specifies the maximum number of objects in pool. */
  uint_t                max;
  /** Specifies the number of unused objects in pool. */
  uint_t                num;
  /** Specifies the size of an object in unsigned integer size. */
  uint_t                size;
};

/**
 * @ingroup util_fifo_tag
 *
 * The @ref util_fifo_tag describes the variables required to process a FIFO.
 */
struct util_fifo_tag
{
  /** Points to the memory used for the queued objects. */
  void**                objects;
  /** Specifies the maximum number of elements. */
  uint_t                max;
  /** Specifies the mask according to the maximum number of elements. */
  uint_t                mask;
  /** Specifies the number of queued elements. */
  uint_t                num;
  /** Specifies the index to the insert object. */
  uint_t                ins;
  /** Specifies the index to the remove object. */
  uint_t                rem;
};

/**
 * @ingroup util_linked_list_tag
 *
 * The @ref util_linked_list_obj_tag describes the necessary header information of each element in the linked list
 */
struct util_linked_list_obj_tag
{
  /* object handle */
  void* obj_handle;
  /** points to the next object in the linked list */
  util_linked_list_obj_t* previous_linked_list_obj;
  /** points to the previous object in the linked list */
  util_linked_list_obj_t* next_linked_list_obj;
  /** points to the user data object */
  void*                   usr_data_object;
};

/**
 * @ingroup util_linked_list_tag
 *
 * The @ref util_linked_list_tag describes the variables required to process a linked list.
 */
struct util_linked_list_tag
{
  /** points to the next object in the linked list */
  util_linked_list_obj_t*  first_linked_list_obj;
  /** points to the previous object in the linked list */
  util_linked_list_obj_t*  last_linked_list_obj;
  /** points to the next object in the linked list */
  util_linked_list_obj_t*  next_linked_list_obj;
  /** pool for the linked list objects */
  util_pool_t              linked_list_obj_pool;
  /** maximum number of objects handled by the linked list */
  uint_t                   max_objects;
};

/****************************************************************************************/
/* utility queue routines */

void util_queue_create (util_queue_t* queue);
void util_queue_destroy (util_queue_t* queue);
void util_queue_async_insert (util_queue_t* queue, void* object);
void* util_queue_async_query (util_queue_t* queue);
void util_queue_sync_insert (handle_t section, util_queue_t* queue, void* object);
void* util_queue_sync_query (handle_t section, util_queue_t* queue);


/****************************************************************************************/
/* utility pool routines */

void util_pool_create (util_pool_t* pool, void* mem, uint_t max, uint_t size);
void util_pool_async_insert (util_pool_t* pool, void* object);
void* util_pool_async_query (util_pool_t* pool);
void util_pool_sync_insert (handle_t section, util_pool_t* pool, void* object);
void* util_pool_sync_query (handle_t section, util_pool_t* pool);


/****************************************************************************************/
/* utility FIFO routines */

void util_fifo_initialize (util_fifo_t* fifo, void* mem, uint_t max);
uint_t util_fifo_async_insert (util_fifo_t* fifo, void* object);
uint_t util_fifo_sync_insert (handle_t section, util_fifo_t* fifo, void* object);
uint_t util_fifo_get_number (util_fifo_t* fifo);
void* util_fifo_get_object (util_fifo_t* fifo);

/****************************************************************************************/
/* utility linked list routines */

util_linked_list_t* util_linked_list_create (uint_t max);
void util_linked_list_destroy (util_linked_list_t* linked_list);
util_linked_list_obj_t* util_linked_list_add_first (util_linked_list_t* linked_list, void* object);
util_linked_list_obj_t* util_linked_list_add_last (util_linked_list_t* linked_list, void* object);
util_linked_list_obj_t* util_linked_list_add_before (util_linked_list_t* linked_list,
                                                     util_linked_list_obj_t* next_linked_list_obj, void* object);
util_linked_list_obj_t* util_linked_list_add_after (util_linked_list_t* linked_list,
                                                    util_linked_list_obj_t* previous_linked_list_obj, void* object);
void util_linked_list_remove (util_linked_list_t* linked_list,
                              util_linked_list_obj_t* linked_list_obj);
void util_linked_list_seek (util_linked_list_t* linked_list);
void* util_linked_list_get_user_data_obj (util_linked_list_obj_t* linked_list_obj);
util_linked_list_obj_t* util_linked_list_get_next (util_linked_list_t* linked_list);
util_linked_list_obj_t* util_linked_list_search (util_linked_list_t* linked_list, void* object);
bool util_linked_list_obj_search (util_linked_list_t* linked_list, util_linked_list_obj_t* linked_list_obj);
bool util_linked_list_obj_check (util_linked_list_t* linked_list, util_linked_list_obj_t* linked_list_obj);

/****************************************************************************************/
/* end of header */
#endif /* UTIL_API_H_ */
