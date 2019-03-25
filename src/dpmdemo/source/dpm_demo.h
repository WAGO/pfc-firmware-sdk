//------------------------------------------------------------------------------
/// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     dp_master.h
///
///  \version  $Revision: 17216 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef DP_MASTER_H_
#define DP_MASTER_H_


//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <stdint.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

// error-numbers
#define SUCCESS                       0
#define ERROR                        -1


typedef const char* string;


typedef struct stPlcDevice
{
   string  *DeviceName;
   int32_t DeviceId;

}tPlcDevice;

typedef struct stPlcDevices
{
   size_t DeviceCount;
   tPlcDevice Devices[10];
}tPlcDevices;

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

void sig_handler(int signo);
void *safe_malloc(size_t size);


//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

#define foreach(iter, list) for(__typeof__(list) iter = list; *iter != NULL; ++iter)
/**
 * Removes compiler warnings about unused parameters.
 */
#define UNUSED_PARAMETER(X) ((void)X)
/**
 * Gets the number of elements of an array.
 */
#define ELEMENT_COUNT(pX) (sizeof((pX))/sizeof(*(pX)))

#define ZERO_MEMORY(pX) memset((pX), 0, sizeof(*(pX)))




#define LINT_SUPPRESS(nr, text) /*lint --e{nr} -save suppress text */
#define LINT_RESTORE() /*lint -restore */

//

#endif /* DP_MASTER_H_ */
//---- End of source file ------------------------------------------------------
