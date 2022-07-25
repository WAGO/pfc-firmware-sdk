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
 *  $Workfile: os_api.h $
 *
 * Last Modification:
 *  $Author: u010391 $
 *  $Modtime: 27.02.2009 $
 *  $Revision: 65684 $
 */
#ifndef OS_API_H_
#define OS_API_H_


/****************************************************************************************/
/* operating system */
#include "os_specific.h"

/****************************************************************************************/
/* compiler */
#include "os_compiler.h"





/***************************************************************************************/
/**
 * @defgroup __TYPES_ Additional Basic types
 *
 * Basics types are for example
 *  - unsigned int
 *  - long
 *
 * But normally type definition will be used instead of the basic types to use
 * always the same type in all sources. For example, the basic type @e unsigned
 * @e int and @e unsigned @e long may have the same size on a 32bit system, but
 * the types have different meanings. In this case the type definition uint_t
 * have to be used.
 *
 * At next a lot of types will be defined, which are not defined by the
 * operating system. Normally operating systems already provides following types
 * in header file stdint.h:
 * - uint32_t
 * - int32_t
 * - uint16_t
 * - int16_t
 * - uint8_t
 * - int8_t
 *
 * @{
 */


/** @} */


/***************************************************************************************/
/**
 * @defgroup __TCHAR_ System Independent Character Type
 *
 * Normally the systems uses characters to define a string. If another languages
 * such like Japanese or Chinese have to be supported, the system has to use
 * width characters (WCHAR).
 *
 * For example the MICROSOFT compilers supports the _UNICODE define to
 * use width character strings. UNICODE routines such like @e wcslen
 * has to be used instead of standard routines such like @e strlen.
 *
 * To be independent to the standard format or the UNICODE format, the system
 * provides TCHAR types and TCHAR routines. For example the MICROSOFT
 * standard libraries provides the routine @e _tcslen, which is mapped to
 * the @e strlen routine, if the UNICODE define is not set. Otherwise the
 * routine is mapped to the UNICODE routine @e wcslen. The type tchar_t
 * is mapped to the basic type @e char (standard) or to the basic type
 * @e unsigned @e char (UNICODE).
 *
 * Macros are required to define static strings depend on the global settings
 * (UNICODE). Normally a static string will be defined shown in
 * the following example:
 *
 * @code
 * static char string_x[] = "example";
 * @endcode
 *
 * The variable @e string_x can not be used for UNICODE operations. To
 * be independent on the UNICODE settings, the string can be defined
 * shown in the following example:
 *
 * @code
 * static tchar_t string_x[] = _TEXT("example");
 * @endcode
 *
 * The string variable can be used without changes in a standard system
 * and an UNICODE system.
 *
 * @note
 * Maybe the macros are already defined by the operating system.
 * Please use the macros to define strings have to be provided by
 * calling routines of the operating systems. For example, the
 * operating system MS Windows CE requires system independent
 * strings of type tchar_t* (LPCTSTR).
 *
 * @{
 */

#ifdef _UNICODE
#define __T(x) L ## x
#else /* ndef _UNICODE */
#define __T(x) x
#endif

/**
 * Specifies a macro maps the @e __T macro to define independent strings.
 * The argument x specifies the string.
 */
#define _T(x) __T(x)

/**
 * Specifies a macro maps the @e __T macro to define independent strings.
 * The argument x specifies the string.
 */
#define _TEXT(x) __T(x)

/**
 * Specifies a macro maps the @e __T macro to define independent strings.
 * The argument x specifies the string.
 */
#define TEXT(x)  __T(x)

/** @} */


/****************************************************************************************/
/**
 * @defgroup os_macros_common Macros (common)
 *
 *
 *
 * Routines:
 *  - @ref OS_ARGUMENT_USED - Suppresses compiler warnings for unused parameters
 *  - @ref OS_ASSERT - Calls the assertion routine of the OS
 *  - @ref OS_MAX_COUNT - Counts the members of an array

 */
/**
 * @defgroup OS_ARGUMENT_USED OS_ARGUMENT_USED
 * @ingroup os_macros_common
 * @{
 */

/**
 * Calling @ref OS_ARGUMENT_USED suppresses a compiler warning for an
 * unused parameter in a function. The do...while construct is used to
 * enforce a ; after the macro call. The following construct redefines
 * variable x as being of type void so it cannot be accessed anymore. As this
 * may seem unintentional to lint, the corresponding messages are suppressed.
 * As most compilers declare the use of a void variable as an error, two
 * different versions of OS_ARGUMENT_USED are declared for the linting and compiling process.
 * @param arg  the unused parameter of the function
 *
 * @return
 * None
 */
#ifdef _lint
  /*lint -emacro((18), OS_ARGUMENT_USED) suppress 'redeclared' */
  /*lint -emacro(18, OS_ARGUMENT_USED) suppress 'redeclared' */
  /*lint -emacro(31, OS_ARGUMENT_USED) suppress 'redefinition' */
  /*lint -emacro(43, OS_ARGUMENT_USED) suppress 'Vacuous type' */
  /*lint -emacro(578, OS_ARGUMENT_USED) suppress 'Declaration of symbol '' hides symbol ''' */
  /*lint -emacro(640, OS_ARGUMENT_USED) suppress 'Expected strong type 'bool' in Boolean context' */
  /*lint -emacro(717, OS_ARGUMENT_USED) suppress 'do ... while(0)' */
  /*lint -emacro((920), OS_ARGUMENT_USED) suppress 'cast from pointer to void' */
  #define OS_ARGUMENT_USED(x) if (x){} void x; do{}while(false)
#else
  /* suppress warnings about unused variables */
  // The do...while construct is used to enforce a ; after the macro call
  #define OS_ARGUMENT_USED(x) do{if (x){}}while(false)
#endif
/**
 * @}
 */

/**
 * @defgroup OS_MAX_COUNT OS_MAX_COUNT
 * @ingroup os_macros_common
 * @{
 */

