//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use, and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
#include "filesys.hpp"
#include "error.hpp"
#include <climits>
#include <cstdlib>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


namespace wago {


std::string get_real_path(const std::string& path)
{
    char rpath[1 + PATH_MAX];

    rpath[PATH_MAX] = '\0';

    if (nullptr != realpath(path.c_str(), rpath))
        return std::string(rpath);
    else
        return std::string();
}


void print_file(const std::string& fname, std::ostream& out)
{
    std::ifstream file(fname);
    std::string line;

    if (file.is_open())
    {
        while (std::getline(file, line))
            out << line << '\n';

        file.close();
    }
    else
        throw invalid_param_error("Couldn't open requested file.");
}


bool file_exists(const std::string& fname)
{
    struct stat buf;

    const int res = lstat(fname.c_str(), &buf);

    return 0 == res && S_IFREG == (S_IFMT & buf.st_mode);
}


bool symlink_exists(const std::string& fname)
{
    struct stat buf;

    const int res = lstat(fname.c_str(), &buf);

    return 0 == res && S_IFLNK == (S_IFMT & buf.st_mode);
}


void remove_file(const std::string& fname, const bool throw_on_fail)
{
    if (0 == fname.length())
        throw invalid_param_error("Cant delete file - empty file path passed.");

    const int ret = ::unlink(fname.c_str());

    if (throw_on_fail && -1 == ret)
        throw system_call_error("Failed to remove file.");
}


} // namespace wago

