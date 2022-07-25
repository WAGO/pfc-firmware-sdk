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
///  \file     get_rts3scfg_value.c
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

#include "config_tool_lib.h"

//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

#define SHOW_ERRORS                         0

// filename of CoDeSys-configfile
#define RTS3S_CFG_FILENAME                  "/etc/rts3s.cfg"

#define MAX_LENGTH_CONFIG_FILE_LINE         200
#define SEPERATOR_SIGN                      '='

#define MAX_LENGTH_HEADLINE_STRING          100
#define MAX_LENGTH_PARAMETER_VALUE          200



//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------

// array with a variable count of strings for the parameters and their values
typedef struct
{
  char* strings[MAX_LENGTH_PARAMETER_VALUE];

} tStringList;


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------

// array with a list of strings; each includes at first the name of the parameter itself, then the variable count of parameter-values
// and at least an empty string (important!) to mark that this is the end of the value list
static tStringList astParameterValues[] = 
{
  { "State",          "Enabled", "Disabled", "" },
  { "Interface",      "/dev/ttyS0", "/dev/ttyS1", "" },
  { "Baud",           "2400", "4800", "9600", "19200", "38400", "57600", "115200", "" },
  { "Data_Bits",      "7", "8", "" },
  { "Parity",         "None", "Odd", "Even", "" },
  { "Stop_Bits",      "1", "2", "" },
  { "Flow_control",   "None", "SW", "HW", "" },

  // this line must always be the last one - don't remove it!
  { "" }

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

  printf("\n* Get values from the rts3s-config-file *\n\n");
  printf("Usage: get_rts3scfg_value <headline-string> <parameter> [parameter-value | \"selection\"]\n\n");

  printf("headline-string: headline of configuration-group in rts3scfg-file which includes the requested parameter\n");
  printf("without the squared brackets. E.g. RS232, MODBUS_UDP, MODBUS_TCP, MODBUS_RTU, ...\n");

  printf("\n");
  printf("parameter: identifire of the requested parameter, exactly like it is used in rts3s.cfg. E.g. Node_Id, Timeout, ...\n");
  printf("Exeption: to get the state, \"state\" must be given as parameter (not \"Enabled\" like it is used in rts3s.cfg)\n");

  printf("\n\n");
  printf("parameter-value: output is \"checked\", if the result is the same as the given string (else no output).\n");
  //printf("                 This is useful for the display of the result in the context of a checkbox on a html-page.\n");
  printf("\"selection\": delivers the html-code of a selection-menu including all possible or expedient values of this parameter.\n");
  printf("\n");
}



int GetValueFromConfigFile(char* pHeadlineString,
                           char* pParameterString,
                           char* pParameterValueString)
//
// Get a parmeter-value from rts3s-config-file.
//
// input: pHeadlineString: String with the headline above a parameter-group (e.g. [MODBUS_RTU])
//        pParameterString: String with the requested parameter, exactly like it is used in config-file (e.g. Node_id, Timeout)
//
// output: pParameterValueString: string with the value of the requested parameter, must be allocated from calling function!
//
// return: error-code
//
{
  int   status              = SUCCESS;
  char* pConfigFileContent  = NULL;

  if((pHeadlineString == NULL) || (pParameterString == NULL) || (pParameterValueString == NULL))
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pParameterValueString, "");

  // get content of fieldbus-info-file
  pConfigFileContent = FileContent_Get("/etc/rts3s.cfg");

  if(pConfigFileContent == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    char  configFileLine[MAX_LENGTH_OUTPUT_LINE]  = "";
    int   lineNr                                  = 0;

    status = NOT_FOUND;

    // loop over the lines of config-file until we find the line with the headline
    lineNr = 1;
    while(   (SUCCESS == FileContent_GetLineByNr(pConfigFileContent, lineNr, configFileLine))
          && (strstr(configFileLine, pHeadlineString) == NULL))
    { 
      ++lineNr;
    }

    // if headline was found
    if(strlen(configFileLine) >  0)
    {
      // search for the requested paramter in the following config-line-block (until the next empty line)
      while(   (SUCCESS == FileContent_GetLineByNr(pConfigFileContent, lineNr, configFileLine)) 
            && (strlen(configFileLine) > 0) 
            && (strlen(pParameterValueString) == 0))
      { 
        // if parameter-string was found, filter parameter value behind it (seperatet with '=')
        if(strstr(configFileLine,pParameterString) != NULL)
        {
          strncpy(pParameterValueString, strchr(configFileLine, SEPERATOR_SIGN) + 1, MAX_LENGTH_PARAMETER_VALUE);
          CutWord(pParameterValueString, MAX_LENGTH_PARAMETER_VALUE);
          status = SUCCESS;
        }
        ++lineNr;
      }
    }

    FileContent_Destruct(&pConfigFileContent);
  }

  return(status);
}
        


