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
///  \file     get_touchscreen_config.c
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

#include "config_tool_lib.h"

//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

#define SHOW_ERRORS                         0

#define DEVICES_FILENAME                    "/proc/bus/input/devices"
//#define DEVICES_FILENAME                    "/etc/config-tools/devices"
#define TOUCHSCREEN_DRIVER_FILENAME         "/etc/ts.conf"

//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------

// Pointer to funktion to search the requested parameter
typedef int (*tFktGetParameter)(int parameterNr, char* outputString);


// structure to join the possible input-strings with the processing-function to get them
typedef struct
{
  // input-string from command-line
  char               inputString[MAX_LENGTH_INPUT_STRING];

  // processing function to get the requested parameter
  tFktGetParameter   pFktGetParameter;

} tParameterJumptab;



//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------

// declaration of all processing functions used for jumptab
int GetCalibrateTouchscreenFlag(int   parameterNr, 
                                char* pOutputString);

int GetActualTouchscreenDevice(int   parameterNr,
                               char* pOutputString);

int GetMouseDeviceNameByNr(int   requestedDeviceNr,
                           char* pOutputString);

int GetMouseDeviceSelection(int   parameterNr,
                            char* pOutputString);

int GetActualTouchscreenDriverName(int   parameterNr,
                                   char* pOutputString);

int GetTouchscreenDriverNameByNr(int   requestedDriverNr,
                                 char* pOutputString);

int GetTouchscreenDriverNameSelection(int   parameterNr,
                                      char* pOutputString);

int GetDeviceNotAvailableInfo(int   parameterNr,
                              char* pOutputString);

int GetMouseConfig(int   parameterNr,
                   char* pOutputString);

int GetMousePointerActive(int   parameterNr,
                          char* pOutputString);

int GetDisplayState(int   parameterNr,
                    char* pOutputString);

int GetCleanmodeState(int   parameterNr,
                      char* pOutputString);

// array of all possible requested parameter (input-strings and the processing-functions to get them)
static tParameterJumptab astParameterJumptab[] =
{
  { "calibrate-touchscreen-flag", GetCalibrateTouchscreenFlag },
  { "device-name",                GetActualTouchscreenDevice },
  { "device-name-by-nr",          GetMouseDeviceNameByNr },
  { "device-name-selection",      GetMouseDeviceSelection },
  { "driver-name",                GetActualTouchscreenDriverName },
  { "driver-name-by-nr",          GetTouchscreenDriverNameByNr },
  { "driver-name-selection",      GetTouchscreenDriverNameSelection },
  { "device-not-available-info",  GetDeviceNotAvailableInfo },
  { "mouse-pointer",              GetMouseConfig },
  { "mouse-pointer-active",       GetMousePointerActive },
  { "display-state",              GetDisplayState },
  { "cleanmode-state",            GetCleanmodeState },

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

  printf("\n* Get several informations about touchscreen-configuration *\n\n");
  printf("Usage: get_touchscreen_config <parameter>\n\n");
  printf("parameter: ");

  // read the possible strings for parameter from array and show them like this: "param1 | param2 | param3"
  while(strlen(astParameterJumptab[parameterIndex].inputString) > 0)
  {
    if(parameterIndex != 0) printf("| ");
    printf("%s ", astParameterJumptab[parameterIndex].inputString);
    ++parameterIndex;
  }
  printf("\n\n");
}



int GetCalibrateTouchscreenFlag(int   parameterNr,
                                char* pOutputString)
//
// Get the information for a html-checkbox, if the calibration of touchscreen at next start is adjusted
// (= flag-file is existing)
//
// input: parameterNr: not used
//
// output: pOutputString: "checked", if calibration is adjusted, else an empty string
//
{
  FILE* fFile = NULL;
  
  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // try to open flag-file to check if it is existing
  fFile = fopen("/etc/config-tools/ts_calibrate_flag_file", "r");
  if(fFile != NULL)
  {
    // flag-file exists, write "checked" to stdout 
    strncpy(pOutputString, "checked", MAX_LENGTH_OUTPUT_STRING);
    fclose(fFile);
  }

  return(SUCCESS);
}



