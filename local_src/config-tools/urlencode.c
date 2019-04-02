//------------------------------------------------------------------------------
/// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     $File:     $:
///
///  \version  $Revision: 6569 $:
///
///  \brief    <Part of the config-tools package: encode URL args, decode HTML>
///
///  \author   <AGa> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "config_tool_lib.h"

void usage(void)
{
    printf("\n* Encode URL args/decode HTML strings *\n\n");
    printf("Usage:    urlencode -e string: Encode \n\n");
    printf("          urlencode -d string: Decode\n");
}

int main(int argc, char **argv)
{
  int status = SUCCESS;

  enum modes {ILLEGAL = -1, ENCODE, DECODE, HELP};
  enum modes mode = ILLEGAL;

  char *result = NULL;

  if(argc < 3)
  {
    return MISSING_PARAMETER;
  }

  if(0 == strcmp(argv[1], "-d"))
  {
    mode = DECODE;
  }
  else if (0 == strcmp(argv[1], "-e"))
  {
    mode = ENCODE;
  }

  switch(mode)
  {
    case ENCODE:
      result = g_uri_escape_string(argv[2], NULL, FALSE);
      break;
    case DECODE:
      result = g_uri_unescape_string(argv[2], "");
      break;
    case HELP:
      usage();
      return SUCCESS;
      break;
    case ILLEGAL:
    default:
      usage();
      status = INVALID_PARAMETER;
      break;
  }

  if(NULL == result)
  {
    status = SYSTEM_CALL_ERROR;
  }
  else
  {
    printf("%s", result);
    free(result);
  }

  return status;
}