int PrintSelectionMenu(char* pParameterString, 
                       char* pParameterValueString)
//
// Show the html-code of a selection-menu with all possible values of the specified parameter. The actual configured value is showed 
// selected. The values of the several parameter were stored in the global array astParameterValues
//
// input: pParameterString: parameter the selection-menu should be created about
//        pParameterValueSTring: actual value of the parameter, which should be showed selected
//
// return: error-code
//
{
  int status      = SUCCESS;
  int parameterNr = 0;

  // check input-parameter
  if((pParameterString == NULL) || (pParameterValueString == NULL))
  {
    return(INVALID_PARAMETER);
  }

  // loop over all possible parameter-strings, stop if the requested parameter was found
  status      = NOT_FOUND;
  parameterNr = 0;
  while((strlen(astParameterValues[parameterNr].strings[0]) > 0) && (status == NOT_FOUND))
  {      
    if(strcmp(astParameterValues[parameterNr].strings[0], pParameterString) == 0)
    {
      status = SUCCESS;
    }
    else
    {
      ++parameterNr;
    }
  }

  // if we found the requested parameter (and the value-string-list for it)
  if(status == SUCCESS)
  {
    int valueNr = 0;
    int found   = FALSE;

    // start selection-menu
    printf("\n              <select name=\"%s\" size=\"1\">\n", pParameterString);

    // loop over all possible values for this parameter from value-string-list and show them as lines in the selection-menu
    valueNr = 1;
    while(strlen(astParameterValues[parameterNr].strings[valueNr]) > 0)
    {
      // if the value-string of the actual menu-line represents the actual configured value - show menu-line selected, else normal
      if(strcmp(pParameterValueString, astParameterValues[parameterNr].strings[valueNr]) == 0)
      {
        printf("                <option selected value=\"%s\" >%s</option>\n", astParameterValues[parameterNr].strings[valueNr],          
                                                                               astParameterValues[parameterNr].strings[valueNr]);
        found = 1;
      }
      else
      {
        printf("                <option value=\"%s\">%s</option>\n", astParameterValues[parameterNr].strings[valueNr],
                                                                     astParameterValues[parameterNr].strings[valueNr]); 
      }
      ++valueNr;
    }

    // if the actual configured value was not included in the value-list - create an extra (selected) menu-line with text "Unknown"
    if(found == FALSE)
    {
      printf("                <option selected value=\"Unknown\" >Unknown</option>\n");
    }

    // close selection-menu
    printf("              </select>\n\n");
  }

  return(status);
}



int main(int    argc, 
         char** argv)
{
  int   status = SUCCESS;

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
      // check if second argument (headline-string) is too long
      if((strlen(argv[1]) > MAX_LENGTH_CONFIG_FILE_LINE - 2) || (strlen(argv[2]) > MAX_LENGTH_CONFIG_FILE_LINE))
      {
        status = INVALID_PARAMETER;
      }
      else
      {
        char  headlineString[MAX_LENGTH_CONFIG_FILE_LINE]       = "";
        char  parameterString[MAX_LENGTH_CONFIG_FILE_LINE]      = "";
        char  parameterValueString[MAX_LENGTH_CONFIG_FILE_LINE] = "";
        char* pRequestedValue                                   = NULL;

        // build headline-string (add squared brackets) and copy string of the requested parameter
        sprintf(headlineString, "[%s]", argv[1]);
        sprintf(parameterString, "%s", argv[2]);

        // if parameter is "state", look for the word "Enabled" instead of "state"
        if(strcmp(parameterString, "state") == 0)
        {
          sprintf(parameterString, "Enabled");
        }

        // if a third parameter is given (= id for selection-menu or requested parameter to see if a checkbox must be checked), take it
        if(argc >= 3)
        {
          pRequestedValue = argv[3];
        }
        //printf("headlineString:%s\nparameterString:%s\nrequestedValue:%s\n", headlineString, parameterString, pRequestedValue);

        status = GetValueFromConfigFile(headlineString, parameterString, parameterValueString);

        // if requested parameter was "state" (transfered to "Enabled"), transfer the result-value to "Enabled" or "Disabled"
        if(strcmp(parameterString, "Enabled") == 0)
        {
          if(strcmp(parameterValueString, "1") == 0)
          {
            sprintf(parameterValueString, "enabled");
          }
          else
          {
            sprintf(parameterValueString, "disabled");
          }
        }

        // if a selection-menu of a specified value of a parameter should be checked
        if(pRequestedValue == NULL)
        {
          // just print the parameter-value itself
          printf("%s", parameterValueString);
        }
        else
        {
          // selection-menu is requested - show it
          if(strcmp(pRequestedValue, "selection") == 0)
          {
            PrintSelectionMenu(parameterString, parameterValueString);
          }

          // special value is requested,print "checked" if real value is like the requested one
          else if(strcmp(parameterValueString, pRequestedValue) == 0)
          {
            printf("checked");
          }
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
