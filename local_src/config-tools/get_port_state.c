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
///  \file     get_port_config.c
///
///  \version  $Revision: 61019 $1
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
#include <fcntl.h>      // for open()
#include <unistd.h>     // For read()

#include "config_tool_lib.h"

//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

#define SHOW_ERRORS                         0

#define MAX_LENGTH_EXE_NAME                 100


//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------

// Pointer to funktion to search the requested parameter
typedef int (*tFktGetParameter)(char* outputString);


// structure to join the possible input-strings with the processing-function to get them
typedef struct
{
  // input-string from command-line
  char inputString[MAX_LENGTH_INPUT_STRING];

  // name of executable file for a specified port
  char nameOfExe[MAX_LENGTH_EXE_NAME];

} tExeNameAssign;


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------


// array of all possible requested parameter (input-strings, strings to search for, processing-function to get them)
static tExeNameAssign astExeNameAssignTab[] =
{
  { "codesys-webserver",  "webserver" },
  { "codesys3-webserver", "" },
  { "ftp",                "pure-ftpd" },
  { "snmp",               "snmpd" },

  // this line must always be the last one - don't remove it!
  { "", "" }
};



//------------------------------------------------------------------------------
// external functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------

// Get state of codesys3 webserver from codesys3 config file.
// WebserverPortNr == 0 means disabled

#define MAX_CONFIGFILE_SIZE     (64 * 1024)

static int getCs3State(void)
{
  int status = SUCCESS;
  //ToDo: create a libplcini which will be used by codesys3 and config-tools for reading and writing the config-files
  FILE * fp = popen("chplcconfig -g -s CmpWebServer -k WebServerPortNr -o", "r");
  status = FILE_READ_ERROR;
  if(fp != NULL)
  {
    char * pLine= NULL;
    size_t n;
    if(0 <= getline(&pLine,&n,fp))
    {
      if(atoi(pLine) != 0)
      {
        printf("enabled");
      }
      else
      {
        printf("disabled");
      }
      free(pLine);
      status = SUCCESS;
    }
    pclose(fp);
  }
  return status;
}

static void ShowHelpText(void)
//
// Show describtion and usage of program on stdout
//
{
  int parameterIndex = 0;

  printf("\n* Get the state of the several ports *\n\n");
  printf("Usage: get_port_state <port> [parameter-value]\n\n");
  printf("port: ");

  // read the possible strings for parameter from array and show them like this: "param1 | param2 | param3"
  while(strlen(astExeNameAssignTab[parameterIndex].inputString) > 0)
  {
    if(parameterIndex != 0) printf("| ");
    printf("%s ", astExeNameAssignTab[parameterIndex].inputString);
    ++parameterIndex;
  }

  printf("\n");
  printf("parameter-value: output is \"checked\", if the result is the same as the given string (else no output).\n");
  //printf("                 This is useful for the display of the result in the context of a checkbox on a html-page.\n");
  printf("\n");
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
    if(argc < 2)
    {
      status = MISSING_PARAMETER;
    }
    else
    {
      char* pPortString   = NULL;
      char* pValueString  = NULL;

      if(argc > 2) pValueString = argv[2];

      // check if parameter is contained in parameter-list and get its index 
      pPortString  = argv[1];

      int  parameterIndex = 0;

      status = INVALID_PARAMETER;
      while((strlen(astExeNameAssignTab[parameterIndex].inputString) > 0) && (status == INVALID_PARAMETER))
      {
        // success - parameter-string is equal to one of our list
        if(strcmp(astExeNameAssignTab[parameterIndex].inputString, pPortString) == 0)
        {
          status = SUCCESS;
        }
        else
        {
          ++parameterIndex;
        }
      }

      // parameter was found -> start processing (get the fitting function from parameter-list)
      if(status == SUCCESS)
      {
        char   outputString[MAX_LENGTH_OUTPUT_STRING]   = "";
        char   systemCallString[MAX_LENGTH_SYSTEM_CALL] = "";
        char*  pPidString                               = NULL;

        if(strcmp((const char *)astExeNameAssignTab[parameterIndex].inputString, "codesys3-webserver") != 0)
        {
          snprintf(systemCallString, MAX_LENGTH_SYSTEM_CALL, "pidof %s", astExeNameAssignTab[parameterIndex].nameOfExe);
          pPidString = SystemCall_GetOutput(systemCallString);

          if(pPidString == NULL)
          {
            status = FILE_READ_ERROR;
          }
          else
          {
            if(strlen(pPidString) > 0)
            {
              strncpy(outputString, "enabled", sizeof(outputString));
            }
            else
            {
              strncpy(outputString, "disabled", sizeof(outputString));
            }

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

            printf("%s", outputString);
            SystemCall_Destruct(&pPidString);
          }
        }
        else
        {
          // handle special case of codesys3 webserver.
          status = getCs3State();
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
