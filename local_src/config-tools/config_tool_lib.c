//------------------------------------------------------------------------------
/// Copyright (c) 2000 - 2019 WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     config_tool_lib.c
///
///  \brief    Commonly used config tool defines and functions.
///
///  \author   SM:  WAGO Kontakttechnik GmbH & Co. KG
///  \author   PEn: WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <libudev.h>
#include <glib.h>
#include <unistd.h>

#include "config_tool_lib.h"

//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

#define DEBUG                       0
#define TEST                        0

#define TEMPFILE_NAME_TEMPLATE      "/tmp/configtoolsXXXXXX"

#define MAX_ERROR_TEXT_LENGTH       200


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

char *safe_strncpy(char       * szDest,
                   char const * szSource,
                   size_t       num)
{
  strncpy(szDest, szSource, num-1);
  szDest[num-1] = '\0';

  return szDest;
}

char *FileContent_Get(char const * szFilename)
{
  struct  stat fileAttributes;
  FILE*   fFile                 = NULL;
  char*   szOutputBuffer        = NULL;

  // open tempfile, read its content to an allocated buffer and afterwards delete it
  fFile = fopen(szFilename, "r");

  if(fFile != NULL)
  {
    // get size of tempfile to allocate buffer
    if(stat(szFilename, &fileAttributes) != -1)
    {
      // allocate buffer for all characters of file and additionally end-of-string
      szOutputBuffer = (char*)malloc(fileAttributes.st_size + 1);
      if(szOutputBuffer != NULL)
      {
        // initialize memory with 0 (end-of-string) and read read whole file-content into buffer 
        memset(szOutputBuffer, 0, fileAttributes.st_size + 1);
        fread(szOutputBuffer, fileAttributes.st_size, 1, fFile);
      }
    }

    fclose(fFile);
  }

  #if DEBUG
  if(pOutputBuffer != NULL)
  {
    int   charIndex = 0;

    printf("\nfileAttributes.st_size:%d\n", fileAttributes.st_size);
    printf("strlen:%d\n", strlen(pOutputBuffer));
    for(charIndex = 0; charIndex < strlen(pOutputBuffer); ++charIndex)
    {
      printf("%c", pOutputBuffer[charIndex]);
    }
    printf("\n");
  }
  #endif

  return(szOutputBuffer);
}


void FileContent_Destruct(char * * pszOutputBuffer)
{
  if((pszOutputBuffer != NULL) && (*pszOutputBuffer != NULL))
  {
    free(*pszOutputBuffer);
    *pszOutputBuffer = NULL;
  }
}


int FileContent_GetLineByNr(char * szFileContent,
                            int    requestedLineNr,
                            char * szLineString)
//
// Get one specified line from a buffer which includes a textfile-content
//
// input: pFileOutput: Pointer to buffer with system-call-output
//        requestedLineNr
//        pLineString: Pointer to buffer to return the requested line. It must be allocated by the calling function
//                     with a size of MAX_LINE_LENGTH
//
// return: error-code: SUCCESS or NOT_FOUND, if the line with the requested number is not existing
//
{
  int   status                = SUCCESS;
  int   actualLineNr          = 0;
  int   fileContentCharIndex  = 0;

  // initialize output-string
  szLineString[0]= 0;

  // search for the first character of the requested line in systemcall-buffer
  actualLineNr = 1;
  while((actualLineNr != requestedLineNr) && (status == SUCCESS))
  {
    // check the single characters until line-end is reached
    while((szFileContent[fileContentCharIndex] != '\n') && (status == SUCCESS))
    {
      // if end-of-string was reached before end-of-line -> line-number is not existing, stop search
      if(szFileContent[fileContentCharIndex] == 0)
      {
        status = NOT_FOUND;
        break;
      }
      ++fileContentCharIndex;
    }
    ++actualLineNr;
    ++fileContentCharIndex;
  }

  // check if string is over after the found line feed - no line follows at all
  if((status == SUCCESS) && (szFileContent[fileContentCharIndex] == 0))
  {
    status = NOT_FOUND;
  }

  // if line with requested number is existing in buffer
  if(status == SUCCESS)
  {
    // copy character by character to output-buffer until end-of-line or end-of-string or the end of the output-buffer is reached 
    int lineStringIndex = 0;
    while(   (szFileContent[fileContentCharIndex] != '\n') && (szFileContent[fileContentCharIndex] != 0)
          && (lineStringIndex < (MAX_LINE_LENGTH - 1)) )
    {
      szLineString[lineStringIndex++] = szFileContent[fileContentCharIndex++];
    }
    // add end-of-string to output-buffer
    szLineString[lineStringIndex] = 0;
  }

  return(status);
}




