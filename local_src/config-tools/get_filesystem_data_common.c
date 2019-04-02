//------------------------------------------------------------------------------
/// Copyright (c) 2000 - 2018 WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     get_filesystem_data.c
///
///  \version  $Revision: 34574 $1
///
///  \brief
///
///  \author   Stefanie Meihöfer : WAGO Kontakttechnik GmbH & Co. KG
///  \author   Maxim Laschinsky : WAGO Kontakttechnik GmbH & Co. KG
///  \author   Patrick Enns : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libudev.h>
#include <assert.h>

#include "config_tool_lib.h"

const char * g_proc_cmdline    = "/proc/cmdline";
const char * g_proc_partitions = "/proc/partitions";

//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

#ifdef  __CPPUTEST__
#define main get_coupler_details_main
#endif

#define SHOW_ERRORS                         0

//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------

// Pointer to funktion to search the requested parameter
typedef int (*tFktGetParameter)(char* outputString,
                                int   additionalParam);


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
static int GetActivePartition(char* pOutputString, int additionalParam);
static int GetActiveDevice(char* pOutputString, int additionalParam);
static int GetSecondActiveDevice(char* pOutputString, int additionalParam);
static int GetPartitionByIndex(char* pOutputString, int deviceNr);
static int GetPartitionByIndexExt(char* pOutputString, int deviceNr);
static int GetDeviceStringByIndex(char* pOutputString, int deviceNr);
static int GetHomeDevice(char* pOutputString, int additionalParam);
static int GetActiveHomePartition(char* pOutputString, int additionalParam);
static int GetInactiveHomePartition(char* pOutputString,int additionalParam);
static int GetActivePartitionMedium(char* pOutputString, int additionalParam);
static int GetActivePartitionMediumText(char* pOutputString, int additionalParam);
static int GetMediumByIndex(char* pOutputString, int requestedMediumIndex);
static int ShowMediumSelection(char* pOutputString, int requestedMediumIndex);
static int ShowMediumListJson(char* pOutputString, int requestedMediumIndex);
static int ShowDeviceDataListJson(char* pOutputString, int requestedMediumIndex);

// array of all possible requested parameter (input-strings, strings to search for, processing-function to get them)
static tParameterJumptab astParameterJumptab[] =
{
  { "active-partition",             GetActivePartition },
  { "active-device",                GetActiveDevice },
  { "home-device",                  GetHomeDevice },        //Deprecated
  { "active-home-partition",        GetActiveHomePartition },
  { "inactive-home-partition",      GetInactiveHomePartition },
  { "second-active-device",         GetSecondActiveDevice },
  { "partition-by-index",           GetPartitionByIndex },
  { "ext-partition-by-index",       GetPartitionByIndexExt },
  { "device-by-index",              GetDeviceStringByIndex },
  { "active-partition-medium",      GetActivePartitionMedium },
  { "active-partition-medium-text", GetActivePartitionMediumText },
  { "medium-by-index",              GetMediumByIndex },
  { "medium-selection",             ShowMediumSelection },
  { "medium-list-json",             ShowMediumListJson },
  { "device-data-list-json",        ShowDeviceDataListJson },

  // this line must always be the last one - don't remove it!
  { "", NULL }
};


//------------------------------------------------------------------------------
// external functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------


static void ShowHelpText(void)
//
// Show describtion and usage of program on stdout
//
{
  int parameterIndex = 0;

  printf("\n* Get informations about the filesystem *\n\n");
  printf("Usage: get_filesystem_data <parameter> [device-number]\n\n");
  printf("parameter: ");

  // read the possible strings for parameter from array and show them like this: "param1 | param2 | param3"
  while(strlen(astParameterJumptab[parameterIndex].inputString) > 0)
  {
    if(parameterIndex != 0) printf("| ");
    printf("%s ", astParameterJumptab[parameterIndex].inputString);
    ++parameterIndex;
  }

  printf("\n\n");
  printf("number: only necessary for special parameter got \"by index\"\n");
  printf("\n");
}


