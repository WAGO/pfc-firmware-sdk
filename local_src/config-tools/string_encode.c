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
///  \file     string_encode.c
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

// Pointer to funktion to search the requested parameter
typedef int (*tFktGetParameter)(char* pacString);


// structure to join the possible input-strings with the processing-function to get them
typedef struct
{
  // input-string from command-line
  char               contextString[20];

  // processing function to get the requested parameter
  tFktGetParameter   pFktGetParameter;

} tParameterJumptab;


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------

// declaration of all processing functions used for jumptab
int ConvertForRegEx(char* inputString);
int ConvertForBash(char* inputString);
int ConvertForSedWrite(char *inputString);


// array of all possible requested parameter (input-strings, strings to search for, processing-function to get them)
static tParameterJumptab astParameterJumptab[] =
{
  { "reg-ex",           ConvertForRegEx },
  { "bash",             ConvertForBash },
  { "sed-write",        ConvertForSedWrite },

  // this line must always be the last one - don't remove it!
  { "", NULL }
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

  printf("\n* Convert a given text for use in other context or programms *\n\n");
  printf("Usage: string_encode < context string >\n\n");
  printf("context: \"reg-ex\"\n");

  // read the possible strings for parameter from array and show them like this: "param1 | param2 | param3"
  while(strlen(astParameterJumptab[parameterIndex].contextString) > 0)
  {
    if(parameterIndex != 0) printf("| ");
    printf("%s ", astParameterJumptab[parameterIndex].contextString);
    ++parameterIndex;
  }

  printf("\n\n");
  printf("string: string to convert\n");

  printf("\n");
}


int ConvertForRegEx(char* inputString)
{
  int   charIndex       = 0;
  char* acMetaCharacter ="\\.[](){}|?+-*^$";

  if(inputString == NULL) return INVALID_PARAMETER;

  while(inputString[charIndex] != 0)
  {
    if(NULL != strchr(acMetaCharacter, inputString[charIndex]))
    {
      printf("\\");
    }

    printf("%c", inputString[charIndex]);
    ++charIndex;
  }

  return SUCCESS;
}


int ConvertForBash(char* inputString)
{
  int   charIndex       = 0;
  char* acMetaCharacter ="\\&";

  if(inputString == NULL) return INVALID_PARAMETER;

  while(inputString[charIndex] != 0)
  {
    if(NULL != strchr(acMetaCharacter, inputString[charIndex]))
    {
      printf("\\");
    }

    printf("%c", inputString[charIndex]);
    ++charIndex;
  }

  return SUCCESS;
}


int ConvertForSedWrite(char* inputString)
{
  int   charIndex       = 0;
  char* acMetaCharacter ="\\&'\"";

  if(inputString == NULL) return INVALID_PARAMETER;

  while(inputString[charIndex] != 0)
  {
    if(NULL != strchr(acMetaCharacter, inputString[charIndex]))
    {
      printf("\\");
    }

    printf("%c", inputString[charIndex]);
    ++charIndex;
  }

  return SUCCESS;
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
      // check if parameter is contained in parameter-list and get its index
      char* pParameterString  = argv[1];
      int   parameterIndex    = 0;

      status = INVALID_PARAMETER;
      while((strlen(astParameterJumptab[parameterIndex].contextString) > 0) && (status == INVALID_PARAMETER))
      {
        // success - parameter-string is equal to one of our list
        if(strcmp(astParameterJumptab[parameterIndex].contextString, pParameterString) == 0)
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
        status = astParameterJumptab[parameterIndex].pFktGetParameter(argv[2]);
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