char *SystemCall_GetOutput(char const * szSystemCallString)
//
// Make a system-call and return it's outputs to stdout in a new allocated buffer.
//
// !!! It is absolutly necessairy to call the function DestructSystemCallOutput after handling
// the outputs to free the allocated memory !!!
//
// Input:  string with the system-call
// Return: pointer to the buffer with the systemcall-outputs to stdout
//         NULL if an error occured
//
{
  FILE* fOutputFile   = NULL;
  int   fileIndex     = 0;
  char* pOutputBuffer = NULL;

  char  completeSystemCallString[MAX_LENGTH_SYSTEM_CALL]  = "";
  char  filename[]                                        = TEMPFILE_NAME_TEMPLATE;

  // use mkstemp to immediately create the temporary file to avoid name clashes with other processes
  int hdl = mkstemp(filename);
  // a name for tempfile was found -> do systemcall, redirect its outputs to tempfile
  if (hdl > -1)
  {
    // Close the file again as we only need its name.
    // The file itself will be kept to avoid subsequent calls from using the same name
    close(hdl);

    // add redirection from sytemcall-outputs to the commandline
    snprintf(completeSystemCallString, MAX_LENGTH_SYSTEM_CALL, "%s > %s", szSystemCallString, filename);
    system(completeSystemCallString);

    #if DEBUG
    printf("completeSystemCallString:%s\n", completeSystemCallString);
    #endif

    // read the content of tempfile to an allocated buffer and afterwards delete it
    pOutputBuffer = FileContent_Get(filename);
    remove(filename);
  }
  return(pOutputBuffer);
}


void SystemCall_Destruct(char * * pszOutputBuffer)
{
  FileContent_Destruct(pszOutputBuffer);
}


int SystemCall_GetLineByNr(char * szSystemCallOutput,
                           int    lineNr,
                           char * szLineString)
//
// Get one specified line from a buffer which includes a system-call-output
//
// input: pSystemCallOutput: Pointer to buffer with system-call-output
//        requestedLineNr
//        pLineString: Pointer to buffer to return the requested line. It must be allocated by the calling function
//                     with a size of MAX_LINE_LENGTH
//
// return: error-code: SUCCESS or NOT_FOUND, if the line with the requested number is not existing
//
{
  return(FileContent_GetLineByNr(szSystemCallOutput, lineNr, szLineString));
}


void CutWord(char * szWord,
             int    maxLength)
//
// Cut input-string directly behind the next word, respectivly at the first space which can be found in input-string.
//
// input: pointer to the string to cut
//
{
  int characterIndex = 0;

  // search for not-printable character (spastrlen(ce, tab, line-feed, end-of-string, etc.)
  while((szWord[characterIndex] > 32) && (szWord[characterIndex] <  127) && (characterIndex < maxLength))
  {
    ++characterIndex;
  }

  // if it was found, replace with string-end
  if(characterIndex < maxLength) szWord[characterIndex] = 0;
}


void CutLine(char * szLine,
             int    maxLength)
//
// Cut input-string directly behind the last printable character - remove line feed
//
// input: pointer to the string to cut
//
{
  int characterIndex = 0;

  // search for not-printable character (spastrlen(ce, tab, line-feed, end-of-string, etc.)
  while((szLine[characterIndex] > 31) && (szLine[characterIndex] <  127) && (characterIndex < maxLength))
  {
    ++characterIndex;
  }

  // if it was found, replace with string-end
  if(characterIndex < maxLength) szLine[characterIndex] = 0;
}


char *GetNextWord(char * szString)
//
// Get pointer to the begin of the next word in string
//
{
  char* szNextWord      = NULL;
  int   characterIndex  = 0;

  // check input parameter
  if(szString == NULL)
  {
    return NULL;
  }

  // search for end of actual word (next not printable character)
  while((szString[characterIndex] > 32) && (szString[characterIndex] <  126) && (characterIndex < MAX_LINE_LENGTH))
  {
    ++characterIndex;
  }

  // if it was found, search for start of next word, break if end of string or maximum count of characters are reached
  if((characterIndex < MAX_LINE_LENGTH) && (szString[characterIndex] != '\0'))
  {
    while(((szString[characterIndex] <= 32) || (szString[characterIndex] >= 126)) && (characterIndex < MAX_LINE_LENGTH) && (szString[characterIndex] != 0))
    {
      ++characterIndex;
    }
  }

  // set return pointer only if next word was really found
  if((characterIndex < MAX_LINE_LENGTH) && (szString[characterIndex] != '\0'))
  {
    szNextWord = &szString[characterIndex];
  }

  return szNextWord;
}


char *DeleteMultibleSpaces(char * szInputString,
                           int    maxLength)
//
// Delete multible spaces in a string. After the processing, the strings includes only single spaces.
// The function directly changes the source-string.
//
// input: pInputString: pointer to string which should be changed
//        maxLength: maximum length of characters to write to pInputString (including trailing zero!)
//
// return: returns a pointer to the result-string (= pointer to the input-string)
//
{
  char resultString[MAX_LINE_LENGTH];
  size_t  resultStringIndex                    = 1;
  unsigned int  inputStringIndex               = 1;
  char lastChar                       = szInputString[0];
  
  // copy the first character of string
  resultString[0] = szInputString[0];

  // loop over all characters of input-string
  while((szInputString[inputStringIndex] != 0) && (resultStringIndex < (sizeof(resultString) - 1)))
  {
    // check if not both - the actual character and the last character - were spaces
    if((szInputString[inputStringIndex] != ' ') || (lastChar != ' '))
    {
      // copy character to result-string and memorise the actual character
      resultString[resultStringIndex] = szInputString[inputStringIndex];
      lastChar = szInputString[inputStringIndex];
      ++resultStringIndex;
    }
    ++inputStringIndex;
  }

  // add end-of-string
  resultString[inputStringIndex] = 0;
  
  safe_strncpy(szInputString, resultString, maxLength);

  return(szInputString);
}

