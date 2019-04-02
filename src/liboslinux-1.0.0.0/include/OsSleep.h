#ifndef D_OsSleep_H
#define D_OsSleep_H

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
/// \version <Revision>:
///
/// \brief short description of the file contents
///
/// \author ${user} : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <stdint.h>

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


//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------

/**
 * Sleeps value seconds.
 * @param value Duration in seconds
 */
void OsSleep_s(u32 duration_s);

/**
 * Sleeps duration_ms milli seconds.
 * @param duration_ms
 */
void OsSleep_ms(u32 duration_ms);

/**
 * Sleeps duration_us micro seconds.
 * @param duration_us
 */
void OsSleep_us(u32 duration_us);

#endif  // D_OsSleep_H
