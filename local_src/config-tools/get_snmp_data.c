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
///  \file     get_snmp_data
///
///  \version  $Revision: 16995 $1
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

#define SHOW_ERRORS                         0

#define SNMP_CONF_FILE                      "/etc/snmp/snmpd.conf"
#define SNMP_TRAP_FILE                      "/etc/snmp/snmptrapd.conf"
#define SNMP_V3_CONF_FILE                   "/var/lib/net-snmp/snmpd.conf"

//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------

// Pointer to funktion to search the requested parameter
typedef int (*tFktGetParameter)(char* pacParameterId,
                                char* pacParameterValue);


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
int GetDeviceName(char* pacDummy,
                  char* pacParameterValue);

int GetDescription(char* pacDummy,
                   char* pacParameterValue);

int GetPhysicalLocation(char* pacDummy,
                        char* pacParameterValue);

int GetContact(char* pacDummy,
               char* pacParameterValue);

int GetObjectID(char* pacDummy,
                char* pacParameterValue);

int GetV1V2cState(char* pacDummy,
                  char* pacParameterValue);

int GetV1V2cCommunityName(char* pacNumber,
                          char* pacParameterValue);

int GetV1V2cTrapReceiverAddress(char* pacNumber,
                                char* pacParameterValue);

int GetV1V2cTrapReceiverCommunityName(char* pacNumber,
                                      char* pacParameterValue);

int GetV1V2cTrapReceiverVersion(char* pacNumber,
                                char* pacParameterValue);

int GetV3AuthName(char* pacNumber,
                  char* pacParameterValue);

int GetV3AuthType(char* pacNumber,
                  char* pacParameterValue);

int GetV3AuthKey(char* pacNumber,
                 char* pacParameterValue);

int GetV3Privacy(char* pacUserNo,
                 char* pacPrivacy);

int GetV3PrivacyKey(char* pacUserNo,
                    char* pacPrivacyKey);

int GetV3NotificationReceiver(char* pacUserNo,
                              char* pacPrivacyKey);

int GetV1V2cTrapReceiverListJson(char* pacUserNo,
                                 char* pacPrivacyKey);

int GetV3UserListJson(char* pacUserNo,
                      char* pacPrivacyKey);


