//------------------------------------------------------------------------------ 
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file process_service.cpp
/// 
/// \brief Function processing service's configuration file.
/// 
/// \author Mariusz Podlesny : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------


#include "process_services.hpp"

#include "error.hpp"
#include "process.hpp"
#include "regex.hpp"
#include <cassert>
#include <sys/types.h>
#include <dirent.h>
#include <algorithm>
#include <iostream>


namespace wago
{


class status
{
public:
    status() = default;
    status(const status&) = default;
    status(status&&) = default;
    status(const std::string& _interface,
                  const std::string& _service,
                  const std::string& _state);
    ~status();
    status& operator=(const status&) = default;
    status& operator=(status&&) = default;

    bool operator<(const status& rhs) const;

    std::string interface;
    std::string service;
    std::string state;
};

status::status(const std::string& _interface,
              const std::string& _service,
              const std::string& _state)
    : interface(_interface), service(_service), state(_state) {}

status::~status() = default;


bool status::operator<(const status& rhs) const
{
    return interface < rhs.interface ||
           (interface == rhs.interface && service < rhs.service) ||
           (interface == rhs.interface && service == rhs.service && state < rhs.state);
}


static std::vector<std::string> filter_files(const std::vector<struct dirent>& des)
{
    regex::regex rex("^.*\\.xml$");
    std::vector<std::string> fnames;

    for (const struct dirent& de : des)
    {
// d_type check doesn't work on target platform.
//        if (DT_REG == de.d_type && regex::is_match(rex, de.d_name))
        if (regex::is_match(rex, de.d_name))
            fnames.push_back(de.d_name);
    }

    return fnames;
}

static std::vector<struct dirent> scan_dir(const std::string& dir)
{
    assert(0 < dir.length());

    DIR* const d = opendir(dir.c_str());

    if (nullptr == d)
        throw invalid_param_error("Can't access requested directory.");

    std::vector<struct dirent> des;
    struct dirent* de;

    while (nullptr != (de = readdir(d)))
        des.push_back(*de);

    closedir(d);

    return des;
}

static std::vector<std::string> list_configs(const std::string& dir)
{
    return filter_files(scan_dir(dir));
}

static void scan_service(const std::string& fname, std::vector<status>& sts)
{
    xmldoc doc(read_file(fname));
    xmlctx ctx(get_ctx(doc));

    const int ncount = get_node_count(ctx, "/f:firewall/f:ipv4/f:service/f:interfaces/f:interface");

    if (ncount < 0)
        return;

    const std::string service(get_string(ctx, "string(/f:firewall/f:ipv4/f:service/@name)"));

    for (int i = 0; i < ncount; ++i)
    {
        const std::string xpbase("string(/f:firewall/f:ipv4/f:service/f:interfaces/f:interface[" + 
                                 std::to_string(1 + i) + "]/@");

        const std::string state(get_string(ctx, xpbase + "state)"));
        const std::string interface(get_string(ctx, xpbase + "if)"));

        sts.push_back(status(interface, service, state));
    }
}

static void print_xml(const std::vector<status>& sts)
{
    std::cout << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << "\n";
    std::cout << "<firewall>" << "\n";
    std::cout << "    <ipv4>" << "\n";

    std::string interface;

    for (const status& st : sts)
    {
        if (interface != st.interface)
        {
            if (0 != interface.length())
                std::cout << "        </interface>" << "\n";
            interface = st.interface;
            std::cout << "        <interface name=\"" << st.interface << "\">" << "\n";
        }

        std::cout << "            <service name=\"" << st.service
                  << "\" state=\"" << st.state << "\"/>" << "\n";

    }

    if (0 < sts.size())
        std::cout << "        </interface>" << "\n";
    std::cout << "    </ipv4>" << "\n";
    std::cout << "</firewall>" << "\n";
}

static void print_json(const std::vector<status>& sts)
{
    // The function is quite simple for now, but in case more types of info
    // should be added to the generated json, please, use a real json library!

    std::string interface;

    std::cout << "{" << "\n";
    std::cout << "    \"interfaces\":" << "\n";
    std::cout << "    {" << "\n";

    for (const status& st : sts)
    {
        if (interface == st.interface)
            std::cout << ",";
        std::cout << "\n";

        if (interface != st.interface)
        {
            if (0 != interface.length())
            {
                std::cout << "            }" << "\n";
                std::cout << "        }," << "\n";
            }
            interface = st.interface;

            std::cout << "        \"" << st.interface << "\":\n";
            std::cout << "        {" << "\n";
            std::cout << "            \"services\":\n";
            std::cout << "            {" << "\n";
        }

        std::cout << "                \"" << st.service << "\": {\"state\": \""
                  << st.state << "\"" << "}";
    }

    if (0 < sts.size())
    {
        std::cout << "\n";
        std::cout << "            }" << "\n";
        std::cout << "        }" << "\n";
    }

    std::cout << "    }" << "\n";
    std::cout << "}" << "\n";
}

static void get_ifs_status(std::string dir,
                           const std::vector<std::string>& argv)
{
    if (dir.length() < 1 || 1 != argv.size())
        throw invalid_param_error();

    if ('/' != dir.at(dir.length() - 1))
        dir += '/';

    const std::string& format(argv[0]);

    if ("xml" != format && "json" != format)
        throw invalid_param_error("Unsupported format requested.");

    const std::vector<std::string> fnames(list_configs(dir));

    std::vector<status> sts; // Statuses of all services on all interfaces.

    for (const std::string& fname : fnames)
        scan_service(dir + fname, sts);

    std::sort(sts.begin(), sts.end());

    if ("xml" == format)
        print_xml(sts);
    else
        print_json(sts);
}

void process_services(const std::string& cmd,
                      const std::string& dir,
                      const std::vector<std::string>& argv)
{
    if ("--get-ifs-status" == cmd)
        get_ifs_status(dir, argv);
    else
        throw invalid_param_error("Unrecognized command.");
}


} // namespace wago

