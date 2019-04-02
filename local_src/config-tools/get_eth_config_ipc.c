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
///  \file     get_eth_config_pfc200.c
///
///  \version  $Revision: 6731 $1
///
///  \brief    
///
///  \author   AGa : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#if 0

/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
*/

int GetDeviceId(char* pPortString, 
                char* pSearchString,
                int   searchLineNr,
                char* pOutputString)
//
// Get get the device-id of a specified interface, according to processor-type.
//
// input: pPortString: input-string for the requested port (eth0 or eth1)
//        pSearchString: not used
//        searchLineOffset: not used
// 
// output: string with the device-id (X8 or X9) - must be allocated by calling function!
//         empty string if device-id can't be discovered
//
{
  char* pProcessorType = NULL;

  // check input-parameter
  if((pPortString == NULL) || (pOutputString == NULL))
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  // get processor-type by systemcall
  pProcessorType = SystemCall_GetOutput("/etc/config-tools/get_coupler_details processor-type 2> /dev/null");

  // set device-id according to processor-type and requested interface
  if(pProcessorType != NULL)
  {
    char *ethPortID0 = "";
    char *ethPortID1 = "";

    if( 0 == strcmp(pProcessorType, GEODE) )
    {
      ethPortID0 = "X9";
      ethPortID1 = "X8";
    }
    else if ( 0 == strcmp(pProcessorType, ARM_PAC200) )
    {
      ethPortID0 = "X1";
      ethPortID1 = "X2";
    }
    else // Pentium/Celeron
    {
      ethPortID0 = "X8";
      ethPortID1 = "X9";
    }

    if ( 0 == strcmp(pPortString, ETH0) )
    {
      strncpy(pOutputString, ethPortID0, MAX_LENGTH_ETH_CONFIG_STRING);
    }
    else if ( 0 == strcmp(pPortString, ETH1) )
    {
      strncpy(pOutputString, ethPortID1, MAX_LENGTH_ETH_CONFIG_STRING);
    }

    SystemCall_Destruct(&pProcessorType);
  }

  return(SUCCESS);
}

int GetInterfacesParameter(char* pPortString,
                           char* pSearchString,
                           int   searchLineOffset,
                           char* pOutputString)
//
// Get a parameter which is delivered from file interfaces.
//
// input: pPortString: input-string for the requested port (eth0 or eth1)
//        pSearchString: the string to search for in the interfaces-file, directly in front of the searched parameter
//        searchLineOffset: relavite number of line after config-type definition which includes requested parameter in interfaces-file
// 
// output: string with the requested parameter - must be allocated by calling function!
//         empty string if parameter can't be found
//
{
  int   status                 = SUCCESS;
  char* pInterfacesFileContent = NULL;

  // check input-parameter
  if((pPortString == NULL) || (pSearchString == NULL) || (pOutputString == NULL))
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  *pOutputString = '\0';

  // get content of fieldbus-info-file
  pInterfacesFileContent = FileContent_Get("/etc/network/interfaces");

  if(pInterfacesFileContent == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    char  interfacesLine[MAX_LENGTH_OUTPUT_LINE]  = "";
    int   configTypeLineNr                        = 0;
    char* pFoundString                            = NULL;

    // get the number of line where the declaration of static eth-configuration starts
    configTypeLineNr = GetConfigTypeLineNr(pInterfacesFileContent, pPortString, CONFIG_TYPE_STATIC);

    // add line-offset to get the line which includes the searched parameter 
    configTypeLineNr += searchLineOffset;

    // if search-string is interface, replace it with the real interface
    if(strcmp(pSearchString, "eth") == 0) snprintf(pSearchString, MAX_LENGTH_OUTPUT_LINE, "%s ", pPortString);

    // read the line which should include the searched parameter
    if(SUCCESS == SystemCall_GetLineByNr(pInterfacesFileContent, configTypeLineNr, interfacesLine))
    {
      // look after search-string
      pFoundString = strstr(interfacesLine, pSearchString);
      if(pFoundString != NULL)
      {
        // set pointer behind search-string (start of parameter), cut the string behind parameter and copy it to output-variable
        pFoundString = pFoundString + strlen(pSearchString);
        CutWord(pFoundString, MAX_LENGTH_ETH_CONFIG_STRING);
        strncpy(pOutputString, pFoundString, MAX_LENGTH_ETH_CONFIG_STRING);
      }
    }

    FileContent_Destruct(&pInterfacesFileContent);
  }

  return(SUCCESS);
}

int CheckValidInterfaceArg(const char *arg)
{
  return !strcmp(arg, ETH0) || !strcmp(ETH1);
}

#endif
