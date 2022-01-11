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
///  \file     get_device_data.c
///
///  \version  $Revision: 59625 $1
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

#include <sys/stat.h>
#include <libudev.h>

#include <stdint.h>

#include "config_tool_lib.h"

//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

#define SHOW_ERRORS                         0

#define NAME_INPUT_STRING                   "name"
#define MEDIUM_INPUT_STRING                 "medium"

#ifdef  __CPPUTEST__ 
#define main get_device_data_main
#endif

//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------

// Pointer to funktion to search the requested parameter
typedef int (*tFktGetParameter)(char* inputString,
                                char* outputString);


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
int GetDeviceMediumByName(char* pacName,
                          char* pacMedium);

int GetDeviceNameByMedium(char* pacMedium,
                          char* pacName);

int GetBootflagByName(char* pacName,
                      char* pacBootflag);

int GetEntireSizeByName(char* pacName,
                        char* pacSize);

int GetLabelByName(char* pacName,
                   char* pacSize);

int GetPureDeviceNameExternal(char* pacName,
                              char* pacSize);

int GetPureDeviceName(char*   pacDeviceName,
                      char*   pacPureDeviceName,
                      size_t  sizeofPureDeviceName);

// array of all possible requested parameter (input-strings, strings to search for, processing-function to get them)
static tParameterJumptab astParameterJumptab[] =
{
  { "medium",           GetDeviceMediumByName },
  { "name",             GetDeviceNameByMedium },
  { "bootflag",         GetBootflagByName },
  { "size",             GetEntireSizeByName },
  { "label",            GetLabelByName },  
  { "pure-device-name", GetPureDeviceNameExternal },

  // this line must always be the last one - don't remove it!
  { "", NULL }
};


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// external functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------

// This wrapper for GetPureDeviceName is to be called from main only.
// That's why we can make an assumption about the result buffer's size.

int GetPureDeviceNameExternal(char *pacDeviceName,
                              char *pacPureDeviceName)
{
  return GetPureDeviceName(pacDeviceName, pacPureDeviceName, MAX_LENGTH_OUTPUT_STRING);
}

int GetPureDeviceName(char*   pacDeviceName,
                      char*   pacPureDeviceName,
                      size_t  sizeofPureDeviceName)
//
// Get pure device name without "/dev/" string and partition number.
//
// input: pacDeviceName: device name with our without path and partition number
//        siezofPureDeviceName: size of string to write result string
// output: pacPureDeviceName: string for result device name
// return: errorcode
//
{
  int status = INVALID_PARAMETER;

  struct udev *udev = NULL;
  struct udev_device *dev = NULL;

  struct stat statBuf;
  
  if(NULL == pacDeviceName ||
     NULL == pacPureDeviceName ||
     0 == sizeofPureDeviceName)
  {
    return status;
  }

  if(NULL == (udev = udev_new()))
  {
    return SYSTEM_CALL_ERROR;
  }

  *pacPureDeviceName = '\0';
  
  // pacDeviceName is passed through by first-level functions which are
  // called from main. The size of pacDeviceName is set in main to
  // MAX_LENGTH_OUTPUT_STRING. Take care when invoking GetPureDeviceName
  // from other functions to not blast the stack by passing too long a 
  // string! 
  char devPath[strlen("/dev/") + strlen(pacDeviceName) + 1];

  *devPath = '\0';

  if(pacDeviceName != strstr(pacDeviceName, "/dev/"))
  { // pacDeviceName has no '/dev/' prefix => construct full path
    strcat(devPath, "/dev/");
    strcat(devPath, pacDeviceName);
  }
  else
  {
    strcat(devPath, pacDeviceName);
  }

  // create device object
  if(0 == stat(devPath, &statBuf)) 
  {
    if(S_ISBLK(statBuf.st_mode)) // hdX/sdX/mmcblkX
    {
      dev = udev_device_new_from_devnum(udev, 'b', statBuf.st_rdev);
    }
    else if(S_ISCHR(statBuf.st_mode)) // ubiX_Y
    {
      dev = udev_device_new_from_devnum(udev, 'c', statBuf.st_rdev);
    }
  }

  // evaluate device object
  if(NULL != dev)
  {
    const char *devType = NULL;
    const char *sysName = NULL;
    if(NULL != (devType = udev_device_get_devtype(dev))) // block/mtd devices
    {

      if(0 == strcmp("disk", devType) ||
         0 == strcmp("mtd", devType) ) // "real" devices, no partitions
      {
        // return name of device (is stored in property named KERNEL)

        sysName = udev_device_get_sysname(dev); 

        if(NULL != sysName)
        { 
          strncpy(pacPureDeviceName, 
                  sysName,
                  sizeofPureDeviceName - 1);
          pacPureDeviceName[sizeofPureDeviceName - 1] = '\0';
        }
        status = SUCCESS;
      }
      else if(0 == strcmp("partition", devType))
      {

        // return parent's name
        struct udev_device *parent = udev_device_get_parent(dev);
        if(NULL != parent)
        {
          sysName = udev_device_get_sysname(parent); 

          if(NULL != sysName)
          {
            strncpy(pacPureDeviceName, 
                sysName,
                sizeofPureDeviceName - 1);

            pacPureDeviceName[sizeofPureDeviceName - 1] = '\0';
            
            status = SUCCESS;
          }
          // DONT cleanup parent device as it is attached to the child
          // and is cleaned up when the child is released!
          // (see libudev-device.c)
          // Uncommenting next line causes a double free and a subsequent
          // SEGFAULT
          //udev_device_unref(parent);
        }
      }
    }
    else // ubi subsystems have no devtype field :(
    {
      const char *subsystem = udev_device_get_subsystem(dev);

      if(NULL != subsystem)
      {
      
        if(0 == strcmp("ubi", subsystem) &&
               (NULL != udev_device_get_sysattr_value(dev, "name")) )
        {
          // ubifs partition is queried
          // => return physical (mtd) parent's name
          struct udev_device *parent = udev_device_get_parent(dev);
          if(NULL != parent)
          {
            sysName = udev_device_get_sysname(parent); 
            status = UbiToMtd(sysName, pacPureDeviceName, sizeofPureDeviceName);
          }
          // DONT cleanup parent device as it is attached to the child
          // and is cleaned up when the child is released!
          // (see libudev-device.c)
          // Uncommenting next line causes a double free and a subsequent
          // SEGFAULT
          //udev_device_unref(parent);
        }
      } // end of (NULL != subsystem)
    } // end of "device has no devtype"
    udev_device_unref(dev);
  }

  udev_unref(udev);

  return status;
}