int GetConfigFileParameterByNr(int   requestedEntryNr,
                               char* pParameterLabel,
                               char* pConfigFileContent,
                               char* pOutputString)
//
// Get a parameter-value from devices-file or ts.conf for a specified parameter-group-number by a specified parameter-label-string.
// Function can be used for every config-file with groups of parameter-definitions seperatet by empty lines. are
//
// input: requestedDeviceNr: number of device
//        pParameterLabel: Label-string which intoduces the parameter
//        pDevicesContent: Buffer with the content of the devices-file
//
// output: pOutputString: String with the parameter-value, must be allocated by calling-function with lengt MAX_LENGTH_OUTPUT_STRING
//
// return: error-code: SUCCESS if parameter-value was found
//
{
  char  configFileLine[MAX_LINE_LENGTH] = "";
  int   status                          = SUCCESS;
  int   lineNr                          = 0;
  int   entryNr                         = 0;
  
  // check input-parameter
  if((requestedEntryNr == 0) || (pParameterLabel == NULL) || (pConfigFileContent == NULL) || (pOutputString == NULL))
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  // loop over the lines of config-file-content, until the requested parameter was found
  // stop if end of file was reached or the actual tested entry-number is already bigger than the requested one
  status    = NOT_FOUND;
  lineNr    = 1;
  entryNr  = 1;
  while(   (SUCCESS == SystemCall_GetLineByNr(pConfigFileContent, lineNr, configFileLine)) 
        && (strlen(pOutputString) == 0) 
        && (entryNr <= requestedEntryNr))
  {
    //printf("lineNr:%d entryNr:%d strlen:%d %s\n", lineNr, entryNr, strlen(configFileLine), configFileLine);
    // if the actual line is in the area of the requested entry-number (each entry-nr includes several parameter-definitions)
    if(entryNr == requestedEntryNr)
    {
      char* pNameString        = NULL;

      // check if the requested parameter is discribed in the actual line and if so, filter its value
      pNameString = strstr(configFileLine, pParameterLabel);
      if(pNameString != NULL)
      {
        pNameString = pNameString + strlen(pParameterLabel) + 1;
        strncpy(pOutputString, pNameString, MAX_LENGTH_OUTPUT_STRING);
        status = SUCCESS;
      }
    }

    // an empty line denotes the end of the definitions for the actual entry and the start of the next
    if(strlen(configFileLine) == 0)
    {
      ++entryNr;
    }
    ++lineNr;
  }

  return(status);
}


int GetMouseDeviceNameByNr(int   requestedMouseDeviceNr,
                           char* pOutputString)