/**
 * Calling @ref OS_MAX_COUNT retrieve array member count of a given array.
 * in a specified time in [ms].
 *
 * @param x   array to be checked
 *
 * @return
 * The number of members in the given array.
 */
#ifndef OS_MAX_COUNT
#define OS_MAX_COUNT(x)           (sizeof(x)/sizeof(x[0]))    /**< retrieve array member count */
#endif
/**
 * @}
 */

/**
 * @defgroup OS_ASSERT OS_ASSERT
 * @ingroup os_macros_common
 * @{
 */

/**
 * Calling @ref OS_ASSERT calls the os-specific assert routine which
 * checks the given condition and aborts the program flow on failure
 * of the condition (e.g. result is false).
 *
 * @param cond   condition to be checked
 *
 * @return
 * None
 */
#ifndef OS_ASSERT
#define OS_ASSERT(cond)           assert(cond)
#endif

/** @} */



/****************************************************************************************/
/**
 * @defgroup OS_SWAP Swap Macros
 *
 * Routines:
 *  - @ref OS_SWAP_16  - swap bytes (16bit value)
 *  - @ref OS_SWAP_32  - swap bytes (32bit value)
 */

/**
 * @defgroup OS_SWAP_16 OS_SWAP_16
 * @ingroup OS_SWAP
 * @{
 */

/**
 * Calling @ref OS_SWAP_16 swaps the higher byte with the lower byte of
 * the 32bit value.
 *
 * For example, the value 0x1234 will be converted to 0x3412.
 *
 * @param x  16bit Specifies value to be swapped.
 *
 * @return
 * The macro returns the swapped value.
 *
 * @sa
 * OS_SWAP_32 OS_SWAP_2X
 */
#define OS_SWAP_16(x)             ((((x)<<0x0008)&0xFF00)|(((x)>>0x0008)&0x00FF))
/** @} */

/**
 * @defgroup OS_SWAP_32 OS_SWAP_32
 * @ingroup OS_SWAP
 * @{
 */

/**
 * Calling @ref OS_SWAP_32 swaps the highest byte of the 32bit value with
 * the lowest byte and the second byte with the third byte.
 *
 * For example, the value 0x12345678 will be converted to 0x78563412.
 *
 * @param x  32bit Specifies value to be swapped.
 *
 * @return
 * The macro returns the swapped value.
 *
 * @sa
 * OS_SWAP_16 OS_SWAP_2X
 */
#define OS_SWAP_32(x)             ((((x)<<0x00000018)&0xFF000000)|\
                                   (((x)<<0x00000008)&0x00FF0000)|\
                                   (((x)>>0x00000008)&0x0000FF00)|\
                                   (((x)>>0x00000018)&0x000000FF))
/** @} */

/**
 * @defgroup OS_SWAP_2X OS_SWAP_2X
 * @ingroup OS_SWAP
 * @{
 */

/**
 * Calling @ref OS_SWAP_2X swaps the highest byte with its previous byte and
 * the lowest byte with its next byte of a 32bit value. The macro swaps not
 * the complete 32bit value, it swaps two 16bit values.
 *
 * For example, the value 0x12345678 will be converted to 0x34127856.
 *
 * @param x  32bit Specifies value to be swapped.
 *
 * @return
 * The macro returns the swapped value.
 *
 * @sa
 * OS_SWAP_16 OS_SWAP_32
 */
#define OS_SWAP_2X(x)             ((((x)<<0x00000008)&0xFF00FF00)|\
                                   (((x)>>0x00000008)&0x00FF00FF))
/** @} */
/** @} */



/****************************************************************************************/
/**
 * @defgroup OS_BYTE_ORDER Byte Order Macros
 *
 * The macros converts values greater than one byte from from host to network
 * byte order or from network to host byte order. The conversion depends on
 * the machine (little-endian or big-endian).
 *
 * The macros are usually required by parsing or building PDUs.
 *
 *
 * Macros:
 *  - @ref OS_HTON_16  - convert from host to network byte order (16bit value)
 *  - @ref OS_HTON_32  - convert from host to network byte order (32bit value)
 *  - @ref OS_NTOH_16  - convert from network to host byte order (16bit value)
 *  - @ref OS_NTOH_32  - convert from network to host byte order (32bit value)
 */

#if (__PROCESSOR_FORMAT == 0)     /* little-endian */

/**
 * @defgroup OS_HTON_16 OS_HTON_16
 * @ingroup OS_BYTE_ORDER
 * @{
 */

/**
 * The @ref OS_HTON_16 macro swaps the highest byte with its previous byte and
 * the lowest byte with its next byte of a 32bit value. The macro swaps not
 * the complete 32bit value, it swaps two 16bit values. The network byte order
 * is always big-endian. The host byte order depends on the settings of the used
 * machine and is little-endian on a little-endian machine and big-endian on a
 * big-endian machine.
 *
 * For example, the value 0x12345678 will be converted to 0x34127856 on a
 * little-endian machine and will be not converted on a big-endian machine.
 *
 * @param x  32bit Specifies value to be swapped.
 *
 * @return
 * The macro returns the value in network byte order.
 *
 * @sa
 * OS_SWAP_16 OS_NTOH_16
 */
#define OS_HTON_16(x)             (uint16_t)OS_SWAP_16((uint16_t)x)

/** @} */

/**
 * @defgroup OS_HTON_2X OS_HTON_2X
 * @ingroup OS_BYTE_ORDER
 * @{
 */

/**
 * The @ref OS_HTON_2X macro converts a 32bit value from the host byte order
 * to the network byte order. The network byte order is always big-endian.
 * The host byte order depends on the settings of the used machine and is
 * little-endian on a little-endian machine and big-endian on a big-endian
 * machine.
 *
 * For example, the value 0x12345678 will be converted to 0x78563412 on a
 * little-endian machine and will be not converted on a big-endian machine.
 *
 * @param x  32bit Specifies value to be swapped.
 *
 * @return
 * The macro returns the value in network byte order.
 *
 * @sa
 * OS_SWAP_2X OS_NTOH_2X
 */
