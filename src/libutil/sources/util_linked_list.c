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
 *  $Workfile: util_linked_list.c $
 *
 * Last Modification:
 *  $Author: u012044 $
 *  $Modtime: 06.07.2012 $
 *  $Revision: 65684 $
 */


/****************************************************************************************/
/* includes */

#include "os_api.h"                         /* operating system */
#include "util_api.h"                       /* utility interface */


/****************************************************************************************/
/* functionality */

/**
 * The @user util_linked_list_create routine creates a linked list to handle user defined objects.
 * On the startup phase the memory for the management structure is allocated. The user only
 * needs a pointer of the management structure to handle the linked list.
 *
 *  @param max Specifies the maximum number of objects have to be managed.
 *  @return
 *  A reference to the created linked list. The value @NULL indicates an error.
 */
util_linked_list_t* util_linked_list_create (uint_t max)
{
  util_linked_list_t* linked_list = NULL;
  uint8_t* mem;
  uint_t mem_size;

  mem_size = sizeof(util_linked_list_t) + (sizeof(util_linked_list_obj_t) * max);

  /* allocate the memory necessary for the linked list object header */
  if (NULL != (mem = os_memory_alloc(mem_size)))
  {
    linked_list = (util_linked_list_t*) (void*) mem;
    mem += sizeof(util_linked_list_t);

    /* initialise the linked list */
    linked_list->max_objects = max;
    linked_list->first_linked_list_obj = NULL;
    linked_list->last_linked_list_obj = NULL;

    util_linked_list_seek(linked_list);

    /* create the pool of the linked list object header */
    util_pool_create (&linked_list->linked_list_obj_pool, mem, max, sizeof(util_linked_list_obj_t));
  }

  return (linked_list);
}

/**
 * The @user util_linked_list_destroy routine destroys the specified linked list. The allocated memory
 * will be deallocated as well.
 *
 *  @param linked_list Specifies the linked list to be destroyed.
 */
void util_linked_list_destroy (util_linked_list_t* linked_list)
{
  OS_ASSERT(NULL != linked_list);

  /* deallocate the memory */
  os_memory_dealloc(linked_list);

  linked_list = NULL;
}

/**
 * The @user util_linked_list_add_first routine adds the specified object at the first position in
 * the linked list.
 *
 *  @param linked_list Specifies the linked list.
 *  @param object Specifies the object to be linked in the list.
 *  @return
 *  A reference to the created linked list object. This reference can be used to remove this object
 *  later. The value @NULL indicates an error.
 */
util_linked_list_obj_t* util_linked_list_add_first (util_linked_list_t* linked_list,
                                                    void* object)
{
  util_linked_list_obj_t* new_linked_list_obj;
  util_linked_list_obj_t* linked_list_obj_temp;

  OS_ASSERT(NULL != linked_list);
  OS_ASSERT(NULL != object);

  /* get a linked list object header from pool */
  if (NULL != (new_linked_list_obj = (util_linked_list_obj_t*)
                  util_pool_async_query(&linked_list->linked_list_obj_pool)))
  {
    /* add the linked list object at the first position in the linked list */
    new_linked_list_obj->usr_data_object = object;

    /* if there is already an object linked */
    if (NULL != linked_list->first_linked_list_obj)
    {
      linked_list_obj_temp = linked_list->first_linked_list_obj;

      linked_list->first_linked_list_obj = new_linked_list_obj;

      new_linked_list_obj->previous_linked_list_obj = NULL;
      new_linked_list_obj->next_linked_list_obj = linked_list_obj_temp;

      linked_list_obj_temp->previous_linked_list_obj = new_linked_list_obj;
    }
    else
    {
      linked_list->first_linked_list_obj = new_linked_list_obj;

      new_linked_list_obj->previous_linked_list_obj = NULL;
      new_linked_list_obj->next_linked_list_obj = NULL;
    }
  }

  return (new_linked_list_obj);
}

/**
 * The @user util_linked_list_add_last routine adds the specified object at the last position in
 * the linked list.
 *
 *  @param linked_list Specifies the linked list.
 *  @param object Specifies the object to be linked in the list.
 *  @return
 *  A reference to the created linked list object. This reference can be used to remove this object
 *  later. The value @NULL indicates an error.
 */
