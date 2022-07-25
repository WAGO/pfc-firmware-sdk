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
///  \file     config_tool_lib.h
///
///  \brief    Commonly used config tool defines and functions.
///
///  \author   SM:  WAGO GmbH & Co. KG
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

#ifndef _config_tool_lib_h_

#define _config_tool_lib_h_

#include "ct_error_handling.h"

// boolean
//#define TRUE                             1
//#define FALSE                            0

#include <stdint.h>
#include <glib.h>


// query ids for RunUdevQuery function
enum eUdevQueryId{
   UDEV_QUERY_BLOCK_PARTITIONS,
   UDEV_QUERY_BLOCK_PARTITIONS_EXT,
   UDEV_QUERY_BLOCK_DEVICES,
   UDEV_QUERY_MTD_PARTITIONS,
   UDEV_QUERY_MTD_DEVICES,
   UDEV_QUERY_UBI_PARTITIONS,
   UDEV_QUERY_UBI_PARTITION_BY_NAME,
   UDEV_QUERY_UBI_DEVICES,
   UDEV_QUERY_INTERNAL_FLASH_NAND // hw-specific query that evaluates /etc/DEVICE_MEDIA
};

#define MAX_ERROR_TEXT_LENGTH                 200

#define MAX_LENGTH_INPUT_STRING                40

#ifndef MAX_LENGTH_SYSTEM_CALL
#define MAX_LENGTH_SYSTEM_CALL                200
#endif

#define MAX_LENGTH_OUTPUT_STRING              100

#define MAX_LENGTH_OUTPUT_LINE                300
#define MAX_LINE_LENGTH                       200

// maximum length of string for the output of the requested parameter in get_eth_config
#define MAX_LENGTH_ETH_CONFIG_STRING          100

// input-strings for the different interfaces
#define ETH0                               "eth0"
#define ETH1                               "eth1"

// output-strings for the different processor-types
#define GEODE                              "Geode SC 1200"
#define CELERON                            "Intel(R) Celeron(R) M processor 600MHz"
#define PENTIUM_M_1000                     "Intel(R) Celeron(R) M processor 1.00GHz"
#define PENTIUM_M_1400                     "Intel(R) Pentium(R) M processor 1.40GHz"
#define ARM_PAC200                         "ARMv7 Processor rev 7 (v7l)"

// comment-character: ignore line of a config-file, if this character is in front of it
#define COMMENT_CHAR                        '#'

#define SEPARATOR                           "="

#ifndef DEVICE_MEDIA_FILENAME
#define DEVICE_MEDIA_FILENAME               "/etc/DEVICE_MEDIA"
#endif

#define INTERNAL_FLASH_NAND                 "internal-flash-nand"

#define INITTAB_GETTY_PATTERN               ":respawn:/sbin/getty"
#define SERIAL_DEV_NAME                     "serial"

//used in elrest-config-tools eDisplay
#define FILE_LAST_ERROR_TXT                 "/tmp/last_error.txt"

//------------------------------------------------------------------------------
// typedefs
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------

int RunUdevQuery(char              * szBuf,
                 size_t              bufSize,
                 enum eUdevQueryId   id,
                 int                 reqIndex,
                 void              * pParam);

int UbiToMtd(char const * szUbiDev,
             char       * szMtdDev,
             size_t       mtdDevSize);

// moved from get_device_data.c
int ctlib_GetDeviceNameByMedium(char const * pacRequestedMedium,
                                char       * pacDeviceName,
                                size_t       pacDeviceNameLength);

int SearchLineInDeviceMediumFile(char const * szSearchText,
                                 char       * szFoundLine);

char *safe_strncpy(char       * szDest,
                   char const * szSource,
                   size_t       num);

char *FileContent_Get(char const * szFilename);

void  FileContent_Destruct(char * * pszOutputBuffer);

int FileContent_GetLineByNr(char * szFileContent,
                            int    requestedLineNr,
                            char * szLineString);

// Make a system-call and return it's outputs to stdout in a new allocated buffer.
char *SystemCall_GetOutput(char const * szSystemCallString);

void SystemCall_Destruct(char * * pszOutputBuffer);

int SystemCall_GetLineByNr(char * szSystemCallOutput,
                           int    lineNr,
                           char * szLineString);

// Cut string directly behind the next word
void CutWord(char * szWord,
             int    maxLength);

// Cut string behind the last printable character
void CutLine(char * szLine,
             int    maxLength);

// Get the begin of the next word
char *GetNextWord(char * szString);

// Delete directly following multiple spaces in a string
char *DeleteMultibleSpaces(char * szInputString,
                           int    maxLength);

// Get error text belonging to a status-number
int GetErrorText(int    status,
                 char * szOut,
                 int    maxLength);

// Append LastError Text to /tmp/last_error.txt
int SetLastError(char const * szText);

// Show error text belonging to a status-number
void ShowErrorText(int   status,
                   char * szAdditionalInfo);


// get html-like command-line parameter
int GetParameterValue(char * szEntireParamString,
                      char * szParamNameString,
                      char * szParamValueString,
                      int    sizeofParamValueString);


// Simple API to work with .ini type config files
#define SETTINGS_CONF_FILE "/etc/eth_settings.conf"

#define SETTINGS_CONF_GROUP_PORTX "Ethernet Switch Port"
#define SETTINGS_CONF_GROUP_PORT0 SETTINGS_CONF_GROUP_PORTX " 0"
#define SETTINGS_CONF_GROUP_PORT1 SETTINGS_CONF_GROUP_PORTX " 1"

typedef void* config_t;

int  ctlib_ConfigOpen(const char * szRessource,
                      config_t   * pConfig,
                      void       * pPrivate);

void ctlib_ConfigClose(config_t config);

int  ctlib_ConfigSaveChanges(config_t   portConfig,
                             void     * pPrivate);

int  ctlib_ConfigGetString(config_t       config,
                           char const   * pKey,
                           char       * * pszValue,
                           void         * pPrivate);

void ctlib_ConfigSetString(config_t     config,
                           char const * pKey,
                           char const * pValue,
                           void       * pPrivate);

int  ctlib_VerifyDomainName(char const * szName);

int  ctlib_IpaddrToInt(char const * szIpAddr,
                       uint32_t   * pResult);

// Typelabel field names
#define TYPELABEL_ORDER_NR        "ORDER"
#define TYPELABEL_PRODUCT_DESC    "SYSDESC"
#define TYPELABEL_ARTICLE_NR      "PSN"
#define TYPELABEL_PRGDATE         "PRGDATE"
#define TYPELABEL_WAGONR          "WAGONR"
#define TYPELABEL_MARKING         "MARKING"
#define TYPELABEL_MAC             "MAC"

#endif
