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
///  \file
///
///  \brief    Config-tool to get various system time and timezone information.
///
///  \author   Stefanie Meihöfer : WAGO GmbH & Co. KG
///                           WF : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "config_tool_lib.h"

//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

/// \brief Maximum length of string for the output of the requested parameter.
#define MAX_LENGTH_CLOCK_OUTPUT           100

/// \brief Display mode of 12-hour-format as string
#define HOUR_FORMAT_12                    "12-hour-format"

/// \brief Display mode of 24-hour-format as string
#define HOUR_FORMAT_24                    "24-hour-format"

//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------

/// \brief Context for sub command function calls.
///
/// Encapsulates function arguments and return values to achieve a common
/// interface for sub command calls.
typedef struct
{
  char * pAdditionalParam;  ///< additional argument of sub command
  char * pOutputString;     ///< return value of the sub command
} tstContext;

/// \brief Common interface for sun command function calls.
typedef int (*tFunGetParameter)(tstContext * pContext);

/// \brief Entry of the table containing names and function pointers of sub commands.
typedef struct
{
  char const * inputString;             ///< input-string from command-line
  tFunGetParameter  pFunGetParameter;   ///< processing function to get the requested parameter
} tstParameterJumptab;



//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \brief Get the string which describes the time-format (12-hour-format or
///        24-hour-format).
///
/// Display mode is returned as string in pContext->pOutputString.
///
/// \param pContext Pointer to call context
///
/// \return SUCCESS of success, otherwise error-code
//------------------------------------------------------------------------------
static int GetDisplayMode(tstContext * pContext);

//------------------------------------------------------------------------------
/// \brief Get local time.
///
/// Local time is returned as string in pContext->pOutputString, either in 24-
/// or 12-hour format, depending on the actual settings.
///
/// \param pContext Pointer to call context
///
/// \return SUCCESS of success, otherwise error-code
//------------------------------------------------------------------------------
static int GetTimeLocal(tstContext * pContext);

//------------------------------------------------------------------------------
/// \brief Get UTC-time.
///
/// UTC time is returned as string in pContext->pOutputString, either in 24-
/// or 12-hour format, depending on the actual settings.
///
/// \param pContext Pointer to call context
///
/// \return SUCCESS of success, otherwise error-code
//------------------------------------------------------------------------------
static int GetTimeUtc(tstContext * pContext);

//------------------------------------------------------------------------------
/// \brief Get local date.
///
/// Local date is returned as string in context->pOutputString.
///
/// \param pContext Pointer to call context
///
/// \return SUCCESS of success, otherwise error-code
//------------------------------------------------------------------------------
static int GetDateLocal(tstContext * pContext);


//------------------------------------------------------------------------------
/// \brief Get date in UTC-time.
///
/// UTC date is returned as string in pContext->pOutputString.
///
/// \param pContext Pointer to call context
///
/// \return SUCCESS of success, otherwise error-code
//------------------------------------------------------------------------------
static int GetDateUtc(tstContext * pContext);

//------------------------------------------------------------------------------
/// \brief Get date in UTC-time.
///
/// UTC date is returned as string in pContext->pOutputString.
///
/// \note This sub command is still existing by compatibility reasons,
///       since we now distinguish between UTC and local time.
///
/// \param pContext Pointer to call context.
///
/// \return SUCCESS of success, otherwise error-code
//------------------------------------------------------------------------------
static int GetDate(tstContext * pContext);

//------------------------------------------------------------------------------
/// \brief Get TZ-string directly from file /etc/TZ.
///
/// TZ-string is returned as string in pContext->pOutputString.
///
/// \param pContext Pointer to call context
///
/// \return SUCCESS of success, otherwise error-code
//------------------------------------------------------------------------------
static int GetTzString(tstContext * pContext);

//------------------------------------------------------------------------------
/// \brief Get a plain description of a timezone by a given index.
///
/// Timezone index is provided as string by pContext->pAdditionParam.
/// Timezone description is returned as string in pContext->pOutputString.
/// If there is no matching timezone for the given index, an empty string is
/// returned.
///
/// \param pContext Pointer to call context
///
/// \return SUCCESS of success, otherwise error-code
//------------------------------------------------------------------------------
static int GetTimezonePlainStringByNr(tstContext * pContext);