#define OS_HTON_2X(x)             (uint32_t)OS_SWAP_2X((uint32_t)x)

/** @} */

/**
 * @defgroup OS_HTON_32 OS_HTON_32
 * @ingroup OS_BYTE_ORDER
 * @{
 */

/**
 * The @ref OS_HTON_32 macro converts a 32bit value from the host byte order
 * to the network byte order. The network byte order is always big-endian.
 * The host byte order depends on the settings of the used machine and is
 * little-endian on a little-endian machine and big-endian on a big-endian
 * machine.
 *
 * For example, the value 0x12345678 will be converted to 0x78563412 on a
 * little-endian machine and will be not converted on a big-endian machine.
 *
 * @param x  32bit Specifies value to be swapped.
 *
 * @return
 * The macro returns the value in network byte order.
 *
 * @sa
 * OS_SWAP_32 OS_NTOH_32
 */
#define OS_HTON_32(x)             (uint32_t)OS_SWAP_32((uint32_t)x)

/** @} */

/**
 * @defgroup OS_NTOH_16 OS_NTOH_16
 * @ingroup OS_BYTE_ORDER
 * @{
 */

/**
 * If the processor format is little endian,
 * calling @ref OS_NTOH_16 converts the given
 * 16bit parameter network value (big endian network order)
 * to little endian format used by the host.
 * It uses the macro @ref OS_SWAP_16 for this conversion.
 * If the processor format is bit endian, this macro does nothing.
 * Example: value 0x3412 is made to 0x1234
 *
 * @param x  16bit value to be swapped
 *
 * @return
 * The macro returns the value in host byte order.
 *
 * @sa
 * OS_SWAP_16 OS_HTON_16
 */
#define OS_NTOH_16(x)             (uint16_t)OS_SWAP_16((uint16_t)x)

/** @} */

/**
 * @defgroup OS_HTON_2X OS_HTON_2X
 * @ingroup OS_BYTE_ORDER
 * @{
 */

/**
 * The @ref OS_HTON_2X macro converts a 32bit value from the network byte
 * order to the host byte order. The network byte order is always big-endian.
 * The host byte order depends on the settings of the used machine and is
 * little-endian on a little-endian machine and big-endian on a big-endian
 * machine.
 *
 * For example, the value 0x12345678 will be converted to 0x78563412 on a
 * little-endian machine and will be not converted on a big-endian machine.
 *
 * @param x  32bit Specifies value to be swapped.
 *
 * @return
 * The macro returns the value in host byte order.
 *
 * @sa
 * OS_SWAP_2X OS_HTON_2X
 */
#define OS_NTOH_2X(x)             (uint32_t)OS_SWAP_2X((uint32_t)x)

/** @} */

/**
 * @defgroup OS_NTOH_32 OS_NTOH_32
 * @ingroup OS_BYTE_ORDER
 * @{
 */

/**
 * If the processor format is little endian,
 * calling @ref OS_NTOH_32 converts the given
 * 32bit parameter network value (big endian network order)
 * to little endian format used by the host.
 * It uses the macro @ref OS_SWAP_32 for this conversion.
 * If the processor format is bit endian, this macro does nothing.
 * Example: value 0x78563412 is made to 0x12345678
 *
 * @param x  32bit value to be swapped
 *
 * @return
 * The macro returns the value in host byte order.
 *
 * @sa
 * OS_SWAP_32 OS_HTON_32
 */
#define OS_NTOH_32(x)             (uint32_t)OS_SWAP_32((uint32_t)x)

/** @} */

#elif (__PROCESSOR_FORMAT == 1)   /* big endian */

/* host byte order */
#define OS_HTON_16(x)             (uint16_t)(x)
#define OS_HTON_2X(x)             (uint32_t)(x)
#define OS_HTON_32(x)             (uint32_t)(x)

/* network byte order */
#define OS_NTOH_16(x)             (uint16_t)(x)
#define OS_NTOH_2X(x)             (uint32_t)(x)
#define OS_NTOH_32(x)             (uint32_t)(x)

#else
#error "Set endianess (__PROCESSOR_FORMAT) -> 0 == little endian, 1 == big endian!"
#endif

/** @} */



/****************************************************************************************/
/* OS Critical Section References */

/**
 * @defgroup os_lock Lock Section References
 *
 * The basic lock routines provide a mechanism to protect the access to specific
 * data. Only one source gets the access rights simultaneously.
 *
 * Usually the interrupts will be disabled globally, whenever the data section
 * will be entered. Now the process can not interrupted by another interrupt or
 * another thread. By leaving the data section the interrupts will be enabled
 * globally. Threads and interrupts can be scheduled now.
 *
 * In the user space of operating systems like MS Windows or LINUX the interrupts
 * will normally not enabled or disabled. In this case a simple lock mechanism
 * provided by the operating system shall be used.
 *
 * Routines:
 *  - @ref os_lock_enter Enter the lock section
 *  - @ref os_lock_leave Leave the lock section
 */

/**
 * @defgroup os_lock_enter os_lock_enter
 * @ingroup os_lock
 * @{
 */

/**
 * Depending on the operating system the @ref os_lock_enter routine waits for the
 * ownership or disables the interrupts to enter a data section. The data can be
 * modified without interruptions.
 *
 * If the data section is not locked by another context, the calling context will
 * enter this section now. Whenever the modifying context leaves the data section,
 * the @ref os_lock_leave routine shall be called.
 *
 * The routine can be called from any context (thread, interrupt routines).
 *
 * @sa
 * os_lock_leave
 */
void os_lock_enter (void);

/**
 * @}
 * @defgroup os_lock_leave os_lock_leave
 * @ingroup os_lock
 * @{
 */

/**
 * Depending on the operating system the @ref os_lock_leave routine releases the
 * ownership or enables the interrupts.
 *
 * @sa
 * os_lock_enter
 */
void os_lock_leave (void);