//used in elrest-config-tools eDisplay
int GetErrorText(int    status,
                 char * szOut,
                 int    maxLength)
{
  int ret = SUCCESS;
  switch(status)
  {
    case MISSING_PARAMETER:        
      safe_strncpy(szOut, "missing parameter", maxLength);
      break;
    case INVALID_PARAMETER:
      safe_strncpy(szOut, "invalid parameter", maxLength);
      break;
    case FILE_OPEN_ERROR:
      safe_strncpy(szOut, "while opening file", maxLength);
      break;
    case FILE_READ_ERROR:
      safe_strncpy(szOut, "reading file", maxLength);
      break;
    case FILE_CLOSE_ERROR:
      safe_strncpy(szOut, "while closing file", maxLength);
      break;
    case SYSTEM_CALL_ERROR:
      safe_strncpy(szOut, "while System Call", maxLength);
      break;
    case CONFIG_FILE_INCONSISTENT:
      safe_strncpy(szOut, "config-file inconsistent", maxLength);
      break;
    default:                
      ret = ERROR;
      break;
  }

  return (ret);
}

//used in elrest-config-tools eDisplay
int SetLastError(char const * szText)
// Append LastError Text to /tmp/last_error.txt
{
  int status = ERROR;
  FILE * f;
    
  f = fopen(FILE_LAST_ERROR_TXT,"a");
  if (f)
  {
    if (fprintf(f, "%s\n", szText) > 0)
      status = SUCCESS;  
      
    fclose(f);
  } 
     
  return status;
}

void ShowErrorText(int    status,
                   char * szAdditionalInfo)
{
  char errorText[MAX_ERROR_TEXT_LENGTH] = "";

  switch(status)
  {
    case MISSING_PARAMETER:         snprintf(errorText, sizeof(errorText), "missing parameter");        break;
    case INVALID_PARAMETER:         snprintf(errorText, sizeof(errorText), "invalid parameter");        break;
    case FILE_OPEN_ERROR:           snprintf(errorText, sizeof(errorText), "while opening file");       break;
    case FILE_READ_ERROR:           snprintf(errorText, sizeof(errorText), "reading file");             break;
    case FILE_CLOSE_ERROR:          snprintf(errorText, sizeof(errorText), "while closing file");       break;
    case SYSTEM_CALL_ERROR:         snprintf(errorText, sizeof(errorText), "while System Call");        break;
    case CONFIG_FILE_INCONSISTENT:  snprintf(errorText, sizeof(errorText), "config-file inconsistent"); break;
    default:                break;
  }

  printf("Error: %s %s\n", errorText, szAdditionalInfo);
  return;
}


int GetParameterValue(char * szEntireParamString,
                      char * szParamNameString,
                      char * szParamValueString,
                      int    sizeofParamValueString)
//
// Filter the value of a parameter from an parameter string given uri-like
//
// input: pEntireParamString: entire string with all parameter/value-pairs, seperated by '&'
//        pParamNameString: name of the searched parameter
//        pParamValueString: pointer to memory for the value-string to read
//        sizeofParamValueString: maximum length of value-string
//
{
  int   status                = NOT_FOUND;
  char* pFoundParamNameString = NULL;

  if((szParamNameString == NULL) || (szParamValueString == NULL))
  {
    return INVALID_PARAMETER;
  }
  //printf("\npEntireParamString:%s\n", pEntireParamString);

  // look after the position of parameter-name
  pFoundParamNameString = strstr(szEntireParamString, szParamNameString);

  if(pFoundParamNameString != NULL)
  {
    // set pointer behind name-string and check if this char is a '='
    if(*(pFoundParamNameString + strlen(szParamNameString)) != '=')
    {
      status = -1;
    }
    else
    {
      char* pFoundParamValueString = pFoundParamNameString + strlen(szParamNameString) + 1;

      // copy value-string from entire-string char by char until end of value-string (or end of entire param-string)
      int charIndexWrite = 0;
      int charIndexRead  = 0;
      while(   (pFoundParamValueString[charIndexRead] != '&') && (pFoundParamValueString[charIndexRead] != 0)
            && (charIndexWrite < sizeofParamValueString) )
      {
        // look if html-sequence must be decoded
        if(pFoundParamValueString[charIndexRead] == '%') 
        {
          // %3A -> :
          if((pFoundParamValueString[charIndexRead + 1] == '3') && (pFoundParamValueString[charIndexRead + 2] == 'A'))
          {
            szParamValueString[charIndexWrite] = ':';
            charIndexRead += 3;
          }
          // %2F -> /
          else if((pFoundParamValueString[charIndexRead + 1] == '2') && (pFoundParamValueString[charIndexRead + 2] == 'F'))
          {
            szParamValueString[charIndexWrite] = '/';
            charIndexRead += 3;
          }
          // %2C -> ,
          else if((pFoundParamValueString[charIndexRead + 1] == '2') && (pFoundParamValueString[charIndexRead + 2] == 'C'))
          {
            szParamValueString[charIndexWrite] = ',';
            charIndexRead += 3;
          }
          // %25 -> %
          else if((pFoundParamValueString[charIndexRead + 1] == '2') && (pFoundParamValueString[charIndexRead + 2] == '5'))
          {
            szParamValueString[charIndexWrite] = '%';
            charIndexRead += 3;
          }
          // %20 -> ' '
          else if((pFoundParamValueString[charIndexRead + 1] == '2') && (pFoundParamValueString[charIndexRead + 2] == '0'))
          {
            szParamValueString[charIndexWrite] = ' ';
            charIndexRead += 3;
          }
          // % -> %
          else
          {
            szParamValueString[charIndexWrite] = '%';
            charIndexRead += 1;
          }
        }
        else if(pFoundParamValueString[charIndexRead] == '+')
        {
          szParamValueString[charIndexWrite] = ' ';
          charIndexRead += 1;
        }

        else
        {
          szParamValueString[charIndexWrite] = pFoundParamValueString[charIndexRead];
          ++charIndexRead;
        }

        ++charIndexWrite;
      }

      szParamValueString[charIndexWrite] = 0;
      status = SUCCESS;
    }
  }

  return status;
}