//------------------------------------------------------------------------------
/// \brief Prints all known timezones to stdout.
///
/// Timezones are printed as plain strings, separated by a pipe ('|').
///
/// \param pContext unused
///
/// \return SUCCESS of success, otherwise error-code
//------------------------------------------------------------------------------
static int GetAllTimezonePlainStrings(tstContext * pContext);

//------------------------------------------------------------------------------
/// \brief Prints all known timezones to stdout.
///
/// Timezones are printes as JSON array.
///
/// \param pContext unused
///
/// \return SUCCESS of success, otherwise error-code
//------------------------------------------------------------------------------
static int GetAllTimezonePlainStringsJson(tstContext * pContext);

//------------------------------------------------------------------------------
/// \brief Get a description of the actual configured timezone.
///
/// Timezone description is returned in pContext->pOutputString.
///
/// \param pContext Pointer to call context
///
/// \return SUCCESS of success, otherwise error-code
//------------------------------------------------------------------------------
static int GetActualTimezonePlainString(tstContext * pContext);

//------------------------------------------------------------------------------
/// \brief Show description and usage of program on stdout.
//------------------------------------------------------------------------------
static void ShowHelpText(void);


/// \brief Initializes a call context to invoke sub commands.
///
/// \param pContext         Pointer to context to initialize
/// \param pAdditionalParam Pointer to additional parameter of the cal
//------------------------------------------------------------------------------
static void InitContext(tstContext * pContext,
                        char * pAdditionalParam);

//------------------------------------------------------------------------------
/// \brief Cleans up a call context of a sub command.
///
/// \param pContext Pointer to context to clean up.
//------------------------------------------------------------------------------
static void CleanupContext(tstContext * pContext);

//------------------------------------------------------------------------------
/// \brief Returns the size of a file
///
/// \param pFileName name of the file
///
/// \return Size of the file or 0 on error.
//------------------------------------------------------------------------------
static off_t GetFileSize(char const * pFileName);

//------------------------------------------------------------------------------
/// \brief Initializes TZ variable
///
//------------------------------------------------------------------------------
static void SetTZVariable(void);

//------------------------------------------------------------------------------
/// \brief Return true if a given string is a number.
///
/// \param pValue String to investigate
///
/// \return true of the string is a number, otherwise false
//------------------------------------------------------------------------------
static bool IsNumber(char const * pValue);

//------------------------------------------------------------------------------
/// \brief Copys a string to a buffer.
///
/// Ensures that the buffer contains a NULL-terminated string afterwards.
///
/// \param pBuffer Buffer to copy the string to
/// \param pSource String to copy
/// \param bufferSize Size of the buffer
//------------------------------------------------------------------------------
static void CopyString(char * pBuffer,
                       char const * pSource,
                       size_t bufferSize);

/// \brief Table containing names of sub commands and function pointers.
static tstParameterJumptab const astParameterJumptab[] =
{
  { "display-mode",           GetDisplayMode },
  { "time-local",             GetTimeLocal },
  { "time-utc",               GetTimeUtc },
  { "date",                   GetDate },
  { "date-local",             GetDateLocal },
  { "date-utc",               GetDateUtc },
  { "tz-string",              GetTzString },
  { "timezone-string-by-nr",  GetTimezonePlainStringByNr },
  { "all-timezone-strings",   GetAllTimezonePlainStrings },
  { "timezone-strings-json",  GetAllTimezonePlainStringsJson },
  { "timezone-string",        GetActualTimezonePlainString }, 

  // this line must always be the last one - don't remove it!
  { "", NULL }
};


//------------------------------------------------------------------------------
// external functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
static void ShowHelpText(void)
{
  printf("\n* Get clock data *\n\n");
  printf("Usage: get_clock_data <parameter> [parameter-value]\n\n");
  printf("parameter:");

  // read the possible strings for parameter from array and show them like this: "param1 | param2 | param3"
  for(size_t i = 0; '\0' != astParameterJumptab[i].inputString[0]; i++)
  {
    if(i != 0) printf("| ");
    printf("%s ", astParameterJumptab[i].inputString);
  }

  printf("\n");
  printf("parameter-value: is used by timezone-string-by-nr to identify the timezone-string\n");
  printf("\n");
}


