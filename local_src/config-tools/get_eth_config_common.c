//------------------------------------------------------------------------------
/// Copyright (c) 2000 - 2013 WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     get_eth_config.c
///
///  \version  $Revision: 6731 $1
///
///  \brief    
///
///  \author   Stefanie Meihöfer : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#include "config_tool_lib.h"

//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

#ifdef  __CPPUTEST__ 
#define main __FILE__##main
#define ShowHelpText __FILE__##ShowHelpText
#endif

// show errors on screen? Normally not, because they would be also displayed e.g. on WBM
#define SHOW_ERRORS                     0

// maximum length of the string to search for in the system-call output
#define MAX_LENGTH_SEARCH_STRING        40

// config-type-strings
#define CONFIG_TYPE_STATIC              "static"
#define CONFIG_TYPE_DHCP                "dhcp"
#define CONFIG_TYPE_BOOTP               "bootp"


//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

extern int GetDeviceId(char* pPortString, 
                       char* pSearchString,
                       int   searchLineNr,
                       char* pOutputString);

extern int GetState(char* pPortString, 
                    char* pSearchString,
                    int   searchLineNr,
                    char* pOutputString);

extern int GetEthPortParameter(char* pPortString, 
                               char* pSearchString,
                               int   searchLineNr,
                               char* pOutputString);

extern int CheckValidInterfaceArg(const char *pPortString);
//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------

// Pointer to funktion to search the requested parameter
typedef int (*tFktGetParameter)(char* pPortString, 
                                char* pSearchString,
                                int   searchLineOffset,
                                char* pOutputString);

// struct to join the possible input-strings with the processing-function to get them
typedef struct
{
  // input-string from command-line
  char              inputString[MAX_LENGTH_INPUT_STRING];

  // string to search for in the output of the sytem-function
  char              searchString[MAX_LENGTH_SEARCH_STRING];

  // number of relative line after config-type definition which includes requested parameter in interfaces-file
  int               searchLineOffset;

  // processing function to get the requested parameter
  tFktGetParameter  pFktGetParameter;

} tstParameterJumptab;


// struct to join the possible config-type-strings with the strings to search for
typedef struct
{
  char              inputString[MAX_LENGTH_INPUT_STRING];
  char              searchString[MAX_LENGTH_SEARCH_STRING];

} tstConfigTypeSearchAssign;

//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------

// function to get a parameter provided by ifconfig
int GetInterfacesParameter(char* pPortString, 
                           char* pSearchString,
                           int   searchLineNr,
                           char* pOutputString);

// function to get the config-type of interface (static, bootp or dhcp)
int GetConfigType(char* pPortString, 
                  char* pSearchString,
                  int   searchLineNr,
                  char* pOutputString);

// function to get the device-id of interface
int GetDeviceId(char* pPortString, 
                char* pSearchString,
                int   searchLineNr,
                char* pOutputString);

// function to get the state of default-gateway of interface
int GetDefaultGatewayState(char* pPortString, 
                           char* pSearchString,
                           int   searchLineNr,
                           char* pOutputString);

// function to get a string with the ethernet speed- and duplex-values
int GetSpeedAndDuplex(char* pPortString, 
                      char* pSearchString,
                      int   searchLineNr,
                      char* pOutputString);

// function to show a selection-menu with all possible eth-speed-values
int PrintSpeedSelection(char* pPortString, 
                        char* pSearchString,
                        int   searchLineNr,
                        char* pOutputString);

// function to show a selection-menu with all possible eth-duplex-values
int PrintDuplexSelection(char* pPortString, 
                         char* pSearchString,
                         int   searchLineNr,
                         char* pOutputString);

// function to show a selection-menu with all possible autoneg-speed-values
int PrintAutonegSelection(char* pPortString, 
                          char* pSearchString,
                          int   searchLineNr,
                          char* pOutputString);

// function to show a selection-menu with all possible ethernet-speed- and -duplex-combinations
int PrintSpeedDuplexSelection(char* pPortString, 
                              char* pSearchString,
                              int   searchLineNr,
                              char* pOutputString);


