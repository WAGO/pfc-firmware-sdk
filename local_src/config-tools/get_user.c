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
///  \file     get_user.c
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

#define USER_FILENAME              "/etc/lighttpd/lighttpd-htpasswd.user"
#define MAX_LINE_LENGTH            200
#define MAX_USER_NAME_LENGTH       200
#define SEPERATOR                  ':'


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


void ShowHelpText(void)
//
// Show describtion and usage of program on stdout
//
{
  int parameterIndex = 0;

  printf("\n* Get WBM user by number in user-list*\n\n");
  printf("Usage: get_user < user-number | \"selection\" >\n\n");
  printf("\"selection\": deliver a selection-menu for html-page which includes all user-names from passwd-file\n");
  printf("user-number: deliver the user-name which belongs to the specified user-number as sequential index in passwd-file\n"); 
  printf("\n");
}


int GetUserNameByIndex(int   userIndex,
                       char* pUserName)
//
// Get user-name specified by its sequential number in passwd-file
//
// input: userIndex: sequential number of user in passwd-file
//
// output: pUserName: string with the requested user-name, must be allocated by calling function
//
// return: error-code
//
{
  int   status              = SUCCESS;
  char* pPasswdFileContent  = NULL;

  if(pUserName == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pUserName, "");

  // get content of cpuinfo-file (it is not possible to read the file directly to buffer, because its size is ostensible always 0)
  pPasswdFileContent = FileContent_Get("/etc/lighttpd/lighttpd-htpasswd.user");

  if(pPasswdFileContent == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    char  passwdFileLine[MAX_LENGTH_OUTPUT_LINE]  = "";
    int   lineNr                                  = 0;

    // loop over the lines of user-file until we find the place at the requested line
    status = NOT_FOUND;
    lineNr = 1;
    while((SUCCESS == FileContent_GetLineByNr(pPasswdFileContent, lineNr, passwdFileLine)) && (status == NOT_FOUND))
    {   
      if((strlen(passwdFileLine) > 0) && (lineNr == userIndex))
      {
        // cut respectively the first word until spererator-sign
        char* pSeperator = NULL;
        strncpy(pUserName, passwdFileLine, MAX_USER_NAME_LENGTH);
        pSeperator = strchr(pUserName, SEPERATOR);
        if(pSeperator != NULL)
        {
          *pSeperator = 0;
        }
        status = SUCCESS;
      }
      ++lineNr;
    }

    FileContent_Destruct(&pPasswdFileContent);
  }
 
  return(status);
}


void ShowUserSelection(void)
//
// Create a selection-menu for html-page with all users from passwd-file and send it to stdout
//
{
  char  userName[MAX_USER_NAME_LENGTH]  = "";
  int   userIndex                       = 0;

  // start selection-field definition
  printf("\n              <select name=\"user\" size=\"1\">\n");

  // loop over all user-names of passwd-file and show them a menu-line
  userIndex = 1;
  while(SUCCESS == GetUserNameByIndex(userIndex, userName))
  {
    printf("                <option value=\"%s\">%s</option>\n", userName, userName); 
    ++userIndex; 
  }

  // close selection-field
  printf("              </selection>\n");

  return;
}



int main(int    argc, 
         char** argv)
{
  int   status          = SUCCESS;
  FILE* fFile           = NULL;
  char* pParameter      = NULL;
  int   requestedUserNr = 0;

  // help-text requested?
  if((argc == 2) && ((strcmp(argv[1], "--help") == 0) || strcmp(argv[1], "-h") == 0))
  {
    ShowHelpText();
  }
  else
  {
    // check if valid user-number is given as input
    if(argc < 2)
    {
      status = MISSING_PARAMETER;
    }
    else
    {
      // selection-menu with all users is requested - create it
      if(strcmp(argv[1], "selection") == 0)
      {
        ShowUserSelection();
      }

      // only one user-name by its index is requested
      else
      {
        // convert string-input to integer-value
        sscanf(argv[1], "%d", &requestedUserNr);
        //printf("requestedUserNr:%d", requestedUserNr);

        // if input is valid, get the corresponding user-name and show it on stdout
        if(requestedUserNr == 0)
        {
          status = INVALID_PARAMETER;
        }
        else
        {
          char userName[MAX_USER_NAME_LENGTH] = "";

          status = GetUserNameByIndex(requestedUserNr, userName);
          printf("%s", userName);
        }
      }
    }
  }

  return(status);
}