/**
 * @}
 */



/****************************************************************************************/
/* OS Critical Section References */

/**
 * @defgroup os_critical_section Critical Section References
 *
 * Critical sections provide synchronization similar to the functionality of mutex
 * objects. A section can be used to protect resources shared between separate threads
 * from being used simultaneously.
 *
 * Normally critical section objects provide a slightly faster, more efficient mechanism
 * for mutual-exclusion synchronization. But these objects can be used only by the
 * threads of a single process. Event, mutex, and semaphore objects can also be used in
 * a single-process application. Like a mutex object, a critical section object can be
 * owned by only one thread at a time, which makes it useful for protecting a shared
 * resource from simultaneous access. Unlike a mutex object, there is no way to tell
 * whether a critical section has been abandoned.
 *
 * A critical section object is a special resource of a subset of operating systems. The
 * functionality is not provided by each operating system. In this case the abstracted
 * routines uses the functionality of a real mutex object.
 *
 * Routines:
 *  - @ref os_critical_section_create Create critical section
 *  - @ref os_critical_section_destroy Destroy critical section
 *  - @ref os_critical_section_lock Lock critical section
 *  - @ref os_critical_section_unlock Unlock critical section
 */

/**
 * @defgroup os_critical_section_create os_critical_section_create
 * @ingroup os_critical_section
 * @{
 */

/**
 * The @ref os_critical_section_create routine initializes a critical section object, before it
 * can be used. A critical section which is no longer required should be cleaned up by
 * calling @ref os_critical_section_destroy.
 *
 * After a critical section object has been initialized, the threads of the process can
 * specify the object in the @ref os_critical_section_lock and the @ref os_critical_section_unlock
 * routine to provide mutually exclusive access to a shared resource. For similar
 * synchronization between the threads of different processes, use a mutex object.
 *
 * @return
 * A handle to the critical section object indicates success, NULL indicates failure.
 *
 * @sa
 * os_critical_section_destroy os_critical_section_lock os_critical_section_unlock
 */
handle_t os_critical_section_create (void);

/**
 * @}
 * @defgroup os_critical_section_destroy os_critical_section_destroy
 * @ingroup os_critical_section
 * @{
 */

/**
 * Any thread of the process can use the @ref os_critical_section_destroy routine to release the
 * system resources were allocated when the critical section object was initialized.
 * After this os_critical_section_destroy has been called, the critical section object can no
 * longer be used for synchronization.
 *
 * @param section     A pointer to the critical section object. The object must have
 *                    been previously initialized with the @ref os_critical_section_create
 *                    routine.
 * @return
 *
 * @sa
 * os_critical_section_create
 */
void os_critical_section_destroy (handle_t section);

/**
 * @}
 * @defgroup os_critical_section_lock os_critical_section_lock
 * @ingroup os_critical_section
 * @{
 */

/**
 * The @ref os_critical_section_lock routine waits for ownership of the specified critical
 * section object. A thread can call @ref os_critical_section_lock to request ownership of the
 * critical section before executing any section of code that accesses the protected
 * resource. The routine returns when the the calling thread is granted ownership.
 *
 * For each time a thread locked a critical section, the thread must call
 * @ref os_critical_section_unlock.
 *
 * A critical section which is not longer required should be cleaned up by calling
 * @ref os_critical_section_destroy.
 *
 * @param section     A pointer to the critical section object. The object must have
 *                    been previously initialized with the @ref os_critical_section_create
 *                    routine.
 * @return
 *
 * @sa
 * os_critical_section_create os_critical_section_destroy os_critical_section_unlock
 */
void os_critical_section_lock (handle_t section);

/**
 * @}
 * @defgroup os_critical_section_unlock os_critical_section_unlock
 * @ingroup os_critical_section
 * @{
 */

/**
 * A call to the @ref os_critical_section_unlock routine releases the ownership of the specified
 * critical section object acquired by a call to @ref os_critical_section_lock. For each time a
 * thread locked a critical section, the thread must call @ref os_critical_section_unlock. If a
 * thread calls @ref os_critical_section_unlock without having the ownership, an error occurs
 * that may cause another thread using @ref os_critical_section_lock to wait indefinitely.
 *
 * A critical section which is not longer required should be cleaned up by calling
 * @ref os_critical_section_destroy.
 *
 * @param section     A pointer to the critical section object. The object must have
 *                    been previously initialized with the @ref os_critical_section_create
 *                    routine.
 * @return
 *
 * @sa
 * os_critical_section_create os_critical_section_destroy os_critical_section_lock
 */
void os_critical_section_unlock (handle_t section);

/**
 * @}
 */



/****************************************************************************************/
/* OS Memory References */

/**
 * @defgroup os_memory Memory References
 *
 * The provided memory functionality of the operating system abstraction layer allows
 * using data memory dynamically. Use these routines of the abstraction layer to allocate
 * and release memory independent on the operating system.
 *
 * Routines:
 *  - @ref os_memory_alloc Allocate memory
 *  - @ref os_memory_dealloc Deallocate memory
 */

/**
 * @defgroup os_memory_alloc os_memory_alloc
 * @ingroup os_memory
 * @{
 */

/**
 * The @ref os_memory_alloc routine allocates a memory block of at least @e size bytes
 * from the standard memory heap. Maybe this heap is cached. The allocated block can be
 * larger than the required bytes because of space required for alignment and maintenance
 * information. It is not allowed to use the additional space. The routine guarantees
 * alignment.
 *
 * The return value of this routine has been always checked, even if the amount of memory
 * requested is small. If an allocated memory block is not longer required, a call to
 * @ref os_memory_dealloc has to be performed to avoid memory leaks.
 *
 * @param size    Bytes to allocate.
 *
 * @return
 * @ref os_memory_alloc returns a void pointer to the allocated space or NULL if there is
 * insufficient memory available. To return a pointer to a type other than void, use a
 * type cast on the return value.
 *
 * @sa
 * os_memory_dealloc
 */
void* os_memory_alloc (uint_t size);

