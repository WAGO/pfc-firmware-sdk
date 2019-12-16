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
///  \file     get_clock_data.c
///
///  \version  $Revision: 41321 $1
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
#include <time.h>

#include "config_tool_lib.h"

//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

// show errors on screen? Normally not, because they would be also displayed e.g. on WBM
#define SHOW_ERRORS                       0

// maximum length of the string to search for in the system-call output
//#define MAX_LENGTH_SEARCH_STRING        40

// maximum length of string for the output of the requested parameter
#define MAX_LENGTH_CLOCK_OUTPUT           100

#define MAX_LENGTH_TZ_STRING              200
#define MAX_LENGTH_PLAIN_TIMEZONE_STRING  200

// output-strings for display-modes
#define HOUR_FORMAT_12                    "12-hour-format"
#define HOUR_FORMAT_24                    "24-hour-format"



//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------

// Pointer to funktion to search the requested parameter
typedef int (*tFktGetParameter)(char* pAdditionalParam,
                                char* pOutputString);

// struct to join the possible input-strings with the processing-function to get them
typedef struct
{
  // input-string from command-line
  char              inputString[MAX_LENGTH_INPUT_STRING];

  // processing function to get the requested parameter
  tFktGetParameter  pFktGetParameter;

} tstParameterJumptab;


// struct to assign the TZ-string to the plain-text for user
typedef struct 
{
  // tz-string from TZ-file
  char  tzString[MAX_LENGTH_TZ_STRING];

  // timezone-string in plain-text
  char  plainTimezoneString[MAX_LENGTH_PLAIN_TIMEZONE_STRING];

} tstTimezoneStringAssign;


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------

// get the actual configured clock-display-mode (12- or 24-hour)
int GetDisplayMode(char* pValueString,
                   char* pOutputString);

// function to get local-time
int GetTimeLocal(char* pAdditionalParam,
                 char* pOutputString);

// function to get UTC-time
int GetTimeUtc(char* pAdditionalParam,
               char* pOutputString);

// function to get local date
int GetDateLocal(char* pAdditionalParam,
                 char* pOutputString);

// function to get UTC date
int GetDateUtc(char* pAdditionalParam,
               char* pOutputString);

// function to get UTC date 
int GetDate(char* pAdditionalParam,
            char* pOutputString);

// function to get tz-string
int GetTzString(char* pAdditionalParam,
                char* pOutputString);

int GetTimezonePlainStringByNr(char* pNrString,
                               char* pOutputString);

int GetAllTimezonePlainStrings(char* pNrString,
                               char* pOutputString);

int GetAllTimezonePlainStringsJson(char* pNrString,
                                   char* pOutputString);

int GetActualTimezonePlainString(char* pAdditionalParam,
                                 char* pOutputString);

int GetTimezoneSelection(char* pNrString,
                         char* pOutputString);


// array of all possible requested parameter (input-strings, strings to search for, processing-function to get them)
static tstParameterJumptab astParameterJumptab[] =
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
  { "timezone-selection",     GetTimezoneSelection },

  // this line must always be the last one - don't remove it!
  { "", NULL }
};


static tstTimezoneStringAssign astTimezoneStringAssign[] =
{
  { "AST4ADT,M3.2.0,M11.1.0",       "AST/ADT: Altzantic Standard Time, Halifax" },
  { "EST5EDT,M3.2.0,M11.1.0",       "EST/EDT: Eastern Standard Time, New York, Toronto" },
  { "CST6CDT,M3.2.0,M11.1.0",       "CST/CDT: Central Standard Time, Chicago, Winnipeg" },
  { "MST7MDT,M3.2.0,M11.1.0",       "MST/MDT: Mountain Standard Time, Denver, Edmonton" },
  { "PST8DPT,M3.2.0,M11.1.0",       "PST/PDT: Pacific Standard Time, Los Angeles, Whitehorse" },
  { "GMT0BST,M3.5.0/1,M10.5.0",     "GTM/BST: Greenwich Main Time, GB, P, IRL, IS, ..." },
  { "CET-1CEST,M3.5.0/2,M10.5.0/3", "CET/CEST: Central European Time, B, DK, D, F, I, CRO, NL, ..." },
  { "EET-2EEST,M3.5.0/3,M10.5.0/4", "EET/EEST: East European Time, BUL, FI, GR, TR, ..." },
  { "CST-8",                        "CST: China Standard Time" },
  { "JST-9",                        "JST: Japan / Korea Standard Time" },

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

  printf("\n* Get clock data *\n\n");
  printf("Usage: get_clock_data <parameter> [parameter-value]\n\n");
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
  printf("\nNote: needs the bash-script \"get_local_time\" to get the local-time-value.\n");
  //printf("                 This is useful for the display of the result in the context of a checkbox on a html-page.\n");
  printf("\n");
}