int GetMediumFrontendString(char* pacMediumIdString,
                            char* pacMediumFrontendString)
{
  int status = SUCCESS;

  if(strcmp(pacMediumIdString, "cf-card") == 0)
  {
    strncpy(pacMediumFrontendString, "CF Card", MAX_LENGTH_OUTPUT_STRING);
  }
  else if(strncmp(pacMediumIdString, "internal-flash", 14) == 0)
  {
    strcpy(pacMediumFrontendString, "internal flash");
  }
  else if(strcmp(pacMediumIdString, "usb1") == 0)
  {
    strncpy(pacMediumFrontendString, "USB1", MAX_LENGTH_OUTPUT_STRING);
  }
  else if(strcmp(pacMediumIdString, "usb2") == 0)
  {
    strncpy(pacMediumFrontendString, "USB2", MAX_LENGTH_OUTPUT_STRING);
  }
  else
  {
    status = -1;
  }

  return status;
}

// prepend '/dev/' prefix to pOutputString.
static inline void AddDevPrefix(char *pOutputString, size_t bufSize)
{
  char prefix[] = "/dev/";

  assert(strlen(prefix) + strlen(pOutputString) + 1 < bufSize);

  memmove(pOutputString + strlen(prefix), pOutputString, strlen(pOutputString) + 1);
  memcpy(pOutputString, prefix, strlen(prefix));
}

static int GetActivePartition(char* pOutputString,
                              int   additionalParam)