/**
 * @}
 * @defgroup os_memory_dealloc os_memory_dealloc
 * @ingroup os_memory
 * @{
 */

/**
 * The @ref os_memory_dealloc routine deallocates a memory block specified by @e pMem
 * that was previously allocated by a call to @ref os_memory_alloc. The number of freed
 * bytes is equivalent to the number of bytes requested when the block was allocated. If
 * @e pMem is NULL, the pointer is ignored and the routine immediately returns.
 *
 * @param mem     Previously allocated memory block to be deallocated.
 *
 * @return
 *
 * @sa
 * os_memory_alloc
 */
void os_memory_dealloc (void* mem);

/**
 * @}
 */



/****************************************************************************************/
/* OS Mutual Exclusion References */

/**
 * @defgroup os_mutex Mutual Exclusion References
 *
 * Mutexes are resources of the operating system to let threads share resources safely.
 * Using a data structure with no locking, the system may run without problems for quite
 * some time, but anytime the data structure will become inconsistent and the system
 * behavior strangely.
 *
 * Mutexes serve as a mutual exclusion mechanism between threads, and cannot be used to
 * synchronize between threads and the interrupt handling subsystem.
 *
 * To use the functionality of mutexes independent on an operating system the mutex
 * specific routines, constants, types and function prototypes of the abstraction layer
 * interface has to be use.
 *
 * Routines:
 *  - @ref os_mutex_create Create mutex
 *  - @ref os_mutex_destroy Destroy mutex
 *  - @ref os_mutex_lock Lock mutex
 *  - @ref os_mutex_unlock Unlock mutex
 */

/**
 * @defgroup os_mutex_create os_mutex_create
 * @ingroup os_mutex
 * @{
 */

/**
 * A call to the @ref os_mutex_create routine initializes a mutex before this object can
 * be used. If the mutex is not longer required, the @ref os_mutex_destroy routine can be
 * used. The @ref os_mutex_create routine are normally called during initialization but
 * may also be called from thread context.
 *
 * The main functions for using a mutex are @ref os_mutex_lock and @ref os_mutex_unlock
 * to lock and to unlock a critical section.
 *
 * @param name    Specifies the name of the mutex object. If @e name is NULL, the
 *                mutex object is created without a name.
 *
 * @return
 * A handle to the mutex object indicates success, NULL indicates failure.
 *
 * @sa
 * os_mutex_destroy os_mutex_lock os_mutex_unlock
 */
handle_t os_mutex_create (void);

/**
 * @}
 * @defgroup os_mutex_destroy os_mutex_destroy
 * @ingroup os_mutex
 * @{
 */

/**
 * If a mutex created by a call to @ref os_mutex_create is no longer required and there
 * are no threads waiting on it then the @ref os_mutex_destroy routine can be used.
 *
 * @param mutex   Specifies the reference to the mutex created by a successful call to
 *                @ref os_mutex_create.
 *
 * @return
 *
 * @sa
 * os_mutex_create os_mutex_lock os_mutex_unlock
 */
void os_mutex_destroy (handle_t mutex);

/**
 * @}
 * @defgroup os_mutex_lock os_mutex_lock
 * @ingroup os_mutex
 * @{
 */

/**
 *
 * To lock a critical section a call to the @ref os_mutex_lock routine has to be
 * performed. In normal operation the routine will return success after claiming the
 * mutex lock. If another threads currently owns the mutex, the call to
 * @ref os_mutex_lock blocks.
 *
 * The current owner of a mutex should call @ref os_mutex_unlock, when a lock is no
 * longer required. This operation must be performed by the owner, not by another
 * thread.
 *
 * @param mutex   Specifies the reference to the mutex created by a successful call to
 *                @ref os_mutex_create.
 *
 * @sa
 * os_mutex_create os_mutex_destroy os_mutex_unlock
 */
void os_mutex_lock (handle_t mutex);

/**
 * @}
 * @defgroup os_mutex_unlock os_mutex_unlock
 * @ingroup os_mutex
 * @{
 */

/**
 * A thread releases the ownership of the mutex by call to the @ref os_mutex_unlock
 * routine. The call should be performed, when a lock is no longer required. Only the
 * thread entered a critical section by a call to @ref os_mutex_lock must be unlock this
 * section.
 *
 * @param mutex   Specifies the reference to the mutex created by a successful call to
 *                @ref os_mutex_create.
 *
 * @return
 *
 * @sa
 * os_mutex_create os_mutex_destroy os_mutex_lock
 */
void os_mutex_unlock (handle_t mutex);

/**
 * @}
 */



/****************************************************************************************/
/* OS Semaphore References */

/**
 * @defgroup os_semaphore Semaphore References
 *
 * Counting semaphores are a synchronization primitive that allow threads to wait until
 * an event has occurred. A counter associated with a semaphore keeps track of the number
 * of events that have not yet been processed. The state of the semaphore is
 * non-signaled, when this counter is zero. A consumer thread waiting on the semaphore
 * will block until another thread posts a new event to the semaphore. The semaphore's
 * counter will be incremented. The state is set to signaled, if the counter is greater
 * than zero. A thread waiting on the semaphore will consume one event. The semaphore's
 * counter will be decremented at this moment.
 *
 * Typically, an application uses a semaphore to limit the number of threads using a
 * resource. The counter would correspond to how many of a certain type of resource are
 * currently available. A thread waits on the semaphore to claim a resource or posts the
 * semaphore to release the resource again.
 *
 * To use the functionality of semaphores independent on an operating system the
 * semaphore specific routines, constants, types and function prototypes of the
 * abstraction layer interface has to be use.
 *
 * Routines:
 *  - @ref os_semaphore_create Create semaphore
 *  - @ref os_semaphore_destroy Destroy semaphore
 *  - @ref os_semaphore_post Post semaphore event
 *  - @ref os_semaphore_wait Wait for semaphore event
 */

/**
 * @defgroup os_semaphore_create os_semaphore_create
 * @ingroup os_semaphore
 * @{
 */