util_linked_list_obj_t* util_linked_list_add_last (util_linked_list_t* linked_list,
                                                   void* object)
{
  util_linked_list_obj_t* new_linked_list_obj;
  util_linked_list_obj_t* linked_list_obj_temp;

  OS_ASSERT(NULL != linked_list);
  OS_ASSERT(NULL != object);

  /* get a linked list object header from pool */
  if (NULL != (new_linked_list_obj =
      (util_linked_list_obj_t*) util_pool_async_query(&linked_list->linked_list_obj_pool)))
  {
    /* add the linked list object at the last position in the linked list */
    new_linked_list_obj->usr_data_object = object;

    /* if there is already an object linked */
    if (NULL != linked_list->first_linked_list_obj)
    {
      linked_list_obj_temp = linked_list->last_linked_list_obj;

      linked_list->last_linked_list_obj = new_linked_list_obj;

      new_linked_list_obj->previous_linked_list_obj = linked_list_obj_temp;
      new_linked_list_obj->next_linked_list_obj = NULL;

      linked_list_obj_temp->next_linked_list_obj = new_linked_list_obj;
    }
    else
    {
      linked_list->first_linked_list_obj = new_linked_list_obj;
      linked_list->last_linked_list_obj = new_linked_list_obj;
      new_linked_list_obj->previous_linked_list_obj = NULL;
      new_linked_list_obj->next_linked_list_obj = NULL;
    }
  }

  return (new_linked_list_obj);
}

/**
 * The @user util_linked_list_add_before routine adds the specified object at the position before the
 * specified linked list object.
 *
 *  @param linked_list Specifies the linked list.
 *  @param next_linked_list_obj Specifies the object to be linked in the list.
 *  @param object Specifies the object to be linked in the list.
 *  @return
 *  A reference to the created linked list object. This reference can be used to remove this object
 *  later. The value @NULL indicates an error.
 */
util_linked_list_obj_t* util_linked_list_add_before (util_linked_list_t* linked_list,
                                                     util_linked_list_obj_t* next_linked_list_obj,
                                                     void* object)
{
  util_linked_list_obj_t* new_linked_list_obj;
  util_linked_list_obj_t* temp_linked_list_obj;

  OS_ASSERT(NULL != linked_list);
  OS_ASSERT(NULL != next_linked_list_obj);
  OS_ASSERT(NULL != object);

  /* get a linked list object header from pool */
  if (NULL != (new_linked_list_obj = (util_linked_list_obj_t*)
                                     util_pool_async_query(&linked_list->linked_list_obj_pool)))
  {
    /* add the linked list object in front of the specified object */
    new_linked_list_obj->usr_data_object = object;

    /* if the next linked list object is the first one in the linked list */
    if (NULL == next_linked_list_obj->previous_linked_list_obj)
    {
      temp_linked_list_obj = linked_list->first_linked_list_obj;

      linked_list->first_linked_list_obj = new_linked_list_obj;

      new_linked_list_obj->previous_linked_list_obj = NULL;
      new_linked_list_obj->next_linked_list_obj = next_linked_list_obj;

      next_linked_list_obj->previous_linked_list_obj = new_linked_list_obj;

    }
    else
    {
      new_linked_list_obj->previous_linked_list_obj = next_linked_list_obj->previous_linked_list_obj;
      new_linked_list_obj->next_linked_list_obj = next_linked_list_obj;

      next_linked_list_obj->previous_linked_list_obj = new_linked_list_obj;
    }
  }

  return (new_linked_list_obj);
}

/**
 * The @user util_linked_list_add_after routine adds the specified object at the position after the
 * specified linked list object.
 *
 *  @param linked_list Specifies the linked list.
 *  @param previous_linked_list_obj Specifies the object to be linked in the list.
 *  @param object Specifies the object to be linked in the list.
 *  @return
 *  A reference to the created linked list object. This reference can be used to remove this object
 *  later. The value @NULL indicates an error.
 */