//------------------------------------------------------------------------------
static int GetDisplayMode(tstContext * pContext)
{
  int   status                  = SUCCESS;
  char* pTimeFormatFileContent  = NULL;
 
  // get content of fieldbus-info-file
  pTimeFormatFileContent = FileContent_Get("/etc/config-tools/TIME_FORMAT");

  // if time-format-file is not existing, create it with 24-hour-format as default
  if(pTimeFormatFileContent == NULL)
  {
    system("echo display-mode=24-hour-format >> /etc/config-tools/TIME_FORMAT");
    system("sync");
    CopyString(pContext->pOutputString, HOUR_FORMAT_24, MAX_LENGTH_CLOCK_OUTPUT);
  }
  else
  {
    char  timeFormatFileLine[MAX_LENGTH_OUTPUT_LINE] = "";
    int   lineNr                                     = 0;

    // loop over the lines of time-format-file until we find the line with the fieldbus-information
    lineNr = 1;
    while((SUCCESS == FileContent_GetLineByNr(pTimeFormatFileContent, lineNr, timeFormatFileLine)) && ('\0' == pContext->pOutputString[0]))
    {   
      // first search for the string which is always directly in front of the time-format-string 
      char const   beginOfTimeFormatString[] = "display-mode=";
      size_t const beginOfTimeFormatString_len = sizeof(beginOfTimeFormatString) - 1;

      // if the line is no comment and introducing string was found
      if(0 == strncmp(timeFormatFileLine, beginOfTimeFormatString, beginOfTimeFormatString_len))
      {
        // read the time-fomat behind it 
        CopyString(pContext->pOutputString, timeFormatFileLine + beginOfTimeFormatString_len, MAX_LENGTH_CLOCK_OUTPUT);
        CutWord(pContext->pOutputString, MAX_LENGTH_CLOCK_OUTPUT);
      }

      ++lineNr;
    }

    FileContent_Destruct(&pTimeFormatFileContent);


//    // if a comparison with a special value should be made for html-page, do it and set output-string accordingly
//    if(pAdditionalParam != NULL)
//    {
//      if(strncmp(pAdditionalParam, pOutputString, MAX_LENGTH_CLOCK_OUTPUT) == 0)
//      {
//        CopyString(pOutputString, "checked", MAX_LENGTH_CLOCK_OUTPUT);
//      }
//      else
//      {
//        CopyString(pOutputString, "", MAX_LENGTH_CLOCK_OUTPUT);
//      }
//    }
  }

  return(status);
}


//------------------------------------------------------------------------------
static int GetTimeLocal(tstContext * pContext)
{
  time_t      tTime = 0;
  struct tm*  stTmTime;
  tstContext  displayModeString;

  InitContext(&displayModeString, NULL);

  // get time-value (first get system-time, then convert it to time-struct as local-time)
  time(&tTime);
  stTmTime = localtime(&tTime);

  // get the display-format from config-file and put time to string depending on it 
  if(SUCCESS == GetDisplayMode(&displayModeString))
  {
    if(strcmp(displayModeString.pOutputString, HOUR_FORMAT_24) == 0)
    {
      // 24-hour-format
      strftime(pContext->pOutputString, MAX_LENGTH_CLOCK_OUTPUT, "%H:%M:%S", stTmTime);
    }
    else
    {
      // 12-hour-format
      strftime(pContext->pOutputString, MAX_LENGTH_CLOCK_OUTPUT, "%I:%M:%S %p", stTmTime);
    }
  }

  CleanupContext(&displayModeString);
  return(SUCCESS);
}


//------------------------------------------------------------------------------
static int GetTimeUtc(tstContext * pContext)
{
  time_t      tTime = 0;
  struct tm*  stTmTime;
  tstContext  displayModeString;

  InitContext(&displayModeString, NULL);

  // get time-value (first get system-time, then convert it to time-struct as utc-time)
  time(&tTime);
  stTmTime = gmtime(&tTime);

  // get the display-format from config-file and put time to string depending on it 
  if(SUCCESS == GetDisplayMode(&displayModeString))
  {
    if(strcmp(displayModeString.pOutputString, HOUR_FORMAT_24) == 0)
    {
      // 24-hour-format
      strftime(pContext->pOutputString, MAX_LENGTH_CLOCK_OUTPUT, "%H:%M:%S", stTmTime);
    }
    else
    {
      // 12-hour-format
      strftime(pContext->pOutputString, MAX_LENGTH_CLOCK_OUTPUT, "%I:%M:%S %p", stTmTime);
    }
  }

  CleanupContext(&displayModeString);
  return(SUCCESS);
}