int GetDeviceMediumByName(char* pacRequestedDeviceName,
                          char* pacDeviceMedium)
//
// Get the according medium to a device given by name.
//
// input: pacRequestedDeviceName: string with device name (hda, hdb, ...)
// output: string with according device medium (cf-card, internal-flash, usb1, usb2)
//         - must be allocated by calling function!
// return: errorcode
//
{
  int   status                                    = NOT_FOUND;
  char  deviceMediaLine[MAX_LENGTH_OUTPUT_LINE]   = "";
  char  acPureDeviceName[MAX_LENGTH_OUTPUT_LINE]  = "";
  //printf("pacRequestedDeviceName:%s\n", pacRequestedDeviceName);

  // check input parameter
  if((pacRequestedDeviceName == NULL) || (pacDeviceMedium == NULL))
  {
    return INVALID_PARAMETER;
  }

  // initialise output-string
  pacDeviceMedium[0] = '\0';

  if(SUCCESS == (status = GetPureDeviceName(pacRequestedDeviceName, acPureDeviceName, sizeof(acPureDeviceName))))
  {
    // search for line with the device name in device media file (comment line already filtered)
    if(SUCCESS == (status = SearchLineInDeviceMediumFile(acPureDeviceName, deviceMediaLine)))
    {
      // search for the requested device
      char* pDeviceString = strstr(deviceMediaLine, acPureDeviceName);
      //printf("deviceMediaLine:%s pDeviceString:%s\n", deviceMediaLine, pDeviceString);

      // if device string was found
      if(pDeviceString != NULL)
      {
        // jump behind device string and check if seperator follows
        pDeviceString += strlen(acPureDeviceName);

        if(strncmp(pDeviceString, SEPARATOR, strlen(SEPARATOR)) == 0)
        {
          // jump behind seperator and leading spaces
          pDeviceString += strlen(SEPARATOR);
          while((*pDeviceString == ' ') && (*pDeviceString != '\0') && (*pDeviceString != '\n'))
          {
            ++pDeviceString;
          }

          // copy device description (until end of string or line feed)
          strncpy(pacDeviceMedium, pDeviceString, MAX_LENGTH_OUTPUT_STRING);
          status = SUCCESS;
        }
      }
    }
  }

  return status;
}

int GetDeviceNameByMedium(char* pacRequestedMedium,
                                char* pacDeviceName)
{
//
// Get the according medium to a device given by name.
//
// input: pacRequestedMedium: string with device medium (cf-card, internal-flash, usb1, usb2)
// output : pacDeviceName: string with according device name (hda, hdb, ...)
//         - must be allocated by calling function!
// return: errorcode
//
  // Assumption: sizeof((char[]) pacDeviceName)) == MAX_LENGTH_OUTPUT_STRING (it is the case in main())
  return ctlib_GetDeviceNameByMedium(pacRequestedMedium, pacDeviceName, MAX_LENGTH_OUTPUT_STRING);
}