//
// Get the name of a device for a mouse specified by its numer in the devices-file.
// Devices which have no "mouse" in their handler-string are ignored.
//
// input: requestedMouseDriverNr
// 
// output: String with the name of the requested device without the bordering quotation-marks
//         must be allocated by calling-function with lengt MAX_LENGTH_OUTPUT_STRING
//
{
  char  systemCallString[MAX_LENGTH_SYSTEM_CALL]  = "";
  int   status                                    = SUCCESS;
  char* pDevicesContent                           = NULL;
  
  //printf("requestedMouseDeviceNr:%d\n", requestedMouseDeviceNr);
  // check input-parameter
  if((pOutputString == NULL) || (requestedMouseDeviceNr == 0)) 
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  // get the content of the devices-file in a buffer by a cat-systemcall
  snprintf(systemCallString, MAX_LENGTH_SYSTEM_CALL, "cat %s", DEVICES_FILENAME);
  pDevicesContent = SystemCall_GetOutput(systemCallString);

  if(pDevicesContent == NULL)
  {
    status = FILE_OPEN_ERROR;
  }
  else
  {
    char  devicesNameString[MAX_LINE_LENGTH]      = "";
    char  devicesHandlersString[MAX_LINE_LENGTH]  = "";
    int   deviceNr                                = 0;
    int   mouseDeviceNr                           = 0;
    
    // loop over all devices (including a set of parameter-definitions) in devices-file
    // stop if the requested name was found or all devices have been tested
    deviceNr = 1;
    mouseDeviceNr = 1;
    while((status == SUCCESS) && (strlen(pOutputString) == 0))
    {
      //printf("deviceNr:%d mouseDeviceNr:%d\n", deviceNr, mouseDeviceNr);

      // get the name-string and the handler-string of the actual tested device
      if(   (SUCCESS != GetConfigFileParameterByNr(deviceNr, "Name", pDevicesContent, devicesNameString))
         || (SUCCESS != GetConfigFileParameterByNr(deviceNr, "Handlers", pDevicesContent, devicesHandlersString)) )
      {
        // if the actual device is no more included in devices-file -> we couldn't find name, stop loop
        status = NOT_FOUND;
      }
      else
      {
        //printf("devicesNameString:%s\n", devicesNameString);
        //printf("devicesHandlersString:%s\n", devicesHandlersString);

        // check if actual device is mouse-device (some devices are no mouse-devices and we are not interested in them)
        if(strstr(devicesHandlersString, "mouse") != NULL)
        {
          //printf("mouse-device\n");
          // check if it is the requested one
          if(mouseDeviceNr == requestedMouseDeviceNr)
          {
            // copy device-name-string to output, in doing so, cut the first and afterwards the final quotation-mark
            strncpy(pOutputString, devicesNameString + 1, MAX_LENGTH_OUTPUT_STRING);
            pOutputString[strlen(pOutputString) - 1] = 0;
          }
          ++mouseDeviceNr;
        }
      }
      ++deviceNr;
    }

    SystemCall_Destruct(&pDevicesContent);
  }

  return(status);
}


int GetTouchscreenDriverNameByNr(int   requestedDriverNr,
                                 char* pOutputString)
//
// Get the name of a driver specified by its numer in the ts.conf-file.
//
// input: requestedDriverNr
// 
// output: String with the name of the requested driver, must be allocated by calling-function with lengt MAX_LENGTH_OUTPUT_STRING
//
{
  int   status          = SUCCESS;
  char* pTsConfContent  = NULL;
  
  // check input-parameter
  if((pOutputString == NULL) || (requestedDriverNr == 0)) 
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  // get the content of the ts.conf-file in a buffer 
  pTsConfContent = FileContent_Get(TOUCHSCREEN_DRIVER_FILENAME);

  if(pTsConfContent == NULL)
  {
    status = FILE_OPEN_ERROR;
  }
  else
  {
    status = GetConfigFileParameterByNr(requestedDriverNr, "drivername:", pTsConfContent, pOutputString);
    FileContent_Destruct(&pTsConfContent);
  }

  return(status);
}


int GetActualUsedTsConfParameter(char* pIntroducingString,
                                 char* pOutputString)