//------------------------------------------------------------------------------
static int GetDateLocal(tstContext * pContext)
{
  time_t      tTime     = 0;
  struct tm*  stTmTime;

   // get time-value (first get system-time, then convert it to time-struct as utc-time) and at least put date to string
  time(&tTime);
  stTmTime = localtime(&tTime);
  strftime(pContext->pOutputString, MAX_LENGTH_CLOCK_OUTPUT, "%d.%m.%Y", stTmTime);
 
  return SUCCESS;
}


//------------------------------------------------------------------------------
static int GetDateUtc(tstContext * pContext)
{
  time_t      tTime     = 0;
  struct tm*  stTmTime;

  // get time-value (first get system-time, then convert it to time-struct as utc-time) and at least put date to string
  time(&tTime);
  stTmTime = gmtime(&tTime);
  strftime(pContext->pOutputString, MAX_LENGTH_CLOCK_OUTPUT, "%d.%m.%Y", stTmTime);

  return(SUCCESS);
}


//------------------------------------------------------------------------------
static int GetDate(tstContext * pContext)
{
  return GetDateUtc(pContext);
}


//------------------------------------------------------------------------------
static int GetTzString(tstContext * pContext)
{
  int   status          = SUCCESS;
  char* pTzFileContent  = NULL;

  // get content of fieldbus-info-file
  pTzFileContent = FileContent_Get("/etc/TZ");

  // if TZ-file is not existing
  if(pTzFileContent == NULL)
  {
    FILE *fp;
    fp = fopen("/etc/localtime","r");
    if(fp != NULL)
    {
      long int fileSize;
      char * fileBuffer;
      fseek(fp, 0, SEEK_END);
      fileSize = ftell(fp);
      rewind(fp);
      fileBuffer=malloc(fileSize+1);
      if(fileBuffer != NULL)
      {
        int i;
        int count = 0;
        memset(fileBuffer,0,fileSize+1);
        fread(fileBuffer, 1, fileSize, fp);
        for(i=fileSize;i>0;i--)
        {
          if(fileBuffer[i] == '\n')
          {
            count++;
            fileBuffer[i]= 0;
          }
          if(count >= 2)break;
        }
        if(i > 0)
        {
          pTzFileContent = strdup(&(fileBuffer[i+1]));
        }
        free(fileBuffer);
      }
      fclose(fp);
    }
    if(pTzFileContent == NULL)
    {
      status = FILE_READ_ERROR;
    }
    else
    {
      free(pContext->pOutputString);
      pContext->pOutputString = pTzFileContent;
      CutWord(pContext->pOutputString, strlen(pTzFileContent) + 1);
    }
  }
  else
  {
    free(pContext->pOutputString);
    pContext->pOutputString = strdup(pTzFileContent);
    CutWord(pContext->pOutputString, strlen(pTzFileContent) + 1);
    FileContent_Destruct(&pTzFileContent);
  }

  return(status);
}


//------------------------------------------------------------------------------
static int GetTzName(tstContext * pContext)
{
  int   status          = SUCCESS;
  char* pTzFileContent  = NULL;

  // get content of fieldbus-info-file
  pTzFileContent = FileContent_Get("/etc/TZname");
  // if TZ-file is not existing
  if(pTzFileContent == NULL || pTzFileContent[0]==0)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    // copy tz-string from file-buffer to output-string
    CopyString(pContext->pOutputString, pTzFileContent, MAX_LENGTH_CLOCK_OUTPUT);
    CutWord(pContext->pOutputString, MAX_LENGTH_CLOCK_OUTPUT);

    FileContent_Destruct(&pTzFileContent);
  }

  return(status);
}


