//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use, and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
#ifndef WAGO_LOGS_HPP
#define WAGO_LOGS_HPP


#include <string>


namespace wago {


//------------------------------------------------------------------------------
/// Log all parameters to syslog.
/// \param argc number of parameters
/// \param argv parameters of main function call
//------------------------------------------------------------------------------
void log_args(int argc, char** argv);

//------------------------------------------------------------------------------
/// Log program exit value.
/// \param result program exit value.
//------------------------------------------------------------------------------
void log_exit(int const result);

//------------------------------------------------------------------------------
/// Open syslog.
/// \param app_name name of the application
//------------------------------------------------------------------------------
void openlog(const std::string& app_name);


} // namespace wago


#endif // WAGO_LOGS_HPP