// array of all possible requested parameter (input-strings, strings to search for, processing-function to get them)
static tParameterJumptab astParameterJumptab[] =
{
  { "device-name",                          GetDeviceName },
  { "description",                          GetDescription },
  { "physical-location",                    GetPhysicalLocation },
  { "contact",                              GetContact },
  { "objectID",                             GetObjectID },

  { "v1-v2c-state",                         GetV1V2cState },
  { "v1-v2c-community-name",                GetV1V2cCommunityName },

  { "v1-v2c-trap-receiver-address",         GetV1V2cTrapReceiverAddress },
  { "v1-v2c-trap-receiver-community-name",  GetV1V2cTrapReceiverCommunityName },
  { "v1-v2c-trap-receiver-version",         GetV1V2cTrapReceiverVersion },

  { "v3-auth-name",                         GetV3AuthName },
  { "v3-auth-type",                         GetV3AuthType },
  { "v3-auth-key",                          GetV3AuthKey },
  { "v3-privacy",                           GetV3Privacy },
  { "v3-privacy-key",                       GetV3PrivacyKey },
  { "v3-notification-receiver",             GetV3NotificationReceiver },

  { "v1-v2c-trap-receiver-list-json",       GetV1V2cTrapReceiverListJson },
  { "v3-user-list-json",                    GetV3UserListJson },

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




int GetSnmpConfParameter(char* pacParameterName,
                         char* pacParameterValue)
//
// Get value of a parameter in snmpd.conf file saved in order "parameterName parameterValue"
//
// input: pacParameterName:
// output: string with the parameter value
//         - must be allocated by calling function!
// return: errorcode
//
{
  int   status              = NOT_FOUND;
  char* pacSnmpConfContent  = NULL;

  // check input parameter
  if(pacParameterValue == NULL)
  {
    return INVALID_PARAMETER;
  }

  // initialise output-string
  sprintf(pacParameterValue, "");
  //printf("pacParameterName:%s\n", pacParameterName);

  // get partitions (only _without_ partition numbers from /proc/partitions
  pacSnmpConfContent = FileContent_Get(SNMP_CONF_FILE);

  //if(pPartitionsContent == NULL)
  if(pacSnmpConfContent == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    char  acSnmpConfLine[MAX_LENGTH_OUTPUT_LINE]  = "";
    int   lineNo                                  = 1;

    while(SUCCESS == (status = FileContent_GetLineByNr(pacSnmpConfContent, lineNo, acSnmpConfLine)))
    {
      char* pacFoundString = NULL;
      //printf("line:%s\n", acSnmpConfLine);

      if(NULL != (pacFoundString = strstr(acSnmpConfLine, pacParameterName)))
      {
        if(pacFoundString = GetNextWord(pacFoundString))
        {
          CutLine(pacFoundString, sizeof(acSnmpConfLine));
          strncpy(pacParameterValue, pacFoundString, MAX_LENGTH_OUTPUT_STRING);
        }
      }

      ++lineNo;
    }

    FileContent_Destruct(&pacSnmpConfContent);
  }

  // reset "not found" status, because it's no error, if an parameter is missing
  if(NOT_FOUND == status) status = SUCCESS;

  return status;
}


int GetDeviceName(char* pacDummy,
                  char* pacParameterValue)
{
  return(GetSnmpConfParameter("sysName", pacParameterValue));
}


int GetDescription(char* pacDummy,
                   char* pacParameterValue)
{
  return(GetSnmpConfParameter("sysDescr", pacParameterValue));
}


int GetPhysicalLocation(char* pacDummy,
                        char* pacParameterValue)
{
  return(GetSnmpConfParameter("sysLocation", pacParameterValue));
}


int GetContact(char* pacDummy,
               char* pacParameterValue)
{
  return(GetSnmpConfParameter("sysContact", pacParameterValue));
}

int GetObjectID(char* pacDummy,
                char* pacParameterValue)
{
  return(GetSnmpConfParameter("sysObjectID", pacParameterValue));
}

#if 0
int GetV1V2cState(char* pacDummy,
                  char* pacParameterValue)
{
  char  acCommunityLineGrepCommand[MAX_LENGTH_OUTPUT_LINE]  = "";
  char  acSecurityV1GrepCommand[MAX_LENGTH_OUTPUT_LINE]     = "";
  char  acSecurityV2cGrepCommand[MAX_LENGTH_OUTPUT_LINE]    = "";
  char* pacCommunityLineGrep  = NULL;
  char* pacSecurityV1Grep     = NULL;
  char* pacSecurityV2cGrep    = NULL;
  int   status                = NOT_FOUND;

  // check input parameter
  if(pacParameterValue == NULL)
  {
    return INVALID_PARAMETER;
  }

  // initialise output-string
  sprintf(pacParameterValue, "");

  // define grep commands to search after lines that disable v2/v2c
  // - no command character in front of line, variable count of spaces between words
  snprintf(acCommunityLineGrepCommand, sizeof(acCommunityLineGrepCommand),
           "grep \"^ *com2sec *notConfigUser *default *public *$\" %s", SNMP_CONF_FILE);
  snprintf(acSecurityV1GrepCommand, sizeof(acSecurityV1GrepCommand),
           "grep \"^ *group *notConfigGroup *v1 *notConfigUser *$\" %s", SNMP_CONF_FILE);
  snprintf(acSecurityV2cGrepCommand, sizeof(acSecurityV2cGrepCommand),
           "grep \"^ *group *notConfigGroup *v2c *notConfigUser *$\" %s", SNMP_CONF_FILE);

  // grep for community line
  pacCommunityLineGrep  = SystemCall_GetOutput(acCommunityLineGrepCommand);
  pacSecurityV1Grep     = SystemCall_GetOutput(acSecurityV1GrepCommand);
  pacSecurityV2cGrep    = SystemCall_GetOutput(acSecurityV2cGrepCommand);

  if((pacCommunityLineGrep == NULL) || (pacSecurityV1Grep == NULL) || (pacSecurityV2cGrep == NULL))
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    //printf("res1:%s\n", pacCommunityLineGrep);
    //printf("res2:%s\n", pacSecurityV1Grep);
    //printf("res3:%s\n", pacSecurityV2cGrep);

    if(strlen(pacCommunityLineGrep) && strlen(pacSecurityV1Grep) && strlen(pacSecurityV2cGrep))
    {
      snprintf(pacParameterValue, MAX_LENGTH_OUTPUT_LINE, "disabled");
    }
    else
    {
      snprintf(pacParameterValue, MAX_LENGTH_OUTPUT_LINE, "enabled");
    }

    SystemCall_Destruct(&pacCommunityLineGrep);
    SystemCall_Destruct(&pacSecurityV1Grep);
    SystemCall_Destruct(&pacSecurityV2cGrep);
  }

  return status;
}
#endif



int GetV1V2cCommunityName(char* pacDummy,
                          char* pacParameterValue)
