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
///  \file     show_video_mode.c
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

// filename of menu.lst-config-file
#define MENU_LST_FILENAME                 "/boot/grub/menu.lst"

// filename of file with the video-mode definitions
#define VIDEO_MODES_FILENAME              "/etc/VIDEO_MODES"

#define LINE_IDENTIFICATION_STRING        "kernel"
#define VGA_DEF_INIT_STRING               "vga="
#define COMMENT_CHAR                      '#'
#define LF                                0x0a

#define MAX_LENGTH_VGA_NR_STRING          10
#define MAX_LENGTH_CONFIG_LINE            200
#define MAX_LENGTH_VIDEO_MODES_LINE       200


//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------

// Pointer to funktion to search the requested parameter
typedef int (*tFktGetParameter)(char* pOoutputString, 
                                char* pAdditionalParam);


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

int GetActualVideoModeNr(char* pOutputString,
                         char* pAdditionalParam);

int GetVideoModeStringByIndex(char* pOutputString,
                              char* pVideoModeIndex);

int GetVideoModeNumberByIndex(char* pOutputString,
                              char* pVideoModeIndex);

int GetActualVideoModeString(char* pOutputString,
                             char* pAdditionalParam);

int GetVideoModeSelection(char* pOutputString,
                          char* pAdditionalParam);

// array of all possible requested parameter (input-strings, strings to search for, processing-function to get them)
static tParameterJumptab astParameterJumptab[] =
{
  { "number",           GetActualVideoModeNr },
  { "by-index",         GetVideoModeStringByIndex },
  { "number-by-index",  GetVideoModeNumberByIndex },
  { "string",           GetActualVideoModeString },
  { "selection",        GetVideoModeSelection },

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

  printf("\n* Get informations about the actual and possible video-modes *\n\n");
  printf("Usage: show_video_mode <parameter> [video-mode-number]\n\n");

  printf("parameter: ");

  // read the possible strings for parameter from array and show them like this: "param1 | param2 | param3"
  while(strlen(astParameterJumptab[parameterIndex].inputString) > 0)
  {
    if(parameterIndex != 0) printf("| ");
    printf("%s ", astParameterJumptab[parameterIndex].inputString);
    ++parameterIndex;
  }

  printf("\n");
  printf("video-mode-number: only nessecairy for parameter \"by-index\"\n");
  printf("\n");
  return;
}
 


int GetActualVgaNr(int* pOutVgaNr)
//
// Get the actual configured VGA-number from menu.lst-file as ingeter-value.
//
// output: pOutVgaNr: VGA-number
//
// return: error-code
//
{
  char* pMenuLstFileContent = NULL;
  int   status              = SUCCESS;

  // check input-parameter
  if(pOutVgaNr == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialize return-value
  *pOutVgaNr = 0;

  // get content of menu.lst-file
  pMenuLstFileContent = FileContent_Get("/boot/grub/menu.lst");

  if(pMenuLstFileContent == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    char  menuLstLine[MAX_LENGTH_OUTPUT_LINE] = "";
    int   lineNr                              = 0;

    // loop over the lines of menu.lst-file until we find the line with the line with the kernel-definitions
    lineNr = 1;
    while((SUCCESS == FileContent_GetLineByNr(pMenuLstFileContent, lineNr, menuLstLine)) && (*pOutVgaNr == 0))
    {   
      char  vgaDefIntroString[] = "vga=";
      //printf("%s\n", menuLstLine);

      // if the line is no comment and introducing string for vga was found
      if((menuLstLine[0] != COMMENT_CHAR) && (strstr(menuLstLine, vgaDefIntroString) != NULL))
      {
        char* pVgaNrString = NULL;
  
        // filter vga-nr-string from line
        pVgaNrString = strstr(menuLstLine, vgaDefIntroString) + strlen(vgaDefIntroString);
        CutWord(pVgaNrString, MAX_LENGTH_OUTPUT_LINE);
        //printf("vgaNrString:%s\n", pVgaNrString);

        // convert vga-nr-string to integer-value
        if(strstr(pVgaNrString, "0x") == NULL)
        {
          sscanf(pVgaNrString, "%d", pOutVgaNr);
        }
        else
        {
          sscanf(pVgaNrString, "%x", pOutVgaNr);
        }
      }

      ++lineNr;
    }

    FileContent_Destruct(&pMenuLstFileContent);

    // check if we really found a vga-nr, else set status to error
    if(*pOutVgaNr == 0)
    {
      status = NOT_FOUND;
    }
  }
  
  return(status);
}


int GetActualVideoModeNr(char* pOutputString,
                         char* pAdditionalParam)
//
// Get the number of the actual configured video-mode.
//
// input: pAdditionalParam: ignored
//
// output: string with the actual configured video-mode-number in hexadecimal notation.
//
// return: error-code
//
{
  int status  = SUCCESS;
  int vgaNr   = 0;

  // get the actual configured video-mode-number and convert it to a string in hexadecimal notation
  status = GetActualVgaNr(&vgaNr);
  if(SUCCESS == status) snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING, "0x%0x", vgaNr);

  return(status);
}