// Run one of the predefined udev queries and return element reqIndex from the result.
// Returns number of devices scanned (needed for the special case of
// Get{Device,Partition}ByNumber)
int RunUdevQuery(char              * szBuf,
                 size_t              bufSize,
                 enum eUdevQueryId   id,
                 int                 reqIndex,
                 void              * pParam)
{
  struct udev *udev = NULL;
  struct udev_enumerate *enumerate = NULL;
  struct udev_list_entry *devices = NULL;
  struct udev_list_entry *dev_list_entry = NULL;
  struct udev_device *dev = NULL;

  unsigned int devIndex = 0;

  char devName[MAX_LENGTH_OUTPUT_LINE];

  if(NULL == szBuf)
    return INVALID_PARAMETER;

  *szBuf = '\0';

  udev = udev_new();
  enumerate = udev_enumerate_new(udev);

  switch(id)
  {
  case UDEV_QUERY_BLOCK_PARTITIONS:
  
    udev_enumerate_add_match_subsystem(enumerate,
                                       "block");
    // Using ID_FS_USAGE leaves us with only partitions containing a filesystem.
    // But we do not care about raw partitions here.
    // Using DEVTYPE=partition would not match any Win7-created sd cards as they
    // are created on raw device (i.e. /dev/sdc) without a partition table
    // (matching DEVTYPE=disk).
    udev_enumerate_add_match_property(enumerate,
                                      "ID_FS_USAGE",
                                      "filesystem");
    break;

  case UDEV_QUERY_BLOCK_PARTITIONS_EXT:

    udev_enumerate_add_match_subsystem(enumerate,
                                       "block");

    // file system type: ext2, ext3 etc.
    udev_enumerate_add_match_property(enumerate, 
                                    "ID_FS_TYPE",
                                     (const char*) pParam);
    break;

  case UDEV_QUERY_MTD_PARTITIONS:
  case UDEV_QUERY_MTD_DEVICES: // no difference between devices and partitions
                               // for mtd subsystem.
    udev_enumerate_add_match_subsystem(enumerate, 
                                      "mtd"); 
    udev_enumerate_add_match_sysattr(enumerate, 
                                     "type", 
                                     "nand");
    break;

  case UDEV_QUERY_UBI_PARTITIONS:
    // subsystem == "ubi" && has "name" attribute 
    // => ubi partition 
    udev_enumerate_add_match_subsystem(enumerate, 
                                      "ubi"); 
    udev_enumerate_add_match_sysattr(enumerate, 
                                     "name", NULL);
    break;

  case UDEV_QUERY_UBI_PARTITION_BY_NAME:
    // subsystem == "ubi" && has "name" attribute 
    // => ubi partition

    if(NULL != pParam)
    {

      udev_enumerate_add_match_subsystem(enumerate, 
                                        "ubi"); 
      udev_enumerate_add_match_sysattr(enumerate, 
                                       "name", 
                                       (const char*) pParam);
    }
    else
    {
      // construct empty set
      udev_enumerate_add_match_sysattr(enumerate,
                                     "make_enumerate_an_empty_set",
                                     NULL);
    }
    
    break;

  case UDEV_QUERY_UBI_DEVICES:
    // subsystem == "ubi" && has "volumes_count" attribute 
    // => ubi "parent"
    udev_enumerate_add_match_subsystem(enumerate, 
                                      "ubi"); 
    udev_enumerate_add_match_sysattr(enumerate, 
                                     "volumes_count", NULL);
    break;

  case UDEV_QUERY_BLOCK_DEVICES:
    // returns virtual devices like loop0, too (see below) 
    udev_enumerate_add_match_subsystem(enumerate,
                                       "block");
    udev_enumerate_add_match_property(enumerate, 
                                    "DEVTYPE", 
                                     "disk");

    break;

  case UDEV_QUERY_INTERNAL_FLASH_NAND:
    *devName = '\0';
    ctlib_GetDeviceNameByMedium(INTERNAL_FLASH_NAND, devName, sizeof(devName));
    if(0 != strlen(devName))
    {
      udev_enumerate_add_match_sysname(enumerate, devName);
    }
    else
    {
      // construct empty set
      udev_enumerate_add_match_sysattr(enumerate,
                                     "make_enumerate_an_empty_set",
                                     NULL);
    }
    break;

  default:
    // construct an empty set ( => illegal id is an error anyway)
    udev_enumerate_add_match_sysattr(enumerate,
                                     "make_enumerate_an_empty_set",
                                     NULL);
  }

  udev_enumerate_scan_devices(enumerate);
  devices = udev_enumerate_get_list_entry(enumerate);

  udev_list_entry_foreach(dev_list_entry, devices) {

    // filter entries based on query type
    const char *path;
    path = udev_list_entry_get_name(dev_list_entry);
    
    switch(id)
    {
      case UDEV_QUERY_BLOCK_DEVICES:

        // ignore virtual block devices (loop, ram etc.)

	// if the udev enumeration changes, its possible that we
	// get mtdblock devices in this query. If the internal flash
	// device is enumerated after the mtdblock device the corresponding
	// create_bootable_image will never show the internal flash as selection
	// option. So just ignore this mtdblock devices during enumeration of
	// block devices.
        if(path == strstr(path, "/sys/devices/virtual/") || strstr(path, "mtdblock"))
        {
          continue;
        }
        break;

      case UDEV_QUERY_BLOCK_PARTITIONS:
      case UDEV_QUERY_BLOCK_PARTITIONS_EXT:
      case UDEV_QUERY_MTD_DEVICES:
      case UDEV_QUERY_MTD_PARTITIONS:
      case UDEV_QUERY_UBI_DEVICES:
      case UDEV_QUERY_UBI_PARTITIONS:
      case UDEV_QUERY_UBI_PARTITION_BY_NAME:

      default:
        break;
    }
    
    if(devIndex++ == (unsigned)reqIndex)
    {
      dev = udev_device_new_from_syspath(udev, path);
      break;
    }

//    ++devIndex;
  }

  if(dev)
  {
    const char *nodeName = udev_device_get_devnode(dev);
    
    if(strlen(nodeName) < bufSize)
    {
      safe_strncpy(szBuf, nodeName, bufSize);
    }
    // TODO: else: return error?

    udev_device_unref(dev);
  }

  if(enumerate)
    udev_enumerate_unref(enumerate);

  if(udev)
    udev_unref(udev);

  return devIndex;
}

