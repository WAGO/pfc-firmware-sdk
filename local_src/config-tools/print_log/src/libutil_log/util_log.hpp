//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     util_print_file.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef SRC_LIBUTIL_LOG_UTIL_LOG_HPP_
#define SRC_LIBUTIL_LOG_UTIL_LOG_HPP_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "boost/filesystem.hpp"
#include "config_tool_lib.h"
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
std::string StringList2JsonArray(const std::vector<std::string> & list);

std::vector<boost::filesystem::path> GetFilePaths(const boost::filesystem::path & folderPath);

std::vector<std::string> GetFilenames(const boost::filesystem::path & folderPath);

unsigned int GetNumberOfLines(const boost::filesystem::path & filePath);

unsigned int ReadFileLineByLine(const boost::filesystem::path & filePath,
                                unsigned int limit,
                                std::ostream & out);

eStatusCode PrintHelpText(std::ostream & out);

eStatusCode PrintExistingFiles(const boost::filesystem::path & folderpath,
                               bool json,
                               std::ostream & out);

eStatusCode PrintFileContent(const std::string & filename,
                             const boost::filesystem::path & folderpath,
                             unsigned int limit,
                             std::ostream & out);



#endif /* SRC_LIBUTIL_LOG_UTIL_LOG_HPP_ */
//---- End of source file ------------------------------------------------------