//------------------------------------------------------------------------------
static int GetAllTimezonePlainStrings(tstContext * pContext)
{
    UNUSED_PARAMETER(pContext);

    int status = SUCCESS;
    int cnt = 1;
    char tzStrNr[4] = ""; // up to 999 timezone strings supported
    tstContext timezoneString;

    InitContext(&timezoneString, tzStrNr);

    do
    {
        if(cnt > 999) // very unlikely
        {
            status = INVALID_PARAMETER; 
            break;
        }

        snprintf(tzStrNr, sizeof(tzStrNr), "%d", cnt);

        timezoneString.pOutputString[0] = '\0';
        status = GetTimezonePlainStringByNr(&timezoneString);
        if(SUCCESS == status)
        {
            if(cnt > 1)
            {
                printf("|");
            }

            printf("%s", timezoneString.pOutputString);
        
            ++cnt; // no endless loop possible: SUCCESS != status is an abort criterium
        }
    }
    while( (SUCCESS == status) && ('\0' != timezoneString.pOutputString[0]) );

    if(cnt > 1) // at least one entry was found
    {
        status = SUCCESS; // GetTimezonePlainStringByNr returns "" and ERROR for the one-after-last element
    }

    CleanupContext(&timezoneString);
    return status;
}


//------------------------------------------------------------------------------
static int GetAllTimezonePlainStringsJson(tstContext * pContext)
{
    UNUSED_PARAMETER(pContext);

    int status = SUCCESS;
    int cnt = 1;
    char tzStrNr[4] = ""; // up to 999 timezone strings supported
    tstContext timezoneString;

    InitContext(&timezoneString, tzStrNr);
    printf("[ ");
    do
    {
        if(cnt > 999) // very unlikely
        {
            status = INVALID_PARAMETER;
            break;
        }

        snprintf(tzStrNr, sizeof(tzStrNr), "%d", cnt);

        timezoneString.pOutputString[0] = '\0';
        status = GetTimezonePlainStringByNr(&timezoneString);
        if(SUCCESS == status)
        {
            if(cnt > 1)
            {
                printf(", ");
            }

            printf("\"%s\"", timezoneString.pOutputString);

            ++cnt; // no endless loop possible: SUCCESS != status is an abort criterium
        }
    }
    while( (SUCCESS == status) && ('\0' != timezoneString.pOutputString[0]) );

    printf("]");


    if(cnt > 1) // at least one entry was found
    {
        status = SUCCESS; // GetTimezonePlainStringByNr returns "" and ERROR for the one-after-last element
    }

    CleanupContext(&timezoneString);
    return status;
}


//------------------------------------------------------------------------------
static int GetTimezonePlainStringByNr(tstContext * pContext)
{
  int status      = ERROR;
  int timezoneNr  = 0;

  if (!IsNumber(pContext->pAdditionalParam))
  {
      return INVALID_PARAMETER;
  }

  // convert the requested timezone-number from input-string
  sscanf(pContext->pAdditionalParam, "%d", &timezoneNr);
  if((timezoneNr > 0))
  {
    int i;
    FILE * fp;
    fp = fopen("/usr/share/zoneinfo/allzones","r");
    status = NOT_FOUND;
    if(fp != NULL)
    {
      char * lineBuffer = NULL;
      size_t  szLineBuf = 0;
      for(i=0;i<timezoneNr && !feof(fp);i++)
      {
        ssize_t read = 0;
        read = getline(&lineBuffer, &szLineBuf, fp);
        //printf("lineBuffer: %s\n",lineBuffer);
        if(read < 0)
        {
          status = ERROR;
          break;
        }
      }
      //printf("i is %d, tzn is : %d\n",i , timezoneNr);
      if(status != ERROR && i == timezoneNr)
      {
        char * pbuf;
        status = SUCCESS;
        lineBuffer[strlen(lineBuffer)-1]=0;
        do
        {
          pbuf = strchr(lineBuffer,' ');
          if(pbuf == NULL)break;
          pbuf = strchr(++pbuf,' ');
          if(pbuf == NULL)break;
          pbuf++;
          CopyString(pContext->pOutputString, pbuf, MAX_LENGTH_CLOCK_OUTPUT);
        }while(0);
      }
      free(lineBuffer);
      fclose(fp);
    }

  }

  return(status);
}