//
// Get the actual active device + partition
//
// input: ignored
//
// output: string with the active device
//         - must be allocated by calling function!
//
// return: error-code
//
{
  int   status              = SUCCESS;
  char* pacActivePartition  = NULL;

  UNUSED_PARAMETER(additionalParam);

  // construct command line: get root=... and print 2nd part using '=' as delimiter
  char pCmdTemplate[] = "cat %s | grep -o 'root=[^[:space:]]\\+' | cut -d = -f 2";
  char cmd[strlen(pCmdTemplate) + strlen(g_proc_cmdline) + 1];

  snprintf(cmd, sizeof(cmd), pCmdTemplate, g_proc_cmdline);

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  *pOutputString = '\0';

  pacActivePartition = SystemCall_GetOutput(cmd);

  if(pacActivePartition == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else // copy string to output
  {

    // CAUTION if strlen(pOutputString) < MAX_LENGTH_OUTPUT_STRING
    // TODO: remove constants from calls like this!
    strncpy(pOutputString, pacActivePartition, MAX_LENGTH_OUTPUT_STRING);
    CutWord(pOutputString, MAX_LENGTH_OUTPUT_STRING);
    SystemCall_Destruct(&pacActivePartition);

    if(pOutputString == strstr(pOutputString, "ubi"))
    {
      // /proc/cmdline accepts 2 ways to define a ubi root partition:
      // 1) root=ubiX_Y
      // 2) root=ubiX:labelY
      // Both need to be handled

      char *delim = strchr(pOutputString, ':');

      if(NULL == delim)
      {
        // 1st case: no /dev/ prefix in kernel cmdline
        AddDevPrefix(pOutputString, MAX_LENGTH_OUTPUT_STRING);

      }
      else
      {
        // delim != NULL => 2nd case
        char ubiLabel[strlen(delim + 1) + 1];
        safe_strncpy(ubiLabel, delim + 1, sizeof(ubiLabel));

         int nrDevFound = RunUdevQuery(pOutputString,
                          MAX_LENGTH_OUTPUT_STRING,
                          UDEV_QUERY_UBI_PARTITION_BY_NAME,
                          0,
                          ubiLabel);
         if(1 != nrDevFound)
         {
           status = ERROR;
         }
      }
    }
  }

  return status;
}

static int GetActiveDevice(char* pOutputString,
                           int   additionalParam)
//
// Get the actual active device without partition number
//
// input: ignored
//
// output: string with the active device
//         - must be allocated by calling function!
//
// return: error-code
//
// ASSUMPTION: GetActivePartition returns one of the strings
// /dev/hdxN
// /dev/sdxN
// /dev/mmcblkXpY
// mtdX (for ubi/ubifs devices).
//
// NOTE1: For ubifs devices the result shall be interpreted differently as
// different ubifs root partitions may be located on the same mtd device. Thus,
// by calling active-device we will be able to find out that we are running a
// system from an mtd device but not to distinguish between the root file
// systems (this (logical) information is located in the EEPROM).
//
// We do this to extend the existing code in a backward-compatible way.
//
// NOTE2: this function, as the whole config tools package, should be
// redesigned to not have to assume such things.
{
  int   status          = SUCCESS;

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // get active partition string from cmdline
  if(SUCCESS != (status = GetActivePartition(pOutputString, additionalParam)) )
  {
     return(status);
  }

  // cut partition number:

  // ipc:
  // hda1 => hda
  // sdb2 => sdb

  // pac200:
  // mmcblk0p1 => mmcblk0

  if(pOutputString == strstr(pOutputString, "/dev/hd") ||
     pOutputString == strstr(pOutputString, "/dev/sd"))
  {
     // cut partition number
     pOutputString[8] = '\0';
  }
  else if(pOutputString == strstr(pOutputString, "/dev/mmcblk"))
  {
     pOutputString[12] = '\0';
  }
  else if(pOutputString == strstr(pOutputString, "/dev/ubi"))
  {
    // special treatment for ubi devices: return corresponding mtd device

   char mtdDevString[MAX_LENGTH_OUTPUT_STRING];

    if (SUCCESS ==
        (status = UbiToMtd(pOutputString, mtdDevString, MAX_LENGTH_OUTPUT_STRING)) )
    {
      strncpy(pOutputString, mtdDevString, MAX_LENGTH_OUTPUT_STRING);
    }
    else
    {
      return status;
    }
  }
  else
  {
     // unknown device
     status = ERROR;
  }

  return status;
}

/* ----------------------------------------------------------------------------
 * Function: GetSecondActiveDevice
 *
 * Used to acquire the name of the inactive device as zero-terminated string.
 * Example: "/dev/mmcblk1" for internal-nand-emmc.
 *
 *  \param[out] pOutputString   The device name is written to this pointer
 *  \param[in]  additionalParam Not used in this function
 *
 *  return status: SUCCESS = 0, FAILED != 0
 * --------------------------------------------------------------------------*/
static int GetSecondActiveDevice(char* pOutputString, int additionalParam)
{
  (void)additionalParam;
  char szActiveDevice[MAX_LENGTH_OUTPUT_STRING] = "";
  char device[MAX_LENGTH_OUTPUT_STRING] = "";
  int result = GetActiveDevice(szActiveDevice, 0);
  if(result != SUCCESS)
  {
    return result;
  }

  int index = 1;
  do {
    result = GetDeviceStringByIndex(device, index);
    if(result == SUCCESS)
    {
      if(strcmp(szActiveDevice, device) != 0)
      {
        (void)strncpy(pOutputString, device, MAX_LENGTH_OUTPUT_STRING);
        return SUCCESS;
      }
    }
    index++;
  } while(result == SUCCESS);

  return result;
}


static int GetPartitionByIndexExt(char *pOutputString,
                                  int   requestedDeviceIndex)
//
// Get a partition (only _with_ partition number) by its the sequential number.
// Similar to GetPartitionByIndex, but returns only ext2/ext3 partitions that e2fsck may be done on.
//
// input: requestedDeviceNr: requested sequential partition number
//
// output: string with the requested device or empty string, if no device fits to this number
//         - must be allocated by calling function!
//
// return: error-code
//
{
  int   status        = SUCCESS;

  int scannedOffset = 0;

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  if(requestedDeviceIndex < 1)
  {
    return(NOT_FOUND);
  }

  // requestedDeviceIndex starts with 1 by API definition
  scannedOffset = RunUdevQuery(pOutputString,
                               MAX_LENGTH_OUTPUT_STRING,
                               UDEV_QUERY_BLOCK_PARTITIONS_EXT,
                               (requestedDeviceIndex-1),
                               "ext2");

  if(0 == strlen(pOutputString))
  {
    // requestedDeviceIndex is an absolute numeration =>
    // we have to substract the number of block devices
    // to start at 0 again
    (void) RunUdevQuery(pOutputString,
                        MAX_LENGTH_OUTPUT_STRING,
                        UDEV_QUERY_BLOCK_PARTITIONS_EXT,
                       (requestedDeviceIndex-1) - scannedOffset,
                       "ext3");
  }


  if(0 == strlen(pOutputString))
  {
    status = NOT_FOUND;
  }

  return status;
}

static int GetPartitionByIndex(char *pOutputString,
                               int   requestedDeviceIndex)
//
// Get a partition (only _with_ partition number) by its the sequential number.
//
// input: requestedDeviceNr: requested sequential partition number
//
// output: string with the requested device or empty string, if no device fits to this number
//         - must be allocated by calling function!
//
// return: error-code
//
{
  int   status        = SUCCESS;

  int scannedOffset = 0;

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  if(requestedDeviceIndex < 1)
  {
    return(NOT_FOUND);
  }

  // requestedDeviceIndex starts with 1 by API definition
  scannedOffset = RunUdevQuery(pOutputString,
                               MAX_LENGTH_OUTPUT_STRING,
                               UDEV_QUERY_BLOCK_PARTITIONS,
                               (requestedDeviceIndex-1),
                               NULL);


  // ubi partitions need a special treatment:
  // they have no "partition" attribute and do not belong to
  // the block subsystem
  if(0 == strlen(pOutputString))
  {
    // requestedDeviceIndex is an absolute numeration =>
    // we have to substract the number of block devices
    // to start at 0 again
    (void) RunUdevQuery(pOutputString,
                        MAX_LENGTH_OUTPUT_STRING,
                        UDEV_QUERY_UBI_PARTITIONS,
                       (requestedDeviceIndex-1) - scannedOffset,
                       NULL);
  }

  if(0 == strlen(pOutputString))
  {
    status = NOT_FOUND;
  }

  return status;
}


static int GetDeviceByIndex(char *pOutputString,
                            int   requestedDeviceIndex)
//
// Get a main device (only _without_ partition number) by its the sequential number.
//
// input: requestedDeviceNr: requested sequential device number
//
// output: string with the requested device or empty string, if no device fits to this number
//         - must be allocated by calling function!
//
// return: error-code
//
{
  int   status        = SUCCESS;

  int scannedOffset = 0;

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // requestedDeviceIndex starts with 1 by API definition
  scannedOffset = RunUdevQuery(pOutputString,
                                 MAX_LENGTH_OUTPUT_STRING,
                                 UDEV_QUERY_BLOCK_DEVICES,
                                 (requestedDeviceIndex-1),
                                 NULL);

  // ubi partitions need a special treatment: they have no "partition"
  // attribute and do not belong to the block subsystem. Furthermore, they are
  // logical devices and do not show up in udev until the internal flash is
  // properly formatted.

  int scannedUbiDevNr = 0;
  if(0 == strlen(pOutputString))
  {
    // requestedDeviceIndex is an absolute numeration =>
    // we have to substract the number of block devices
    // to start at 0 again
    scannedUbiDevNr = RunUdevQuery(pOutputString,
                                   MAX_LENGTH_OUTPUT_STRING,
                                   UDEV_QUERY_UBI_DEVICES,
                                   (requestedDeviceIndex-1) - scannedOffset,
                                   NULL);

    if(0 != strlen(pOutputString))
    {
      // convert ubi device to mtd
      char mtdDevString[MAX_LENGTH_OUTPUT_STRING];

      if (SUCCESS ==
          UbiToMtd(pOutputString, mtdDevString, MAX_LENGTH_OUTPUT_STRING) )
      {
        strncpy(pOutputString, mtdDevString, MAX_LENGTH_OUTPUT_STRING);
      }
      else
      {
        *pOutputString = '\0';
      }
    }
  }

  // Last resort if no ubi devices found due to unpartitioned or messed up mtd device:
  // try to find the mtd device configured at /etc/DEVICE_MEDIA.
  if(0 == strlen(pOutputString) &&
     0 == scannedUbiDevNr)
  {
      // If the device has no internal-flash-nand configured, pOutputString
      // will remain empty.
      (void) RunUdevQuery(pOutputString,
                          MAX_LENGTH_OUTPUT_STRING,
                          UDEV_QUERY_INTERNAL_FLASH_NAND,
                          (requestedDeviceIndex-1) - scannedOffset,
                          NULL);
  }

  if(0 == strlen(pOutputString))
  {
    status = NOT_FOUND;
  }

  return status;
}

static int GetDeviceStringByIndex(char *pOutputString,
                                  int   requestedDeviceIndex)
//
// Get a main device path _without_ partition number by its the sequential number.
// Only devices in white list file /etc/DEVICE_MEDIA are counted.
//
// input: requestedDeviceNr: requested sequential device number
//
// output: string with the requested device or empty string, if no device fits to this number
//         - must be allocated by calling function!
//
// return: error-code
//
{
  int   status        = SUCCESS;
  char  medialine[MAX_LENGTH_OUTPUT_LINE];
  int   currentIndex = requestedDeviceIndex;

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  while (1) {
    medialine[0] = '\0';
    status = GetDeviceByIndex(pOutputString, currentIndex);
    if (status || pOutputString[0] == '\0')
    {
      break;
    }

    // Whitelist search for device name skipping /dev/
    SearchLineInDeviceMediumFile(pOutputString + 5, medialine);
    if (medialine[0] != '\0')
    {
      break;
    }
    pOutputString[0] = '\0';
    currentIndex++;
  }
  return status;
}

static int GetHomeDevice(char* pOutputString,
                         int   additionalParam)
//
// Get name of just now active device.
//
// input: ignored
//
// output: string with the device
//         - must be allocated by calling function!
//
// return: error-code
//
// NOTE by AGa: this function's name is a bit misleading: it actually returns a
// partition where the home directory is located (/dev/hda5, /dev/ubi0_1), not
// what is referred to as 'device' by other functions (/dev/hda or /dev/mtd)
//
{
  int   status                  = SUCCESS;
  char* pDfContent              = NULL;

  UNUSED_PARAMETER(additionalParam);

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  *pOutputString = '\0';

  // get output of df-system-call
  pDfContent = SystemCall_GetOutput("df /home");

  if(pDfContent == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    char  dfLineString[MAX_LENGTH_OUTPUT_LINE]  = "";

    // We have to look at the 2nd line (GetLineByNr starts to count with 1)
    //
    // df output looks like this:
    //
    // Filesystem           1K-blocks      Used Available Use% Mounted on
    // /dev/root                81249     48380     28773  63% /

    if(SUCCESS == SystemCall_GetLineByNr(pDfContent, 2, dfLineString))
    {

      // device is directly the first string in line, so we just must cut the rest
      CutWord(dfLineString, MAX_LENGTH_OUTPUT_LINE);

      // if /home is located within the rootfs, df returns "/dev/root" instead of the real device
      if(0 == strcmp(dfLineString, "/dev/root"))
      {
         status = GetActivePartition(dfLineString, 0);
      }

      strncpy(pOutputString, dfLineString, MAX_LENGTH_OUTPUT_STRING);
    }

  }

  SystemCall_Destruct(&pDfContent);

  return status;
}

static int GetIndexOfActiveRootfs(void)
{
  int result = -1;
  char* strSystem1Active = SystemCall_GetOutput("/etc/config-tools/get_systeminfo 1 active");
  if(NULL != strSystem1Active)
  {
    if(strcmp(strSystem1Active, "true") == 0)
    {
      result = 1;
    }
    else if (strcmp(strSystem1Active, "false") == 0)
    {
      result = 2;
    }
    else
    {
      result = -1;
    }
    SystemCall_Destruct(&strSystem1Active);
  }
  return result;
}

static int GetHomePartitionFromIndex(char* pOutputString, int homeNameIndex)
{
  int status = NOT_FOUND;
  char cmd[MAX_LENGTH_OUTPUT_STRING];
  snprintf(cmd, sizeof cmd, "/sbin/blkid | grep -E 'home%d' | cut -d: -f1 | tr '\\n' '\\0'", homeNameIndex);
  char* strHomePartition = SystemCall_GetOutput(cmd);
  if(NULL != strHomePartition)
  {
    strncpy(pOutputString, strHomePartition, MAX_LENGTH_OUTPUT_STRING);
    status = SUCCESS;
    SystemCall_Destruct(&strHomePartition);
  }
  return status;
}

static int GetActiveHomePartition(char* pOutputString, int   additionalParam)
{
  (void)additionalParam;
  int result = -1;
  //active rootfs index = active home index!
  int actHomeNameIndex = GetIndexOfActiveRootfs();
  if ((actHomeNameIndex >= 1) && (actHomeNameIndex <= 2))
  {
    result = GetHomePartitionFromIndex(pOutputString, actHomeNameIndex);
  }
  return result;
}

static int GetInactiveHomePartition(char* pOutputString, int   additionalParam)
{
  (void)additionalParam;
  int result = -1;
  //active rootfs index = active home index!
  int actHomeNameIndex = GetIndexOfActiveRootfs();
  if((actHomeNameIndex >= 1) && (actHomeNameIndex <= 2))
  {
    //select next index (range 0..1)
    result = GetHomePartitionFromIndex(pOutputString, (actHomeNameIndex == 1) ? 2 : 1);
  }
  return result;
}

static int GetActivePartitionMedium(char* pOutputString,
                                    int   additionalParam)
//
// Get name (meaning) of just now active partition (device)
//
// input: ignored
//
// output: string with the name
//         - must be allocated by calling function!
//
// return: error-code
//
{
  int   status                                              = SUCCESS;
  char  completeHomeDeviceString[MAX_LENGTH_OUTPUT_STRING]  = "";
  char* pHomeDeviceString                                   = NULL;
  char* pHomeDeviceMedium                                   = NULL;
  char  systemCall[MAX_LENGTH_SYSTEM_CALL]                   = "";

  UNUSED_PARAMETER(additionalParam);

  if(pOutputString == NULL)
  {
    return INVALID_PARAMETER;
  }

  // initialise output-string
  sprintf(pOutputString, "");

  (void)GetHomeDevice(completeHomeDeviceString, 0);
  //printf("completeHomeDeviceString:%s\n", completeHomeDeviceString);

  // cut leading "/dev/" string if existing
  pHomeDeviceString = completeHomeDeviceString;
  if(strncmp(pHomeDeviceString, "/dev/", strlen("/dev/")) == 0)
  {
    pHomeDeviceString += strlen("/dev/");
  }

  // get medium of home device
  (void)snprintf(systemCall, sizeof(systemCall), "/etc/config-tools/get_device_data medium %s", pHomeDeviceString);
  pHomeDeviceMedium = SystemCall_GetOutput(systemCall);

  if(pHomeDeviceMedium == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    strncpy(pOutputString, pHomeDeviceMedium, MAX_LENGTH_OUTPUT_STRING);
    SystemCall_Destruct(&pHomeDeviceMedium);
  }

  return status;
}

static int GetActivePartitionMediumText(char* pOutputString,
                                        int   additionalParam)
//
// Get name in of just now active partition (device) as "nice" text for use in frontend.
//
// input: ignored
//
// output: string with the name
//         - must be allocated by calling function!
//
// return: error-code
//
{
  int   status                                                = SUCCESS;
  char  activePartitionMediumString[MAX_LENGTH_OUTPUT_STRING] = "";

  UNUSED_PARAMETER(additionalParam);

  if(pOutputString == NULL)
  {
    return INVALID_PARAMETER;
  }

  // initialise output-string
  sprintf(pOutputString, "");

  if(SUCCESS == (status = GetActivePartitionMedium(activePartitionMediumString, 0)))
  {
    //printf("activePartitionmediumString:%s", activePartitionMediumString);
    status = GetMediumFrontendString(activePartitionMediumString, pOutputString);
  }

  return status;
}



static int GetMediumByIndex(char* pOutputString,
                            int   requestedMediumIndex)
//
// Get memory medium name by sequential number (of all existing media)
//
// input: sequential number of requested medium
//
// output: string with medium name
//         - must be allocated by calling function!
//
// return: error-code
//
{
  int   status                            = SUCCESS;
  char  acDevice[MAX_LENGTH_OUTPUT_LINE]  = "";

  if((pOutputString == NULL) || (requestedMediumIndex < 1))
  {
    return INVALID_PARAMETER;
  }

  // initialise output-string
  sprintf(pOutputString, "");

  //printf("requestedMediumIndex:%d\n", requestedMediumIndex);
  if(SUCCESS != GetDeviceByIndex(acDevice, requestedMediumIndex))
  {
    status = NOT_FOUND;
  }
  else
  {
    char* pacMedium                         = NULL;
    char  systemCall[MAX_LENGTH_SYSTEM_CALL] = "";
    //printf("acDevice:%s\n", acDevice);

    // get medium of device
    (void)snprintf(systemCall, sizeof(systemCall), "/etc/config-tools/get_device_data medium %s", acDevice);
    pacMedium = SystemCall_GetOutput(systemCall);

    if((pacMedium == NULL) || (!strlen(pacMedium)))
    {
      status = NOT_FOUND;
      //strncpy(pOutputString, acDevice, MAX_LENGTH_OUTPUT_STRING);
    }
    else
    {
      strncpy(pOutputString, pacMedium, MAX_LENGTH_OUTPUT_STRING);
      SystemCall_Destruct(&pacMedium);
    }
  }

  return status;
}

static int ShowMediumSelection(char* pOutputString,
                               int   additionalParam)
//
// Show a selection-menu for html-page with the media of all devices - exept the actual active device.
//
// input: pAdditionalParam: ignored
//
// output: pOutputString: empty string; all necessary outputs where made directly to stdout
//
// return: error-code
//
{
  int   status                                                = SUCCESS;
  int   mainDeviceIndex                                       = 0;
  char  mediumIdString[MAX_LENGTH_OUTPUT_STRING]              = "";
  char  mediumFrontendString[MAX_LENGTH_OUTPUT_STRING]        = "";
  char  activePartitionMediumString[MAX_LENGTH_OUTPUT_STRING] = "";

  UNUSED_PARAMETER(additionalParam);
  UNUSED_PARAMETER(pOutputString);

  (void)GetActivePartitionMedium(activePartitionMediumString, 0);
  //printf("activePartitionmediumString:%s", activePartitionMediumString);

  // start selection-menu
  //printf("\n              <select name=\"device-medium\" size=\"1\" >");

  // loop over all possible devices and write a menu-line for each one
  mainDeviceIndex = 1;
  while(SUCCESS == GetMediumByIndex(mediumIdString, mainDeviceIndex))
  {
    if(strcmp(activePartitionMediumString, mediumIdString) != 0)
    {
      GetMediumFrontendString(mediumIdString, mediumFrontendString);
      printf("\n                <option value=\"%s\">%s</option>", mediumIdString, mediumFrontendString);
    }
    ++mainDeviceIndex;
  }

  //printf("\n              </select>\n");

  return status;
}


static int ShowMediumListJson(char* pOutputString,
                              int   additionalParam)
//
// get a list (json array) with the media of all devices
//
// input: pAdditionalParam: ignored
//
// output: pOutputString: empty string; all necessary outputs where made directly to stdout
//
// return: error-code
//
{
  int   status                                    = SUCCESS;
  int   mainDeviceIndex                           = 0;
  char  mediumIdString[MAX_LENGTH_OUTPUT_STRING]  = "";

  UNUSED_PARAMETER(additionalParam);
  UNUSED_PARAMETER(pOutputString);

  printf("[ ");

  // loop over all possible devices and write a menu-line for each one
  mainDeviceIndex = 1;
  while(SUCCESS == GetMediumByIndex(mediumIdString, mainDeviceIndex))
  {
    if(mainDeviceIndex > 1) printf(", ");
    printf("\"%s\"", mediumIdString);
    ++mainDeviceIndex;
  }

  printf(" ]");

  return status;
}

static int ShowDeviceDataListJson(char* pOutputString,
                                  int   additionalParam)
//
// get a list (json array) with all devices
//
// input: pAdditionalParam: ignored
//
// output: pOutputString: empty string; all necessary outputs where made directly to stdout
//
// return: error-code
//
{
  int   status                                    = SUCCESS;
  int   mainDeviceIndex                           = 0;
  char  deviceString[MAX_LENGTH_OUTPUT_STRING]    = "";

  UNUSED_PARAMETER(additionalParam);
  UNUSED_PARAMETER(pOutputString);

  printf("[ ");

  // loop over all possible devices and write a menu-line for each one
  for(mainDeviceIndex = 1; SUCCESS == GetDeviceByIndex(deviceString, mainDeviceIndex); ++mainDeviceIndex)
  {
    char  mediumString[MAX_LENGTH_OUTPUT_STRING]  = "";
    char  labelString[MAX_LENGTH_OUTPUT_STRING]   = "";
    char* pacMedium                               = NULL;
    char* pacLabel                                = NULL;
    char  systemCall[MAX_LENGTH_SYSTEM_CALL]      = "";
    int   resultlen;

    // get medium of device
    (void)snprintf(systemCall, sizeof(systemCall), "/etc/config-tools/get_device_data medium %s", deviceString);
    pacMedium = SystemCall_GetOutput(systemCall);
    if(NULL == pacMedium)
    {
      continue;
    }
    strncpy(mediumString, pacMedium, MAX_LENGTH_OUTPUT_STRING);
    resultlen = strlen(pacMedium);
    SystemCall_Destruct(&pacMedium);
    if(0 == resultlen)
    {
      continue;
    }

    // get label of device
    (void)snprintf(systemCall, sizeof(systemCall), "/etc/config-tools/get_device_data label %s", deviceString);
    pacLabel = SystemCall_GetOutput(systemCall);
    if(NULL == pacLabel)
    {
      continue;
    }
    strncpy(labelString, pacLabel, MAX_LENGTH_OUTPUT_STRING);
    SystemCall_Destruct(&pacLabel);

    if(mainDeviceIndex > 1) printf(", ");
    printf("{ \"deviceName\": \"%s\",", deviceString);
    printf("\"deviceMedium\": \"%s\",", mediumString);
    printf("\"deviceLabel\": \"%s\" }", labelString);
  }

  printf(" ]");

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
      //char* pNumberString     = NULL;
      int   deviceNr          = 0;

      //pNumberString = argv[2];
      if(argc > 2)
      {
        // convert requested device-number from string to int
        sscanf(argv[2], "%d", &deviceNr);
      }

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

      // parameter was found -> start processing (get the fitting function from parameter-list)
      if(status == SUCCESS)
      {
        char outputString[MAX_LENGTH_OUTPUT_STRING] = "";
        status = astParameterJumptab[parameterIndex].pFktGetParameter(outputString, deviceNr);
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