// Converts ubi device name to the corresponding mtd device name
// Example: ubi0 -> mtd2
int UbiToMtd(char const * szUbiDev,
             char       * szMtdDev,
             size_t       mtdDevSize)
{
  int ret = ERROR;
  struct udev *udev = NULL;
  struct udev_device *dev = NULL;
  struct stat statBuf;

  bool appendDevPrefixToRes = false; // if false, '/dev/' prefix is suppressed in result.
                                     // if true, result string retains the '/dev/' prefix
                                     // when it is present in input string.

  do{

    if(szMtdDev == NULL)
    {
      break;
    }

    udev = udev_new();
    
    // TODO: refactor (device creation copied from get_???_data.c)
    char szUbiDevPath[strlen("/dev/") + strlen(szUbiDev) + 1];

    if(szUbiDev != strstr(szUbiDev, "/dev/"))
    { // ubiDev has no '/dev/' prefix => construct full path
      safe_strncpy(szUbiDevPath, "/dev/", sizeof(szUbiDevPath));
      strcat(szUbiDevPath, szUbiDev);
    }
    else
    {
      appendDevPrefixToRes = true;
      safe_strncpy(szUbiDevPath, szUbiDev, sizeof(szUbiDevPath));
    }

    // create device object
    if(0 != stat(szUbiDevPath, &statBuf) ||
       !S_ISCHR(statBuf.st_mode)) 
    {
      break;
    }

    dev = udev_device_new_from_devnum(udev, 'c', statBuf.st_rdev);

    const char *mtd_nr = NULL;

    if(NULL == dev)
    {
     break;
    } 

    // 1st check: /dev/ubiX
    // 2nd check: /dev/ubiX_Y
    if(NULL == (mtd_nr = udev_device_get_sysattr_value(dev, "mtd_num")) &&
       NULL == (mtd_nr = udev_device_get_sysattr_value(udev_device_get_parent(dev),
                                                      "mtd_num") ) )
    {
      break;
    }

    size_t lenResult = 0;
   
    if(appendDevPrefixToRes)
    {
      lenResult += strlen("/dev/");
    }

    lenResult += strlen("mtd" + strlen(mtd_nr) + 1);    

    if( mtdDevSize < lenResult)
    {
      break;
    }

    *szMtdDev = '\0';
   
    // size of mtdDev is checked above => strcat safe here 
    if(appendDevPrefixToRes)
    {
      strcat(szMtdDev, "/dev/");
    }
    
    strcat(szMtdDev, "mtd");
    strcat(szMtdDev, mtd_nr);

    ret = SUCCESS;

  } while(0);

  if(udev)
  {
    udev_unref(udev);
  }

  if(dev)
  {
    udev_device_unref(dev);
  }

  return ret;
}