/**
 * Creating a counting semaphore will be performed by a call to the
 * @ref os_semaphore_create routine. The initial counter value specified in the
 * @e countInit parameter can be any number, zero or positive, but typically a value of
 * zero is used to indicate that no events have occurred yet. In this case the semaphore
 * is non-signaled.
 *
 * Each time a waiting thread is released because of the semaphores signaled state, the
 * counter of the semaphore is decreased by 1. A next call to @ref os_semaphore_wait
 * a thread will be suspended, if the count is zero. A thread can call
 * @ref os_semaphore_post to increment the semaphore's count.
 *
 * The semaphore remains in the system until it will be removed by a call to
 * @ref os_semaphore_destroy.
 *
 * @param count_init  Specifies an initial count for the semaphore. This value must be
 *                    greater than or equal to zero and less than or equal to @e countMax.
 * @param count_max   Specifies the maximum count for the semaphore object. This value
 *                    must be greater than zero.
 *
 * @return
 * A reference to the created semaphore indicates success. NULL indicates failure.
 *
 * @sa
 * os_semaphore_delete os_semaphore_post os_semaphore_wait
 */
handle_t os_semaphore_create (uint_t count_init, uint_t count_max);

/**
 * @}
 * @defgroup os_semaphore_destroy os_semaphore_destroy
 * @ingroup os_semaphore
 * @{
 */

/**
 * The @ref os_semaphore_destroy routine deletes a counting semaphore which has been
 * created by a call to @ref os_semaphore_create.
 *
 * @param semaphore   Specifies the reference to the semaphore returned by a successful
 *                    call to @ref os_semaphore_create.
 *
 * @return
 *
 * @sa
 * os_semaphore_create
 */
void os_semaphore_destroy (handle_t semaphore);

/**
 * @}
 * @defgroup os_semaphore_post os_semaphore_post
 * @ingroup os_semaphore
 * @{
 */

/**
 * A thread uses the @ref os_semaphore_post routine when an event has occured. The
 * operation increments the counter and releases a thread waiting on the semaphore. The
 * waked up thread continues running and the @ref os_semaphore_wait routine decrements
 * the counter again before returning.
 *
 * @param semaphore   Specifies the reference to the semaphore returned by a successful
 *                    call to @ref os_semaphore_create.
 *
 * @return
 *
 * @sa
 * os_semaphore_create os_semaphore_wait
 */
void os_semaphore_post (handle_t semaphore);

/**
 * @}
 * @defgroup os_semaphore_wait os_semaphore_wait
 * @ingroup os_semaphore
 * @{
 */

/**
 * The @ref os_semaphore_wait routine is used by a consumer thread to wait for an event.
 * The semaphore's state is signaled, if its counter is greater than zero. An event has
 * already occurred in the past. In this case the counter will be decremented and the
 * call will return immediately. Otherwise the current thread will be blocked until there
 * is a @ref os_semaphore_post call.
 *
 * @param semaphore   Specifies the reference to the semaphore returned by a successful
 *                    call to @ref os_semaphore_create.
 *
 * @return
 *
 * @sa
 * os_semaphore_create os_semaphore_post
 */
void os_semaphore_wait (handle_t semaphore);

/**
 * @}
 */



/****************************************************************************************/
/* OS Thread References */

/**
 * @defgroup os_thread Thread References
 *
 * Threads are resources of an operating system to execute code. Additional threads are
 * created to wait for several events to occur so another threads are not blocked.
 *
 * To use the functionality of threads independent on an operating system the thread
 * specific routines, constants, types and function prototypes of the abstraction layer
 * interface has to be use.
 *
 * Routines:
 *  - @ref os_thread_create Create thread
 *  - @ref os_thread_destroy Destroy thread
 *  - @ref os_thread_exit Terminate thread for itself
 *
 * Prototypes:
 *  - @ref os_thread_routine_t Thread execution routine
 */


/**
 * @defgroup os_thread_routine_t os_thread_routine_t
 * @ingroup os_thread
 * @{
 */

/**
 * The type specifies the function prototype of the thread execution routine. The routine
 * is an application-defined routine, which serves as the starting address for a thread.
 * The address has to be specified by calling the @ref os_thread_create routine.
 *
 * @param pParam    specifies the thread data passed to the function using the
 *                  @e user parameter of the @ref os_thread_create routine
 *
 * @return The thread execution routine should return a value that indicates its success
 *         or failure.
 *
 * @sa
 * os_thread_create
 */
typedef uint32_t (* os_thread_routine_t) (void* user);

/**
 * @}
 * @defgroup os_thread_create os_thread_create
 * @ingroup os_thread
 * @{
 */

/**
 * The @ref os_thread_create routine creates a thread to execute within the address space
 * specified for a new thread. The thread execution begins at the routine of type
 * @ref os_thread_routine_t specified by the @e routine parameter. If this
 * execution routine returns, the return value is used to terminate the thread in an
 * implicit call to the @ref os_thread_exit routine. If the start address is invalid when
 * the thread runs, an exception occurs, and the thread terminates.
 *
 * The thread remains in the system until the thread has terminated and all references to
 * it has been closed. A call to the @ref os_thread_destroy removes a created thread and
 * releases the thread resources.
 *
 * @param stack_size  Specifies the size of the virtual memory reserved for the thread.
 * @param prio        Specifies the priority value. Valid values are 0-255 with 0 having
 *                    the highest priority
 * @param routine     Points to the user-defined routine of type
 *                    @ref os_thread_routine_t to be executed by the thread. This
 *                    routine represents the starting address of the thread.
 * @param user        Points to user specific data passed to the thread.
 *
 * @return
 * A reference to the created thread indicates success. NULL indicates failure.
 *
 * @remark
 * The number of threads can create is limited by the available memory.
 *
 * @sa
 * os_thread_create_name os_thread_destroy os_thread_exit os_thread_routine_t
 */