//
// Get the community name defined in /etc/snmp/snmpd.conf by a line like for example:
// rwcommunity "community name"
// If an ip address, "localhost" or similar follows, snmp would only be enabled for this special address,
// so we ignore such lines.
//
{
  char  acGrepCommand[MAX_LENGTH_OUTPUT_LINE] = "";
  char* pacGrepResult  = NULL;
  int   status                = NOT_FOUND;

  // check input parameter
  if(pacParameterValue == NULL)
  {
    return INVALID_PARAMETER;
  }

  // initialise output-string
  sprintf(pacParameterValue, "");

  // define and execute grep command to search after line(s) with community name definition
  // - no command character in front of line, variable count of spaces between words:
  // string format: rwcommunity "community name"
  //snprintf(acGrepCommand, sizeof(acGrepCommand), "grep \"^ *rwcommunity .*$\" %s", SNMP_CONF_FILE);
  snprintf(acGrepCommand, sizeof(acGrepCommand), "egrep \"^ *rwcommunity [^ ]+ *$\" %s", SNMP_CONF_FILE);
  //printf("acGrepCommand:%s\n", acGrepCommand);

  pacGrepResult  = SystemCall_GetOutput(acGrepCommand);
  if(pacGrepResult == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    // take the first line we found (in a correct config file, community name is the same in all lines)
    char  acCommunityNameLine[MAX_LENGTH_OUTPUT_LINE]  = "";
    //printf("pacGrepResult:%s\n", pacGrepResult);

    if(SUCCESS == (status = SystemCall_GetLineByNr(pacGrepResult, 1, acCommunityNameLine)))
    {
      char* pacFoundString = NULL;
      //printf("acCommunityNameLine:%s\n", acCommunityNameLine);

      // search start of word "rwcommunity"
      if(NULL != (pacFoundString = strstr(acCommunityNameLine, "rwcommunity")))
      {
        // get start of next word (= community name), cut the rest of line after it
        if(pacFoundString = GetNextWord(pacFoundString))
        {
          CutWord(pacFoundString, sizeof(acCommunityNameLine));
          strncpy(pacParameterValue, pacFoundString, MAX_LENGTH_OUTPUT_STRING);
        }
      }
    }

    SystemCall_Destruct(&pacGrepResult);
  }

  return status;
}


int GetV1V2cState(char* pacDummy,
                  char* pacParameterValue)
//
// Check if snmp is enabled - file /etc/snmp/snmpd.conf must include a like:
// rwcommunity "community name"
// If an ip address, "localhost" or similar follows, snmp would only be enabled for this special address,
// so we ignore such lines.
//
// String after "community name" is not considered, "community name" is taken from the first founded line
// (should be equal in all lines, if more then one is existing).
//
{
  char  acCommunityName[MAX_LENGTH_OUTPUT_STRING] = "";

  // check input parameter
  if(pacParameterValue == NULL)
  {
    return INVALID_PARAMETER;
  }

  // initialise output-string
  sprintf(pacParameterValue, "disabled");

  if((SUCCESS == GetV1V2cCommunityName(NULL, acCommunityName)) && strlen(acCommunityName))
  {
    strncpy(pacParameterValue, "enabled", MAX_LENGTH_OUTPUT_STRING);
  }

  return SUCCESS;
}


int GetV1V2cTrapReceiverLine(int    trapReceiverNo,
                             char*  pacReceiverLine,
                             int    sizeofReceiverLine)
{
  char  acGrepCommand[MAX_LENGTH_OUTPUT_LINE] = "";
  char* pacGrepResult                         = NULL;
  int   status                                = NOT_FOUND;

  // define and execute grep command to search after line(s) with trap address definition
  // - no command character in front of line, variable count of spaces between words:
  // complete string format: trapsink|trap2sink HOST [COMMUNITY[PORT]] (port is not used in our config)
  // we just look after the trapsink string
  snprintf(acGrepCommand, sizeof(acGrepCommand), "egrep \"^ *trap2?sink .*$\" %s", SNMP_CONF_FILE);
  //printf("acGrepCommand:%s\n", acGrepCommand);

  pacGrepResult  = SystemCall_GetOutput(acGrepCommand);
  if(pacGrepResult == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    // filter the config line with the requested running number
    char  acTrapAddressLine[MAX_LENGTH_OUTPUT_LINE]  = "";
    //printf("pacGrepResult:\n%s\n", pacGrepResult);

    if(SUCCESS == (status = SystemCall_GetLineByNr(pacGrepResult, trapReceiverNo, acTrapAddressLine)))
    {
      //printf("acTrapAddressLine:%s\n", acTrapAddressLine);
      if(strlen(acTrapAddressLine) > (sizeofReceiverLine - 1))
      {
        status = INVALID_PARAMETER;
      }
      else
      {
        strncpy(pacReceiverLine, acTrapAddressLine, sizeofReceiverLine - 1);
      }
    }

    SystemCall_Destruct(&pacGrepResult);
  }
  return status;
}


