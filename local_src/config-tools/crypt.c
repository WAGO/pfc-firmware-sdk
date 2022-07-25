//------------------------------------------------------------------------------
/// Copyright (c) 2000 - 2022 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     crypt.c
///
///  \version  $Revision: 65689 $1
///
///  \brief    
///
///  \author   Stefanie Meihöfer : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <crypt.h>

#include "config_tool_lib.h"

//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// external functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------


int main(int    argc, 
         char** argv)
{
  int   status     = SUCCESS;
  
  // help-text requested?
  if((argc == 2) && ((strcmp(argv[1], "--help") == 0) || strcmp(argv[1], "-h") == 0))
  {
    printf("\n* Get key by crypt-routine *\n\n");
    printf("Usage: crypt <password> <salt>\n\n");
  }
  else
  {
    // check count of parameter
    if(argc < 3)
    {
      status = MISSING_PARAMETER;
    }
    else
    {
      char* key = NULL;
      //printf("argv:%s %s\n", argv[1], argv[2]);
      key = crypt(argv[1], argv[2]);
      printf("%s", key);
    }
  }

  return(status);
}
