//------------------------------------------------------------------------------
/// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
/// manufacturing, reproduction, use, and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \file ${file_name}
///
/// \version <Revision>: $Rev: 5981 $
///
/// \author ${user} $Author$ : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------


#include "CppUTest/TestHarness.h"
#include <cstring>
#include <cstdio>

#include "config_tool_lib.h"
#include "wago_ct_get_rts_info.h"

extern const char * g_proc_cpuinfo;

static char s_path[256];
static char *s_path_end;

extern "C" 
{
  int ShowProjectInfo(char* psnParameterName,
                      char *pOutputString);
};

#define INDEX_EMPTY 0
#define INDEX_MAX_LEN 1
#define INDEX_NO_SPECIAL_CHARS 2
#define INDEX_SPECIAL_CHARS 3
#define INDEX_MAX_LEN_NO_NULL_TERM 4

static int g_index;

// ISO-8859-1 codes: Ae    Ae    Oe    oe    Ue    ue    ss
static char umlautsAnsi[8] = {0xc4, 0xe4, 0xd6, 0xf6, 0xdc, 0xfc, 0xdf}; 

static char *umlautsUtf = "ÄäÖöÜüß"; 

tstMsgProjectInfo ipcResponseList[] = 
{
  // All empty
  {
    0,  // DATE
    "", // STPROJECT 
    "", // STTITLE
    "", // STVERSION
    "", // STAUTHOR
    "", // STDESCRIPTION
  },

  // Max length (fill in setup()) 
  {
    0,  // DATE
    "", // STPROJECT 
    "", // STTITLE
    "", // STVERSION
    "", // STAUTHOR
    "", // STDESCRIPTION
  },

  // No special characters 
  {
    0,  // DATE
    "Custom Project", // STPROJECT 
    "Custom Title", // STTITLE
    "0.1.2.3", // STVERSION
    "Andrej Gantvorg", // STAUTHOR
    "Test data", // STDESCRIPTION
  },

  // Special characters (ANSI, fill in setup) 
  {
    0,  // DATE
    "", // STPROJECT 
    "", // STTITLE
    "", // STVERSION
    "", // STAUTHOR
    "", // STDESCRIPTION
  },

  // Max length w/o Null-termination (fill in setup()) 
  {
    0,  // DATE
    "", // STPROJECT 
    "", // STTITLE
    "", // STVERSION
    "", // STAUTHOR
    "", // STDESCRIPTION
  },

};

extern "C" int ipcMsgCom_Send(int    messageType,
                   void*  pMessageContent,
                   int    sizeofContent,
                   int    responseRequested,
                   int    timeout)
{
  tstMsgProjectInfo *response = (tstMsgProjectInfo *) pMessageContent;

  strcpy(response->STVERSION, ipcResponseList[g_index].STVERSION);
  strcpy(response->STTITLE, ipcResponseList[g_index].STTITLE);
  strcpy(response->STAUTHOR, ipcResponseList[g_index].STAUTHOR);
  strcpy(response->STDESCRIPTION, ipcResponseList[g_index].STDESCRIPTION);

  return SUCCESS;
}

TEST_GROUP(get_rts_info)
{
  void setup()
  {

    // init max-len fake responses
    int i;

    for(i=0;i<256;++i)
    {
      ipcResponseList[INDEX_MAX_LEN].STVERSION[i] = 'a';
      ipcResponseList[INDEX_MAX_LEN].STTITLE[i] = 'b';
      ipcResponseList[INDEX_MAX_LEN].STAUTHOR[i] = 'c';
      ipcResponseList[INDEX_MAX_LEN].STDESCRIPTION[i] = '~';
      
      ipcResponseList[INDEX_MAX_LEN_NO_NULL_TERM].STVERSION[i] = 'a';
      ipcResponseList[INDEX_MAX_LEN_NO_NULL_TERM].STTITLE[i] = 'b';
      ipcResponseList[INDEX_MAX_LEN_NO_NULL_TERM].STAUTHOR[i] = 'c';
      ipcResponseList[INDEX_MAX_LEN_NO_NULL_TERM].STDESCRIPTION[i] = '~';

    }
    for(i=255;i<1024;++i)
    {
      ipcResponseList[INDEX_MAX_LEN].STDESCRIPTION[i] = '~';
      ipcResponseList[INDEX_MAX_LEN_NO_NULL_TERM].STDESCRIPTION[i] = '~';
    }

    // Now 0-terminate INDEX_MAX_LEN (INDEX_MAX_LEN_NO_NULL_TERM remains unterminated)
    ipcResponseList[INDEX_MAX_LEN].STVERSION[255] = '\0';
    ipcResponseList[INDEX_MAX_LEN].STTITLE[255] = '\0';
    ipcResponseList[INDEX_MAX_LEN].STAUTHOR[255] = '\0';

    ipcResponseList[INDEX_MAX_LEN].STDESCRIPTION[1023] = '\0';

    // init fake response w/special characters

//    strcpy(ipcResponseList[INDEX_SPECIAL_CHARS].STVERSION,     "         Custom Project");
//    strcpy(ipcResponseList[INDEX_SPECIAL_CHARS].STTITLE,       "         Custom title");
//    strcpy(ipcResponseList[INDEX_SPECIAL_CHARS].STAUTHOR,      "         Author McAuthor");
//    strcpy(ipcResponseList[INDEX_SPECIAL_CHARS].STDESCRIPTION, "         Fake description");
//
    strcpy(ipcResponseList[INDEX_SPECIAL_CHARS].STVERSION, umlautsAnsi);
    strcpy(ipcResponseList[INDEX_SPECIAL_CHARS].STTITLE,   umlautsAnsi);
    strcpy(ipcResponseList[INDEX_SPECIAL_CHARS].STAUTHOR,  umlautsAnsi);
    strcpy(ipcResponseList[INDEX_SPECIAL_CHARS].STDESCRIPTION, umlautsAnsi);

  }

  void teardown()
  {
  }
};