// array of all possible requested parameter (input-strings, strings to search for, processing-function to get them)
static tstParameterJumptab astParameterJumptab[] =
{
  { "ip-address",               "eth",          1, GetInterfacesParameter },
  { "subnet-mask",              "netmask ",     1, GetInterfacesParameter },
  { "broadcast",                "broadcast ",   1, GetInterfacesParameter },
  { "default-gateway",          "gw ",          2, GetInterfacesParameter },
  { "default-gateway-state",    "gw ",          2, GetDefaultGatewayState }, 
  { "default-gateway-netmask",  "netmask ",     2, GetInterfacesParameter },
  { "config-type",              "" ,            0, GetConfigType },
  { "state",                    "",             0, GetState },
  { "device-id",                "",             0, GetDeviceId },
  { "speed",                    "speed ",       5, GetInterfacesParameter },
  { "duplex",                   "duplex ",      5, GetInterfacesParameter },
  { "autoneg",                  "autoneg ",     5, GetInterfacesParameter },
  { "speed-duplex",             "",             0, GetSpeedAndDuplex },
  { "speed-selection",          "",             0, PrintSpeedSelection },
  { "duplex-selection",         "",             0, PrintDuplexSelection },
  { "autoneg-selection",        "",             0, PrintAutonegSelection },
  { "speed-duplex-selection",   "",             0, PrintSpeedDuplexSelection },

  // this line must always be the last one - don't remove it!
  { "", "", 0, NULL }
};

static tstConfigTypeSearchAssign astConfigTypeSearchAssign[] =
{
  { CONFIG_TYPE_STATIC, "up sudo /sbin/ifconfig" },
  { CONFIG_TYPE_DHCP,   "up /sbin/udhcpc -i" },
  { CONFIG_TYPE_BOOTP,  "up /sbin/bootpc-startup" },

  // this line must always be the last one - don't remove it!
  { "", "" }
};

//------------------------------------------------------------------------------
// external functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------


void ShowHelpText(void)
//
// Show describtion and usage of program on stdout
//
{
  int parameterIndex = 0;

  printf("\n* Get the interface-parameters configured by file interfaces *\n\n");
  printf("Usage: get_eth_config <interface> <parameter> [parameter-value]\n\n");
  printf("interface: %s | %s\n", ETH0, ETH1);
  printf("parameter:");

  // read the possible strings for parameter from array and show them like this: "param1 | param2 | param3"
  while(strlen(astParameterJumptab[parameterIndex].inputString) > 0)
  {
    if(parameterIndex != 0) printf("| ");
    printf("%s ", astParameterJumptab[parameterIndex].inputString);
    ++parameterIndex;
  }

  printf("\n");
  printf("parameter-value: output is \"checked\", if the result is the same as the given string (else no output).\n");
  //printf("                 This is useful for the display of the result in the context of a checkbox on a html-page.\n");
  printf("\n");
}


int GetConfigTypeLineNr(char* pInterfacesFileContent,
                        char* pPortString,
                        char* pConfigTypeString)
//
// Get number of the line, in which the declaration for a specified port and config-type starts in interfaces-file
//
// input: pInterfacesFileContent: content of interfaces-file in a buffer
//        pPortString: string which describes the interface (eth0 or eth1)
//        pConfigTypeString: string which describes the requested config-type (static, dhcp or bootp)
//
// return: requested line-number; 0 if config-type/port can't be found
//
{
  int foundLineNr = 0;

  // check input-parameter
  if((pPortString != NULL) && (pInterfacesFileContent != NULL) && (pConfigTypeString != NULL))
  {
    char  interfacesLine[MAX_LENGTH_OUTPUT_LINE]  = "";
    char  searchString[MAX_LENGTH_OUTPUT_LINE]    = "";
    int   testLineNr                              = 0;

    // evaluate which search string belongs to the specified config-type-string by looping across the assignment-table
    int   searchTabIndex                          = 0;
    while((strlen(astConfigTypeSearchAssign[searchTabIndex].inputString) > 0) && (strlen(searchString) == 0))
    {
      if(strcmp(pConfigTypeString, astConfigTypeSearchAssign[searchTabIndex].inputString) == 0)
      {
        snprintf(searchString, sizeof(searchString), "%s %s", astConfigTypeSearchAssign[searchTabIndex].searchString, pPortString);
      }
      ++searchTabIndex;
    }

    // loop over the lines of interfaces-file to find the line with the config-type-declaration
    testLineNr = 1;
    while((foundLineNr == 0) && (SUCCESS == SystemCall_GetLineByNr(pInterfacesFileContent, testLineNr, interfacesLine)))
    {   
      // if the line with the model-name-string was found -> memorize line-number
      if(strstr(interfacesLine, searchString) != NULL)
      {
        foundLineNr = testLineNr;
      }
      ++testLineNr;
    }

    // if line with config-type-declaration was found, look if the previos line (= first line of declaration) is also existing
    if(foundLineNr > 0)
    {
      int status = SUCCESS;

      // compose search-string with the requested port
      snprintf(searchString, sizeof(searchString), "iface %s inet manual", pPortString);
      --foundLineNr;

      // if the first line of declaration was not found -> reset result-line-number to "not found"
      status = SystemCall_GetLineByNr(pInterfacesFileContent, foundLineNr, interfacesLine);
      if((status != SUCCESS) || (strstr(interfacesLine, searchString) == NULL))
      {
        foundLineNr = 0;
      }
    }
  }

  return(foundLineNr);
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

  // Hook for different platform-specific handling of autoneg/speed/duplex etc.
  status = GetEthPortParameter(pPortString, pSearchString, searchLineOffset, pOutputString);

  if(SUCCESS == status)
  {
    return status;
  }

  // initialise output-string
  sprintf(pOutputString, "");

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



int GetDefaultGatewayState(char* pPortString,
                           char* pSearchString,
                           int   searchLineOffset,
                           char* pOutputString)
//
// Get state of default-gateway of one interface.
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
  sprintf(pOutputString, "");

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
        // if the found default-gateway-line is not commented out, fill the output-string
        if(interfacesLine[0] != '#')
        {
          strncpy(pOutputString, "enabled", MAX_LENGTH_ETH_CONFIG_STRING);
        }
        else
        {
          strncpy(pOutputString, "disabled", MAX_LENGTH_ETH_CONFIG_STRING);
        }

      }
    }

    FileContent_Destruct(&pInterfacesFileContent);
  }

  return(SUCCESS);
}



