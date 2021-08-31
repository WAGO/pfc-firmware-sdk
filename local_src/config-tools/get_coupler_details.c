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
///  \file     get_coupler_details.c
///
///  \version  $Revision: 57110 $1
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <glib.h>

#ifdef __CT_WITH_TYPELABEL
#include <typelabel_API.h>
#endif

#include <libgen.h>               // for basename()

#include "config_tool_lib.h"
#include "libnet/ct_libnet.h"

//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

#ifdef  __CPPUTEST__
#define main get_filesystem_data_main
#endif

#define SHOW_ERRORS                         0

#define MAX_LENGTH_COUPLER_DETAIL_STRING    260

// processor-type-strings
#define GEODE                               "Geode SC 1200"
#define CELERON                             "Intel(R) Celeron(R) M processor 600MHz"
#define PENTIUM_M_1000                      "Intel(R) Celeron(R) M processor 1.00GHz"
#define PENTIUM_M_1400                      "Intel(R) Pentium(R) M processor 1.40GHz"
#define ARM_CORTEX_A8                       "ARMv7 Processor rev 7 (v7l)"


// fieldbus-type-strings
#define NO_FIELDBUS                         "No Fieldbus"
#define PROFIBUS_MASTER                     "Profibus-Master"
#define PROFIBUS_SLAVE                      "Profibus-Slave"
#define CAN_OPEN_MASTER                     "CanOpen-Master"
#define CAN_OPEN_SLAVE                      "CanOpen-Slave"
#define DEVICE_NET_MASTER                   "DeviceNet-Master"
#define DEVICE_NET_SLAVE                    "DeviceNet-Slave"

// CoDeSys-license-strings
#define NO_CODESYS_LICENSE                  ""
#define CODESYS_LICENSE                     "Codesys-Runtime-License"


//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------

// Pointer to funktion to search the requested parameter
typedef int (*tFktGetParameter)(char* outputString);


// structure to join the possible input-strings with the processing-function to get them
typedef struct
{
  // input-string from command-line
  char               inputString[MAX_LENGTH_INPUT_STRING];

  // processing function to get the requested parameter
  tFktGetParameter   pFktGetParameter;

} tParameterJumptab;


// structure with the assignment between fieldbus-type-number and fieldbustype-string
typedef struct
{
  int  nr;
  char string[MAX_LENGTH_COUPLER_DETAIL_STRING];

} tFieldbusTypeAssign;


// structure with the assignment of order-number depending on processor-type, fieldbus-type and license-information
typedef struct
{
  char processorType[MAX_LENGTH_COUPLER_DETAIL_STRING];
  char fieldbusType[MAX_LENGTH_COUPLER_DETAIL_STRING];
  char licenseInformation[MAX_LENGTH_COUPLER_DETAIL_STRING];
  char orderNumber[MAX_LENGTH_COUPLER_DETAIL_STRING];

} tOrderNumberAssign;


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------

const char * g_proc_cpuinfo    = "/proc/cpuinfo";

// declaration of all processing functions used for jumptab
int GetProcessorType(char* pProcessorTypeString);
int GetFieldbusType(char* pFieldbusTypeString);
int GetFirmwareRevision(char* pFirmwareRevisionString);
int GetLicenseInformation(char* pLicenseInformationString);
int GetHtmlPagesRevision(char* pHtmlPagesRevisionString);
int GetHostname(char* pHostnameString);
int GetActualHostname(char* pHostnameString);
int GetDomainName(char* pDomainNameString);
int GetActualDomainName(char *pDomainNameString);
int GetKeyboardLayout(char* pOutputString);
int GetPossibleBackup(char* pOutputString);
int GetBootLocation(char* pOutputString);
int GetRS232Owner(char* pOutputString);
int GetKBusFirmwareRevision(char* pOutputString);
int GetWebvisuPort(char* pOuputString);
int GetWebserverPort(char* pOuputString);
int GetDefaultWebserver(char* pOuputString);
int GetCodesysWebserverVersion(char* pOutputString);
//int GetArticleNumber(char *pOutputString);
int GetProductDescription(char *pOutputString);
int GetBootloaderVersion(char *pOutputString);
int GetRS232OwnerAfterReboot(char *pOutputString);
int GetSerialNumber(char *pOutputString);

#ifdef __CT_WITH_TYPELABEL
int GetOrderNumber(char* pOrderNumberString);
#endif

#ifdef TELECONTROL
static int GetValueFromTypelabel(char *pOutputString, const char *fieldName);
#endif

// array of all possible requested parameter (input-strings, strings to search for, processing-function to get them)
static tParameterJumptab astParameterJumptab[] =
{
  { "processor-type",                       GetProcessorType },
  { "fieldbus-type",                        GetFieldbusType },
  { "firmware-revision",                    GetFirmwareRevision },
  { "license-information",                  GetLicenseInformation },
  { "html-pages-revision",                  GetHtmlPagesRevision },
  { "hostname",                             GetHostname },
  { "actual-hostname",                      GetActualHostname },
  { "domain-name",                          GetDomainName },
  { "actual-domain-name",                   GetActualDomainName },
  { "keyboard-layout",                      GetKeyboardLayout },
  { "possible-backup",                      GetPossibleBackup },
  { "boot-location",                        GetBootLocation },
  { "RS232-owner",                          GetRS232Owner },
  { "RS232-owner-after-reboot",             GetRS232OwnerAfterReboot },
  { "kbus-fw-revision",                     GetKBusFirmwareRevision },
  { "webvisu-port",                         GetWebvisuPort },
  { "webserver-port",                       GetWebserverPort },
  { "default-webserver",                    GetDefaultWebserver },
  { "codesys-webserver-version",            GetCodesysWebserverVersion },

#warning "Option bootloader-version requires  /sys/class/wago/system/bootversion!"
  { "bootloader-version",         GetBootloaderVersion },

#ifdef __CT_WITH_TYPELABEL
  { "order-number",               GetOrderNumber },
  { "product-description",        GetProductDescription },  // requires information from typelabel
  { "serial-number",              GetSerialNumber },  // requires information from typelabel
#endif

  // this line must always be the last one - don't remove it!
  { "", NULL }
};


// array for the definition of all possible fieldbus-configuration with their numbers and their types as string
static tFieldbusTypeAssign astFieldbusTypeAssign[] =
{
  { 200, NO_FIELDBUS },
  { 240, NO_FIELDBUS },
  { 280, NO_FIELDBUS },

  { 201, PROFIBUS_MASTER },
  { 241, PROFIBUS_MASTER },
  { 281, PROFIBUS_MASTER },

  { 202, PROFIBUS_SLAVE },
  { 242, PROFIBUS_SLAVE },
  { 282, PROFIBUS_SLAVE },

  { 203, CAN_OPEN_MASTER },
  { 243, CAN_OPEN_MASTER },
  { 283, CAN_OPEN_MASTER },

  { 204, CAN_OPEN_SLAVE },
  { 244, CAN_OPEN_SLAVE },
  { 284, CAN_OPEN_SLAVE },

  { 205, DEVICE_NET_MASTER },
  { 245, DEVICE_NET_MASTER },
  { 285, DEVICE_NET_MASTER },

  { 206, DEVICE_NET_SLAVE },
  { 246, DEVICE_NET_SLAVE },
  { 286, DEVICE_NET_SLAVE },

  // this line must always be the last one - dont't remove it!
  {   0, "Unknown" }
};


