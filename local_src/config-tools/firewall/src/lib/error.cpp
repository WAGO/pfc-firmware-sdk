//------------------------------------------------------------------------------ 
// Copyright (c) WAGO GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file error.cpp
/// 
/// \brief Error helper routines, such as standard messages, logging function
///        and extended exception class.
/// 
/// \author Mariusz Podlesny : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------


#include "error.hpp"
#include <iostream>


namespace wago {
namespace firewall {


const std::string error_msg_unknown = "Unknown execution error.";
const std::string error_msg_missing_param = "Missing application parameter(s).";
const std::string error_msg_invalid_param = "Invalid application parameter(s).";
const std::string error_msg_file_open_error = "Can't open a requested file.";
const std::string error_msg_file_write_error = "Can't write a requested file.";
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

execution_error::execution_error(const std::string& what, const int code, const std::string& msg)
    : std::runtime_error(what), code_(code), msg_(msg) {}

execution_error::execution_error(const int code, const std::string& msg)
    : std::runtime_error(std::string()), code_(code), msg_(msg) {}

execution_error::~execution_error() { }

int execution_error::code(void) const
        { return code_; }

std::string execution_error::msg(void) const
        { return msg_; }


unknown_error::unknown_error(const std::string& _what,
                     const std::string& _msg)
    : execution_error(_what, SYSTEM_CALL_ERROR, _msg) {}

unknown_error::~unknown_error() { }

missing_param_error::missing_param_error(const std::string& _what,
                           const std::string& _msg)
    : execution_error(_what, MISSING_PARAMETER, _msg) {}

missing_param_error::~missing_param_error() { }

invalid_param_error::invalid_param_error(const std::string& _what,
                           const std::string& _msg)
    : execution_error(_what, INVALID_PARAMETER, _msg) {}

invalid_param_error::~invalid_param_error() { }

file_open_error::file_open_error(const std::string& _what,
                       const std::string& _msg)
    : execution_error(_what, FILE_OPEN_ERROR, _msg) {}

file_open_error::~file_open_error() { }

file_write_error::file_write_error(const std::string& _what,
                       const std::string& _msg)
    : execution_error(_what, FILE_WRITE_ERROR, _msg) {}

file_write_error::~file_write_error() { }

file_read_error::file_read_error(const std::string& _what,
                       const std::string& _msg)
    : execution_error(_what, FILE_READ_ERROR, _msg) {}

file_read_error::~file_read_error() { }

file_close_error::file_close_error(const std::string& _what,
                        const std::string& _msg)
    : execution_error(_what, FILE_CLOSE_ERROR, _msg) {}

file_close_error::~file_close_error() { }

system_call_error::system_call_error(const std::string& _what,
                         const std::string& _msg)
    : execution_error(_what, SYSTEM_CALL_ERROR, _msg) {}

system_call_error::~system_call_error() { }

invalid_config_error::invalid_config_error(const std::string& _what,
                            const std::string& _msg)
    : execution_error(_what, CONFIG_FILE_INCONSISTENT, _msg) {}

invalid_config_error::~invalid_config_error() { }

} // namespace firewall
} // namespace wago

