//------------------------------------------------------------------------------ 
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file error.cpp
/// 
/// \version $Id: error.cpp 31840 2018-08-13 16:23:51Z falk.werner@wago.com $ 
/// 
/// \brief Error helper routines, such as standard messages, logging function
///        and extended exception class.
/// 
/// \author Mariusz Podlesny : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------


#include "error.hpp"
#include <iostream>


namespace wago
{


const std::string error_msg_unknown = "Unknown execution error.";
const std::string error_msg_missing_param = "Missing application parameter(s).";
const std::string error_msg_invalid_param = "Invalid application parameter(s).";
const std::string error_msg_file_open_error = "Can't open a requested file.";
const std::string error_msg_file_read_error = "Can't read a requested file.";
const std::string error_msg_file_close_error = "Couldn't properly close a requested file.";
const std::string error_msg_system_call_error = "System call error.";
const std::string error_msg_invalid_config = "Invalid configuration.";


void log_error_message(const std::string& msg)
{
#ifndef DEBUGPC
    const int res = ct_liblog_setLastError(msg.c_str());

    #ifdef SHOW_ERRORS
    if (SUCCESS != res)
        std::cout << "Couldn't log last error message: [" << msg << "]";
    #else
    (void)res;
    #endif
#else
    (void)msg;
#endif
}


} // namespace wago