int GetV1V2cTrapReceiverAddress(char* pacTrapReceiverNo,
                                char* pacParameterValue)
//
//
{
  int   status                                      = NOT_FOUND;
  int   trapReceiverNo                              = 1;
  char  acTrapReceiverLine[MAX_LENGTH_OUTPUT_LINE]  = "";

  // check input parameter
  if(pacParameterValue == NULL)
  {
    return INVALID_PARAMETER;
  }

  // initialise output-string
  sprintf(pacParameterValue, "");

  if(pacTrapReceiverNo)
  {
    sscanf(pacTrapReceiverNo, "%d", &trapReceiverNo);
    if(trapReceiverNo < 1) trapReceiverNo = 1;
  }

  if(SUCCESS == (status = GetV1V2cTrapReceiverLine(trapReceiverNo, acTrapReceiverLine, sizeof(acTrapReceiverLine))))
  {
    char* pacFoundString = NULL;
    //printf("acTrapReceiverLine:%s\n", acTrapReceiverLine);

    // search word "trapsink/trap2sink" in two steps, because we don't know which one we actually have
    if(NULL != (pacFoundString = strstr(acTrapReceiverLine, "trap")))
    {
      if(NULL != (pacFoundString = strstr(acTrapReceiverLine, "sink")))
      {
        // get end of word "sink" and start of next (= ip address), cut the rest of line after it
        if(pacFoundString = GetNextWord(pacFoundString))
        {
          CutWord(pacFoundString, sizeof(acTrapReceiverLine));
          strncpy(pacParameterValue, pacFoundString, MAX_LENGTH_OUTPUT_STRING);
        }
      }
    }
  }

  return status;
}


int GetV1V2cTrapReceiverCommunityName(char* pacTrapReceiverNo,
                                      char* pacParameterValue)
{
  int   status                                      = NOT_FOUND;
  int   trapReceiverNo                              = 1;
  char  acTrapReceiverLine[MAX_LENGTH_OUTPUT_LINE]  = "";

  // check input parameter
  if(pacParameterValue == NULL)
  {
    return INVALID_PARAMETER;
  }

  // initialise output-string
  sprintf(pacParameterValue, "");

  if(pacTrapReceiverNo)
  {
    sscanf(pacTrapReceiverNo, "%d", &trapReceiverNo);
    if(trapReceiverNo < 1) trapReceiverNo = 1;
  }

  if(SUCCESS == (status = GetV1V2cTrapReceiverLine(trapReceiverNo, acTrapReceiverLine, sizeof(acTrapReceiverLine))))
  {
    char* pacFoundString = NULL;
    //printf("acTrapReceiverLine:%s:\n", acTrapReceiverLine);

    // cut possible spaces at the end of the line
    pacFoundString = &acTrapReceiverLine[strlen(acTrapReceiverLine) - 1];
    while(*pacFoundString == ' ')
    {
      *pacFoundString = '\0';
      --pacFoundString;
    }

    // search for the last space (must be the one in front of community name)
    if(NULL != (pacFoundString = strrchr(acTrapReceiverLine, ' ')))
    {
      // copy following word
      strncpy(pacParameterValue, pacFoundString + 1, MAX_LENGTH_OUTPUT_STRING);
    }
  }

  return status;
}


int GetV1V2cTrapReceiverVersion(char* pacTrapReceiverNo,
                                char* pacParameterValue)
{
  int   status                                      = NOT_FOUND;
  int   trapReceiverNo                              = 1;
  char  acTrapReceiverLine[MAX_LENGTH_OUTPUT_LINE]  = "";

  // check input parameter
  if(pacParameterValue == NULL)
  {
    return INVALID_PARAMETER;
  }

  // initialise output-string
  sprintf(pacParameterValue, "");

  if(pacTrapReceiverNo)
  {
    sscanf(pacTrapReceiverNo, "%d", &trapReceiverNo);
    if(trapReceiverNo < 1) trapReceiverNo = 1;
  }

  if(SUCCESS == (status = GetV1V2cTrapReceiverLine(trapReceiverNo, acTrapReceiverLine, sizeof(acTrapReceiverLine))))
  {
    //printf("acTrapReceiverLine:%s\n", acTrapReceiverLine);

    // search word "trapsink/trap2sink"
    if(NULL != strstr(acTrapReceiverLine, "trapsink"))
    {
      strncpy(pacParameterValue, "v1", MAX_LENGTH_OUTPUT_STRING);
    }
    else if(NULL != strstr(acTrapReceiverLine, "trap2sink"))
    {
      strncpy(pacParameterValue, "v2c", MAX_LENGTH_OUTPUT_STRING);
    }
  }

  return status;
}


