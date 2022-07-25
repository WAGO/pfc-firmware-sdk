//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use, and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
#include "logs.hpp"
#include <sstream>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>


namespace wago {


void log_args(int argc, char** argv)
{
    try
    {
        std::ostringstream oss;

        oss << "pid: " << getpid() << ", ppid: " << getppid();
        syslog(LOG_INFO, oss.str().c_str());
        oss.str("");

        oss << "uid: " << getuid() << ", euid: " << geteuid();
        syslog(LOG_INFO, oss.str().c_str());
        oss.str("");

        oss << "gid: " << getgid() << ", egid: " << getegid();
        syslog(LOG_INFO, oss.str().c_str());
        oss.str("");

        oss << "execution call: ";
        for (int i = 0; i < argc; ++i)
        {
            oss << (nullptr != argv[i] ? argv[i] : "[null]");
            oss << " ";
        }
        syslog(LOG_INFO, oss.str().c_str());
    }
    catch (const std::exception& ex)
    {
        syslog(LOG_ERR, "Failed to log call parameters - std::exception: %s.", ex.what());
    }
    catch (...)
    {
        syslog(LOG_ERR, "Failed to log call parameters - unknown reason.");
    }
}


void log_exit(int const result)
{
    try
    {
        std::ostringstream oss;

        oss << "exit value: " << result << (0 == result ? " (success)" : " (failure)");
        syslog(LOG_INFO, oss.str().c_str());
    }
    catch (const std::exception& ex)
    {
        syslog(LOG_ERR, "Failed to log exit value - std::exception: %s.", ex.what());
    }
    catch (...)
    {
        syslog(LOG_ERR, "Failed to log exit value - unknown reason.");
    }
}


void openlog(const std::string& app_name)
{
    static char prefix[30];

    snprintf(prefix, 30, "%s[%i]", app_name.c_str(), getpid());
    ::openlog(prefix, LOG_ODELAY, LOG_USER);
}


} // namespace wago