//------------------------------------------------------------------------------
static int GetActualTimezonePlainString(tstContext * pContext)
{
  int   status                             = SUCCESS;
  tstContext tzString;

  InitContext(&tzString, NULL);
  status = GetTzName(&tzString);
  strncat(tzString.pOutputString, " ", (MAX_LENGTH_CLOCK_OUTPUT - strlen(tzString.pOutputString) - 1));
  if(SUCCESS == status)
  {
    FILE *fp;
    fp = fopen("/usr/share/zoneinfo/allzones","r");
    if(fp != NULL)
    {
      long int fileSize;
      char * fileBuffer;
      fseek(fp, 0, SEEK_END);
      fileSize = ftell(fp);
      rewind(fp);
      fileBuffer=malloc(fileSize+1);
      if(fileBuffer != NULL)
      {
        char * pTZ = NULL;
        memset(fileBuffer,0,fileSize+1);
        fread(fileBuffer, 1, fileSize, fp);
        pTZ = strstr(fileBuffer, tzString.pOutputString);
        if(pTZ != NULL)
        {
          char * pPlain = strchr(pTZ,' ');
          if(pPlain != NULL)
          {
            char * pEnd = strchr(pPlain,'\n');
            if(pEnd != NULL) *pEnd=0;
            pPlain++;
            pPlain = strchr(pPlain,' ');
            if(pPlain != NULL)
            {
              CopyString(pContext->pOutputString, ++pPlain, MAX_LENGTH_CLOCK_OUTPUT);
            }
          }
        }
        free(fileBuffer);
      }
      fclose(fp);
    }
  }

  CleanupContext(&tzString);
  return(status);
}


//------------------------------------------------------------------------------
static off_t GetFileSize(char const * pFileName)
{
  off_t result = 0;
  struct stat attributes;
  int rc;

  rc = stat(pFileName, &attributes);
  if (0 == rc)
  {
    result = attributes.st_size;
  }

  return result;
}


static void SetTZVariable(void)
{
  setenv("TZ","/etc/localtime", 1);

  if (0 == GetFileSize("/etc/localtime"))
  {
    char * timezone = FileContent_Get("/etc/TZ");
    if (NULL != timezone)
    {
      setenv("TZ", timezone, 1);
      FileContent_Destruct(&timezone);
    }
  }

  tzset();
}


//------------------------------------------------------------------------------
static bool IsNumber(char const * pValue)
{
    if ((NULL == pValue) || ('\0' == *pValue))
    {
        return false;
    }

    while ('\0' != *pValue)
    {
        if ((*pValue < '0') || ('9' < *pValue))
        {
            return false;
        }

        pValue++;
    }

    return true;
}


//------------------------------------------------------------------------------
static void CopyString(char * pBuffer,
                       char const * pSource,
                       size_t bufferSize)
{
    size_t const sourceLength = strlen(pSource) + 1;
    size_t const bytesToCopy = bufferSize < sourceLength ? bufferSize : sourceLength;
    memcpy(pBuffer, pSource, bytesToCopy);
    pBuffer[bytesToCopy - 1] = '\0';
}


//------------------------------------------------------------------------------
static void InitContext(tstContext * pContext,
                        char * pAdditionalParam)
{
    pContext->pAdditionalParam = pAdditionalParam;
    pContext->pOutputString = malloc(MAX_LENGTH_CLOCK_OUTPUT);
    pContext->pOutputString[0] = '\0';
}


//------------------------------------------------------------------------------
static void CleanupContext(tstContext * pContext)
{
    free(pContext->pOutputString);
}


//------------------------------------------------------------------------------
int main(int    argc, 
         char** argv)
{
  int   status            = SUCCESS;

  // set TZ variable to ensure locale is usable by date and time functions
  SetTZVariable();

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
      // check if the single parameters are valid
      char* pParameterString  = argv[1];
      char* pAdditionalParam  = NULL;

      if(argc > 2) pAdditionalParam = argv[2];

      // check if parameter is contained in parameter-list and get its index 
      int  parameterIndex = 0;

      status = INVALID_PARAMETER;
      while(('\0' != astParameterJumptab[parameterIndex].inputString[0]) && (status == INVALID_PARAMETER))
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

      // parameter was found -> start processing (get the fitting function from parameter-list)
      if(status == SUCCESS)
      {
        tstContext context;
        InitContext(&context, pAdditionalParam);
        status = astParameterJumptab[parameterIndex].pFunGetParameter(&context);

        // send result to stdout
        printf("%s", context.pOutputString);
        CleanupContext(&context);
      }
    }
  }

  return(status);
}
