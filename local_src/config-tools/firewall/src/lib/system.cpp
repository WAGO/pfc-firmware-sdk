//------------------------------------------------------------------------------ 
// Copyright (c) WAGO GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file system.cpp
/// 
/// \brief System related helper functions.
/// 
/// \author Mariusz Podlesny : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------


#include "system.hpp"
#include <cstdio>
#include <sstream>
#include <stdexcept>
#include <cstdlib>


namespace wago {
namespace firewall {

// Process RAII class.
class process
{
public:
    explicit process(FILE* const _handle = NULL)
        : handle(_handle) {}

    process(const process& other) = delete;

    process(process&& other)
        : handle(other.handle)
    {
        other.handle = NULL;
    }

    ~process()
    {
        if (NULL != handle){
           handle = NULL;
        }
    }

    process& operator=(const process& rhs) = delete;

    process& operator=(process&& rhs)
    {
        if (NULL != handle)
            pclose(handle);
        handle = rhs.handle;
        rhs.handle = NULL;
        return *this;
    }

    int close_pipe (void) const
            { return pclose(handle); }

    FILE* get(void) const
            { return handle; }

    FILE* release(void)
            { FILE* const _handle = handle; handle = NULL; return _handle; }

    bool is_empty(void) const
            { return NULL == handle; }

private:
    FILE* handle;
};

//------------------------------------------------------------------------------
/// Executes an external command (can be shell call).
/// \param cmd command to be executed
/// \param exit_code reference to return status of executed command
/// (last exited child).
/// \return string returned by the executed command
//------------------------------------------------------------------------------
std::string exe_cmd(const std::string& cmd, int &exit_code)
{
    if (0 == cmd.size())
        throw std::logic_error("Can't execute an empty command.");

    process pipe(popen(cmd.c_str(), "r"));

    if (pipe.is_empty())
        throw std::runtime_error("Failed to execute a requested command.");

    static const size_t buffer_size = 1024;
    char buffer[buffer_size];
    std::ostringstream oss;

    while (!feof(pipe.get()))
    {
        if (NULL != fgets(buffer, buffer_size, pipe.get()))
            oss << buffer;
    }
    // get return status of last exited child
    int ret = pipe.close_pipe();
    exit_code= WEXITSTATUS (ret);
    return oss.str();
}


} // namespace firewall
} // namespace wago