TEST(get_rts_info, get_project_info)
{

  /*
   *
   * Test empty description
   *
   */

  g_index = INDEX_EMPTY;
  char result[256];

  int ret = ShowProjectInfo("version", result);
  LONGS_EQUAL(SUCCESS, ret);
  STRCMP_EQUAL("", result);

  ret = ShowProjectInfo("title", result);
  LONGS_EQUAL(SUCCESS, ret);
  STRCMP_EQUAL("", result);

  ret = ShowProjectInfo("author", result);
  LONGS_EQUAL(SUCCESS, ret);
  STRCMP_EQUAL("", result);

  /*
   *
   * Test max length
   *
   */

  g_index = INDEX_MAX_LEN;

  ret = ShowProjectInfo("version", result);
  LONGS_EQUAL(SUCCESS, ret);
  STRCMP_EQUAL(ipcResponseList[INDEX_MAX_LEN].STVERSION, result);

  ret = ShowProjectInfo("title", result);
  LONGS_EQUAL(SUCCESS, ret);
  STRCMP_EQUAL(ipcResponseList[INDEX_MAX_LEN].STTITLE, result);

  ret = ShowProjectInfo("author", result);
  LONGS_EQUAL(SUCCESS, ret);
  STRCMP_EQUAL(ipcResponseList[INDEX_MAX_LEN].STAUTHOR, result);

  /*
   *
   * Test max length w/o 0-Termination
   *
   */

  g_index = INDEX_MAX_LEN_NO_NULL_TERM;

  ret = ShowProjectInfo("version", result);
  LONGS_EQUAL(SUCCESS, ret);
  // ShowProjectInfo has to return no more than 256 chars even
  // if the struct contains not 0-terminated strings 
  STRCMP_EQUAL(ipcResponseList[INDEX_MAX_LEN].STVERSION, result);

  ret = ShowProjectInfo("title", result);
  LONGS_EQUAL(SUCCESS, ret);
  // ShowProjectInfo has to return no more than 256 chars even
  // if the struct contains not 0-terminated strings 
  STRCMP_EQUAL(ipcResponseList[INDEX_MAX_LEN].STTITLE, result);

  ret = ShowProjectInfo("author", result);
  LONGS_EQUAL(SUCCESS, ret);
  // ShowProjectInfo has to return no more than 256 chars even
  // if the struct contains not 0-terminated strings 
  STRCMP_EQUAL(ipcResponseList[INDEX_MAX_LEN].STAUTHOR, result);

  /*
   *
   * Test ascii-only description
   *
   */

  g_index = INDEX_NO_SPECIAL_CHARS;
  
  ret = ShowProjectInfo("version", result);
  LONGS_EQUAL(SUCCESS, ret);
  STRCMP_EQUAL(ipcResponseList[INDEX_NO_SPECIAL_CHARS].STVERSION, result);

  ret = ShowProjectInfo("title", result);
  LONGS_EQUAL(SUCCESS, ret);
  STRCMP_EQUAL(ipcResponseList[INDEX_NO_SPECIAL_CHARS].STTITLE, result);

  ret = ShowProjectInfo("author", result);
  LONGS_EQUAL(SUCCESS, ret);
  STRCMP_EQUAL(ipcResponseList[INDEX_NO_SPECIAL_CHARS].STAUTHOR, result);

  /*
   *
   * Test umlauts
   *
   */

  g_index = INDEX_SPECIAL_CHARS;
  
  ret = ShowProjectInfo("version", result);
  LONGS_EQUAL(SUCCESS, ret);
  STRCMP_EQUAL(umlautsUtf, result);

  ret = ShowProjectInfo("title", result);
  LONGS_EQUAL(SUCCESS, ret);
  STRCMP_EQUAL(umlautsUtf, result);

  ret = ShowProjectInfo("author", result);
  LONGS_EQUAL(SUCCESS, ret);
  STRCMP_EQUAL(umlautsUtf, result);

}
