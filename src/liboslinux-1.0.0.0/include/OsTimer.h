//------------------------------------------------------------------------------
/// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
/// manufacturing, reproduction, use, and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \file ${file_name}
///
/// \version <Revision>: $Rev: 5624 $
///
/// \brief short description of the file contents
///
/// \author ${user} $Author$ : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef D_OsTimer_H
#define D_OsTimer_H

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <inttypes.h>
#include <stdbool.h>
#include <sys/cdefs.h>

#include "OsTime.h"
#include "OsTypedefs.h"
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

typedef struct stOsTimer tOsTimer;
typedef void (*fnTimerCallback)(tOsTimer* timer, void *user);

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------


tOsTimer* OsTimer_Create(fnTimerCallback callback, void const *userData);

void OsTimer_Destroy(tOsTimer* self);


i32 OsTimer_StartInterval(tOsTimer * self, tOsTime *first, tOsTime* interval);

i32 OsTimer_Start(tOsTimer *self, tOsTime *value);


#endif  // D_OsTime_H