int GetConfigType(char* pPortString, 
                  char* pSearchString,
                  int   searchLineNr,
                  char* pOutputString)
//
// Get get the config-type (static, bootp or dhcp) for a specified interface.
//
// input: pPortString: input-string for the requested port (eth0 or eth1)
//        pSearchString: not used
//        searchLineOffset: not used
// 
// output: string with the config-tpye - must be allocated by calling function!
//         empty string if config-type can't be discovered
//
{
  int   status                  = SUCCESS; 
  char* pInterfacesFileContent  = NULL;

  // check input-parameter
  if((pPortString == NULL) || (pOutputString == NULL))
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  // get content of fieldbus-info-file
  pInterfacesFileContent = FileContent_Get("/etc/network/interfaces");

  if(pInterfacesFileContent == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    // loop over all config-types which are described in the config-type-assignment-table, stop if the actually used one was found
    int configTypeTabIndex = 0;
    while((strlen(pOutputString) == 0) && (strlen(astConfigTypeSearchAssign[configTypeTabIndex].inputString) > 0))
    {
      char  configTypeLine[MAX_LENGTH_OUTPUT_LINE]  = "";
      int   configTypeLineNr                        = 0;

      // get the number of line where the declaration of actual config-type eth-configuration starts
      configTypeLineNr = GetConfigTypeLineNr(pInterfacesFileContent, pPortString,
                                             astConfigTypeSearchAssign[configTypeTabIndex].inputString);

      // if (line-number of) declaration was found, read the according line
      if((configTypeLineNr > 0) && (SUCCESS == SystemCall_GetLineByNr(pInterfacesFileContent, configTypeLineNr, configTypeLine)))     
      {
        // check yet if line is no commented out 
        if(configTypeLine[0] != COMMENT_CHAR)
        {
          // success! actual tested config-type is the one which is actually used -> copy the type to output-string
          strncpy(pOutputString, astConfigTypeSearchAssign[configTypeTabIndex].inputString, MAX_LENGTH_ETH_CONFIG_STRING);
        }
      }

      ++configTypeTabIndex;
    }

    FileContent_Destruct(&pInterfacesFileContent);
  }

  return(SUCCESS);
}



int GetSpeedAndDuplex(char* pPortString, 
                      char* pSearchString,
                      int   searchLineNr,
                      char* pOutputString)
//
// Get a string with the configuration of ethernet-speed and -duplex
//
// input: pPortString: input-string for the requested port (eth0 or eth1)
//        pSearchString, searchLineNr: not used
// output: string with the configuration-data - must be allocated by calling function!
//         empty string if configuration can't be discovered
//
{
  int   status = SUCCESS;
  char  speedString[MAX_LENGTH_ETH_CONFIG_STRING] = "";
  char  duplexString[MAX_LENGTH_ETH_CONFIG_STRING] = "";

  if((pPortString == NULL) || (pOutputString == NULL))
  {
    return(INVALID_PARAMETER);
  }

  if(   (SUCCESS != GetInterfacesParameter(pPortString, "speed ", 5, speedString))
     || (SUCCESS != GetInterfacesParameter(pPortString, "duplex ", 5, duplexString)))
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    snprintf(pOutputString, MAX_LENGTH_ETH_CONFIG_STRING, "%s/%s", speedString, duplexString);
  } 

  return(status);
}