static const char* GetMediumNameFromMediaLine(const char* mediaLine)
{
  const char* result = NULL;
  int i=0;
  do
  {
    if(mediaLine[i] == SEPARATOR[0])
    {
      result = &mediaLine[i+1]; //do not trim -> spaces are not allowed here
    }
  } while((result == NULL) && (mediaLine[i++] != '\0'));
  return result;
}

int SearchLineInDeviceMediumFile(char const * szSearchText,
                                 char       * szFoundLine)
//
// Search for a line including a given text in device media file
//
// input: pSearchText: string with searched text
// output: pFoundLine: whole line which includes the searched text or empty string, if text was not found
//                     - must be allocated by calling function!
// return: errorcode
//
{
  int   status                  = NOT_FOUND;
  char* pDeviceMediaFileContent = NULL;

  if((szSearchText == NULL) || (szFoundLine == NULL))
  {
    return INVALID_PARAMETER;
  }

  // Initialize output string
  *szFoundLine = '\0';

  // get content of device description-file
  pDeviceMediaFileContent = FileContent_Get(DEVICE_MEDIA_FILENAME);

  if(pDeviceMediaFileContent == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    char  deviceMediaLine[MAX_LENGTH_OUTPUT_LINE] = "";
    int   lineNo                                  = 0;

    // loop over the lines of device/media file until we find the line with the line with the searched string
    lineNo = 1;
    while(   (SUCCESS == FileContent_GetLineByNr(pDeviceMediaFileContent, lineNo, deviceMediaLine))
          && (strlen(szFoundLine) == 0))
    {
      if(deviceMediaLine[0] != COMMENT_CHAR)
      {
        //try to match pSearchedMedium as a device name
        int lenSearchedMedium = strlen(szSearchText);
        if(   (lenSearchedMedium < (sizeof deviceMediaLine))
           && (strncmp(szSearchText, deviceMediaLine, lenSearchedMedium) == 0)
           && (deviceMediaLine[lenSearchedMedium] == SEPARATOR[0]))
        {
          status = SUCCESS;
        }
        else
        {
          //try to match pSearchedMedium as a medium name
          const char* medName = GetMediumNameFromMediaLine(deviceMediaLine);

          // if line is no comment and searched medium matches medium in current device media line
          if((medName != NULL) && (strcmp(medName, szSearchText) == 0))
          {
            status = SUCCESS;
          }
        }
        if(SUCCESS == status)
        {
          safe_strncpy(szFoundLine, deviceMediaLine, MAX_LENGTH_OUTPUT_LINE);
          break;
        }
      }
      ++lineNo;
    }

    FileContent_Destruct(&pDeviceMediaFileContent);
  }

  return status;
}


int ctlib_GetDeviceNameByMedium(char const * pacRequestedMedium,
                                char       * pacDeviceName,
                                size_t       pacDeviceNameLength)
//
// Get the according medium to a device given by name.
//
// input: pacRequestedMedium: string with device medium (cf-card, internal-flash, usb1, usb2)
// output : pacDeviceName: string with according device name (hda, hdb, ...)
//         - must be allocated by calling function!
// return: errorcode
//
{
  int   status                                    = NOT_FOUND;
  char  acDeviceMediaLine[MAX_LENGTH_OUTPUT_LINE] = "";
  //printf("pacRequestedMedium:%s\n", pacRequestedMedium);

  // check input parameter
  if((pacRequestedMedium == NULL) || (pacDeviceName == NULL))
  {
    return INVALID_PARAMETER;
  }

  // initialise output-string
  *pacDeviceName = '\0';

  // search for line with the device name in device media file (comment line already filtered)
  if(SUCCESS == SearchLineInDeviceMediumFile(pacRequestedMedium, acDeviceMediaLine))
  {
    // search for the requested device
    char* pMediumString = strstr(acDeviceMediaLine, pacRequestedMedium);
    //printf("acDeviceMediaLine:%s pMediumString:%s\n", acDeviceMediaLine, pMediumString);

    // if medium string was found
    if(pMediumString != NULL)
    {
      // get index where medium string starts in line
      int mediumStringCharIndex = pMediumString - acDeviceMediaLine;
      //printf("mediumStringCharIndex:%d\n", mediumStringCharIndex);

      // get the characters in front of medium string one by one, to find the last character in front of separator
      // jump over spaces in front of medium string and stop at the latest at first char in string
      while(mediumStringCharIndex > 0)
      {
        --mediumStringCharIndex;
        if(acDeviceMediaLine[mediumStringCharIndex] != ' ')
        {
          if(strncmp(&acDeviceMediaLine[mediumStringCharIndex], SEPARATOR, strlen(SEPARATOR)) == 0)
          {
            break;
          }
        }
      }

      // if we found separator and characters are left in front of it - copy them as device name
      if(mediumStringCharIndex > 0 &&
        (unsigned)mediumStringCharIndex < pacDeviceNameLength)
      {
        strncpy(pacDeviceName, acDeviceMediaLine, mediumStringCharIndex);
        pacDeviceName[mediumStringCharIndex] = '\0';
        status = SUCCESS;
      }
    }
  }

  return status;
}

