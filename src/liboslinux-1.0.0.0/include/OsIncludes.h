#ifndef __OS_INCLUDES__H
#define __OS_INCLUDES__H

#if _MSC_VER >= 1000
  #ifndef CIFX_DRIVER
    #include "windows.h"
  #else
    #pragma warning (disable: 4214 4201)
    #include "windef.h"
  #endif
#else

/*  #error "TODO: Define all neccessary typedefinitions, and add include files"*/

/* The following types need to be defined

  APIENTRY
  UINT32
*/
#ifndef APIENTRY
	#define APIENTRY
#endif

#include <inttypes.h>

typedef u32 UINT32;

#ifndef NULL
 #define NULL 0
#endif

#endif

#ifndef UNREFERENCED_PARAMETER
  #define UNREFERENCED_PARAMETER(a) (a=a)
#endif

#endif /* __OS_INCLUDES__H */