int PrintSelectionMenu(char* pItemString,
                       char* pActualValueString,
                       char* paSelectionStrings[])
//
// Create a selection-menu for html-page (size: one line) with the several possible values of one configuration-data.
// The menu-line with the actual data-value will be showed selected. If the actual value does not represend one of
// the given possibilities, an extra line with this value will be created.
//
// Input: pItemString: selection-menu-name
//        pActualValueString: string with the actual value of data
//        paSelectionStrings: array with the several possible strings of data-values.
//
{
  int stringNr  = 0;
  int found     = 0;

  if((pActualValueString == NULL) || (paSelectionStrings == NULL))
  {
    return(INVALID_PARAMETER);
  }

  // show introducing line of selection-menu
  printf("\n              <select name=\"%s\" size=\"1\">\n", pItemString);
  stringNr = 0;
  while(strlen(paSelectionStrings[stringNr]) > 0)
  {
    // show single line of menu, depending on if the actual loop value is the real actual speed-value
    if(strcmp(paSelectionStrings[stringNr], pActualValueString) != 0)
    {
      printf("                <option value=\"%s\">%s</option>\n", 
                              paSelectionStrings[stringNr], paSelectionStrings[stringNr]);
    }
    else
    {
      printf("                <option selected value=\"%s\">%s</option>\n", 
                              paSelectionStrings[stringNr], paSelectionStrings[stringNr]);
      found = 1;
    }
    ++stringNr;
  }

  // show an extra line, if the real acutal value is not included in our list of possible values
  if(found == 0)
  {
    printf("                <option selected value=\"%s\">%s</option>\n", pActualValueString, pActualValueString);
  }

  printf("              </select>\n");

  return(SUCCESS);
}


int PrintSpeedSelection(char* pPortString,
                        char* pSearchString,
                        int   searchLineNr,
                        char* pOutputString)
//
// Create a selection-menu for html-page with the several possible values of ethernet-speed.
// The actual value will be showed selected.
//
{
  char* aSpeedSelectionStrings[MAX_LENGTH_ETH_CONFIG_STRING] =
  {
    "10",
    "100",
    ""
  };

  char  actualValueString[MAX_LENGTH_ETH_CONFIG_STRING] = "";

  if((pPortString == NULL) || (pOutputString == NULL))
  {
    return(INVALID_PARAMETER);
  }

  GetInterfacesParameter(pPortString, "speed ", 5, actualValueString);
  PrintSelectionMenu("speed", actualValueString, aSpeedSelectionStrings);

  return(SUCCESS);
}


int PrintDuplexSelection(char* pPortString, 
                         char* pSearchString,
                         int   searchLineNr,
                         char* pOutputString)
//
// Create a selection-menu for html-page with the several possible values of ethernet-duplex.
// The actual value will be showed selected.
//
{
  char* aDuplexSelectionStrings[MAX_LENGTH_ETH_CONFIG_STRING] =
  {
    "half",
    "full",
    ""
  };

  char  actualValueString[MAX_LENGTH_ETH_CONFIG_STRING] = "";

  if((pPortString == NULL) || (pOutputString == NULL))
  {
    return(INVALID_PARAMETER);
  }

  GetInterfacesParameter(pPortString, "duplex ", 5, actualValueString);
  PrintSelectionMenu("duplex", actualValueString, aDuplexSelectionStrings);

  return(SUCCESS);
}


int PrintAutonegSelection(char* pPortString,
                          char* pSearchString,
                          int   searchLineNr,
                          char* pOutputString)
//
// Create a selection-menu for html-page with the several possible values of ethernet-autoneg.
// The actual value will be showed selected.
//
{
  char* aAutonegSelectionStrings[MAX_LENGTH_ETH_CONFIG_STRING] =
  {
    "on",
    "off",
    ""
  };

  char  actualValueString[MAX_LENGTH_ETH_CONFIG_STRING] = "";

  if((pPortString == NULL) || (pOutputString == NULL))
  {
    return(INVALID_PARAMETER);
  }

  GetInterfacesParameter(pPortString, "autoneg ", 5, actualValueString);
  PrintSelectionMenu("autoneg", actualValueString, aAutonegSelectionStrings);

  return(SUCCESS);
}


int PrintSpeedDuplexSelection(char* pPortString, 
                              char* pSearchString,
                              int   searchLineNr,
                              char* pOutputString)