int DeviceExisting(char* pacDeviceName)
//
// Checks if the given device is existing by reading /proc/partitions.
//
// input: pacDeviceName
// return: 1: device is existing
//         0: device is not existing
//
{
  int deviceExisting = 0;

  if(pacDeviceName != NULL)
  {
    char  acBashCall[MAX_LENGTH_OUTPUT_LINE]    = "";
    char* pacBashOutput                         = NULL;

    // check by a system, if the device name can be found in partition list
    snprintf(acBashCall, sizeof(acBashCall), "cat /proc/partitions | grep \"%s\"", pacDeviceName);
    if(NULL != (pacBashOutput = SystemCall_GetOutput(acBashCall)))
    {
      // grep returns an output, if name is found - else no output is existing, return flag must still be 0
      if(strlen(pacBashOutput)) deviceExisting = 1;
      SystemCall_Destruct(&pacBashOutput);
    }
  }

  return deviceExisting;
}


int GetBootflagByName(char* pacRequestedDeviceName,
                      char* pacBootflag)
//
// Check if bootflag is set for a given device.
//
// input: pacRequestedDeviceName: name of device, accepted with or without path and partition number
// output:  pacBootflag: string with bootflag - "0" or "1"
//
{
  int   status = SUCCESS;

  char  acPureDeviceName[MAX_LENGTH_OUTPUT_LINE] = "";
  //printf("pacRequestedDeviceName:%s\n", pacRequestedDeviceName);

  // check input parameter
  if((pacRequestedDeviceName == NULL) || (pacBootflag == NULL))
  {
    status = INVALID_PARAMETER;
  }

  // initialise output-string
  pacBootflag[0] = '\0';

  // filter pure device name without path or partition number
  if(SUCCESS == (status = GetPureDeviceName(pacRequestedDeviceName, acPureDeviceName, sizeof(acPureDeviceName))))
  {
    // check if device is existing, else set error
    if(!DeviceExisting(acPureDeviceName))
    {
      status = INVALID_PARAMETER;
    }
    else
    {
      char  acFdiskCall[MAX_LENGTH_OUTPUT_LINE]  = "";
      char* pacFdiskOutput                       = NULL;

      // check if bootflag is set for device by system call
      // sfdisk returns a list of all partitions, with an '*' in a line if bootflag is set for this partition
      // error output of sfdisk must be discardet, because it also includes a '*'
      snprintf(acFdiskCall, sizeof(acFdiskCall), "sfdisk /dev/%s -l 2> /dev/null | grep \\*", acPureDeviceName);
      if(NULL == (pacFdiskOutput = SystemCall_GetOutput(acFdiskCall)))
      {
        status = FILE_READ_ERROR;
      }
      else
      {
        // grep returns an ouput (line with the searched text), if '*' is found -> bootflag is set!
        if(strlen(pacFdiskOutput))
        {
          snprintf(pacBootflag, MAX_LENGTH_OUTPUT_STRING, "1");
        }
        else
        {
          snprintf(pacBootflag, MAX_LENGTH_OUTPUT_STRING, "0");
        }

        SystemCall_Destruct(&pacFdiskOutput);
      }
    }
  }

  return status;
}


int GetEntireSizeByName(char* pacRequestedDeviceName,
                        char* pacSize)
//
// Get entire size of a device.
//
// input: pacRequestedDeviceName: name of device, accepted with or without path and partition number
// output:  pacSize: string with entife size
//
{
  int   status = ERROR;

  struct udev *udev = NULL;
  struct udev_device *dev = NULL;

  const char * attrSizeName = NULL;
  const char * attrSizeValue = NULL;
  int factor = 1; // factor to convert the attribute's value to KBytes
  double usablePercentage = 1.0;

  struct stat statBuf;

  char devPath[strlen("/dev/") + strlen(pacRequestedDeviceName) + 1];
  
  // check input parameter
  if((pacRequestedDeviceName == NULL) || (pacSize == NULL))
  {
    status = INVALID_PARAMETER;
  }

  // initialise output-string
  pacSize[0] = '\0';

  if(pacRequestedDeviceName != strstr(pacRequestedDeviceName, "/dev/"))
  { // pacDeviceName has no '/dev/' prefix => construct full path
    strncpy(devPath, "/dev/", sizeof(devPath));
    strcat(devPath, pacRequestedDeviceName);
  }
  else
  {
    strncpy(devPath, pacRequestedDeviceName, sizeof(devPath));
  }

  if(NULL == (udev = udev_new()))
  {
    status = SYSTEM_CALL_ERROR;
  }
  else
  {
    // create device object
    if(0 == stat(devPath, &statBuf)) 
    {
      if(S_ISBLK(statBuf.st_mode)) // hdX/sdX/mmcblkX
      {
        dev = udev_device_new_from_devnum(udev, 'b', statBuf.st_rdev);
        attrSizeName = "size";
        factor = 2; // size is given in 512 bytes large blocks.
      }
      else if(S_ISCHR(statBuf.st_mode)) // mtdX or ubiX_Y
      { 
        dev = udev_device_new_from_devnum(udev, 'c', statBuf.st_rdev);

        if(NULL != strstr(pacRequestedDeviceName, "mtd"))
        {
            attrSizeName = "size";
            factor = 1024;
            usablePercentage = 0.95; // ubi reserves part of mtd device
        }
        else if (NULL != strstr(pacRequestedDeviceName, "ubi"))
        {
          attrSizeName = "data_bytes";
          factor = 1024;
        }
        else // unknown char device
        {
          udev_device_unref(dev);
          dev = NULL;
        }
      }
    }

    if(NULL != dev)
    {

      if(NULL != (attrSizeValue = udev_device_get_sysattr_value(dev, attrSizeName)))
      {

        uint64_t size = strtoll(attrSizeValue, NULL, 10);
        if(0 != size)
        {
          uint64_t sizeKb = size / factor * usablePercentage;
          if (0 < snprintf(pacSize, MAX_LENGTH_OUTPUT_LINE, "%llu", sizeKb))
          {
            status = SUCCESS;
          }
        }
      }

      udev_device_unref(dev);
    }

    udev_unref(udev);
  }

  return status;
}