int GetDisplayMode(char* pValueString,
                   char* pOutputString)
//
// Get the string which describes the time-format (12-hour-format or 24-hour-format).
//
// output: string with the time-format - must be allocated by calling function!
//
// return: error-code: SUCCESS: no error
//
{
  int   status                  = SUCCESS;
  char* pTimeFormatFileContent  = NULL;
 
  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  // get content of fieldbus-info-file
  pTimeFormatFileContent = FileContent_Get("/etc/config-tools/TIME_FORMAT");

  // if time-format-file is not existing, create it with 24-hour-format as default
  if(pTimeFormatFileContent == NULL)
  {
    system("echo display-mode=24-hour-format >> /etc/config-tools/TIME_FORMAT");
    system("sync");
    strncpy(pOutputString, HOUR_FORMAT_24, MAX_LENGTH_CLOCK_OUTPUT);
  }
  else
  {
    char  timeFormatFileLine[MAX_LENGTH_OUTPUT_LINE] = "";
    int   lineNr                                     = 0;

    // loop over the lines of time-format-file until we find the line with the fieldbus-information
    lineNr = 1;
    while((SUCCESS == FileContent_GetLineByNr(pTimeFormatFileContent, lineNr, timeFormatFileLine)) && (strlen(pOutputString) == 0))
    {   
      // first search for the string which is always directly in front of the time-format-string 
      const char beginOfTimeFormatString[] = "display-mode=";

      // if the line is no comment and introducing string was found
      if((timeFormatFileLine[0] != COMMENT_CHAR) && (strstr(timeFormatFileLine, beginOfTimeFormatString) != NULL))
      {
        // read the time-fomat behind it 
        strncpy(pOutputString, timeFormatFileLine + strlen(beginOfTimeFormatString), MAX_LENGTH_CLOCK_OUTPUT);
        CutWord(pOutputString, MAX_LENGTH_CLOCK_OUTPUT);
      }

      ++lineNr;
    }

    FileContent_Destruct(&pTimeFormatFileContent);

    // if a comparison with a special value should be made for html-page, do it and set output-string accordingly
    if(pValueString != NULL)
    {
      if(strncmp(pValueString, pOutputString, MAX_LENGTH_CLOCK_OUTPUT) == 0)
      {
        strncpy(pOutputString, "checked", MAX_LENGTH_CLOCK_OUTPUT);
      }
      else
      {
        strncpy(pOutputString, "", MAX_LENGTH_CLOCK_OUTPUT);
      }
    }
  }

  return(status);
}


int GetTimeLocal(char* pAdditionalParam,
                 char* pOutputString)
//
// Get local time.
//
// input: pAdditionalParam: ignored
//
// output: String with local time, either in 24- or 12-hour-format, depending on the according settings.
//
// return: error-code: SUCCESS: no error
//
{
  char        displayModeString[MAX_LENGTH_CLOCK_OUTPUT] = "";
  time_t      tTime                                      = 0;
  struct tm*  stTmTime;

  char*       pTimeString                                = NULL;

  // check input-parameter
  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  // get time-value (first get system-time, then convert it to time-struct as local-time)
  time(&tTime);
  stTmTime = localtime(&tTime);

  // get the display-format from config-file and put time to string depending on it 
  if(SUCCESS == GetDisplayMode(NULL, displayModeString))
  {
    if(strcmp(displayModeString, HOUR_FORMAT_24) == 0)
    {
      // 24-hour-format
      strftime(pOutputString, MAX_LENGTH_CLOCK_OUTPUT, "%H:%M:%S", stTmTime);
    }
    else
    {
      // 12-hour-format
      strftime(pOutputString, MAX_LENGTH_CLOCK_OUTPUT, "%I:%M:%S %p", stTmTime);
    }
  }

  return(SUCCESS);
}