#ifdef TELECONTROL
static tOrderNumberAssign astOrderNumberAssign[] =
{
  { GEODE,          NO_FIELDBUS,      CODESYS_LICENSE,    "0758-0870-0000-0130" },
  { GEODE,          PROFIBUS_MASTER,  CODESYS_LICENSE,    "0758-0870-0000-0131" },

  { CELERON,        NO_FIELDBUS,      CODESYS_LICENSE,    "0758-0874-0000-0130" },
  { CELERON,        PROFIBUS_MASTER,  CODESYS_LICENSE,    "0758-0874-0000-0131" },

  { PENTIUM_M_1000, NO_FIELDBUS,      CODESYS_LICENSE,    "0758-0875-0000-0130" },
  { PENTIUM_M_1000, PROFIBUS_MASTER,  CODESYS_LICENSE,    "0758-0875-0000-0131" },
  // this line must always be the last one - dont't remove it!
  { "",             "",               "",                 "" }
};

#else
static tOrderNumberAssign astOrderNumberAssign[] =
{
  { GEODE,          NO_FIELDBUS,      NO_CODESYS_LICENSE, "0758-0870-0000-0310" },
  { GEODE,          NO_FIELDBUS,      CODESYS_LICENSE,    "0758-0870-0000-0110" },

  { GEODE,          PROFIBUS_MASTER,  NO_CODESYS_LICENSE, "0758-0870-0000-0311" },
  { GEODE,          PROFIBUS_MASTER,  CODESYS_LICENSE,    "0758-0870-0000-0111" },

  { GEODE,          CAN_OPEN_MASTER,  NO_CODESYS_LICENSE, "0758-0870-0000-0312" },
  { GEODE,          CAN_OPEN_MASTER,  CODESYS_LICENSE,    "0758-0870-0000-0112" },

  { CELERON,        NO_FIELDBUS,      NO_CODESYS_LICENSE, "0758-0874-0000-0001" },
  { CELERON,        NO_FIELDBUS,      CODESYS_LICENSE,    "0758-0874-0000-0110" },

  { CELERON,        PROFIBUS_MASTER,  NO_CODESYS_LICENSE, "0758-0874-0000-0011" },
  { CELERON,        PROFIBUS_MASTER,  CODESYS_LICENSE,    "0758-0874-0000-0111" },

  { CELERON,        CAN_OPEN_MASTER,  NO_CODESYS_LICENSE, "0758-0874-0000-0012" },
  { CELERON,        CAN_OPEN_MASTER,  CODESYS_LICENSE,    "0758-0874-0000-0112" },

  { PENTIUM_M_1000, NO_FIELDBUS,      NO_CODESYS_LICENSE, "0758-0875-0000-0010" },
  { PENTIUM_M_1000, NO_FIELDBUS,      CODESYS_LICENSE,    "0758-0875-0000-0110" },

  { PENTIUM_M_1000, PROFIBUS_MASTER,  NO_CODESYS_LICENSE, "0758-0875-0000-0011" },
  { PENTIUM_M_1000, PROFIBUS_MASTER,  CODESYS_LICENSE,    "0758-0875-0000-0111" },

  { PENTIUM_M_1000, CAN_OPEN_MASTER,  NO_CODESYS_LICENSE, "0758-0875-0000-0012" },
  { PENTIUM_M_1000, CAN_OPEN_MASTER,  CODESYS_LICENSE,    "0758-0875-0000-0112" },

  { PENTIUM_M_1400, NO_FIELDBUS,      NO_CODESYS_LICENSE, "0758-0876-0000-0010" },
  { PENTIUM_M_1400, NO_FIELDBUS,      CODESYS_LICENSE,    "0758-0876-0000-0110" },

  { PENTIUM_M_1400, PROFIBUS_MASTER,  NO_CODESYS_LICENSE, "0758-0876-0000-0011" },
  { PENTIUM_M_1400, PROFIBUS_MASTER,  CODESYS_LICENSE,    "0758-0876-0000-0111" },

  { PENTIUM_M_1400, CAN_OPEN_MASTER,  NO_CODESYS_LICENSE, "0758-0876-0000-0012" },
  { PENTIUM_M_1400, CAN_OPEN_MASTER,  CODESYS_LICENSE,    "0758-0876-0000-0112" },

  // this line must always be the last one - dont't remove it!
  { "",             "",               "",                 "" }
};
#endif

//------------------------------------------------------------------------------
// external functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local functions
//------------------------------------------------------------------------------

#ifdef __CT_WITH_TYPELABEL
static int GetValueFromTypelabel(char *pOutputString, const char *fieldName)
{
  int ret = SUCCESS;
  tTypeLabel typelabel = typelabel_OBJECT_New();

  if(TYPELABEL_RETURN_OK == typelabel_OBJECT_Open(typelabel))
  {
      typelabel_OBJECT_Sync(typelabel);

      char * value = typelabel_OBJECT_GetValue(typelabel, fieldName);

      if(NULL != value)
      {
        g_strlcpy(pOutputString, value, MAX_LENGTH_COUPLER_DETAIL_STRING);
        free(value);
      }
      else
      {
        ret = INVALID_PARAMETER;
      }

      typelabel_OBJECT_Destroy(typelabel);
  }
  else
  {
    ret = SYSTEM_CALL_ERROR;
  }

  return ret;
}
#endif

static void ShowHelpText(void)
//
// Show describtion and usage of program on stdout
//
{
  int parameterIndex = 0;

  printf("\n* Get several informations about coupler *\n\n");
  printf("Usage: get_coupler_details <parameter> [parameter-value]\n\n");
  printf("parameter: ");

  // read the possible strings for parameter from array and show them like this: "param1 | param2 | param3"
  while(strlen(astParameterJumptab[parameterIndex].inputString) > 0)
  {
    if(parameterIndex != 0) printf("| ");
    printf("%s ", astParameterJumptab[parameterIndex].inputString);
    ++parameterIndex;
  }

  printf("\n\n");
  printf("parameter-value: output is \"checked\", if the result is the same as the given string (else no output).\n");
  //printf("                 This is useful for the display of the result in the context of a checkbox on a html-page.\n");
  printf("\n");
}