int GetLabelByName(char* pacDeviceName,
                   char* pacLabel)
//
// Get label of a FAT formated medium.
//
// input: pacDeviceName: name of device, accepted with or without path and partition number
// output:  pacLabel: label string
//
{
  int   status = SUCCESS;

  char  acPureDeviceName[MAX_LENGTH_OUTPUT_LINE] = "";
  //printf("pacDeviceName:%s\n", pacDeviceName);

  // check input parameter
  if((pacDeviceName == NULL) || (pacLabel == NULL))
  {
    status = INVALID_PARAMETER;
  }

  // Initialize output-string
  pacLabel[0] = '\0';

  // filter pure device name without path or partition number
  if(SUCCESS == (status = GetPureDeviceName(pacDeviceName, acPureDeviceName, sizeof(acPureDeviceName))))
  {
    // check if device is existing, else set error
    if(!DeviceExisting(acPureDeviceName))
    {
      status = INVALID_PARAMETER;
    }
    else
    {
      char  acBlkidCall[MAX_LENGTH_OUTPUT_LINE]  = "";
      char* pacBlkidOutput                       = NULL;

      if(strstr(acPureDeviceName, "mmcblk") == acPureDeviceName)
      {
        snprintf(acBlkidCall, sizeof(acBlkidCall), "blkid /dev/%sp1 | grep -o LABEL=\\\"..*", acPureDeviceName);
      }
      else
      {
        snprintf(acBlkidCall, sizeof(acBlkidCall), "blkid /dev/%s1 | grep -o LABEL=\\\"..*", acPureDeviceName);
      }

      if(NULL == (pacBlkidOutput = SystemCall_GetOutput(acBlkidCall)))
      {
        status = FILE_READ_ERROR;
      }
      else if(strlen(pacBlkidOutput))
      {
        // set pointer to first char of label string (behind introducing string)
        char* pacFoundString      = pacBlkidOutput + strlen("LABEL=\"");
        char* pcEndOfLabelString  = NULL;

        // try to find the first character behind label string (= next '"')
        if(NULL != (pcEndOfLabelString = strchr(pacFoundString, '"')))
        {
          // cut string here and copy it to output variable
          *pcEndOfLabelString = '\0';
          strncpy(pacLabel, pacFoundString, MAX_LENGTH_OUTPUT_STRING);
        }
      }

      SystemCall_Destruct(&pacBlkidOutput);
    }
  }

  return status;
}


void ShowHelpText(void)
//
// Show describtion and usage of program on stdout
//
{
  printf("\n* Get medium of specified device or partition *\n\n");
  printf("Usage: Usage: get_device_data <\"name\" | \"medium\" | \"bootflag\" | \"label\" > <device | medium>\n\n");
  printf("device: name of device (hda, hdb, ...), given by medium\n");
  printf("medium: medium of device (CF Card, Internal Flash, ...), given by name\n");
  printf("bootflag: 1 or 0, depending on bootflag is set. Device must be given by name (hda, hdb, ...)");

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
    if(argc < 3)
    {
      status = MISSING_PARAMETER;
    }
    else
    {
      // check if parameter is contained in parameter-list and get its index
      char* pParameterString  = argv[1];
      int   parameterIndex    = 0;

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
        char outputString[MAX_LENGTH_OUTPUT_STRING] = "";
        status = astParameterJumptab[parameterIndex].pFktGetParameter(argv[2], outputString);
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
