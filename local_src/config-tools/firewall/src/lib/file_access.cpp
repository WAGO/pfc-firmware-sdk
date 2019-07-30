//------------------------------------------------------------------------------
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     file_access.cpp
///
///  \brief    Provides file accesses functionality.
///
///  \author   WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "file_access.hpp"
#include "error.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
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

//------------------------------------------------------------------------------
/// Returns default path name to configuration file.
///
/// \param name name of type of configuration, e.g. ebtables, iptables, wbm, etc.
/// \return path to cofiguration file
/// \throws std::invalid_argument if a given name isn't recognized
//------------------------------------------------------------------------------
std::string get_config_fname(const std::string& name)
{
    if ("ebtables" == name)
        return "/etc/firewall/ebtables/ebwlist.xml";
    else if ("iptables" == name)
        return "/etc/firewall/iptables/ipcmn.xml";
    else if ("params" == name)
        return "/etc/firewall/params.xml";
    else if ("params_gen" == name)
        return "/etc/firewall/params_gen.xml";
    else if ("services" == name)
        return "/etc/firewall/services/";
    else
        return "/etc/firewall/services/" + name + ".xml";
}

//------------------------------------------------------------------------------
/// Prints content of a text file onto given output stream.
/// \param fname path to file which should be printed
/// \param out output stream to which the file should be dropped
/// \throw std::invalid_argument if file can't be opened
//------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------
/// Reads and parses xml file.
/// \param conf xml file name
/// \param stdio if false 'conf' file will be read, if true standard input is used
/// \return xml document or empty document if failed to read/parse
//------------------------------------------------------------------------------
xmldoc read_configuration(const std::string& conf, const bool stdio)
{
    xmldoc doc;

    if (stdio)
    {
        std::ostringstream oss;
        std::string line;

        while (std::getline(std::cin, line))
        {
            oss << line << "\n";
        }

        doc = parse_string(oss.str());
    }
    else
    {
        doc = read_file(get_config_fname(conf));
    }

    return doc;
}


//------------------------------------------------------------------------------
/// Stores xml document.
/// \param conf xml file name
/// \param stdio if false 'conf' file will be written to, if true standard output is used
//------------------------------------------------------------------------------
void store_configuration(const std::string& conf, const bool stdio, xmldoc& doc)
{
    remove_text_nodes(doc);

    if (stdio)
    {
        std::cout << store_string(doc) << std::endl;
    }
    else
    {
        store_file(get_config_fname(conf), doc);
    }
}

//------------------------------------------------------------------------------
/// Copy file.
/// \param src_file source file name
/// \param dst_file destination file name
//------------------------------------------------------------------------------
void copy_file(const std::string& src_file, const std::string& dst_file)
{
    std::ofstream dst(dst_file, std::ios::binary);
    std::ifstream src(src_file, std::ios::binary);
    if (dst && src)
    {
        dst << src.rdbuf();
    }
    else
    {
        throw invalid_param_error("Couldn't copy file:" + src_file);
    }
}

//------------------------------------------------------------------------------
/// Check if file exists and not empty.
/// \param name  name of file to check
/// \return true if file exists and not empty, otherwise false
//------------------------------------------------------------------------------
bool check_file(const std::string& name)
{
    struct stat buffer;
    int ret = stat (name.c_str(), &buffer);
    return (ret == 0 && buffer.st_size  != 0);
}

} // namespace wago

//---- End of source file ------------------------------------------------------