int GetV3UserConfigLine(int    userNo,
                        char*  pacV3UserLine,
                        int    sizeofV3UserLine)
{
  char  acGrepCommand[MAX_LENGTH_OUTPUT_LINE] = "";
  char* pacGrepResult                         = NULL;
  int   status                                = NOT_FOUND;

  if(pacV3UserLine == NULL)
  {
    return INVALID_PARAMETER;
  }

  // define and execute grep command to search after line(s) with user definition
  // - no command character in front of line, variable count of spaces between words:
  // complete string format: createUser [-e ENGINEID] username (MD5|SHA) authpassphrase [DES|AES] [privpassphrase]
  // we just look after the createUser string
  snprintf(acGrepCommand, sizeof(acGrepCommand), "egrep \"^ *createUser .*$\" %s", SNMP_V3_CONF_FILE);
  //printf("acGrepCommand:%s\n", acGrepCommand);

  pacGrepResult  = SystemCall_GetOutput(acGrepCommand);
  if(pacGrepResult == NULL)
  {
    status = FILE_READ_ERROR;
  }
  else
  {
    // filter the config line with the requested running number
    char  acV3UserLine[MAX_LENGTH_OUTPUT_LINE]  = "";
    //printf("pacGrepResult:\n%s\n", pacGrepResult);

    if(SUCCESS == (status = SystemCall_GetLineByNr(pacGrepResult, userNo, acV3UserLine)))
    {
      //printf("acTrapAddressLine:%s\n", acTrapAddressLine);
      if(strlen(acV3UserLine) > (sizeofV3UserLine - 1))
      {
        status = INVALID_PARAMETER;
      }
      else
      {
        strncpy(pacV3UserLine, acV3UserLine, sizeofV3UserLine - 1);
      }
    }

    SystemCall_Destruct(&pacGrepResult);
  }
  return status;
}



int GetV3AuthName(char* pacUserNo,
                  char* pacAuthName)
{
  int   status                                      = NOT_FOUND;
  int   userNo                              = 1;
  char  acV3UserLine[MAX_LENGTH_OUTPUT_LINE]  = "";

  // check input parameter
  if(pacAuthName == NULL)
  {
    return INVALID_PARAMETER;
  }

  // initialize output-string
  sprintf(pacAuthName, "");

  if(pacUserNo)
  {
    sscanf(pacUserNo, "%d", &userNo);
    if(userNo < 1) userNo = 1;
  }

  if(SUCCESS == (status = GetV3UserConfigLine(userNo, acV3UserLine, sizeof(acV3UserLine))))
  {
    char* pacFoundString = NULL;
    //printf("acV3UserLine:%s\n", acV3UserLine);

    // search the word "createUser" and the following string
    if(NULL != (pacFoundString = strstr(acV3UserLine, "createUser")))
    {
      pacFoundString = GetNextWord(pacFoundString);

      // if -e option follows, jump over it
      if(pacFoundString && !strncmp(pacFoundString, "-e", strlen("-e")))
      {
        pacFoundString = GetNextWord(pacFoundString);
        pacFoundString = GetNextWord(pacFoundString);
      }

      // if auth name was found (after "createUser" or after -e option) - copy it to output variable
      if(pacFoundString)
      {
        CutWord(pacFoundString, MAX_LENGTH_OUTPUT_LINE);
        strncpy(pacAuthName, pacFoundString, MAX_LENGTH_OUTPUT_STRING);
      }
    }
  }

  return status;
}


int GetV3AuthType(char* pacUserNo,
                  char* pacAuthType)
{
  int   status                                      = NOT_FOUND;
  int   userNo                              = 1;
  char  acV3UserLine[MAX_LENGTH_OUTPUT_LINE]  = "";

  // check input parameter
  if(pacAuthType == NULL)
  {
    return INVALID_PARAMETER;
  }

  // initialize output-string
  sprintf(pacAuthType, "");

  if(pacUserNo)
  {
    sscanf(pacUserNo, "%d", &userNo);
    if(userNo < 1) userNo = 1;
  }

  if(SUCCESS == (status = GetV3UserConfigLine(userNo, acV3UserLine, sizeof(acV3UserLine))))
  {
    sprintf(pacAuthType, "none");

    if(strstr(acV3UserLine, "MD5"))
    {
      strncpy(pacAuthType, "MD5", MAX_LENGTH_OUTPUT_STRING);
    }
    else if(strstr(acV3UserLine, "SHA"))
    {
      strncpy(pacAuthType, "SHA", MAX_LENGTH_OUTPUT_STRING);
    }
  }

  return status;
}


