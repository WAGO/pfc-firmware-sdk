//----------------------------------------------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All manufacturing, reproduction, use and 
// sales rights pertaining to this subject matter are governed by the license agreement. The recipient of this software 
// implicitly accepts the terms of the license.
//----------------------------------------------------------------------------------------------------------------------
///---------------------------------------------------------------------------------------------------------------------
///  \file
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
///---------------------------------------------------------------------------------------------------------------------

#include <wc/log.h>
#include <iostream>
#include <string>
#include <gtest/gtest.h>

namespace
{

char const * loglevel_str(log_level_t  const log_level)
{
    switch (log_level)
    {
        case log_level::off: return "off";
        case log_level::fatal: return "fatal";
        case log_level::error: return "error";
        case log_level::warning: return "warning";
        case log_level::info: return "info";
        case log_level::debug: return "debug";
        default: return "unknown";
    }
}

}

extern "C"
{

void wc_log_output(
        log_level_t  const log_level,
        char const * const message)
{
    std::cout << loglevel_str(log_level) << ": " << message << std::endl;
}

log_level_t wc_get_log_level(void)
{
    return log_level::debug;
}

//void wc_Fail(char const * const szReason,
//             char const * const szFile,
//             char const * const szFunction,
//             int const line)
//{
//    std::string problem = szReason;
//    problem += " [";
//    problem += "from ";
//    problem += szFile;
//    problem += " in function ";
//    problem += szFunction;
//    problem += ", line ";
//    problem += std::to_string(line);
//    problem += "]";
//
//    // Trigger test fail
//    ASSERT_STREQ("", problem.c_str());
//}



}


