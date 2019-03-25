//------------------------------------------------------------------------------
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test_utils.cpp
///
///  \brief    Implementation of test utils
///
///  \author   WF : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "test_utils.hpp"

#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------

namespace wago
{

std::string create_temp_dir(std::string const & prefix)
{
  std::string tempDirTemplate = std::string("/tmp/") + prefix + "XXXXXX";
  char * szTemplate = strdup(tempDirTemplate.c_str());

  std::string result = mkdtemp(szTemplate);
  free(szTemplate);
  return result;
}

void create_dir(std::string const & name)
{
  std::string command = std::string("mkdir -p ") + name;
  ::system(command.c_str());
}

void remove_dir(std::string const & name)
{
  std::string command = std::string("rm -rf ") + name;
  system(command.c_str());
}

bool file_exists(std::string const & name)
{
  struct stat buffer;
  return ( stat (name.c_str(), &buffer) == 0);
}

void copy_file(std::string const & source, std::string const & target)
{
  std::string command = ("cp " + source + " " + target);
  ::system(command.c_str());
}


}

//---- End of source file ------------------------------------------------------