int GetV3AuthKey(char* pacUserNo,
                 char* pacAuthKey)
{
  int   status                                      = NOT_FOUND;
  int   userNo                              = 1;
  char  acV3UserLine[MAX_LENGTH_OUTPUT_LINE]  = "";

  // check input parameter
  if(pacAuthKey == NULL)
  {
    return INVALID_PARAMETER;
  }

  // initialize output-string
  sprintf(pacAuthKey, "");

  if(pacUserNo)
  {
    sscanf(pacUserNo, "%d", &userNo);
    if(userNo < 1) userNo = 1;
  }

  if(SUCCESS == (status = GetV3UserConfigLine(userNo, acV3UserLine, sizeof(acV3UserLine))))
  {
    char acLeadingString[MAX_LENGTH_OUTPUT_LINE] = "";

    // search for auth type - this string (if existing) is standing in front of authentification key
    if(SUCCESS == (status = GetV3AuthType(pacUserNo, acLeadingString)))
    {
      // if no auth type is given, the user name must be the string in front of auth key - get it
      if(!strcmp(acLeadingString, "none"))
      {
        status = GetV3AuthName(pacUserNo, acLeadingString);
      }
    }

    //printf("acLeadingString:%s\n", acLeadingString);
    // if we found the string which must be placed in front of auth key...
    // (and string is not privacy string -> auth key is completely missing)
    if(strlen(acLeadingString) && strcmp(acLeadingString, "DES") && strcmp(acLeadingString, "AES"))
    {
      // ... search this string and jump behind it to next word (= auth key) - copy this to output variable
      char* pacFoundString = strstr(acV3UserLine, acLeadingString);
      if(pacFoundString = GetNextWord(pacFoundString))
      {
        CutWord(pacFoundString, MAX_LENGTH_OUTPUT_LINE);
        if(strcmp(pacFoundString, "DES") && strcmp(pacFoundString, "AES"))
        {
          strncpy(pacAuthKey, pacFoundString, MAX_LENGTH_OUTPUT_STRING);
        }
      }
    }
  }

  return status;
}


int GetV3Privacy(char* pacUserNo,
                 char* pacPrivacy)
{
  int   status                                      = NOT_FOUND;
  int   userNo                              = 1;
  char  acV3UserLine[MAX_LENGTH_OUTPUT_LINE]  = "";

  // check input parameter
  if(pacPrivacy == NULL)
  {
    return INVALID_PARAMETER;
  }

  // initialize output-string
  sprintf(pacPrivacy, "");

  if(pacUserNo)
  {
    sscanf(pacUserNo, "%d", &userNo);
    if(userNo < 1) userNo = 1;
  }

  if(SUCCESS == (status = GetV3UserConfigLine(userNo, acV3UserLine, sizeof(acV3UserLine))))
  {
    sprintf(pacPrivacy, "none");

    if(strstr(acV3UserLine, "DES"))
    {
      strncpy(pacPrivacy, "DES", MAX_LENGTH_OUTPUT_STRING);
    }
    else if(strstr(acV3UserLine, "AES"))
    {
      strncpy(pacPrivacy, "AES", MAX_LENGTH_OUTPUT_STRING);
    }
  }

  return status;
}

int GetV3PrivacyKey(char* pacUserNo,
                    char* pacPrivacyKey)
{
  int   status                                      = NOT_FOUND;
  int   userNo                              = 1;
  char  acV3UserLine[MAX_LENGTH_OUTPUT_LINE]  = "";

  // check input parameter
  if(pacPrivacyKey == NULL)
  {
    return INVALID_PARAMETER;
  }

  // initialize output-string
  sprintf(pacPrivacyKey, "");

  if(pacUserNo)
  {
    sscanf(pacUserNo, "%d", &userNo);
    if(userNo < 1) userNo = 1;
  }

  if(SUCCESS == (status = GetV3UserConfigLine(userNo, acV3UserLine, sizeof(acV3UserLine))))
  {
    char acLeadingString[MAX_LENGTH_OUTPUT_LINE] = "";

    // search for auth type - this string (if existing) is standing in front of authentification key
    if(SUCCESS == (status = GetV3Privacy(pacUserNo, acLeadingString)))
    {
      // if no auth type is given, the user name must be the string in front of privacy key - get it
      if(!strcmp(acLeadingString, "none"))
      {
        status = GetV3AuthKey(pacUserNo, acLeadingString);
      }
    }
    //printf("acLeadingString:%s\n", acLeadingString);

    // if we found the string which must be placed in front of privacy key...
    if(strlen(acLeadingString))
    {
      // ... search this string and jump behind it to next word (= privacy key) - copy this to output variable
      char* pacFoundString = strstr(acV3UserLine, acLeadingString);
      //printf("pacFoundString:%s\n", pacFoundString);
      if(pacFoundString = GetNextWord(pacFoundString))
      {
        //printf("GetNextWord pacFoundString:%s\n", pacFoundString);
        CutWord(pacFoundString, MAX_LENGTH_OUTPUT_LINE);
        strncpy(pacPrivacyKey, pacFoundString, MAX_LENGTH_OUTPUT_STRING);
      }
    }
  }

  return status;
}


