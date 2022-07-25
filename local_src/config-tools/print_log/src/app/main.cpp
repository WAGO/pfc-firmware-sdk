//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     main.cpp
///
///  \brief    file print function for WBM diagnostic.
///
///  \author   N. Baade : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <getopt.h>
#include <util_log.hpp>
#include <iostream>
#include <string>       // std::string
#include <sstream>      // std::stringbuf
#include <algorithm>    // std::find

#include "config_tool_lib.h"
//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
#define NO_ARGUMENT (0)
#define REQUIRED_ARGUMENT (1)
//#define OPTIONAL_ARGUMENT (2)
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------
struct option g_longopts[] = {
   { "help",        NO_ARGUMENT,        nullptr,   'h' },
   { "json",        NO_ARGUMENT,        nullptr,   'j' },
   { "read",        REQUIRED_ARGUMENT,  nullptr,   'r' },
   { "scan",        NO_ARGUMENT,        nullptr,   's' },
   { "limit",       REQUIRED_ARGUMENT,  nullptr,   'l' },
   // last line
   { nullptr,       0,                  nullptr,   0   }
};
// path to syslogger files
const char * g_syslogPath = "/var/log/";

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// argc =    1,   2,   3,   4,  ...
// argv =   [0], [1], [2], [3], ...
//------------------------------------------------------------------------------
int main(int    argc,
         char** argv)
{
  int status = SUCCESS;

  // operation code
  int oc;

  // buffer
  int buffer_oc = 0;
  unsigned int buffer_limit = 0;
  bool buffer_json = false;
  std::string buffer_filename;

  // check arg
  if(argc <= 1) {
    status = MISSING_PARAMETER;
  }
  else {

    // first check all options
    while ((oc = getopt_long(argc,
                             argv,
                             "hjr:sl:",
                             &g_longopts[0],
                             nullptr)) != -1) {
      switch (oc) {
        // valid options
        case 'h':
          status = PrintHelpText(std::cout);
          break;
        case 'j':
          buffer_json = true;
          break;
        case 'l':
          buffer_limit = strtoul (optarg, nullptr, 0);
          break;
        // valid option for later doing
        case 'r':
          buffer_oc = oc;
          buffer_filename = optarg;
          break;
        case 's':
          buffer_oc = oc;
          break;
        // invalid options or missing option argument
        case '?':
          status = INVALID_PARAMETER;
          break;
        case ':':
          status = MISSING_PARAMETER;
          break;
        default:
          status = ERROR;
          break;
      }
    }

    // second work with all options
    if(0 != buffer_oc) {
      switch(buffer_oc) {
        case 'r':
          status = INVALID_PARAMETER;
          // check filename characters
          if(buffer_filename.find('/') == std::string::npos)
          {
            // check filename exist
            std::vector<std::string> files = GetFilenames(g_syslogPath);
            if(std::find(files.begin(), files.end(), buffer_filename) != files.end())
            {
              status = PrintFileContent(buffer_filename,
                                        g_syslogPath,
                                        buffer_limit,
                                        std::cout);
            }
          }
          break;
        case 's':
          status = PrintExistingFiles(g_syslogPath,
                                      buffer_json,
                                      std::cout);
          break;
        default:
          status = ERROR;
          break;
      }
    }
  }

  return status;
}

//---- End of source file ------------------------------------------------------