util_linked_list_obj_t* util_linked_list_add_after (util_linked_list_t* linked_list,
                                                    util_linked_list_obj_t* previous_linked_list_obj,
                                                    void* object)
{
  util_linked_list_obj_t* new_linked_list_obj;
  util_linked_list_obj_t* temp_linked_list_obj;

  OS_ASSERT(NULL != linked_list);
  OS_ASSERT(NULL != previous_linked_list_obj);
  OS_ASSERT(NULL != object);

  /* get a linked list object header from pool */
  if (NULL != (new_linked_list_obj = (util_linked_list_obj_t*)
                                     util_pool_async_query(&linked_list->linked_list_obj_pool)))
  {
    /* add the linked list object in front of the specified object */
    new_linked_list_obj->usr_data_object = object;

    /* if the previous linked list object is the last one in the linked list */
    if (NULL == previous_linked_list_obj->next_linked_list_obj)
    {
      temp_linked_list_obj = linked_list->last_linked_list_obj;

      linked_list->last_linked_list_obj = new_linked_list_obj;

      new_linked_list_obj->previous_linked_list_obj = previous_linked_list_obj;
      new_linked_list_obj->next_linked_list_obj = NULL;

      previous_linked_list_obj->next_linked_list_obj = new_linked_list_obj;

    }
    else
    {
      new_linked_list_obj->next_linked_list_obj = previous_linked_list_obj->next_linked_list_obj;
      new_linked_list_obj->previous_linked_list_obj = previous_linked_list_obj;

      previous_linked_list_obj->next_linked_list_obj = new_linked_list_obj;
    }
  }

  return (new_linked_list_obj);
}

/**
 * The @user util_linked_list_remove routine removes the specified linked list object out of the
 * linked list.
 *
 *  @param linked_list Specifies the linked list.
 *  @param linked_list_obj Specifies the object to be removed from the linked list.
 */
void util_linked_list_remove (util_linked_list_t* linked_list,
                              util_linked_list_obj_t* linked_list_obj)
{
  OS_ASSERT(NULL != linked_list);
  OS_ASSERT(NULL != linked_list_obj);

  /* if this linked list object is the first in the linked list */
  if (NULL == linked_list_obj->previous_linked_list_obj)
  {
    linked_list->first_linked_list_obj = linked_list_obj->next_linked_list_obj;

    /* if there is a next linked list object */
    if (NULL != linked_list_obj->next_linked_list_obj)
    {
      linked_list_obj->next_linked_list_obj->previous_linked_list_obj = NULL;
    }
  }
  else
  {
    /* if this linked list object is the last in the linked list */
    if (NULL == linked_list_obj->next_linked_list_obj)
    {
      linked_list->last_linked_list_obj = linked_list_obj->previous_linked_list_obj;
      linked_list_obj->previous_linked_list_obj->next_linked_list_obj = NULL;
    }
    else
    {
      /* this linked list object is the middle of the linked list */
      linked_list_obj->next_linked_list_obj->previous_linked_list_obj = linked_list_obj->previous_linked_list_obj;
      linked_list_obj->previous_linked_list_obj->next_linked_list_obj = linked_list_obj->next_linked_list_obj;
    }
  }

  /* update the next pointer */
  if (linked_list->next_linked_list_obj == linked_list_obj)
  {
    linked_list->next_linked_list_obj = linked_list_obj->next_linked_list_obj;
  }

  /* release the queue entry */
  linked_list_obj->previous_linked_list_obj = NULL;
  linked_list_obj->next_linked_list_obj = NULL;

  util_pool_async_insert(&linked_list->linked_list_obj_pool, linked_list_obj);

}

/**
 * The @user util_linked_list_seek routine sets the reference for the routine @util_linked_list_get_next
 * to the first object in the linked list.
 *
 *  @param linked_list Specifies the linked list.
 */
void util_linked_list_seek (util_linked_list_t* linked_list)
{
  OS_ASSERT(NULL != linked_list);

  linked_list->next_linked_list_obj = linked_list->first_linked_list_obj;
}

/**
 * The @user util_linked_list_get_user_data_obj routine returns the reference to the user data object in the
 * specified linked list object.
 *
 *  @param linked_list Specifies the linked list.
 *  @param linked_list_obj Specifies the linked list object where the user data object is stored.
 *  @return
 *  A reference to the user data object.
 */
void* util_linked_list_get_user_data_obj (util_linked_list_obj_t* linked_list_obj)
{
  return (linked_list_obj->usr_data_object);
}

