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
 *  $Workfile: os_api_win32.h $
 *
 * Last Modification:
 *  $Author: u010391 $
 *  $Modtime: 20.04.2011 $
 *  $Revision: 65684 $
 */
#ifndef OS_SPECIFIC_H_
#define OS_SPECIFIC_H_


/****************************************************************************************/
/* operating system */


/* LINUX Systems */
#ifdef LINUX

#include <stdint.h>                         /* standard */
#include <stdio.h>                          /* standard */
#include <stdlib.h>                         /* standard */
#include <assert.h>                         /* standard */
#include <string.h>                         /* standard */

typedef char                      tchar_t;        /**< system dependent character type */
typedef int                       boolean_t;      /**< boolean (true/false) */
typedef int                       bool_t;         /**< bool (true/false) */
typedef int                       int_t;          /**< integer */
typedef unsigned int              uint_t;         /**< unsigned integer */
typedef char                      char_t;         /**< signed char type (8bit) */
typedef unsigned char             uchar_t;        /**< unsigned char type */
typedef short                     short_t;        /**< signed short type (16bit min.) */
typedef unsigned short            ushort_t;       /**< unsigned short type (16bit min.) */
typedef long                      long_t;         /**< signed long type (32bit min.) */
typedef unsigned long             ulong_t;        /**< unsigned long type (32bit min.) */
typedef void*                     handle_t;       /**< handle, reference */
typedef unsigned long             status_t;       /**< status */

# ifndef false
#  define false 0
# endif

# ifndef true
#  define true (!false)
# endif

/* ecos */
#elif defined ECOS

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>                         /* standard */
#include <string.h>                         /* standard */

#ifndef _SYS_BSDTYPES_H_                    /* maybe BSD types are included */
#define _SYS_BSDTYPES_H_

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_arch.h>

#define __BIT_TYPES_DEFINED__
#undef uint8_t
#undef uint16_t
#undef uint32_t
#undef int8_t
#undef int16_t
#undef int32_t

typedef __signed char      int8_t;
typedef unsigned char    u_int8_t;
typedef unsigned char     uint8_t;
typedef short       int16_t;
typedef unsigned short    u_int16_t;
typedef unsigned short     uint16_t;
typedef int       int32_t;
typedef unsigned int    u_int32_t;
typedef unsigned int     uint32_t;
typedef long long     int64_t;
typedef unsigned long long  u_int64_t;
typedef unsigned long long   uint64_t;

// Types inherited from HAL

typedef CYG_ADDRESS               vaddr_t;
typedef CYG_ADDRWORD              vsize_t;
typedef CYG_ADDRESS               paddr_t;
typedef CYG_ADDRWORD              psize_t;

typedef CYG_ADDRESS           vm_offset_t;
typedef CYG_ADDRWORD            vm_size_t;

// No good HAL definition for this

typedef CYG_ADDRWORD           register_t;


// From <arch/ansi.h>
/*
 * Types which are fundamental to the implementation and may appear in
 * more than one standard header are defined here.  Standard headers
 * then use:
 *  #ifdef  _BSD_SIZE_T_
 *  typedef _BSD_SIZE_T_ size_t;
 *  #undef  _BSD_SIZE_T_
 *  #endif
 */
#define _BSD_SSIZE_T_ int    /* byte count or error */
#define _BSD_CLOCKID_T_ int
#define _BSD_TIMER_T_ int
#ifndef __time_t_defined                 // As defined/used by eCos libc
#define _BSD_CLOCK_T_ cyg_int64  /* clock() */
#define _BSD_TIME_T_  cyg_count32  /* time() */
#endif

#endif /* _SYS_BSDTYPES_H_ */

typedef char                      tchar_t;
typedef int                       int_t;
typedef unsigned int              uint_t;
typedef void*                     handle_t;
typedef unsigned long             status_t;



/* MS Windows Systems */
#elif defined WIN32

#ifdef (__WIN32__ || __NT__)                /* MS Windows 95 line or MS Windows NT line */
#include <stdint.h>                         /* standard */
#include <stdio.h>                          /* standard */
#include <stdlib.h>                         /* standard */
#include <assert.h>                         /* standard */
#include <string.h>                         /* standard */

#else                                       /* FIXME: MINGW32 ? */
#error "There is no compatible operating system!"
#endif


/* NUCLEUS systems */
#elif defined (__NUCLEUS__)

#include <stdint.h>                         /* standard */
#include <stdio.h>                          /* standard */
#include <stdlib.h>                         /* standard */
#include <stdbool.h>                        /* standard */
#include <stdarg.h>                         /* standard */
#include <assert.h>                         /* standard */
#include <string.h>                         /* standard */

typedef char                      tchar_t;        /**< system dependent character type */
typedef bool                      boolean_t;      /**< boolean (true/false) */
typedef bool                      bool_t;         /**< bool (true/false) */
typedef int                       int_t;          /**< integer */
typedef unsigned int              uint_t;         /**< unsigned integer */
typedef void*                     handle_t;       /**< handle, reference */
typedef unsigned long             status_t;       /**< status */

/* SYS/BIOS systems */
#elif defined (__SYSBIOS__)

	#include <stdint.h>                         /* standard */
	#include <stdio.h>                          /* standard */
	#include <stdlib.h>                         /* standard */
	#include <stdbool.h>                        /* standard */
	#include <stdarg.h>                         /* standard */
	#include <assert.h>                         /* standard */
	#include <string.h>                         /* standard */
    #include <stddef.h>

	typedef char                      tchar_t;        /**< system dependent character type */
	typedef bool                      boolean_t;      /**< boolean (true/false) */
	typedef bool                      bool_t;         /**< bool (true/false) */
	typedef int                       int_t;          /**< integer */
	typedef unsigned int              uint_t;         /**< unsigned integer */
	typedef void*                     handle_t;       /**< handle, reference */
	typedef unsigned long             status_t;       /**< status */

/* another operating systems */
#else
#error "The selected operating system is not supported!"
#endif


/****************************************************************************************/
/* end of header */
#endif /* OS_SPECIFIC_H_ */