int GetTimeUtc(char* pAdditionalParam,
               char* pOutputString)
//
// Get UTC-time.
//
// input: pAdditionalParam: ignored
//
// output: String with UTC-time, either in 24- or 12-hour-format, depending on the according settings.
//
// return: error-code: SUCCESS: no error
//
{
  char        displayModeString[MAX_LENGTH_CLOCK_OUTPUT] = "";

  time_t      tTime     = 0;
  struct tm*  stTmTime;

  // check input-parameter
  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  // get time-value (first get system-time, then convert it to time-struct as utc-time)
  time(&tTime);
  stTmTime = gmtime(&tTime);

  // get the display-format from config-file and put time to string depending on it 
  if(SUCCESS == GetDisplayMode(NULL, displayModeString))
  {
    if(strcmp(displayModeString, HOUR_FORMAT_24) == 0)
    {
      // 24-hour-format
      strftime(pOutputString, MAX_LENGTH_CLOCK_OUTPUT, "%H:%M:%S", stTmTime);
    }
    else
    {
      // 12-hour-format
      strftime(pOutputString, MAX_LENGTH_CLOCK_OUTPUT, "%I:%M:%S %p", stTmTime);
    }
  }

  return(SUCCESS);
}


int GetDateLocal(char* pAdditionalParam,
                 char* pOutputString)
//
// Get local date.
//
// input: pAdditionalParam: ignored
//
// output: String with local date
//
// return: error-code: SUCCESS: no error
//
{
  char* pDateString = NULL;
  time_t      tTime     = 0;
  struct tm*  stTmTime;

  // check input-parameter
  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

   // get time-value (first get system-time, then convert it to time-struct as utc-time) and at least put date to string
  time(&tTime);
  stTmTime = localtime(&tTime);
  strftime(pOutputString, MAX_LENGTH_CLOCK_OUTPUT, "%d.%m.%Y", stTmTime);
 
  return SUCCESS;
}


int GetDateUtc(char* pAdditionalParam,
               char* pOutputString)
//
// Get date.
//
// input: pAdditionalParam: ignored
//
// output: String with date (format dd.mm.yyyy)
//
// return: error-code: SUCCESS: no error
//
{
  time_t      tTime     = 0;
  struct tm*  stTmTime;

  // check input-parameter
  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  // get time-value (first get system-time, then convert it to time-struct as utc-time) and at least put date to string
  time(&tTime);
  stTmTime = gmtime(&tTime);
  strftime(pOutputString, MAX_LENGTH_CLOCK_OUTPUT, "%d.%m.%Y", stTmTime);

  return(SUCCESS);
}


int GetDate(char* pAdditionalParam,
            char* pOutputString)
//
// Get date in UTC-time (still existing by compability reason, now we differ between UTC and local).
//
// input: pAdditionalParam: ignored
//
// output: String with date (format dd.mm.yyyy)
//
// return: error-code: SUCCESS: no error
//
{
  GetDateUtc(pAdditionalParam, pOutputString);
}


int GetTzString(char* pAdditionalParam,
                char* pOutputString)
//
// Get TZ-string directly from file TZ.
//
// input: pAdditionalParam: ignored
//
// output: content of TZ-string - must be allocated by calling function!
//
// return: error-code: SUCCESS: no error
//
{
  int   status          = SUCCESS;
  char* pTzFileContent  = NULL;
 
  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

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
      strncpy(pOutputString, pTzFileContent, MAX_LENGTH_CLOCK_OUTPUT);
      CutWord(pOutputString, MAX_LENGTH_CLOCK_OUTPUT);
      //FileContent_Destruct(&pTzFileContent);
      free(pTzFileContent);
    }
  }
  else
  {
    // copy tz-string from file-buffer to output-string
    strncpy(pOutputString, pTzFileContent, MAX_LENGTH_CLOCK_OUTPUT);
    CutWord(pOutputString, MAX_LENGTH_CLOCK_OUTPUT);

    FileContent_Destruct(&pTzFileContent);
  }

  return(status);
}

int GetTzName(char* pAdditionalParam,
                char* pOutputString)