//
// Get a parameter-value of the actual selected touchscreen-driver according to file ts.conf.
//
// input: pIntroducingString: string in front of the searched parameter (normally "# devicename: " or "# drivername:")
//
// output: pOutputString: String with the actual used parameter
//         must be allocated by calling-function with lengt MAX_LENGTH_OUTPUT_STRING
// 
{
  int   status          = SUCCESS;
  char* pTsConfContent  = NULL;
  
  // check input-parameter
  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  // get the content of the ts.conf-file in a buffer 
  pTsConfContent = FileContent_Get(TOUCHSCREEN_DRIVER_FILENAME);

  if(pTsConfContent == NULL)
  {
    status = FILE_OPEN_ERROR;
  }
  else
  {
    char  tsConfLine[MAX_LINE_LENGTH] = "";
    int   lineNr                      = 0;

    // loop over all lines of ts-conf-file and look after the actual used driver and its definitions
    lineNr = 1;
    while((SUCCESS == FileContent_GetLineByNr(pTsConfContent, lineNr, tsConfLine)) && (strlen(pOutputString) == 0)) 
    {
      // if the actual line contains the actual driver-defitition (no comment-character and module_raw as introducing string)
      if((tsConfLine[0] != COMMENT_CHAR) && (strstr(tsConfLine, "module_raw") != NULL))
      {
        // search for the requested parameter several lines in front of the found line by a loop over the single lines
        // stop also if line can't be read or an empty line is reached (= end of the parameter-group of this driver)
        int lineOffset = 1;
        while(   (SUCCESS == FileContent_GetLineByNr(pTsConfContent, lineNr - lineOffset, tsConfLine)
              && (strlen(tsConfLine) > 0) && (strlen(pOutputString) == 0)) )
        {
          // if line containes the searched parameter, filter its value behind the introducing string
          if(strstr(tsConfLine, pIntroducingString) != NULL)
          {
            strncpy(pOutputString, tsConfLine + strlen(pIntroducingString), MAX_LENGTH_OUTPUT_STRING);
          }
          ++lineOffset;
        }
      }
      ++lineNr;
    }

    FileContent_Destruct(&pTsConfContent);
  }

  return(status);
}


int GetActualTouchscreenDriverName(int   parameterNr,
                                   char* pOutputString)
//
// Get the actual selected touchscreen-driver according to file ts.conf.
//
// input: parameterNr: not used
//
// output: pOutputString: String with the actual used touchscreen-driver
//         must be allocated by calling-function with lengt MAX_LENGTH_OUTPUT_STRING
// 
{
  return(GetActualUsedTsConfParameter("# drivername: ", pOutputString));
}



int GetActualTouchscreenDevice(int   parameterNr,
                               char* pOutputString)
//
// Get the actual selected touchscreen-device according to file ts.conf.
//
// input: parameterNr: not used
//
// output: pOutputString: String with the actual used touchscreen-device
//         must be allocated by calling-function with lengt MAX_LENGTH_OUTPUT_STRING
// 
{
  return(GetActualUsedTsConfParameter("# devicename: ", pOutputString));
}


int GetMouseDeviceSelection(int   parameterNr,
                            char* pOutputString)
//
// Write html-selection-menu about all possible mouse-device-names to stdout. The actual device-name which is given 
// by file ts.conf is showed as the selected line. If the actual device-name is not existing in devices-file at the moment,
// his name is although written in a single option-line, added with the string "currently not available".
//
// input: parameterNr: not used
//
// output: pOutputString: not used
//
{
  int   status                                      = SUCCESS;
  char  actualDeviceName[MAX_LENGTH_OUTPUT_STRING]  = "";
  char  deviceNameString[MAX_LENGTH_OUTPUT_STRING]  = "";

  status = GetActualTouchscreenDevice(0, actualDeviceName);

  if(SUCCESS == status)
  {
    int mouseDeviceNr    = 0;
    int actualValueFound = FALSE;

    printf("\n              <select name=\"device-name\" size=\"1\" >\n");

    // loop over all possible device-names, read them and show them as an option for the selection-menu
    mouseDeviceNr = 1;
    while(SUCCESS == GetMouseDeviceNameByNr(mouseDeviceNr, deviceNameString))
    {
      // if this device-name is the actual selected (by ts.conf), show the line as selected
      if(strcmp(deviceNameString, actualDeviceName) == 0)
      {
        printf("                <option value=\"%s\" selected >%s</option>\n", deviceNameString, deviceNameString);
        actualValueFound = TRUE;
      }
      else
      {
        printf("                <option value=\"%s\">%s</option>\n", deviceNameString, deviceNameString);
      }
      ++mouseDeviceNr;
    }

    // if the actual selected name can't be found in devices-file, show its name in an extra (selected) option-line with added information
    if(actualValueFound == FALSE)
    {
      printf("                <option value=\"not available\" selected >%s (currently not available)</option>\n", actualDeviceName);
    }

    printf("              </select>\n");
  }
  
  return(status);
}