int GetVideoModeValuesByIndex(int   requestedVideoModeIndex,
                              int*  pVideoModeNr,
                              char* pVideoModeString)
//
// Get the belonging together values video-mode-number and video-mode-string from video-modes-file for a specified sequential index.
//
// input: videoModeIndex: sequential number of the video-mode in video-modes-string
//
// output: pVideoModeNr: video-mode-number, 0 if index was not found
//         pVideoModeString: video-mode-string, empty if index was not found
//
// return: error-code
//
{
  char* pVideoModesFileContent  = NULL;
  int   status                  = SUCCESS;

  // check input-parameter
  if((pVideoModeNr == NULL) || (pVideoModeString == NULL))
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-values
  sprintf(pVideoModeString, "");
  *pVideoModeNr = 0;

  // get content of menu.lst-file
  pVideoModesFileContent = FileContent_Get("/etc/VIDEO_MODES");

  if(pVideoModesFileContent == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    char  videoModesLine[MAX_LENGTH_OUTPUT_LINE]  = "";
    int   lineNr                                  = 0;
    int   videoModeIndex                          = 0;

    // loop over the lines of videomode-file until we find the line with the line with the requested video-mode
    lineNr          = 1;
    videoModeIndex  = 1;
    status          = NOT_FOUND;
    while((SUCCESS == FileContent_GetLineByNr(pVideoModesFileContent, lineNr, videoModesLine)) && (NOT_FOUND == status))
    {   
      // if the line is no comment and introducing string for vga was found
      if((videoModesLine[0] != COMMENT_CHAR) && (strlen(videoModesLine) > 0))
      {
        // if actual video-mode is the requested one
        if(videoModeIndex == requestedVideoModeIndex)
        {
          status = SUCCESS;

          // string in front of the first ':' in video-mode-line contains the video-mode-number, convert it to int
          sscanf(videoModesLine, "%d:", pVideoModeNr);

          // now get the VGA-string - it starts behind first ':'
          char* pVgaString = strchr(videoModesLine, ':') + 1;

          // cut possible linefeed from vga-string
          if(pVgaString[strlen(pVgaString)-1] == LF)
          {
            pVgaString[strlen(pVgaString)-1] = 0;
          }

          // jump over possible spaces in front of vga-string (shift startpointer to first real character or to end-of-string)
          while((*pVgaString == ' ') && (*pVgaString != 0))
          {
            ++pVgaString;
          }

          // copy vga-string to output-variable
          strcpy(pVideoModeString, pVgaString);
        }

        ++videoModeIndex;
      }

      ++lineNr;
    }      

    FileContent_Destruct(&pVideoModesFileContent);
  }

  return(status);
}


int GetVideoModeStringByIndex(char* pOutputString,
                              char* pVideoModeIndexString)
//
// Get the video-mode-string from video-modes-file belonging to a specified index in file
//
// input: pVideoModeIndexString: string with the requested index
//
// output: pOutputString: string with the video-mode
//
// return: error-code
//
{
  char* pVideoModesFileContent  = NULL;
  int   status                  = SUCCESS;
  int   videoModeIndex          = 0;
  int   videoModeNrDummy        = 0;

  // check input-parameter
  if((pOutputString == NULL) || (pVideoModeIndexString == NULL))
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  // convert requested video-mode-nr from string to int
  sscanf(pVideoModeIndexString, "%d", &videoModeIndex);

  // get the requested video-mode-string and write it to output-variable
  status = GetVideoModeValuesByIndex(videoModeIndex, &videoModeNrDummy, pOutputString);

  return(status);
}
  
int GetVideoModeNumberByIndex(char* pOutputString,
                              char* pVideoModeIndexString)