//
// Create a selection-menu for html-page with the several possible values of ethernet-speed and -duplex.
// The line with the actual values will be showed selected.
//
{
  char* aSpeedSelectionStrings[MAX_LENGTH_ETH_CONFIG_STRING] =
  {
    "10",
    "100",
    ""
  };

  char* aDuplexSelectionStrings[MAX_LENGTH_ETH_CONFIG_STRING] =
  {
    "half",
    "full",
    ""
  };

  char  actualSpeedString[MAX_LENGTH_ETH_CONFIG_STRING] = "";
  char  actualDuplexString[MAX_LENGTH_ETH_CONFIG_STRING] = "";

  int status = SUCCESS;

  if((pPortString == NULL) || (pOutputString == NULL))
  {
    return(INVALID_PARAMETER);
  }

  if(  (SUCCESS != GetInterfacesParameter(pPortString, "speed ", 5, actualSpeedString))
    || (SUCCESS != GetInterfacesParameter(pPortString, "duplex ", 5, actualDuplexString)))
  {
    status = FILE_READ_ERROR;
    printf("error\n");
  }
  else
  {
    printf("\n              <select name=\"speed-duplex\">\n");

    int speedStringNr = 0;
    while(strlen(aSpeedSelectionStrings[speedStringNr]) > 0)
    {
      int duplexStringNr = 0;
      while(strlen(aDuplexSelectionStrings[duplexStringNr]) > 0)
      {
        // show single line of menu, depending on if the actual loop value is the real actual speed-value
        if(   (strcmp(aDuplexSelectionStrings[duplexStringNr], actualDuplexString) != 0)
           || (strcmp(aSpeedSelectionStrings[speedStringNr], actualSpeedString) != 0))
        {
          printf("                <option value=\"%s-%s\">%s MBit %s duplex</option>\n", 
                                  aSpeedSelectionStrings[speedStringNr], aDuplexSelectionStrings[duplexStringNr],
                                  aSpeedSelectionStrings[speedStringNr], aDuplexSelectionStrings[duplexStringNr]);
        }
        else
        {
          printf("                <option selected value=\"%s-%s\">%s MBit %s duplex</option>\n", 
                                  aSpeedSelectionStrings[speedStringNr], aDuplexSelectionStrings[duplexStringNr],
                                  aSpeedSelectionStrings[speedStringNr], aDuplexSelectionStrings[duplexStringNr]);
        }
        ++duplexStringNr;
      }
      ++speedStringNr;
    }

    printf("              </select>\n");
  }

  return(SUCCESS);
}



int main(int    argc, 
         char** argv)
{
  int   status            = SUCCESS;

  // help-text requested?
  if((argc == 2) && ((strcmp(argv[1], "--help") == 0) || strcmp(argv[1], "-h") == 0))
  {
    ShowHelpText();
  }
  else
  {
    // check if the count of parameters is valid
    if(argc < 3)
    {
      status = MISSING_PARAMETER;
    }
    else
    {
      // check if the single parameters are valid
      char* pPortString       = argv[1];
      char* pParameterString  = argv[2];
      char* pValueString      = NULL;

      if(argc > 3) pValueString = argv[3];

      if(!CheckValidInterfaceArg(pPortString)) 
      {
        status = INVALID_PARAMETER;
      }
      else
      {
        // check if parameter is contained in parameter-list and get its index 
        int  parameterIndex = 0;

        status = INVALID_PARAMETER;
        while((strlen(astParameterJumptab[parameterIndex].inputString) > 0) && (status == INVALID_PARAMETER))
        {
          // success - parameter-string is equal to one of our list
          if(strcmp(astParameterJumptab[parameterIndex].inputString, pParameterString) == 0)
          {
            status = SUCCESS;
          }
          else
          {
            ++parameterIndex;
          }
        }

        // parameter was fount -> start processing (get the fitting function from parameter-list)
        if(status == SUCCESS)
        {
          char outputString[MAX_LENGTH_ETH_CONFIG_STRING] = "";
          status = astParameterJumptab[parameterIndex].pFktGetParameter(pPortString, astParameterJumptab[parameterIndex].searchString,
                                                                        astParameterJumptab[parameterIndex].searchLineOffset, outputString);

          // if a comparison with a special value should be made for html-page, do it and set output-string accordingly
          if(pValueString != NULL)
          {
            if(strncmp(pValueString, outputString, sizeof(outputString)) == 0)
            {
              strncpy(outputString, "checked", sizeof(outputString));
            }
            else
            {
              strncpy(outputString, "", sizeof(outputString));
            }
          }

          // send result to stdout
          printf("%s", outputString);
        }
      }
    }
  }

  #if SHOW_ERRORS
  if(status != SUCCESS)
  {
    ShowErrorText(status, "");
    ShowHelpText();
  }
  #endif

  return(status);
}