//
// Get TZ-string directly from file TZ.
//
// input: pAdditionalParam: ignored
//
// output: content of TZ-string - must be allocated by calling function!
//
// return: error-code: SUCCESS: no error
//
{
  int   status          = SUCCESS;
  char* pTzFileContent  = NULL;

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

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
    strncpy(pOutputString, pTzFileContent, MAX_LENGTH_CLOCK_OUTPUT);
    CutWord(pOutputString, MAX_LENGTH_CLOCK_OUTPUT);

    FileContent_Destruct(&pTzFileContent);
  }

  return(status);
}

int GetAllTimezonePlainStrings(char* pNrString,
                               char* pOutputString)
{
    UNUSED_PARAMETER(pNrString);

    int status = SUCCESS;
    int cnt = 1;
    char tzStrNr[4] = ""; // up to 999 timezone strings supported

    do
    {
        if(cnt > 999) // very unlikely
        {
            status = INVALID_PARAMETER; 
            break;
        }

        snprintf(tzStrNr, sizeof(tzStrNr), "%d", cnt);

        status = GetTimezonePlainStringByNr(tzStrNr, pOutputString); 

        if(SUCCESS == status)
        {
            if(cnt > 1)
            {
                printf("|");
            }

            printf("%s", pOutputString);
        
            ++cnt; // no endless loop possible: SUCCESS != status is an abort criterium
        }
    }
    while( (SUCCESS == status) && (0 != strcmp("", pOutputString)) );

    if(cnt > 1) // at least one entry was found
    {
        status = SUCCESS; // GetTimezonePlainStringByNr returns "" and ERROR for the one-after-last element
    }

    return status;
}


int GetAllTimezonePlainStringsJson(char* pNrString,
                                   char* pOutputString)
{
    UNUSED_PARAMETER(pNrString);

    int status = SUCCESS;
    int cnt = 1;
    char tzStrNr[4] = ""; // up to 999 timezone strings supported

    printf("[ ");
    do
    {
        if(cnt > 999) // very unlikely
        {
            status = INVALID_PARAMETER;
            break;
        }

        snprintf(tzStrNr, sizeof(tzStrNr), "%d", cnt);

        status = GetTimezonePlainStringByNr(tzStrNr, pOutputString);

        if(SUCCESS == status)
        {
            if(cnt > 1)
            {
                printf(", ");
            }

            printf("\"%s\"", pOutputString);

            ++cnt; // no endless loop possible: SUCCESS != status is an abort criterium
        }
    }
    while( (SUCCESS == status) && (0 != strcmp("", pOutputString)) );

    printf("]");


    if(cnt > 1) // at least one entry was found
    {
        status = SUCCESS; // GetTimezonePlainStringByNr returns "" and ERROR for the one-after-last element
    }

    return status;
}


int GetTimezonePlainStringByNr(char* pNrString,
                               char* pOutputString)
//
// Get plain describtion-text for timezone from the list of all plain-strings which were offered for timezone on WBM.
//
// input: number of text in the list of all possible ones.
//
// output: content of timezone-plain-string - must be allocated by calling function!
//         if a text for this number is not existing, the returned string is empty
//
// return: error-code: SUCCESS: no error
//
{
  int status      = ERROR;
  int timezoneNr  = 0;


  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  // convert the requested timezone-number from input-string
  sscanf(pNrString, "%d", &timezoneNr);
#if 0
  timezoneNr--;
  // check if the timezone-number is existing in timezone-assign-table
  if((timezoneNr >= 0) && (timezoneNr < (sizeof(astTimezoneStringAssign) / sizeof(astTimezoneStringAssign[0]))))
  {
    // copy timezone-string to output-buffer
    strncpy(pOutputString, astTimezoneStringAssign[timezoneNr].plainTimezoneString, MAX_LENGTH_CLOCK_OUTPUT);
  }
#else
  if((timezoneNr > 0))
  {
    int i;
    FILE * fp;
    fp = fopen("/usr/share/zoneinfo/zoneinfos","r");
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
          strncpy(pOutputString, pbuf, MAX_LENGTH_CLOCK_OUTPUT);
        }while(0);
      }
      free(lineBuffer);
      fclose(fp);
    }

  }