// Manage .ini type config files (currently a wrapper for glib's key-value-parser)

/**
 * @brief Opens a configuration resource (i.e. a config file).
 *
 * @param ressource A string with the resource description (i.e. path to file).
 * @param config    A double pointer to the config representation.
 * @param priv      An arbitrary pointer.
 *
 * @return SUCCESS if the configuration could be initialised, 
 *         FILE_OPEN_ERROR otherwise.
 */
int ctlib_ConfigOpen(const char * szRessource,
                     config_t   * pConfig,
                     void       * pPrivate)
{
  (void) pPrivate;

  int ret = SUCCESS;
  GError *error = NULL;

  GKeyFile *keyFile = g_key_file_new();

  if( NULL != keyFile &&
      TRUE == g_key_file_load_from_file( keyFile, 
                                         szRessource,
                                         G_KEY_FILE_KEEP_COMMENTS,
                                         &error) )
  {
    *pConfig = keyFile;
  }
  else
  {
    ret = FILE_OPEN_ERROR;
  }

  return ret;
}

/**
 * @brief Closes a configuration opened by ConfigOpen.
 *
 * @param config Double pointer to config representation.
 */
void ctlib_ConfigClose(config_t config)
{
  g_key_file_free((GKeyFile*) config);
}

/**
 * @brief FIXME
 *
 * @param config
 * @param key
 * @param value
 * @param priv
 *
 * @return 
 */
int ctlib_ConfigGetString(config_t       config,
                          char const   * pKey,
                          char       * * pszValue,
                          void         * pPrivate)
{
  int ret = SUCCESS;
  char const * szGroup = (char const *) pPrivate;
//  GError *error = NULL;
  
  *pszValue = g_key_file_get_string((GKeyFile *) config,
                                 szGroup,
                                 pKey,
                                 NULL);

  if(NULL == *pszValue)
  {
    ret = NOT_FOUND;
  }

  return ret;
}

/**
 * @brief FIXME
 *
 * @param config
 * @param key
 * @param value
 * @param priv
 */
void ctlib_ConfigSetString(config_t     config,
                           char const * pKey,
                           char const * pValue,
                           void       * pPrivate)
{
  char const * szGroup = (char const *) pPrivate;

  g_key_file_set_string((GKeyFile*) config,
                        szGroup,
                        pKey,
                        pValue);
}

/**
 * @brief FIXME
 *
 * @param portConfig
 * @param priv
 *
 * @return 
 */
int ctlib_ConfigSaveChanges(config_t   portConfig,
                            void     * pPrivate)
{

  int ret = SUCCESS;
  char const * szConfigFile = (char const * ) pPrivate;
  
  GError *error = NULL;
  gsize fileLen = 0;
  
  gchar *contents = g_key_file_to_data((GKeyFile*) portConfig, &fileLen, &error);

  if(NULL == error)
  {
    g_file_set_contents(szConfigFile,
                        contents,
                        fileLen,
                        &error);
    
  }

  if(NULL != error)
  {
    ret = SYSTEM_CALL_ERROR;
  }

  g_free(contents);    

  return ret;
}


/**
 * @brief Check for valid domain names.
 *
 * A domain name string has maximum length of 255 chararcters. The string consists of labels each max 63
 * characters long, separated by dots.
 *
 * @param name  Domain name to check.
 *
 * @return != 0 (true)  Domain name valid
 *         0 (false)    Domain name invalid 
 */

