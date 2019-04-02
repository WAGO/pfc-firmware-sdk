//------------------------------------------------------------------------------
/// Copyright (c) 2000 - 2006 WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     get_filesystem_data_pfc200.c
///
///  \version  $Revision: 6731 $1
///
///  \brief    
///
///  \author   AGa : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//#if 0

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

//linux style data types are used in pac-startup.h
typedef uint8_t u8;

#include <pac_boot_table.h>
#include <pac-startup.h>

#include "config_tool_lib.h"

char *g_pac_eeprom ="/sys/class/i2c-adapter/i2c-2/2-0054/eeprom";

int GetSecondActiveDevice(char* pOutputString,
                          int   additionalParam)
//
// Get the string with the active NAND partition
//
// input: ignored
//
// output: string with the active NAND partition
//         - must be allocated by calling function!
//
// return: error-code
//
{
  int   status                                                = SUCCESS;
  char* pMenuLstFileContent                                   = NULL;

  FILE *eepromFp;

  UNUSED_PARAMETER(additionalParam);

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  *pOutputString = '\0';

   
	eepromFp = fopen(g_pac_eeprom, "rb");

  if(eepromFp > 0)
  {
    
    struct pac_eeprom_xload_section_hdr xsectionHeader;
    int ret = fread(&xsectionHeader, 1, sizeof(xsectionHeader), eepromFp);

    if (sizeof(xsectionHeader) == ret)
    { 

      if(xsectionHeader.boot_mode_id & PAC_BOOT_RECOVER)
      {
        sprintf(pOutputString, "%d", 2); // 2nd root partition
      }
      else
      {
        sprintf(pOutputString, "%d", 1); // 1st root partition
      }
    }
    else
    {
      status = SYSTEM_CALL_ERROR;
    }

    fclose(eepromFp);
  }
  else
  {
    status = SYSTEM_CALL_ERROR;
  }

  return(status);
}