#endif

  return(status);
}


int GetActualTimezonePlainString(char* pAdditionalParam,
                                 char* pOutputString)
//
// Get plain describtion-text of the actual configured timezone.
//
// input: number of text in the list of all possible ones.
//
// output: content of actual configured timezone-plain-string - must be allocated by calling function!
//
// return: error-code: SUCCESS: no error
//
{
  char  tzString[MAX_LENGTH_CLOCK_OUTPUT]  = "";
  int   status                             = SUCCESS;

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  status = GetTzName(NULL, tzString);
  strncat(tzString, " ", (MAX_LENGTH_CLOCK_OUTPUT - strlen(tzString) - 1));
  if(SUCCESS == status)
  {
#if 0
    int timezoneIndex = 0;
    
    while((strlen(astTimezoneStringAssign[timezoneIndex].tzString) > 0) && (strlen(pOutputString) == 0))
    {
      if(strcmp(tzString, astTimezoneStringAssign[timezoneIndex].tzString) == 0)
      {
        strncpy(pOutputString, astTimezoneStringAssign[timezoneIndex].plainTimezoneString, MAX_LENGTH_CLOCK_OUTPUT);
      }
      ++timezoneIndex;
    }
#else
    FILE *fp;
    fp = fopen("/usr/share/zoneinfo/zoneinfos","r");
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
        pTZ = strstr(fileBuffer, tzString);
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
              strncpy(pOutputString, ++pPlain, MAX_LENGTH_CLOCK_OUTPUT);
            }
          }
        }
        free(fileBuffer);
      }
      fclose(fp);
    }
#endif
  }

  return(status);
}



int GetTimezoneSelection(char* pAdditionalParam,
                         char* pOutputString)
//
// Show the html-code of a selection-menu about all plain-strings for timezone which where inserted to the list for WBM.
// The actual configured timezone is showed as selected menu-point. If none of the timezone-plain-strings of the list
// represents the actual configured timezone (only the most chosen timezones where represented by the list),
//  an additional line with the text "Unknown" will be showed selected.
//
// input: pAdditionalParam: ignored
//
// output: empty string - all the outputs of the function where directly send to stdout.
//
// return: error-code: SUCCESS: no error
//
{
  char  tzString[MAX_LENGTH_CLOCK_OUTPUT] = "";
  int   status                            = SUCCESS;

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  // get actual configured tz-string
  if(SUCCESS == GetTzString(NULL, tzString))
  {
    char timezonePlainString[MAX_LENGTH_CLOCK_OUTPUT] = "";
    int  timezoneIndex                                = 0;
    int  found                                        = FALSE;

    // start selection-menu
    printf("\n        <select class=\"wide\" name=\"timezone\" size=\"1\">\n");

    // loop over all timezone-plain-strings of our internal list
    while(strlen(astTimezoneStringAssign[timezoneIndex].tzString) > 0)
    {
      // show the string as menu-line; if the string represents the actual configured timezone, show it selected
      if(strcmp(astTimezoneStringAssign[timezoneIndex].tzString, tzString) == 0)
      {
        printf("          <option selected>%s</option>\n", astTimezoneStringAssign[timezoneIndex].plainTimezoneString);
        found = TRUE;
      }
      else
      {
        printf("          <option>%s</option>\n", astTimezoneStringAssign[timezoneIndex].plainTimezoneString);
      }
      ++timezoneIndex;
    }

    // if no timezone was showed selected until now, show an extra-menu-line with text "Unknown" as selected
    if(found == FALSE)
    {
      printf("          <option selected>Unknown</option>\n");
    }
    
    printf("        </select>\n");
  }

  return(status);
}

                                  


int main(int    argc, 
         char** argv)
{
  int   status            = SUCCESS;

  // set TZ variable to ensure locale is usable by date and time functions
  setenv("TZ","/etc/localtime", 1);
  tzset();

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

      // parameter was found -> start processing (get the fitting function from parameter-list)
      if(status == SUCCESS)
      {
        char outputString[MAX_LENGTH_CLOCK_OUTPUT] = "";
        status = astParameterJumptab[parameterIndex].pFktGetParameter(pAdditionalParam, outputString);

        // send result to stdout
        printf("%s", outputString);
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
