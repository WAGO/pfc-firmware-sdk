#ifndef D_OsThread_H
#define D_OsThread_H

//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
/// SPDX-License-Identifier: MPL-2.0the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \author WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <inttypes.h>
#include <stddef.h>
#include <sys/cdefs.h>

#include "OsCommon.h"


//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

#define DEFAULT_STACK_SIZE (0x10000U) //!< The default stack size for newly created thread is 64KB. Linux default is about 2MB stack.

#define DEFAULT_RT_SCHEDULER SCHED_FIFO   //!< The default scheduler for realtime tasks is the fifo scheduler.

#define MAX_THREAD_NAME_LENGTH (32U)

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

typedef struct stOsThread tOsThread;
typedef void *(*ThreadRoutine)(void*);

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------


#if 0
/**
 * Call this function from within an thread you want to exit.
 * This function does not return.
 * The supplied result can be retrieved using OsThread_Join.
 * @param result The result of the thread.
 */
void OsThread_Exit(void* result)  __attribute__ ((__noreturn__));
#endif


/**
 * Cancels a running thread.
 * @param thread The thread to cancel.
 * @return OS_S_OK on success.
 */
i32 OsThread_Cancel(tOsThread *thread) __nonnull((1));

/**
 * Join a thread in execution. This function blocks until the supplied
 * thread has finished its execution.
 * @param thread The thread to join.
 * @param threadResult The result of the thread.
 * @return
 */
i32 OsThread_Join(tOsThread *thread, void** threadResult) __nonnull((1));

/**
 * Creates a new thread that will be scheduled by the realtime scheduler.
 * @param routine The routine to be executed within the thread's context.
 * @param routineArgs Arguments for the routine.
 * @param priority The desired priority of the of the thread.
 * @return OS_S_OK on success, error code otherwise.
 */
tOsThread * OsThread_CreateRealtime(ThreadRoutine routine,
                                void* routineArgs,
                                i32 priority) __nonnull((1));

/**
 * Creates a new thread that will be scheduled by the realtime scheduler.
 * Same as @sa OsThread_CreateRealtime() but with additional parameter
 * to specify the stack size.
 * @param routine
 * @param routineArgs
 * @param priority
 * @param stackSize
 * @return OS_S_OK on success, error code otherwise.
 */
tOsThread * OsThread_CreateRealtimeEx(ThreadRoutine routine,
                                void* routineArgs,
                                i32 priority,
                                size_t  stackSize,
                                const char* name)__nonnull((1));


/**
 *
 * @param routine
 * @param routineArgs
 * @param priority
 * @return
 */
tOsThread * OsThread_Create(ThreadRoutine routine,
                        void* routineArgs,
                        i32 priority)__nonnull((1));

/**
 *
 * @param routine
 * @param routineArgs
 * @param priority
 * @param stackSize
 * @return
 */
tOsThread * OsThread_CreateEx(ThreadRoutine routine,
                           void* routineArgs,
                           i32 priority,
                           size_t stackSize,
                           const char* name)__nonnull((1));

/**
 * Enables thread cancellation. Call this from within
 * a thread to enable thread cancellation.
 * @return OS_S_OK on success
 */
i32 OsThread_CancelEnable(void);

/**
 * Disables thread cancellation. Call this function
 * from within a thread to disable thread cancellation.
 * @return OS_S_OK on success, error code oterwise.
 */
i32 OsThread_CancelDisable(void);


i32 OsThread_CancelTypeDeferred(void);

i32 OsThread_CancelTypeAsynchronous(void);

/**
 *
 * @param thread
 * @param threadResult
 * @return
 */
i32 OsThread_Destroy(tOsThread *thread,
                         void** threadResult)__nonnull((1));
#endif  // D_OsThread_H
