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
#include <fstream>
#include <sstream>

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

} // namespace wago

//---- End of source file ------------------------------------------------------