//
// Get the video mode number from video modes file belonging to a specified index in file.
// Note that the returned mode number is a string and it's provided in hex format, prefixed with "0x"
//
// input: pVideoModeIndexString: string with the requested index
//
// output: pOutputString: string with the video mode number
//
// return: error-code
//
{
  int   status                                  = SUCCESS;
  int   videoModeIndex                          = 0;
  int   vgaNo                             = 0;
  char  videoModeString[MAX_LENGTH_OUTPUT_LINE] = "";

  // check input-parameter
  if((pOutputString == NULL) || (pVideoModeIndexString == NULL))
  {
    return INVALID_PARAMETER;
  }

  // initialise output-string
  sprintf(pOutputString, "");

  // convert requested video-mode-nr from string to int
  sscanf(pVideoModeIndexString, "%d", &videoModeIndex);

  // get the requested video mode number and write it to output-variable in hex format
  if(SUCCESS == (status = GetVideoModeValuesByIndex(videoModeIndex, &vgaNo, videoModeString)))
  {
    snprintf(pOutputString, MAX_LENGTH_OUTPUT_STRING, "0x%0x", vgaNo);
  }

  return status;
}



int GetActualVideoModeString(char* pOutputString,
                             char* pAdditionalParam)
//
// Get the string which describes the actual used video-mode
//
// input: pAdditionalParam: ignored
//
// output: pOutputString: String with the actual used video-mode
//
// return: error-code
//
{
  int status            = SUCCESS;
  int actualVideoModeNr = 0;  

  // check input-parameter
  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");
  
  // get the number of the actual used video-mode
  status = GetActualVgaNr(&actualVideoModeNr);

  if(SUCCESS == status)
  { 
    char  videoModeString[MAX_LENGTH_OUTPUT_STRING] = "";
    int   videoModeIndex                            = 0;
    int   configFileVideoModeNr                     = 0;

    // loop over all possible video-modes in video-modes-list-file, get respectivly the video-mode-number and the -string
    videoModeIndex = 1;
    while((SUCCESS == GetVideoModeValuesByIndex(videoModeIndex, &configFileVideoModeNr, videoModeString)) && (strlen(pOutputString) == 0))
    {
      // if the video-mode-number belonging to the actual index is the actual video-mode-number...
      if(configFileVideoModeNr == actualVideoModeNr)
      {
        // ... copy the corresponding video-mode-string to output-variable
        strncpy(pOutputString, videoModeString, strlen(videoModeString));
      }
      ++videoModeIndex;
    }
  }
    
  return(status);
}


int GetVideoModeSelection(char* pOutputString,
                          char* pAdditionalParam)
//
// Show a selection-menu for html-page with all video-modes in video-mode-list-file. The line with the actual 
// configured video-mode is showed as selected.
//
// input: pAdditionalParam: ignored
//
// output: pOutputString: empty string; all nessecairy ouputs where made direktly to stdout
//
// return: error-code
//
{
  int status            = SUCCESS;
  int actualVideoModeNr = 0;  

  // check input-parameter
  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");
  
  // get the number of the actual used video-mode
  status = GetActualVgaNr(&actualVideoModeNr);

  if(SUCCESS == status)
  { 
    char  videoModeString[MAX_LENGTH_OUTPUT_STRING] = "";
    int   videoModeIndex                            = 0;
    int   configFileVideoModeNr                     = 0;
    int   videoModeNrFound                          = FALSE;

    // start selection-menu 
    printf("\n              <select name=\"video-mode\" size=\"1\" >");

    // loop over all possible video-modes in video-modes-list-file, get respectivly the video-mode-number and the -string
    videoModeIndex = 1;
    while(SUCCESS == GetVideoModeValuesByIndex(videoModeIndex, &configFileVideoModeNr, videoModeString))
    {
      // write html-option line; show line as selected, if the video-mode-nr from the actual index is the actual used one
      if(configFileVideoModeNr == actualVideoModeNr)
      {
        printf("\n                <option value=\"0x%x\" selected >%s</option>", configFileVideoModeNr, videoModeString);
        videoModeNrFound = TRUE;
      }
      else
      {
        printf("\n                <option value=\"0x%x\">%s</option>", configFileVideoModeNr, videoModeString);
      }

      ++videoModeIndex;
    }

    // if the actual used video-mode-nr was not contained in video-modes-list...
    if(videoModeNrFound == FALSE)
    {
      // ... show an additional menu-line with text "Unknown" which is selected 
      printf("\n                <option value=\"0\" selected >Unknown</option>");
    }

    printf("\n              </select>\n");
  }
    
  return(status);
}



int main(int    argc, 
         char** argv)
{
  int   status  = SUCCESS;
  int   vgaNr   = 0;

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
      char* pParameterString    = NULL;
      char* pVideoModeNrString  = NULL;

      if(argc > 2) pVideoModeNrString = argv[2];

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
        char outputString[MAX_LENGTH_OUTPUT_STRING] = "";
        status = astParameterJumptab[parameterIndex].pFktGetParameter(outputString, pVideoModeNrString);
        printf("%s", outputString);
      }
    }
  }

  return(status);
}
