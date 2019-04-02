#ifndef D_OsRessource_H
#define D_OsRessource_H

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

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include "OsCommon.h"

#include <stdint.h>
#include <stdio.h>

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

#define MAX_OS_RESSOURCE_NAME_LENGTH (32U)

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

#define OS_RESSOURCE_SET_NAME(res, format, ...)\
   BEGIN\
      char tempName[MAX_OS_RESSOURCE_NAME_LENGTH + 1];\
      snprintf(tempName, sizeof(tempName), format, __VA_ARGS__);\
      OsRessource_SetName(&res, tempName);\
   END

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------

typedef struct stOsRessource
{
   char Name[MAX_OS_RESSOURCE_NAME_LENGTH + 1]; //!< +1 for the zero termination.
   u32 Id;
} tOsRessource;

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

void OsRessource_SetName(tOsRessource *ressource,
                         const char* name);

void OsRessource_GetName(tOsRessource *ressource,
                         size_t bufferLength,
                         char *name);

#endif  // D_OsRessource_H