int GetProcessorType(char* pProcessorTypeString)
//
// Get the string which describes the processor-type from cpuinfo-file
//
// output: string with the processor-type - must be allocated by calling function!
//
// return: error-code
//
{
  int   status          = SUCCESS;
  char* pCpuinfoContent = NULL;

  if(pProcessorTypeString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  *pProcessorTypeString = '\0';

  char pCmdTemplate[] = "cat %s";
  char cmd[strlen(pCmdTemplate) + strlen(g_proc_cpuinfo) + 1];

  snprintf(cmd, sizeof(cmd), pCmdTemplate, g_proc_cpuinfo);

  // get content of cpuinfo-file (it is not possible to read the file directly to buffer, because its size is ostensible always 0)
  pCpuinfoContent = SystemCall_GetOutput(cmd);

  if(pCpuinfoContent == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    char  cpuinfoLine[MAX_LENGTH_OUTPUT_LINE] = "";
    int   lineNr                              = 1; // first line according to SystemCall_GetLineByNr :(
    char* pFoundProcessorTypeString           = NULL;

    // loop over the lines of cpuinfo to find the line with the model-name-string (x86-specific)
    while((SUCCESS == SystemCall_GetLineByNr(pCpuinfoContent, lineNr, cpuinfoLine)) && (pFoundProcessorTypeString == NULL))
    {
      // if the line with the model-name-string was found (x86)
      if(strstr(cpuinfoLine, "model name") != NULL)
      {
        // look after the ':'-character and then search the begin of next word (that's what we are looking for)
        pFoundProcessorTypeString = strchr(cpuinfoLine, ':');
        pFoundProcessorTypeString = GetNextWord(pFoundProcessorTypeString);
        strncpy(pProcessorTypeString, pFoundProcessorTypeString, MAX_LENGTH_COUPLER_DETAIL_STRING);

        // processor-type-string includes several multiple spaces which are disturbing for the output
        DeleteMultibleSpaces(pProcessorTypeString, MAX_LENGTH_COUPLER_DETAIL_STRING);

        // "unknown"-processor-tpye represents geode -> change string if necessary
        if(strcmp(pProcessorTypeString, "Unknown") == 0) strncpy(pProcessorTypeString, GEODE, MAX_LENGTH_COUPLER_DETAIL_STRING);
      }
      else if(strstr(cpuinfoLine, "Processor") != NULL) // ARM
      {
        // look after the ':'-character and then search the begin of next word (that's what we are looking for)
        pFoundProcessorTypeString = strchr(cpuinfoLine, ':');
        pFoundProcessorTypeString = GetNextWord(pFoundProcessorTypeString);
        strncpy(pProcessorTypeString, pFoundProcessorTypeString, MAX_LENGTH_COUPLER_DETAIL_STRING);

        // processor-type-string includes several multiple spaces which are disturbing for the output
        DeleteMultibleSpaces(pProcessorTypeString, MAX_LENGTH_COUPLER_DETAIL_STRING);
      }


      ++lineNr;
    }

    SystemCall_Destruct(&pCpuinfoContent);
  }

  return(status);
}


int GetFieldbusType(char* pFieldbusTypeString)
//
// Get the string which describes the fieldbus-type. Get the fieldbus-number from name-file and associate the fitting string.
//
// output: string with the fieldbus-type - must be allocated by calling function!
//
// return: error-code
//
{
  int   status                    = SUCCESS;
  char* pFieldbusTypeFileContent  = NULL;

  if(pFieldbusTypeString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pFieldbusTypeString, "");

  // get content of fieldbus-info-file
  pFieldbusTypeFileContent = FileContent_Get("/sys/class/uio/uio0/name");

  if(pFieldbusTypeFileContent == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    char  fieldbusTypeFileLine[MAX_LENGTH_OUTPUT_LINE] = "";
    int   lineNr                                       = 0;

    // loop over the lines of fieldbus-type-file until we find the line with the fieldbus-information
    lineNr = 1;
    while((SUCCESS == FileContent_GetLineByNr(pFieldbusTypeFileContent, lineNr, fieldbusTypeFileLine)) && (strlen(pFieldbusTypeString) == 0))
    {
      // first search for the string which is always directly in front of the fieldbus-number
      const char beginOfFieldbusTypeString[] = "Wago-IPC PLX 0";

      // if the line is no comment and introducing string was found
      if((fieldbusTypeFileLine[0] != COMMENT_CHAR) && (strstr(fieldbusTypeFileLine, beginOfFieldbusTypeString) != NULL))
      {
        int fieldbusTypeNr          = 0;
        int fieldbusTypeAssignIndex = 0;

        // read the fieldbus-type-number behind it and convert it to int
        sscanf(fieldbusTypeFileLine + strlen(beginOfFieldbusTypeString), "%d", &fieldbusTypeNr);

        // iterate through the assignment-list to find the number and its associated string
        // if number can't be found, stop at the last entry of list (with associated string = "Unknown")
        fieldbusTypeAssignIndex = 0;
        while(   (astFieldbusTypeAssign[fieldbusTypeAssignIndex].nr != fieldbusTypeNr)
              && (astFieldbusTypeAssign[fieldbusTypeAssignIndex].nr != 0))
        {
          ++fieldbusTypeAssignIndex;
        }

        // copy string from assignment-table to output-variable
        strncpy(pFieldbusTypeString, astFieldbusTypeAssign[fieldbusTypeAssignIndex].string, MAX_LENGTH_COUPLER_DETAIL_STRING);
      }

      ++lineNr;
    }

    FileContent_Destruct(&pFieldbusTypeFileContent);
  }

  return(status);
}


int GetFirmwareRevision(char *pFirmwareRevisionString)
//
// Get the string which describes the firmware-revision.
//
// output: string with the firmware-revision- must be allocated by calling function!
//
// return: error-code
//
{
  int   status                 = SUCCESS;
  char* pRevisionsFileContent  = NULL;

  if(pFirmwareRevisionString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pFirmwareRevisionString, "");

  // get content of revisions-file
  pRevisionsFileContent = FileContent_Get("/etc/REVISIONS");

  if(pRevisionsFileContent == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    char  revisionsLine[MAX_LENGTH_OUTPUT_LINE] = "";
    int   lineNr                                = 0;

    // loop over the lines of license-file until we find the line with the revision-information
    lineNr = 1;
    while((SUCCESS == FileContent_GetLineByNr(pRevisionsFileContent, lineNr, revisionsLine)) && (strlen(pFirmwareRevisionString) == 0))
    {
      // first search for the string which is always directly in front of the firmware-revision
      const char beginOfRevisionString[] = "FIRMWARE=";

      // if the line is no comment and introducing string was found
      if((revisionsLine[0] != COMMENT_CHAR) && (strstr(revisionsLine, beginOfRevisionString) != NULL))
      {
        // read the following firmware-revision-string and remove possible line-feed or similar behind it
        strncpy(pFirmwareRevisionString, revisionsLine + strlen(beginOfRevisionString), MAX_LENGTH_COUPLER_DETAIL_STRING);
        CutWord(pFirmwareRevisionString, MAX_LENGTH_COUPLER_DETAIL_STRING);
      }

      ++lineNr;
    }

    FileContent_Destruct(&pRevisionsFileContent);
  }

  return(status);
}


int GetLicenseInformation(char* pLicenseInformationString)
//
// Get the string which describes the license-information
//
// output: string with the with the license-information - must be allocated by calling function!
//
// return: error-code
//
{
  int   status               = SUCCESS;
  char* pLicenseFileContent  = NULL;

  if(pLicenseInformationString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pLicenseInformationString, "");

  // get content of revisions-file
  pLicenseFileContent = FileContent_Get("/etc/LICENSE");

  if(pLicenseFileContent == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    char  licenseLine[MAX_LENGTH_OUTPUT_LINE] = "";
    int   lineNr                              = 0;

    // loop over the lines of license-file until we find the line with the license-information
    lineNr = 1;
    while((SUCCESS == FileContent_GetLineByNr(pLicenseFileContent, lineNr, licenseLine)) && (strlen(pLicenseInformationString) == 0))
    {
      // first search for the string which is always directly in front of the license-information
      const char beginOfLicenseString[] = "LICENSE=";

      // if the line is no comment and introducing string was found
      if((licenseLine[0] != COMMENT_CHAR) && (strstr(licenseLine, beginOfLicenseString) != NULL))
      {
        // read the following license-information-string and remove possible line-feed or similar behind it
        strncpy(pLicenseInformationString, licenseLine + strlen(beginOfLicenseString), MAX_LENGTH_COUPLER_DETAIL_STRING);
        CutWord(pLicenseInformationString, MAX_LENGTH_COUPLER_DETAIL_STRING);
      }

      ++lineNr;
    }

    FileContent_Destruct(&pLicenseFileContent);
  }

  return(status);
}



int GetHtmlPagesRevision(char* pHtmlPagesRevisionString)
//
// Get the string which describes the html-pages-revision.
//
// output: string with the html-pages-revision- must be allocated by calling function!
//
// return: error-code
//
{
  int   status                 = SUCCESS;
  char* pRevisionsFileContent  = NULL;

  if(pHtmlPagesRevisionString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pHtmlPagesRevisionString, "");

  // get content of revisions-file
  pRevisionsFileContent = FileContent_Get("/var/www/wbm/REVISIONS");

  if(pRevisionsFileContent == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    char  revisionsLine[MAX_LENGTH_OUTPUT_LINE] = "";
    int   lineNr                                = 0;

    // loop over the lines of license-file until we find the line with the revision-information
    lineNr = 1;
    while((SUCCESS == FileContent_GetLineByNr(pRevisionsFileContent, lineNr, revisionsLine)) && (strlen(pHtmlPagesRevisionString) == 0))
    {
      // first search for the string which is always directly in front of the html-pages-revision
      const char beginOfRevisionString[] = "HTML_PAGES=";

      // if the line is no comment and introducing string was found
      if((revisionsLine[0] != COMMENT_CHAR) && (strstr(revisionsLine, beginOfRevisionString) != NULL))
      {
        // read the following firmware-revision-string and remove possible line-feed or similar behind it
        strncpy(pHtmlPagesRevisionString, revisionsLine + strlen(beginOfRevisionString), MAX_LENGTH_COUPLER_DETAIL_STRING);
        CutWord(pHtmlPagesRevisionString, MAX_LENGTH_COUPLER_DETAIL_STRING);
      }

      ++lineNr;
    }

    FileContent_Destruct(&pRevisionsFileContent);
  }

  return(status);
}

#ifdef __CT_WITH_TYPELABEL
int GetOrderNumber(char* pOrderNumberString)
//
// Get the string which describes the order-number.
//
// output: string with the html-pages-revision- must be allocated by calling function!
//
// return: error-code
//
{
  int  status = SUCCESS;

  char processorType[MAX_LENGTH_COUPLER_DETAIL_STRING + 1]        = "";
  char fieldbusType[MAX_LENGTH_COUPLER_DETAIL_STRING + 1]         = "";
  char licenseInformation[MAX_LENGTH_COUPLER_DETAIL_STRING + 1]   = "";

  if(pOrderNumberString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  pOrderNumberString[0] = '\0';

  // try to read the type label first
  if(SUCCESS != (status = GetValueFromTypelabel(pOrderNumberString, TYPELABEL_ORDER_NR)) )
  {

    // read processor-type, fieldbus-type and license-information, check error
    if(   (SUCCESS != GetProcessorType(processorType))
        || (SUCCESS != GetFieldbusType(fieldbusType))
        || (SUCCESS != GetLicenseInformation(licenseInformation)) )
    {
      status = FILE_READ_ERROR;
    }
    else
    {
      int orderNumberAssignIndex = 0;

      // processor-type-string includes several multible spaces; for the string-comparison we first must delete them
      DeleteMultibleSpaces(processorType, sizeof(processorType));

      // iterate through the assignment-list to find the order-number belonging to processor-type, fieldbus-type and license-information
      // if processor-type can't be found, stop at the last entry of list (with associated string = "")
      orderNumberAssignIndex = 0;
      while((  (strcmp(astOrderNumberAssign[orderNumberAssignIndex].processorType,       processorType)      != 0)
            || (strcmp(astOrderNumberAssign[orderNumberAssignIndex].fieldbusType,        fieldbusType)       != 0)
            || (strcmp(astOrderNumberAssign[orderNumberAssignIndex].licenseInformation,  licenseInformation) != 0) )
            && (strlen(astOrderNumberAssign[orderNumberAssignIndex].processorType) > 0) )
      {
        ++orderNumberAssignIndex;
      }

      // copy string from assignment-table to output-variable
      g_strlcpy(pOrderNumberString, astOrderNumberAssign[orderNumberAssignIndex].orderNumber, MAX_LENGTH_COUPLER_DETAIL_STRING);
    }
  }

  return(status);
}
#endif

static int ReadVariable(char* pFilename, char* pVarName, char* pOutputString)
{
  char* pFileContent  = NULL;
  int   status = SUCCESS;
  char  varnameLine[MAX_LENGTH_OUTPUT_LINE] = "";
  int   varNameSize = strlen(pVarName);
  int   lineNr = 1;

  // initialise output-string
  if(pOutputString == NULL)
  {
    return INVALID_PARAMETER;
  }

  *pOutputString = '\0';
  pFileContent = FileContent_Get(pFilename);

  if(pFileContent)
  {
    // loop over the lines and look for the hostname parameter
    while(SUCCESS == FileContent_GetLineByNr(pFileContent, lineNr, varnameLine))
    {
      if(strncmp(varnameLine, pVarName, varNameSize) == 0)
      {
          CutWord(varnameLine + varNameSize, MAX_LENGTH_OUTPUT_LINE - varNameSize);
          g_strlcpy(pOutputString, varnameLine + varNameSize, MAX_LENGTH_OUTPUT_LINE);
          break;
      }
      lineNr++;
    }
    FileContent_Destruct(&pFileContent);
  }
  else status = FILE_READ_ERROR;
  return status;
}

int GetHostname(char* pHostnameString)
//
// Get the configured hostname.
//
// output: string with the domain-name - must be allocated by calling function!
//
// return: error-code
//
{
  char* cfgfile = "/etc/specific/host.conf";

  return ReadVariable(cfgfile, "HOSTNAME=", pHostnameString);
}

int GetActualHostname(char* pHostnameString)
//
// Get actual used hostname.
//
// output: string with the hostname - must be allocated by calling function!
//
// return: error-code
//
{
  int   status                = SUCCESS;
  char* pEnvironmentHostname  = NULL;

  if(pHostnameString == NULL)
  {
    return INVALID_PARAMETER;
  }

  // initialise output-string
  *pHostnameString = '\0';

  pEnvironmentHostname = SystemCall_GetOutput("hostname");

  if(pEnvironmentHostname == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    // cut line feed and copy hostname string to output variable
    (void)CutLine(pEnvironmentHostname, strlen(pEnvironmentHostname));
    strncpy(pHostnameString, pEnvironmentHostname, MAX_LENGTH_COUPLER_DETAIL_STRING);
    SystemCall_Destruct(&pEnvironmentHostname);
  }

  return status;
}



int GetActualDomainName(char *pDomainNameString)
//
// Get the domain-name
//
// output: string with the domain-name - must be allocated by calling function!
//
// return: error-code
//
{
  int   status = SUCCESS;
  char* pDomainname = NULL;

  if(pDomainNameString == NULL)
  {
    return INVALID_PARAMETER;
  }

  // initialise output-string
  *pDomainNameString = '\0';

  pDomainname = SystemCall_GetOutput("dnsdomainname");

  if(pDomainname == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    // cut line feed and copy domainname string to output variable
    (void)CutLine(pDomainname, strlen(pDomainname));
    strncpy(pDomainNameString, pDomainname, MAX_LENGTH_COUPLER_DETAIL_STRING);
    SystemCall_Destruct(&pDomainname);
  }

  return status;
}


int GetDomainName(char *pDomainNameString)
//
// Get the domain-name
//
// output: string with the domain-name - must be allocated by calling function!
//
// return: error-code
//
{
  char* cfgfile = "/etc/specific/host.conf";

  return ReadVariable(cfgfile, "DNSDOMAIN=", pDomainNameString);
}


int GetKeyboardLayout(char *pOutputString)
//
// Get the layout of the keyboard (german or english)
//
// output: string with the keyboard-layout - must be allocated by calling function!
//
// return: error-code
//
{
  int   status              = SUCCESS;
  char* pKeymapFileContent  = NULL;

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  // get content of revisions-file
  pKeymapFileContent = FileContent_Get("/etc/init.d/keymap");

  if(pKeymapFileContent == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    char  keymapLine[MAX_LENGTH_OUTPUT_LINE]  = "";
    int   lineNr                              = 0;

    // loop over the lines of keymap-file until we find the line with the keyboard-layout
    lineNr = 1;
    while((SUCCESS == FileContent_GetLineByNr(pKeymapFileContent, lineNr, keymapLine)) && (strlen(pOutputString) == 0))
    {
      // first search for the string which is always directly in front of the domain-name
      const char beginOfKeyboardString[] = "loadkeys ";

      // if the line is no comment and introducing string was found
      if((keymapLine[0] != COMMENT_CHAR) && (strstr(keymapLine, beginOfKeyboardString) != NULL))
      {
        // read the following domain-string and remove possible line-feed or similar behind it
        strncpy(pOutputString, strstr(keymapLine, beginOfKeyboardString) + strlen(beginOfKeyboardString), MAX_LENGTH_COUPLER_DETAIL_STRING);
        CutWord(pOutputString, MAX_LENGTH_COUPLER_DETAIL_STRING);

        // change the keyboard-layout-string to the plain-text for html-page
        if(strcmp(pOutputString, "/usr/share/kbd/keymaps/i386/qwertz/de-latin1") == 0)
        {
          strncpy(pOutputString, "German", MAX_LENGTH_COUPLER_DETAIL_STRING);
        }
        else if(strcmp(pOutputString, "/usr/share/kbd/keymaps/i386/qwerty/uk") == 0)
        {
          strncpy(pOutputString, "English", MAX_LENGTH_COUPLER_DETAIL_STRING);
        }
      }
      ++lineNr;
    }

    FileContent_Destruct(&pKeymapFileContent);
  }

  return(status);
}


int GetPossibleBackup(char *pOutputString)
//
// Get the possible direction of cf-card- backup
//
// output: string with the possible direction of backup ("from internal Flash to CF Card" or "from CF Card to internal Flash")
//         - must be allocated by calling function!
//
// return: error-code
//
{
  char  processorTypeString[MAX_LENGTH_COUPLER_DETAIL_STRING] = "";
  int   status                                                = SUCCESS;
  char* pMenuLstFileContent                                   = NULL;

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  status = GetProcessorType(processorTypeString);
  //printf("processorTpye:%s\n", processorTypeString);

  if(status == SUCCESS)
  {
    // get content of menu.lst-file
    pMenuLstFileContent = FileContent_Get("/boot/grub/menu.lst");

    if(pMenuLstFileContent == NULL)
    {
      status = FILE_READ_ERROR;
    }
    else
    {
      char  menuLstLine[MAX_LENGTH_OUTPUT_LINE]         = "";
      int   lineNr                                      = 0;

      // loop over the lines of menu.lst-file until we find the line with the line with the root-string
      lineNr = 1;
      while((SUCCESS == FileContent_GetLineByNr(pMenuLstFileContent, lineNr, menuLstLine)) && (strlen(pOutputString) == 0))
      {
        //printf("%s\n", menuLstLine);
        // if the line is no comment and introducing string for root was found
        if((menuLstLine[0] != COMMENT_CHAR) && (strstr(menuLstLine, "root=/dev/") != NULL))
        {
          // set the direction for backup depending on root-string and processor-type
          if(   (strcmp(processorTypeString, GEODE) == 0) && (strstr(menuLstLine, "root=/dev/hdb") != NULL)
             || (strcmp(processorTypeString, GEODE) != 0) && (strstr(menuLstLine, "root=/dev/hda") != NULL) )
          {
            strncpy(pOutputString, "from CF Card to internal Flash", MAX_LENGTH_COUPLER_DETAIL_STRING);
          }
          else
          {
            strncpy(pOutputString, "from internal Flash to CF Card", MAX_LENGTH_COUPLER_DETAIL_STRING);
          }
        }
        ++lineNr;
      }
    }

    FileContent_Destruct(&pMenuLstFileContent);
  }

  return(status);
}


int GetBootLocation(char *pOutputString)
//
// Get location where coupler booted from.
//
// output: string with the boot location ("intern" or "card")
//         - must be allocated by calling function!
//
// return: error-code
//
{
  char  processorTypeString[MAX_LENGTH_COUPLER_DETAIL_STRING] = "";
  int   status                                                = SUCCESS;
  char* pMenuLstFileContent                                   = NULL;

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  status = GetProcessorType(processorTypeString);
  //printf("processorTpye:%s\n", processorTypeString);

  if(status == SUCCESS)
  {
    // get content of menu.lst-file
    pMenuLstFileContent = FileContent_Get("/boot/grub/menu.lst");

    if(pMenuLstFileContent == NULL)
    {
      status = FILE_READ_ERROR;
    }
    else
    {
      char  menuLstLine[MAX_LENGTH_OUTPUT_LINE]         = "";
      char  rootString[MAX_LENGTH_OUTPUT_LINE]          = "";
      int   lineNr                                      = 0;

      // loop over the lines of menu.lst-file until we find the line with the line with the root-string
      lineNr = 1;
      while((SUCCESS == FileContent_GetLineByNr(pMenuLstFileContent, lineNr, menuLstLine)) && (strlen(pOutputString) == 0))
      {
        //printf("%s\n", menuLstLine);
        // if the line is no comment and introducing string for root was found
        if((menuLstLine[0] != COMMENT_CHAR) && (strstr(menuLstLine, "root=/dev/") != NULL))
        {
          // set the direction for backup depending on root-string and processor-type
          if(   (strcmp(processorTypeString, GEODE) == 0) && (strstr(menuLstLine, "root=/dev/hdb") != NULL)
             || (strcmp(processorTypeString, GEODE) != 0) && (strstr(menuLstLine, "root=/dev/hda") != NULL) )
          {
            strncpy(pOutputString, "CF card", MAX_LENGTH_COUPLER_DETAIL_STRING);
          }
          else
          {
            strncpy(pOutputString, "internal flash", MAX_LENGTH_COUPLER_DETAIL_STRING);
          }
        }
        ++lineNr;
      }
    }

    FileContent_Destruct(&pMenuLstFileContent);
  }

  return(status);
}


int RS232CodesysIsOwner(void)
//
// Return 1, if CoDeSys is owner of the RS232 interface (in accordance with CoDeSys config file)
//
{
  int   codesysIsOwner    = 0;
  char* pRts3scfgSetting  = NULL;
  char  processorTypeString[MAX_LENGTH_COUPLER_DETAIL_STRING] = "";

  // get the CoDeSys setting concerning RS232

  char *pGetSerialStatusCmd;
  char *pSerialOff;
  int ret;

  if( SUCCESS != (ret = GetProcessorType(processorTypeString)) )
  {
    return ret;
  }

//  The check is mis-placed here, because if the RS232-DEVICEFILE is set in rts3scfg.conf,
// it actually means that the owner is 'None' (the string used on IPC when the RS232
// is free to be used by Codesys libs).
//  if(0 == strcmp(processorTypeString, ARM_CORTEX_A8)) // pac200
//  {
//    pGetSerialStatusCmd = "/etc/config-tools/get_rts3scfg_value RS232 DEVICEFILE";
//
//    // pSerialOff = YES is ugly but
//    // 1) it is defined by 3S this way
//    // 2) The device name string in pac case is no constant
//    //
//    //
//    //              |   Serial ON  |  Serial OFF
//    //  -----------------------------------------
//    //    pac200    | device name  | empty string
//    //  -----------------------------------------
//    //    default   |      NO      |     YES
//
//    pSerialOff = "";
//  }
//  else // default
//  {
    pGetSerialStatusCmd = "/etc/config-tools/get_rts3scfg_value PLC DisableSerialProgramming";
    pSerialOff = "YES";
//  }

  pRts3scfgSetting = SystemCall_GetOutput(pGetSerialStatusCmd);

  if(pRts3scfgSetting != NULL)
  {
    //printf("\npRts3sCfgSetting:%s\n", pRts3scfgSetting);

    // query to rts3s.cfg returned sth else than "Serial OFF" => assume "Serial ON"
    if(strcmp(pRts3scfgSetting, pSerialOff) != 0)
    {
      codesysIsOwner = 1;
    }
  }

  SystemCall_Destruct(&pRts3scfgSetting);
  //printf("CodesysIsOwner:%d\n", codesysIsOwner);

  return codesysIsOwner;
}


int RS232LinuxIsOwner(void)
{
  // Get the boot-time serial console owner from inittab
  //
  // Background:
  //
  // The serial interface owner is changed in 2 steps:
  //
  // 1a. user changes setting for the next bootup
  // 1b. eeprom setting is changed so that the kernel already
  // knows if it is allowed to use the serial console when it
  // boots next time.
  //
  // 2. the boot script compares the owner stored in the file
  // with the current owner and edites the config files including
  // inittab.

  // Thus, the EEPROM flag will contain the same information
  // as the file with the owner information and is no reliable
  // source for the current settings.
  // The boot id is unreliable either as it is derived from the
  // EEPROM flag at boot time.

  char* pMenuLstFileContent = NULL;
  int   linuxIsOwner        = 0;

  pMenuLstFileContent = FileContent_Get("/etc/inittab");

  if(pMenuLstFileContent != NULL)
  {
    char  menuLstLine[MAX_LENGTH_OUTPUT_LINE]         = "";
    char  rootString[MAX_LENGTH_OUTPUT_LINE]          = "";
    int   lineNr                                      = 0;

    lineNr = 1;
    while((SUCCESS == FileContent_GetLineByNr(pMenuLstFileContent, lineNr, menuLstLine)))
    {
      //printf("%s\n", menuLstLine);
      // if the line is no comment and introducing string for root was found
      if(   (menuLstLine[0] != COMMENT_CHAR)
          && (strstr(menuLstLine, INITTAB_GETTY_PATTERN) != NULL)
          && (strstr(menuLstLine, SERIAL_DEV_NAME) != NULL) )
      {
        linuxIsOwner = 1;
      }
      ++lineNr;
    }

    FileContent_Destruct(&pMenuLstFileContent);
  }

  return linuxIsOwner;
}

int RS232ModbusIsOwner(void)
//
// Return 1, if modbus is owner of the RS232 interface (in accordance with CoDeSys config file)
//
{
  int   modbusIsOwner     = 0;
  char* pRts3scfgSetting  = NULL;

  // get the CoDeSys setting concerning RS232
  pRts3scfgSetting = SystemCall_GetOutput("/etc/config-tools/get_rts3scfg_value MODBUS_RTU state");

  if(pRts3scfgSetting != NULL)
  {
    //printf("\npRts3sCfgSetting:%s\n", pRts3scfgSetting);
    if(strcmp(pRts3scfgSetting, "enabled") == 0)
    {
      modbusIsOwner = 1;
    }

    SystemCall_Destruct(&pRts3scfgSetting);
  }

  //printf("modbusIsOwner:%d\n", modbusIsOwner);
  return modbusIsOwner;
}

int GetRS232OwnerAfterReboot(char *pOutputString)
{
  char* pRS232OwnerFileContent  = NULL;
  int status = SUCCESS;

  // get content of config file
  pRS232OwnerFileContent = FileContent_Get("/etc/config-tools/RS232_OWNER");

  if(pRS232OwnerFileContent == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    g_strlcpy(pOutputString, pRS232OwnerFileContent, MAX_LENGTH_COUPLER_DETAIL_STRING);
  }

  FileContent_Destruct(&pRS232OwnerFileContent);

  return status;
}

int RS232IoCheckIsOwner(void)
//
// Return 1, if io check is owner of the RS232 interface (in accordance with RS232 owner file)
//
{
  int   ioCheckIsOwner  = 0;
  char* pRS232OwnerFileContent  = NULL;

  // get content of config file
  pRS232OwnerFileContent = FileContent_Get("/etc/config-tools/RS232_OWNER");

  if(pRS232OwnerFileContent != NULL)
  {
    if(strcmp(pRS232OwnerFileContent, "IO-Check") == 0)
    {
      ioCheckIsOwner = 1;
    }

    FileContent_Destruct(&pRS232OwnerFileContent);
  }

  //printf("ioCheckIsOwner:%d\n", ioCheckIsOwner);
  return ioCheckIsOwner;
}

int GetRS232Owner(char *pOutputString)
//
// Get the owner of the RS232-interface
//
// output: string with the RS232-owner - must be allocated by calling function!
//
// return: error-code
//
{
  int   status                  = SUCCESS;
  char* pRS232OwnerFileContent  = NULL;

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  /*if(RS232CodesysIsOwner()) strncpy(pOutputString, "CoDeSys", MAX_LENGTH_COUPLER_DETAIL_STRING);
  else*/ if(RS232LinuxIsOwner()) strncpy(pOutputString, "Linux", MAX_LENGTH_COUPLER_DETAIL_STRING);
  /*else if(RS232ModbusIsOwner()) strncpy(pOutputString, "Modbus", MAX_LENGTH_COUPLER_DETAIL_STRING);*/
  /*else if(RS232IoCheckIsOwner()) strncpy(pOutputString, "IO-Check", MAX_LENGTH_COUPLER_DETAIL_STRING);*/
  else strncpy(pOutputString, "None", MAX_LENGTH_COUPLER_DETAIL_STRING);

  return(status);
}

int GetKBusFirmwareRevision(char* pOutputString)
//
// Get the string which describes the KBus-firmware-revision.
//
// output: string with the KBus-firmware-revision- must be allocated by calling function!
//
// return: error-code
//
{
  int   status                 = SUCCESS;
  char* pRevisionsFileContent  = NULL;

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  // get content of revisions-file
  pRevisionsFileContent = FileContent_Get("/tmp/fw_index_read");

  if(pRevisionsFileContent == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    char  revisionsLine[MAX_LENGTH_OUTPUT_LINE] = "";
    int   lineNr                                = 0;

    // the first line of revision-file includes the searched string - read it
    if(SUCCESS == FileContent_GetLineByNr(pRevisionsFileContent, 1, revisionsLine))
    {
      // copy it to ouput-string and remove possible line-feed or similar behind it
      strncpy(pOutputString, revisionsLine, MAX_LENGTH_COUPLER_DETAIL_STRING);
      CutWord(pOutputString, MAX_LENGTH_COUPLER_DETAIL_STRING);
    }

    FileContent_Destruct(&pRevisionsFileContent);
  }

  return(status);
}


int GetWebvisuPort(char* pOutputString)
//
// Get port of the webvisu webserver.
//
// output: string with the webvisu webserver port ("80" / "8080") - must be allocated by calling function!
//
// return: error-code
//
{
  int   status                    = SUCCESS;
  char* pLighttpdConfFileContent  = NULL;
  char* pX86ConfFileContent       = NULL;

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  // get content of the configuration file
  pX86ConfFileContent = FileContent_Get("/etc/webserver_conf.xml");

  if(pX86ConfFileContent == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    char  webserverConfLine[MAX_LENGTH_OUTPUT_LINE] = "";
    int   lineNr                                    = 0;

    // loop over the lines of webserver conf file until we find the line with the line with the port
    lineNr = 1;
    while((SUCCESS == FileContent_GetLineByNr(pX86ConfFileContent, lineNr, webserverConfLine)) && (strlen(pOutputString) == 0))
    {
      char* portStartString = "<webserver-port-nr>";
      //printf("%s\n", webserverConfLine);

      // if introducing string for port was found
      if(strstr(webserverConfLine, portStartString) != NULL)
      {
        int port = 0;
        char* pPortString = strstr(webserverConfLine, portStartString) + strlen(portStartString);
        //printf("pPortString:%s\n", pPortString);
        sscanf(pPortString, "%d", &port);
        if(port > 0)
        {
          snprintf(pOutputString, MAX_LENGTH_COUPLER_DETAIL_STRING, "%d", port);
        }
      }
      ++lineNr;
    }

    FileContent_Destruct(&pX86ConfFileContent);
  }

  return(status);
}


int GetWebserverPort(char* pOutputString)
//
// Get port of the standar webserver.
//
// output: string with the standard webserver port ("80" / "8080") - must be allocated by calling function!
//         Note: in HTTPS mode the actual port would be 443, but requests on 80/8080, depending on whether
//              lighttpd is the default web server, would be redirected to https.
//
// return: error-code
//
{
  int   status                    = SUCCESS;
  char* pLighttpdConfFileContent  = NULL;
  char* pX86ConfFileContent       = NULL;

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  // get content of the webserver configuration files
  pLighttpdConfFileContent  = FileContent_Get("/etc/lighttpd/http_port_nr.inc");

  if(pLighttpdConfFileContent == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    char  lighttpdConfLine[MAX_LENGTH_OUTPUT_LINE]  = "";
    int   lineNr                                    = 0;

    // loop over the lines of configuration file until we find the line with the line with the port number
    lineNr = 1;
    while((SUCCESS == FileContent_GetLineByNr(pLighttpdConfFileContent, lineNr, lighttpdConfLine)) && (strlen(pOutputString) == 0))
    {
      //printf("%s\n", lighttpdConfLine);
      // if introducing string for port number was found
      if(strstr(lighttpdConfLine, "server.port") != NULL)
      {
        int   port = 0;
        char* pPortString = strchr(lighttpdConfLine, '=') + 1;
        //printf("pPortString:%s\n", pPortString);
        sscanf(pPortString, "%d", &port);
        if(port > 0)
        {
          snprintf(pOutputString, MAX_LENGTH_COUPLER_DETAIL_STRING, "%d", port);
        }
      }
      ++lineNr;
    }

    FileContent_Destruct(&pLighttpdConfFileContent);
  }

  return(status);
}


int GetDefaultWebserver(char* pOutputString)
//
// Get default webserver.
//
// output: string with the default webserver ("Webvisu" / "Webserver" / "Codesys3Visu") - must be allocated by calling function!
//
// return: error-code
//
{
  int   status = INVALID_PARAMETER;
  char  symlinkString[64];
  char  *webvisuFile = "redirect_webvisu.conf";
  char  *webserverFile = "redirect_wbm.conf";

  // Determine the default "webserver" according to the link target of default_redirect.conf.
  *pOutputString = '\0';
  ssize_t llen = readlink("/etc/lighttpd/redirect_default.conf", symlinkString, sizeof(symlinkString));
  if(llen > 0 && llen < sizeof(symlinkString))
  {
    symlinkString[llen] = '\0';
    if(strcmp(basename(symlinkString), webvisuFile) == 0)
    {
      snprintf(pOutputString, MAX_LENGTH_COUPLER_DETAIL_STRING, "Webvisu");
      status = SUCCESS;
    }
    else if(strcmp(basename(symlinkString), webserverFile) == 0)
    {
      snprintf(pOutputString, MAX_LENGTH_COUPLER_DETAIL_STRING, "Webserver");
      status = SUCCESS;
    }
  }
  return(status);
}


int GetCodesysWebserverVersion(char* pOutputString)
//
// Get version string of codesys webserver.
//
// output: string with webserver version - must be allocated by calling function!
//
// return: error-code
//
{
  int   status            = SUCCESS;
  char* pacWebserverVersion = NULL;

  // initialise output-string
  sprintf(pOutputString, "none");

  if (g_file_test("/usr/sbin/webserver", G_FILE_TEST_EXISTS | G_FILE_TEST_IS_EXECUTABLE))
  {
    // get version number (note: only works with webserver version as off 1.1.9.10 !)
    if(NULL != (pacWebserverVersion = SystemCall_GetOutput("/usr/sbin/webserver --version")))
    {
      CutWord(pacWebserverVersion, MAX_LENGTH_COUPLER_DETAIL_STRING);
      snprintf(pOutputString, MAX_LENGTH_COUPLER_DETAIL_STRING, pacWebserverVersion);
      SystemCall_Destruct(&pacWebserverVersion);
    }
  }

  return status;
}

#ifdef __CT_WITH_TYPELABEL
int GetProductDescription(char *pOutputString)
{
  return GetValueFromTypelabel(pOutputString, TYPELABEL_PRODUCT_DESC);
}
#endif

int GetBootloaderVersion(char *pOutputString)
{
  int   status                 = SUCCESS;
    char* pRevisionsFileContent  = NULL;

    if(pOutputString == NULL)
    {
      return(INVALID_PARAMETER);
    }

    // initialise output-string
    sprintf(pOutputString, "");

    // get content of revisions-file
    pRevisionsFileContent = FileContent_Get("/sys/class/wago/system/bootversion");

    if(pRevisionsFileContent == NULL)
    {
      status = FILE_READ_ERROR;
    }
    else
    {
      char  revisionsLine[MAX_LENGTH_OUTPUT_LINE] = "";
      int   lineNr                                = 0;

      // the first line of revision-file includes the searched string - read it
      if(SUCCESS == FileContent_GetLineByNr(pRevisionsFileContent, 1, revisionsLine))
      {
        // copy it to ouput-string and remove possible line-feed or similar behind it
        strncpy(pOutputString, revisionsLine, MAX_LENGTH_COUPLER_DETAIL_STRING);
        CutWord(pOutputString, MAX_LENGTH_COUPLER_DETAIL_STRING);
      }

      FileContent_Destruct(&pRevisionsFileContent);
    }

    return(status);
}

#ifdef __CT_WITH_TYPELABEL
int GetSerialNumber(char *pOutputString)
{
  int   status                 = SUCCESS;
  char* pRevisionsFileContent  = NULL;
  char  stPrgdate[MAX_LENGTH_COUPLER_DETAIL_STRING + 1] = "";
  char  stWagoNr[MAX_LENGTH_COUPLER_DETAIL_STRING + 1] = "";
  uint32_t iWagoNr = 0;
  char  stMarking[MAX_LENGTH_COUPLER_DETAIL_STRING + 1] = "";
  char stMac[MAX_LENGTH_COUPLER_DETAIL_STRING + 1] = "";
  unsigned int macInt[6]={0,0,0,0,0,0};

  if(pOutputString == NULL)
  {
    return(INVALID_PARAMETER);
  }

  // initialise output-string
  sprintf(pOutputString, "");

  do{
    if(SUCCESS != GetValueFromTypelabel(stMarking, TYPELABEL_MARKING))
    {
      if(SUCCESS != GetValueFromTypelabel(stMarking, TYPELABEL_ORDER_NR))
      {
        break;
      }

      // Sonderbehandlung fuer 750-8203 ohne 'MARKING'
      if( strncmp(stMarking, "750-8203", 8) == 0 )
      {
        strcpy(stMarking, "PFC");
      }
      else
      {
        strcpy(stMarking, "");
        break;
      }
    }
    if(SUCCESS != GetValueFromTypelabel(stPrgdate, TYPELABEL_PRGDATE))
    {
      break;
    }

    char  stTmp[MAX_LENGTH_COUPLER_DETAIL_STRING + 1] = "";
    const size_t serialNbLgt = 7;

    if(SUCCESS != GetValueFromTypelabel(stTmp, TYPELABEL_WAGONR))
    {
      // Sonderbehandlung fuer Panels
      if ((strncmp(stMarking, "TP", 2) == 0) || (strncmp(stMarking, "EC", 2) == 0))
      {
        if(SUCCESS == GetValueFromTypelabel(stTmp, "UII"))
        {
          size_t uiiLgt = strlen(stTmp);
          if (uiiLgt >= serialNbLgt)
          {
            strncpy(stWagoNr, &stTmp[uiiLgt-serialNbLgt], serialNbLgt);
          }
          else break;
        }
        else break;
      }
      else break;
    }
    else
    {
      // Sonderbehandlung PFC G2 (10-stellige WAGONR)
      size_t wagoNrLgt = strlen(stTmp);
      if (wagoNrLgt >= serialNbLgt)
      {
        strncpy(stWagoNr, &stTmp[wagoNrLgt-serialNbLgt], serialNbLgt);
      }
      else break;
    }

    if(SUCCESS != GetValueFromTypelabel(stMac, TYPELABEL_MAC))
    {
      break;
    }
  }while(0);

  iWagoNr = strtoul(stWagoNr,NULL,0);
  sscanf(stMac,
         "%X:%X:%X:%X:%X:%X",
          &macInt[0], &macInt[1],
          &macInt[2], &macInt[3],
          &macInt[4], &macInt[5]);
  sprintf(pOutputString, "SN%s-%.7u#%s|%.2X%.2X%.2X%.2X%.2X%.2X",
          stPrgdate,iWagoNr,stMarking,macInt[0],macInt[1],
          macInt[2],macInt[3],macInt[4],macInt[5]);

  return(status);
}
#endif

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
      char* pValueString      = NULL;

      if(argc > 2) pValueString = argv[2];

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
        char outputString[MAX_LENGTH_COUPLER_DETAIL_STRING] = "";
        status = astParameterJumptab[parameterIndex].pFktGetParameter(outputString);

        // if a comparison with a special value should be made for html-page, do it and set output-string accordingly
        if(pValueString != NULL)
        {
          if(strncmp(pValueString, outputString, sizeof(outputString)) == 0)
          {
            strncpy(outputString, "checked", sizeof(outputString));
          }
          else
          {
            strncpy(outputString, "", sizeof(outputString));
          }
        }

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