/**
 * The @user util_linked_list_get_next routine returns the reference the next object returned by this
 * routine. Use the routine @util_linked_list_seek to set the reference back to the first object in the
 * linked list.
 *
 *  @param linked_list Specifies the linked list.
 *  @return
 *  A reference to the next object in the linked list. The value @NULL indicates that there is no next list object.
 */
util_linked_list_obj_t* util_linked_list_get_next (util_linked_list_t* linked_list)
{
  util_linked_list_obj_t* next_linked_list_obj;

  OS_ASSERT(NULL != linked_list);

  next_linked_list_obj = linked_list->next_linked_list_obj;

  /* if there is an next linked list object available */
  if (NULL != linked_list->next_linked_list_obj)
  {
    linked_list->next_linked_list_obj = linked_list->next_linked_list_obj->next_linked_list_obj;
  }

  return (next_linked_list_obj);
}

/**
 * The @user util_linked_list_search routine returns the reference of object in the linked list
 * where the saved user data pointer matches the specified user data pointer.
 *
 *  @param linked_list Specifies the linked list.
 *  @param object Specifies the user data pointer should be found in the linked list.
 *  @return
 *  A reference to the matching linked list object. The value @NULL indicates that no matching object could
 *  be found.
 */
util_linked_list_obj_t* util_linked_list_search (util_linked_list_t* linked_list,
                                                 void* object)
{
  util_linked_list_obj_t* current_linked_list_obj;
  util_linked_list_obj_t* found_linked_list_obj = NULL;

  OS_ASSERT(NULL != linked_list);

  current_linked_list_obj = linked_list->first_linked_list_obj;

  while (   (NULL != current_linked_list_obj)
         && (NULL == found_linked_list_obj))
  {
    if (current_linked_list_obj->usr_data_object == object)
    {
      found_linked_list_obj = current_linked_list_obj;
    }
    current_linked_list_obj = current_linked_list_obj->next_linked_list_obj;
  }

  return (found_linked_list_obj);
}

/**
 * The @user util_linked_list_obj_search routine searches the specified linked list object
 *  \ref linked_list_obj in the specified linked list \ref linked_list.
 *
 *  @param linked_list Specifies the linked list.
 *  @param object Specifies the user data pointer should be found in the linked list.
 *  @return
 *  The routine returns \ref true if the specified linked list object is part of the specified linked list. In all
 *  other cases it returns \ref false.
 */
bool util_linked_list_obj_search (util_linked_list_t* linked_list,
                                  util_linked_list_obj_t* linked_list_obj)
{
  util_linked_list_obj_t* current_linked_list_obj;
  bool found_linked_list_obj = false;

  OS_ASSERT(NULL != linked_list);

  current_linked_list_obj = linked_list->first_linked_list_obj;

  while (   (NULL != current_linked_list_obj)
         && (!(found_linked_list_obj)))
  {
    if (current_linked_list_obj == linked_list_obj)
    {
      found_linked_list_obj = true;
    }
    current_linked_list_obj = current_linked_list_obj->next_linked_list_obj;
  }

  return (found_linked_list_obj);
}

/**
 * The @user util_linked_list_obj_check routine checks the specified linked list object if it could be part of the
 *  specified linked list \ref linked_list.
 *
 *  @param linked_list Specifies the linked list.
 *  @param object Specifies the user data pointer should be found in the linked list.
 *  @return
 *  The routine returns \ref true if the specified linked list object could be part of the specified linked list. That
 *  doesn't mean that the object is currently part of the linked list. In all other cases it returns \ref false.
 */
bool util_linked_list_obj_check (util_linked_list_t* linked_list,
                                 util_linked_list_obj_t* linked_list_obj)
{
  bool obj_valid = false;
  uint_t mem_size;

  OS_ASSERT(NULL != linked_list);

  mem_size = sizeof(util_linked_list_t) + (sizeof(util_linked_list_obj_t) * linked_list->max_objects);

  if (   (((uint_t) linked_list_obj) >= ((uint_t) linked_list->linked_list_obj_pool.mem))
      && (((uint_t) linked_list_obj) < (((uint_t) linked_list->linked_list_obj_pool.mem) + mem_size)))
  {
    obj_valid = true;
  }

  return (obj_valid);
}

/****************************************************************************************/
/* end of source */