int GetV3NotificationReceiver(char* pacUserNo,
                              char* pacNotificationReceiver)
{
  int   status                                              = NOT_FOUND;
  int   userNo                                              = 1;
  //char  acV3UserLine[MAX_LENGTH_OUTPUT_LINE]  = "";
  char  acNotificationReceiverLine[MAX_LENGTH_OUTPUT_LINE]  = "";
  char  acAuthName[MAX_LENGTH_OUTPUT_LINE]                  = "";
  char  acAuthType[MAX_LENGTH_OUTPUT_LINE]                  = "";
  char  acAuthKey[MAX_LENGTH_OUTPUT_LINE]                   = "";

  // check input parameter
  if(pacNotificationReceiver == NULL)
  {
    return INVALID_PARAMETER;
  }

  // initialize output-string
  sprintf(pacNotificationReceiver, "");

  if(pacUserNo)
  {
    sscanf(pacUserNo, "%d", &userNo);
    if(userNo < 1) userNo = 1;
  }

  if(   (SUCCESS == (status = GetV3AuthName(pacUserNo, acAuthName)))
     && (SUCCESS == (status = GetV3AuthType(pacUserNo, acAuthType)))
     && (SUCCESS == (status = GetV3AuthKey(pacUserNo, acAuthKey))) )
  {
    char  acGrepCommand[MAX_LENGTH_OUTPUT_LINE]     = "";
    char* pacGrepResult                             = NULL;
    char  acAuthTypeOption[MAX_LENGTH_OUTPUT_LINE]  = "";

    if(strcmp(acAuthType, "none"))
    {
      sprintf(acAuthTypeOption, "-a %s", acAuthType);
    }

    // define and execute grep command to search after line(s) with trap address definition
    // - no command character in front of line, variable count of spaces between words:
    // complete string format: trepsess -v 3 -u auth-name -a auth-type -A auth-key address
    snprintf(acGrepCommand, sizeof(acGrepCommand),
             //"egrep \"^ *trapsess  *-v 3  *-u %s  *%s  *-A %s  *-l authNoPriv .*$\" %s",
             //acAuthName, acAuthTypeOption, acAuthKey, SNMP_V3_CONF_FILE);
            "egrep \"^ *trapsess  *-v 3  *-u %s .*$\" %s", acAuthName, SNMP_V3_CONF_FILE);
    //printf("acGrepCommand(%d):%s\n", strlen(acGrepCommand), acGrepCommand);

    pacGrepResult = SystemCall_GetOutput(acGrepCommand);
    //printf("pacGrepResult:\n%s\n", pacGrepResult);
    if(pacGrepResult == NULL)
    {
      status = FILE_READ_ERROR;
    }
    else if(strlen(pacGrepResult))
    {
      // search for "authNoPriv" string (and variants) and get the word behind (= trap receive address)
      char* pacFoundString = NULL;
      if(NULL == (pacFoundString = strstr(pacGrepResult, "noAuthNoPriv")))
      {
        if(NULL == (pacFoundString = strstr(pacGrepResult, "authNoPriv")))
        {
          if(NULL == (pacFoundString = strstr(pacGrepResult, "noAuthPriv")))
          {
            pacFoundString = strstr(pacGrepResult, "authPriv");
          }
        }
      }

      pacFoundString = GetNextWord(pacFoundString);
      if(pacFoundString)
      {
        CutWord(pacFoundString, MAX_LENGTH_OUTPUT_LINE);
        strncpy(pacNotificationReceiver, pacFoundString, MAX_LENGTH_OUTPUT_STRING);
      }
      SystemCall_Destruct(&pacGrepResult);
    }
  }

  return status;
}