// Map character code to character class for state machine to check a domain name. Available classes:
//   0 = Illegal character
//   1 = digits
//   2 = alphabetic
//   3 = Hyphen
//   4 = Label terminating '.'
//   5 = String terminating '\0'
static char dom_cclass[] = 
{
  5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
  0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0,
  0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// States of the domain name check routine.
typedef enum
{
  ERR = -1,
  STOP = 0,
  START,
  INLABEL,
  HYPHEN                         // Input character was a hyphen
} dom_state;

// Domain check state transition table.
static dom_state dom_transitions[] =
{
  /*             ill. digit    letter   -       .      \0 */
  /* STOP */     ERR, ERR,     ERR,     ERR,    ERR,   ERR,
  /* START */    ERR, ERR,     INLABEL, ERR,    ERR,   ERR,
  /* INLABEL */  ERR, INLABEL, INLABEL, HYPHEN, START, STOP,
  /* HYPHEN */   ERR, INLABEL, INLABEL, HYPHEN, ERR,   ERR,
};

// Statemachine to check domain/host name according to RFC 1034.
int ctlib_VerifyDomainName(char const * szName)
{
  bool isname = true;
  int labelidx = 0;
  int charidx = 0;
  dom_state state = START;
  while(state > 0 && isname && charidx <= 255)
  {
    unsigned int c = *szName & 255;
    int ccl = dom_cclass[c];
    dom_state nstate = dom_transitions[state * 6 + ccl];
    switch(state)
    {
    case START:
      isname = (ccl == 2);
      labelidx = 0;
      break;
    case INLABEL:
      isname = (ccl > 0);
      break;
    case HYPHEN:
      isname = (ccl == 1 || ccl == 2 || ccl == 3);
      break;
    default:
      break;
    }
    state = nstate;
    szName++;
    labelidx++;
    charidx++;
  }
  return isname && state == STOP && labelidx <= 64;
}

/**
 * @brief Convert ip address from ascii to unsigned int
 *
 * Not using inet_aton() in favor of more strict formal checks.
 * Set result to NULL for checking only.
 *
 * @param ipaddr    Input string.
 * @param resultp   Pointer to the uint32 output variable.
 *
 * @return status value SUCCESS if call succeeded. Error code otherwise.
 */
int ctlib_IpaddrToInt(char const * szIpAddr,
                      uint32_t   * pResult)
{
  int i;
  char **decimalparts = g_strsplit(szIpAddr, ".", 0);
  int status = SUCCESS;
  unsigned result = 0;

  if(4 == g_strv_length(decimalparts))
  {
    for(i = 0; i < 4; i++)
    {
      char *p = decimalparts[i];
      unsigned part;
      if('\0' != *p)
      {
        while(*p != '\0')
        {
          if(!isdigit(*p))
          {
              status = INVALID_PARAMETER;
              break;
          }
          p++;
        }
        if(SUCCESS != status)
        {
          break;
        }
        part = (unsigned)strtoul(decimalparts[i], NULL, 10);
        if(part <= 255)
        {
          result = (result << 8) + part;
        }
        else
        {
          status = INVALID_PARAMETER;
          break;
        }
      }
      else
      {
        status = INVALID_PARAMETER;
        break;
      }
    }
  }
  else
  {
    status = INVALID_PARAMETER;
  }
  if(SUCCESS == status && NULL != pResult)
  {
    *pResult = result;
  }
  return status;
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

#if TEST
void TestCutWord(void)
{
  char testWord[2 * MAX_LINE_LENGTH];
  char referenceWord[2 * MAX_LINE_LENGTH];

  printf("Test CutWord\n");

  // CASE 1: word is longer than maximum size (-> no change)
  memset(testWord, 'x', sizeof(testWord));
  testWord[MAX_LINE_LENGTH] = ' ';
  memcpy(referenceWord, testWord, sizeof(referenceWord));

  CutWord(testWord);
  if(memcmp(testWord, referenceWord, sizeof(testWord)) != 0)
  {
    printf("Error CASE 1\n");
  }

  // CASE 2: space at last possible index (-> cut at space)
  memset(testWord, 'x', sizeof(testWord));
  memset(referenceWord, 'x', sizeof(referenceWord));
  testWord[MAX_LINE_LENGTH - 1] = ' ';
  referenceWord[MAX_LINE_LENGTH - 1] = 0;

  CutWord(testWord);
  if(memcmp(testWord, referenceWord, sizeof(testWord)) != 0)
  {
    printf("Error CASE 2\n");
  }

  // CASE 3: Cut tab
  memset(testWord, 'x', sizeof(testWord));
  testWord[10] = 9;
  memset(referenceWord, 'x', sizeof(referenceWord));
  referenceWord[10] = 0;

  CutWord(testWord);
  if(memcmp(testWord, referenceWord, sizeof(testWord)) != 0)
  {
    printf("Error CASE 3\n");
  }

  // CASE 4: Cut lf
  memset(testWord, 'x', sizeof(testWord));
  testWord[10] = '\n';
  memset(referenceWord, 'x', sizeof(referenceWord));
  referenceWord[10] = 0;

  CutWord(testWord);
  if(memcmp(testWord, referenceWord, sizeof(testWord)) != 0)
  {
    printf("Error CASE 4\n");
  }

  // CASE 5: Cut string-end (-> no change)
  memset(testWord, 'x', sizeof(testWord));
  testWord[10] = 0;
  memset(referenceWord, 'x', sizeof(referenceWord));
  referenceWord[10] = 0;

  CutWord(testWord);
  if(memcmp(testWord, referenceWord, sizeof(testWord)) != 0)
  {
    printf("Error CASE 5\n");
  }
  
}

void TestConfigToolLib(void)
{
  printf("Test ConfigToolLib\n");
  TestCutWord();
}

#endif
