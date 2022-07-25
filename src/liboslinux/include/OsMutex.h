//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
/// SPDX-License-Identifier: MPL-2.0
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \brief The OsMutex module implements mutexes. On Linux it creates rt-mutexes
///        by default. If NDEBUG is not defined, the mutexes will also be robust.
///        When an onwer dies a message is printed and the mutex is made consistent
///        again.
///
/// \author WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef D_OsMutex_H
#define D_OsMutex_H

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <inttypes.h>
#include <sys/cdefs.h>

#include "OsCommon.h"

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

typedef struct stOsMutex tOsMutex;


//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------

/**
 * Creates a new, ready to use mutex. By default the PRIORITY_INHERIT protocol is used.
 * @return Returns a pointer to the newly created mutex on success; NULL otherwise.
 */
tOsMutex *OsMutex_Create(void);

/**
 * Creates a new, ready to use mutex. By default the PRIORITY_INHERIT protocol is used.
 * @return Returns a pointer to the newly created mutex on success; NULL otherwise.
 */
tOsMutex *OsMutex_CreateEx(const char* name);

tOsMutex *OsMutex_CreateRobust(void);
tOsMutex *OsMutex_CreateRecursive(void);
tOsMutex *OsMutex_CreateRobustRecursive(void);

tOsMutex *OsMutex_CreateRobustEx(const char* name);
tOsMutex *OsMutex_CreateRecursiveEx(const char* name);
tOsMutex *OsMutex_CreateRobustRecursiveEx(const char* name);

/**
 * Gets a pointer to the underlying pthread_mutex_t structure.
 * @param self
 */
void * OsMutex_GetOsHandle(tOsMutex *self) __nonnull((1));


/**
 *
 * @param self
 * @return
 */
i32 OsMutex_Init(tOsMutex *self);

/**
 *
 * @param self
 * @return
 */
i32 OsMutex_Deinit(tOsMutex *self);

/**
 *
 * @param self
 * @return
 */
i32 OsMutex_Destroy(tOsMutex *self) __nonnull((1));

/**
 *
 * @param self
 * @return
 */
i32 OsMutex_Lock(tOsMutex *self);
/**
 *
 * @param self
 * @return
 */
i32 OsMutex_Unlock(tOsMutex *self);



#endif  // D_OsMutex_H