int GetV1V2cTrapReceiverListJson(char* pacDummyInput, char* pacDummyOutput)
{
  int status      = SUCCESS;
  int receiverNo  = 1;

  printf("[ ");

  // loop over all existing trap receiver by there sequential number
  while((SUCCESS == status) && (receiverNo < 100))
  {
    char  acReceiverNo[MAX_LENGTH_OUTPUT_LINE]    = "";
    char  acAddress[MAX_LENGTH_OUTPUT_LINE]       = "";
    char  acCommunityName[MAX_LENGTH_OUTPUT_LINE] = "";
    char  acVersion[MAX_LENGTH_OUTPUT_LINE]       = "";

    // convert receiver no to string for read functions
    sprintf(acReceiverNo, "%d", receiverNo);

    // if no trap receiver with actual number exists, first function will return NOT_FOUND, loop will stop
    if(   (SUCCESS == (status = GetV1V2cTrapReceiverAddress(acReceiverNo, acAddress)))
       && (SUCCESS == (status = GetV1V2cTrapReceiverCommunityName(acReceiverNo, acCommunityName)))
       && (SUCCESS == (status = GetV1V2cTrapReceiverVersion(acReceiverNo, acVersion))) )
    {
      // write all trap receiver data directly to stdout (we don't know how much output we get)
      if(1 != receiverNo) printf(", ");

      printf("{ \"address\": \"%s\", \"communityName\": \"%s\", \"version\": \"%s\" }",
             acAddress, acCommunityName, acVersion);
    }

    ++receiverNo;
  }

  printf(" ]");

  if(NOT_FOUND == status) status = SUCCESS;

  return status;
}


int GetV3UserListJson(char* pacDummyInput, char* pacDummyOutput)
{
  int status      = SUCCESS;
  int userNo      = 1;

  printf("[ ");

  // loop over all existing trap receiver by there sequential number
  while((SUCCESS == status) && (userNo < 100))
  {
    char  acUserNo[MAX_LENGTH_OUTPUT_LINE]                  = "";
    char  acAuthName[MAX_LENGTH_OUTPUT_LINE]                = "";
    char  acAuthType[MAX_LENGTH_OUTPUT_LINE]                = "";
    char  acAuthKey[MAX_LENGTH_OUTPUT_LINE]                 = "";
    char  acPrivacy[MAX_LENGTH_OUTPUT_LINE]                 = "";
    char  acPrivacyKey[MAX_LENGTH_OUTPUT_LINE]              = "";
    char  acNotificationReceiverIp[MAX_LENGTH_OUTPUT_LINE]  = "";

    // convert user no to string for read functions
    sprintf(acUserNo, "%d", userNo);

    if(   (SUCCESS == (status = GetV3AuthName(acUserNo, acAuthName)))
       && (SUCCESS == (status = GetV3AuthType(acUserNo, acAuthType)))
       && (SUCCESS == (status = GetV3AuthKey(acUserNo, acAuthKey)))
       && (SUCCESS == (status = GetV3Privacy(acUserNo, acPrivacy)))
       && (SUCCESS == (status = GetV3PrivacyKey(acUserNo, acPrivacyKey)))
       && (SUCCESS == (status = GetV3NotificationReceiver(acUserNo, acNotificationReceiverIp))) )
    {
      // write all trap receiver data directly to stdout (we don't know how much output we get)
      if(1 != userNo) printf(", ");

      printf("{ \"authName\": \"%s\", \"authType\": \"%s\", \"authKey\": \"%s\", \"privacy\": \"%s\", \"privacyKey\": \"%s\", \"notificationReceiver\": \"%s\" }",
             acAuthName, acAuthType, acAuthKey, acPrivacy, acPrivacyKey, acNotificationReceiverIp);

    }

    ++userNo;
  }

  printf(" ]");

  if(NOT_FOUND == status) status = SUCCESS;

  return status;
}


void ShowHelpText(void)
//
// Show describtion and usage of program on stdout
//
{
  int parameterIndex = 0;

  printf("\n* Get SNMP configuration data *\n\n");
  printf("Usage: get_snmp_data < parameter >\n\n");
  printf("parameter: ");

  // read the possible strings for parameter from array and show them like this: "param1 | param2 | param3"
  while(strlen(astParameterJumptab[parameterIndex].inputString) > 0)
  {
    if(parameterIndex != 0) printf("| ");
    printf("%s ", astParameterJumptab[parameterIndex].inputString);
    ++parameterIndex;
  }

  printf("\n\n");

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
        char  outputString[MAX_LENGTH_OUTPUT_STRING] = "";
        char* pacAdditionalParam  = (argc > 2) ? argv[2] : NULL;

        status = astParameterJumptab[parameterIndex].pFktGetParameter(pacAdditionalParam, outputString);
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