handle_t os_thread_create (uint_t stack_size, uint_t prio, os_thread_routine_t routine, void* user);

/**
 * The @ref os_thread_create_name routine creates a thread to execute within the address space
 * specified for a new thread. The thread execution begins at the routine of type
 * @ref os_thread_routine_t specified by the @e routine parameter. If this
 * execution routine returns, the return value is used to terminate the thread in an
 * implicit call to the @ref os_thread_exit routine. If the start address is invalid when
 * the thread runs, an exception occurs, and the thread terminates.
 *
 * The thread remains in the system until the thread has terminated and all references to
 * it has been closed. A call to the @ref os_thread_destroy removes a created thread and
 * releases the thread resources.
 *
 * Additional to the @ref os_thread_create routine the @ref os_thread_create_name routine
 * allows to created a thread with a name provided by the argument @ref name.
 *
 * @param stack_size  Specifies the size of the virtual memory reserved for the thread.
 * @param prio        Specifies the priority value. Valid values are 0-255 with 0 having
 *                    the highest priority
 * @param routine     Points to the user-defined routine of type
 *                    @ref os_thread_routine_t to be executed by the thread. This
 *                    routine represents the starting address of the thread.
 * @param user        Points to user specific data passed to the thread.
 * @param name        Specifies the name of the thread.
 *
 * @return
 * A reference to the created thread indicates success. NULL indicates failure.
 *
 * @remark
 * The number of threads can create is limited by the available memory.
 *
 * @sa
 * os_thread_create os_thread_destroy os_thread_exit os_thread_routine_t
 */
handle_t os_thread_create_name (uint_t size, uint_t prio, os_thread_routine_t routine, void* user, char *name);


/**
 * @}
 * @defgroup os_thread_destroy os_thread_destroy
 * @ingroup os_thread
 * @{
 */

/**
 * The @ref os_thread_destroy routine destroys a thread. The thread object remains in the
 * system until the thread has terminated and all references to the thread object are
 * closed. A shutdown sequence has to be initiated to terminate the thread in an implicit
 * call to the @ref os_thread_exit routine. The @ref os_thread_destroy routine does not
 * return as long the thread is still active.
 *
 * @note
 * The routine does not terminate a thread. A thread will be terminated whenever the
 * execution routine of type @ref os_thread_routine_t returns.
 *
 * @param thread  Specifies the reference to the thread created by a successful call to
 *                @ref os_thread_create.
 *
 * @return
 *
 * @sa
 * os_thread_create os_thread_exit os_thread_routine_t
 */
void os_thread_destroy (handle_t thread);

/**
 * @}
 * @defgroup os_thread_exit os_thread_exit
 * @ingroup os_thread
 * @{
 */

/**
 * The @ref os_thread_exit allows a thread to terminate itself, thus ensuring that it
 * will not be run again by the scheduler. When this routine is called explicitly or by
 * returning from the thread execution procedure, the thread terminates. The state of
 * the thread object becomes signaled.
 *
 * The thread remains in use by the operating system and the reference returned by the
 * @ref os_thread_create routine is still valid. The thread should be deleted by a call
 * to the @ref os_thread_destroy routine, after the thread has exited and is no longer
 * required.
 *
 * @param exit_code      Specifies the exit code for the calling thread.
 *
 * @return
 *
 * @sa
 * os_thread_create os_thread_destroy os_thread_routine_t
 */
void os_thread_exit (uint_t exit_code);

/**
 * @}
 */



/****************************************************************************************/
/* OS Timer References */

/**
 * @defgroup os_timer Timer References
 *
 * Timers are resources of an operating system to schedule an event for an application
 * after a specified time has elapsed. For each event (specified interval or time-out
 * value) a timer elapses, the operating system notifies the application associated with
 * the timer. Because a timer's accuracy depends on the system clock rate, the time-out
 * value is only approximate.
 *
 * To use the functionality of timers independent on an operating system the timer
 * specific routines, constants, types and function prototypes of the abstraction layer
 * interface has to be use.
 *
 * Routines:
 *  - @ref os_timer_activate_periodical Activate timer
 *  - @ref os_timer_activate_onetime Activate timer
 *  - @ref os_timer_deactivate Deactivate timer
 *
 *  Prototypes:
 *  - @ref os_timeout_routine_t Time-out event routine
 */

/**
 * @defgroup os_timeout_routine_t os_timeout_routine_t
 * @ingroup os_timer
 * @{
 */

/**
 * The type specifies the function prototype of the time-out routine. The routine is an
 * user-defined callback routine, which is called once on expiration of a single event or
 * periodically on expiration of periodic events.
 *
 * @param user    Specifies a reference to user specific data passed to the function
 *                using the @e user parameter of @ref os_timer_activate_periodical.
 *
 * @return
 *
 * @sa os_timer_activate_periodical os_timer_activate_onetime
 *
 */
typedef void (* os_timeout_routine_t) (handle_t user);

/**
 * @}
 * @defgroup os_timer_activate_periodical os_timer_activate_periodical
 * @ingroup os_timer
 * @{
 */

/**
 * Calling @ref os_timer_activate_periodical activates a timer. This timer starts timing the
 * interval as soon as it is activated. For each event the elapsed timer notifies the
 * user by using the registered callback routine. The timer runs until stopped by calling
 * @ref os_timer_stop. Reactivation after a stop is done via @ref os_timer_restart_onetime or
 * @ref os_timer_restart_periodical.
 *
 * A timer which is not longer required can be destroyed by using the
 * @ref os_timer_deactivate routine.
 *
 * @param user     Specifies the timer related reference of the user.
 * @param timeout  Specifies the time-out value, in milliseconds. If this value is
 *                 not in the range of the minimum and maximum event delays supported
 *                 by the timer, the function returns an error.
 * @param routine  Pointer to a callback routine which is called once on
 *                 expiration of a single event or periodically on expiration of
 *                 periodic events. The parameter is interpreted as a function
 *                 pointer. The prototype is specified at
 *                 @ref os_timeout_routine_t.
 *
 * @return
 * A reference to the timer indicates success. NULL indicates failure.
 *
 * @sa
 * os_timer_deactivate os_timer_stop
 */