int GetTouchscreenDriverNameSelection(int   parameterNr,
                                      char* pOutputString)
//
// Write html-selection-menu about all possible touchscreen-driver-names from ts.conf to stdout. 
// The actual driver-name is showed as the selected line. 
//
// input: parameterNr: not used
//
// output: pOutputString: not used
//
{
  int   status                                        = SUCCESS;
  char  actualTsDriverName[MAX_LENGTH_OUTPUT_STRING]  = "";           
  char  tsDriverNameString[MAX_LENGTH_OUTPUT_STRING]  = "";

  status = GetActualTouchscreenDriverName(0, actualTsDriverName);
  if(SUCCESS == status)
  {
    int tsDriverNr       = 0;
    int actualValueFound = FALSE;

    printf("\n              <select name=\"driver-name\" size=\"1\" >\n");

    // loop over all possible driver-names, read them and show them as options for the selection-menu
    tsDriverNr = 1;
    while(SUCCESS == GetTouchscreenDriverNameByNr(tsDriverNr, tsDriverNameString))
    {
      // if this name is the actual selected by ts.conf, show it selected in the menu
      if(strcmp(tsDriverNameString, actualTsDriverName) == 0)
      {
        printf("                <option selected value=\"%s\" >%s</option>\n", tsDriverNameString, tsDriverNameString);
        actualValueFound = TRUE;
      }
      else
      {
        printf("                <option value=\"%s\">%s</option>\n", tsDriverNameString, tsDriverNameString);
      }
      ++tsDriverNr;
    }

    // if the actual name can't be found in ts.conf (must be an error), show an addional (selected) option "Unknown"
    if(actualValueFound == FALSE)
    {
      printf("                <option value=\"Unknown\" selected >Unknown</option>\n");
    }

    printf("              </select>\n");
  }

  return(status);
}


int GetDeviceNotAvailableInfo(int   parameterNr,
                              char* pOutputString)
//
// Get an according info-text, if the actual selected device is not available at the moment. 
//
// input: parameterNr: not used
//
// output: pOutputString: string "(currently not available)", if device is not available
//         else an empty string
//
//
{
  int   status                                      = SUCCESS;
  char  actualDeviceName[MAX_LENGTH_OUTPUT_STRING]  = "";
  int   deviceFound                                 = FALSE;

  // check input-parameter
  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  if(SUCCESS == GetActualTouchscreenDevice(0, actualDeviceName))
  {
    char deviceNameString[MAX_LENGTH_OUTPUT_STRING] = "";

    int mouseDeviceNr = 1;
    while(SUCCESS == GetMouseDeviceNameByNr(mouseDeviceNr, deviceNameString) && (deviceFound == FALSE))
    {
      if(strcmp(deviceNameString, actualDeviceName) == 0)
      {
        deviceFound = TRUE;
      }
      ++mouseDeviceNr;
    }
  }

  if(deviceFound == FALSE)
  {
    strncpy(pOutputString, "(currently not available)", MAX_LENGTH_OUTPUT_STRING);
  }

  return(status);
}


int GetMouseConfig(int   parameterNr,
                   char* pOutputString)
