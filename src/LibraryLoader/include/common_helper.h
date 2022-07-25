//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
/// manufacturing, reproduction, use, and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \file CommonHelper.h
///
/// \version <Revision>: $Rev$
///
/// \brief The common_helper module contains helper functions and defines.
///
/// \author <JSo> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef _COMMONHELPER_H
#define _COMMONHELPER_H

#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include "LibraryLoader_Logging.h"

/**
 * Removes compiler warnings about unused parameters.
 */
#define UNUSED_PARAMETER(X) ((void)X)

/**
 * Gets the number of elements fitting in an array.
 */
#define ELEMENT_COUNT(pX) (sizeof((pX))/sizeof(*(pX)))

#define BEGIN do {
#define END } while(0)

/**
 * @note Note the return statement!
 * @param ptr
 * Checks the supplied pointer for NULL and returns the calling function.
 */
#define CHECK_POINTER(ptr)\
   BEGIN\
      if (ptr == NULL)\
      { \
         /*lint -e{904} suppress "Return statement before end of function"*/\
         /* Ignore lint warning as the whole point of this macro is to bail out early. */\
         return LL_FAILURE;\
      }\
   END

/**
 * Convenient macro to check the result of the supplied function call against
 * zero and returns the calling function with LL_FAILURE.
 * @note: Note the return statement!
 */
#define CHECK_RESULT(func) \
   BEGIN\
   const int32_t res = func;\
   if ( res != 0 )\
   {\
      ERROR("Failed with %d at %s", res, #func);\
      return LL_FAILURE;\
   }\
   END\


/**
 *
 */
#define ZERO_MEMORY(pX) memset((pX), 0, sizeof(*(pX)))

//-- Function: safe_calloc -----------------------------------------------------
///
///   calloc wrapper that aborts the execution on error.
///   Allocates count * elsize bytes and fills the result with zeros.
///
/// \param count Number of elements.
/// \param elSize Size of one element.
///
/// \return Pointer to allocated memory.
//------------------------------------------------------------------------------
void *safe_calloc(size_t count,
                  size_t elsize);

#endif /* COMMONHELPER_H_ */