handle_t os_timer_activate_periodical (handle_t user, uint_t timeout, os_timeout_routine_t routine);

/**
 * @}
 * @defgroup os_timer_activate_onetime os_timer_activate_onetime
 * @ingroup os_timer
 * @{
 */

/**
 * Calling @ref os_timer_activate_onetime activates a timer. This timer starts timing the
 * interval as soon as it is activated. The timer is set to expire only once. For each event
 * the elapsed timer notifies the user by using the registered callback routine. After expiring
 * the timer is stopped and can be restarted by calling @ref os_timer_restart_onetime or
 * @ref os_timer_restart_periodical.
 *
 * A timer which is not longer required can be destroyed by using the
 * @ref os_timer_deactivate routine.
 *
 * @param user     Specifies the timer related reference of the user.
 * @param timeout  Specifies the time-out value, in milliseconds. If this value is
 *                 not in the range of the minimum and maximum event delays supported
 *                 by the timer, the function returns an error.
 * @param routine  Pointer to a callback routine which is called once on
 *                 expiration of a single event or periodically on expiration of
 *                 periodic events. The parameter is interpreted as a function
 *                 pointer. The prototype is specified at
 *                 @ref os_timeout_routine_t.
 *
 * @return
 * A reference to the timer indicates success. NULL indicates failure.
 *
 * @sa
 * os_timer_deactivate os_timer_restart_onetime os_timer_restart_periodical
 */
handle_t os_timer_activate_onetime (handle_t user, uint_t timeout, os_timeout_routine_t routine);

/**
 * @}
 * @defgroup os_timer_deactivate os_timer_deactivate
 * @ingroup os_timer
 * @{
 */

/**
 * A call to the @ref os_timer_deactivate deactivates a timer. The timer stops timing the
 * interval immediately. After the routine returns a time-out event will not be notified
 * anymore. The timer can't be reactivated after calling this function.
 *
 * @param timer_h      Specifies the timer related reference of the timer.
 *
 * @return
 *
 * @see
 * os_timer_activate_periodical os_timer_activate_onetime
 */
void os_timer_deactivate (handle_t timer_h);

/**
 * @}
 * @defgroup os_timer_stop os_timer_stop
 * @ingroup os_timer
 * @{
 */

/**
 * A call to the @ref os_timer_stop stops timing the interval immediately. After the
 * routine returns a time-out event will not be notified anymore.
 *
 * @param timer_h      Specifies the timer related reference of the timer.
 *
 * @return
 *
 * @see
 * os_timer_activate_periodical os_timer_activate_onetime
 */
void os_timer_stop (handle_t timer_h);

/**
 * @}
 * @defgroup os_timer_restart_periodical os_timer_restart_periodical
 * @ingroup os_timer
 * @{
 */

/**
 * Calling @ref os_timer_restart_periodical activates a timer. This timer starts timing the
 * interval as soon as it is activated. For each event the elapsed timer notifies the
 * user by using the registered callback routine @ref os_timer_activate_periodical.
 *
 * A timer which is not longer required can be destroyed by using the
 * @ref os_timer_deactivate routine.
 *
 * @param timer_h  Specifies the timer related reference of the timer.
 * @param timeout  Specifies the time-out value, in milliseconds. If this value is
 *                 not in the range of the minimum and maximum event delays supported
 *                 by the timer, the function returns an error.
 *
 * @return
 * A reference to the timer (parameter @e ref) indicates success. NULL indicates failure.
 *
 * @sa
 * os_timer_deactivate os_timer_activate_periodical
 */
handle_t os_timer_restart_periodical (handle_t timer_h, uint_t timeout);

/**
 * @}
 * @defgroup os_timer_restart_onetime os_timer_restart_onetime
 * @ingroup os_timer
 * @{
 */

/**
 * Calling @ref os_timer_restart_onetime restarts a timer for a single shot after which it
 * is stopped again. The timer starts timing the interval as soon as it is activated. For
 * each event the elapsed timer notifies the user by using the registered callback routine
 * @ref os_timer_activate_onetime.
 *
 * A timer which is not longer required can be destroyed by using the
 * @ref os_timer_deactivate routine.
 *
 * @param timer_h  Specifies the timer related reference of the timer.
 * @param timeout  Specifies the time-out value, in milliseconds. If this value is
 *                 not in the range of the minimum and maximum event delays supported
 *                 by the timer, the function returns an error.
 *
 * @return
 * A reference to the timer (parameter @e ref) indicates success. NULL indicates failure.
 *
 * @sa
 * os_timer_deactivate os_timer_activate_onetime
 */
handle_t os_timer_restart_onetime (handle_t timer_h, uint_t timeout);

/**
 * @}
 */



/****************************************************************************************/
/* OS Time References */

/**
 * @defgroup os_time Time References
 *
 *
 *
 * Routines:
 *  - @ref os_time_get_time Get the time of the system
 */

/**
 * @defgroup os_time_get_time os_time_get_time
 * @ingroup os_time
 * @{
 */

/**
 * Calling @ref os_time_get_time returns the value of a continuously running
 * time counter starting at 0 on system startup.
 *
 * The maximum counter value of the continuous counting tick clock is 4,294,967,294
 * (e.g. 2^32-2). The clock automatically resets to 0 on the tick after the maximum value
 * is reached.
 * As the parameter values are directly calculated from the tick value
 * of the system clock, values @e time_s and @e time_us are both reseted 0 at the same time.
 * The user has to consider this when calculating time differences from the clock values.
 *
 * @param time_s   Points to an address to store the seconds of the time value.
 * @param time_us  Points to an address to store the microseconds of the time value.
 */
void os_time_get_time (uint32_t* time_s, uint32_t* time_us);

/**
 * @}
 */


/****************************************************************************************/
/* end of header */
#endif /* OS_API_H_ */