//
// Get the string which the state of the touchscreen-mouse-konfiguration.
//
// input: parameterNr: not used
//
// output: string with the touchscreen-mouse-config - must be allocated by calling function!
//
// return: error-code
//
{
  int   status                = SUCCESS;
  char* pCodesysFileContent   = NULL;

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  // get content of revisions-file
  pCodesysFileContent = FileContent_Get("/etc/init.d/codesys");

  if(pCodesysFileContent == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    char  codesysLine[MAX_LENGTH_OUTPUT_LINE]               = "";
    char  beginOfMouseConfigString[MAX_LENGTH_OUTPUT_LINE]  = "TSLIB_MOUSEPOINTER=";
    int   lineNr                                            = 0;

    // loop over the lines of codesys-file until we find the line with the line with the mouse-string
    lineNr = 1;
    while((SUCCESS == FileContent_GetLineByNr(pCodesysFileContent, lineNr, codesysLine)) && (strlen(pOutputString) == 0))
    {
      //printf("%s\n", codesysLine);
      // if the line is no comment and introducing string for mousepointer-config was found
      if((codesysLine[0] != COMMENT_CHAR) && (strstr(codesysLine, beginOfMouseConfigString) != NULL))
      {
        // read the following mouse-config-string and remove possible line-feed or similar behind it
        strncpy(pOutputString, strstr(codesysLine, beginOfMouseConfigString) + strlen(beginOfMouseConfigString),
                MAX_LENGTH_OUTPUT_STRING);
        CutWord(pOutputString, MAX_LENGTH_OUTPUT_STRING);
      }
      ++lineNr;
    }

    FileContent_Destruct(&pCodesysFileContent);
  }

  return(status);
}


int GetMousePointerActive(int   parameterNr,
                          char* pOutputString)
//
// Get the string "checked" (for html-checkbox) if the touchscreen-mouse-pointer is activated in config-file.
//
// input: parameterNr: not used
//
// output: string "checked" or empty string - must be allocated by calling function!
//
// return: error-code
//
{
  int   status                = SUCCESS;

  char  mouseConfigString[MAX_LENGTH_OUTPUT_STRING] = "";

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  if(SUCCESS == GetMouseConfig(0, mouseConfigString) && (strcmp(mouseConfigString, "Yes") == 0))
  {
    strncpy(pOutputString, "checked", MAX_LENGTH_OUTPUT_STRING);
  }

  return(status);
}


int GetDisplayState(int   parameterNr,
                        char* pOutputString)
{
  int   status        = SUCCESS;
  char* pDisplayState = NULL;

  if(pOutputString == NULL)
  {
    return INVALID_PARAMETER;
  }

  // Initialize output-string
  sprintf(pOutputString, "on");

  // get content of revisions-file
  if(NULL != (pDisplayState = FileContent_Get("/tmp/display_state")))
  {
    CutWord(pDisplayState, MAX_LENGTH_OUTPUT_STRING);
    strncpy(pOutputString, pDisplayState, MAX_LENGTH_OUTPUT_STRING);
    FileContent_Destruct(&pDisplayState);
  }

  return status;
}


int GetCleanmodeState(int   parameterNr,
                      char* pOutputString)
{
  int   status          = SUCCESS;
  char* pCleanmodeState = NULL;

  if(pOutputString == NULL)
  {
    return INVALID_PARAMETER;
  }

  // Initialize output-string
  sprintf(pOutputString, "off");

  // get content of revisions-file
  if(NULL != (pCleanmodeState = FileContent_Get("/tmp/cleanmode_state")))
  {
    CutWord(pCleanmodeState, MAX_LENGTH_OUTPUT_STRING);
    strncpy(pOutputString, pCleanmodeState, MAX_LENGTH_OUTPUT_STRING);
    FileContent_Destruct(&pCleanmodeState);
  }

  return status;
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
      char* pParameterString  = NULL;

      // check if parameter is contained in parameter-list and get its index 
      pParameterString  = argv[1];

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
        char  outputString[MAX_LENGTH_OUTPUT_STRING]  = "";
        int   parameterNr                             = 0;

        if(argc > 2) sscanf(argv[2], "%d", &parameterNr);

        status = astParameterJumptab[parameterIndex].pFktGetParameter(parameterNr, outputString);
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
