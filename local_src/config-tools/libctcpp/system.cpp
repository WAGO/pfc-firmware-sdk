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
/// \version $Id: system.cpp 65689 2022-03-11 14:37:43Z falk.werner@wago.com $ 
/// 
/// \brief System related helper functions.
/// 
/// \author Mariusz Podlesny : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------


#include "system.hpp"
#include <cstdio>
#include <sstream>
#include <stdexcept>


namespace wago
{


// Process RAII class.
class process
{
public:
    explicit inline process(FILE* const _handle = NULL)
        : handle(_handle) {}

    inline process(const process& other) = delete;

    inline process(process&& other)
        : handle(other.handle)
    {
        other.handle = NULL;
    }

    inline ~process()
    {
        if (NULL != handle)
            pclose(handle);
    }

    inline process& operator=(const process& rhs) = delete;

    inline process& operator=(process&& rhs)
    {
        if (NULL != handle)
            pclose(handle);
        handle = rhs.handle;
        rhs.handle = NULL;
        return *this;
    }

    inline FILE* get(void) const
            { return handle; }
    inline FILE* release(void)
            { FILE* const _handle = handle; handle = NULL; return _handle; }

    inline bool is_empty(void) const
            { return NULL == handle; }

private:
    FILE* handle;
};


std::string exe_cmd(const std::string& cmd)
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

    return oss.str();
}


} // namespace wago

