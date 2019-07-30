//------------------------------------------------------------------------------ 
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file backup.cpp
/// 
/// \brief Backup and restore procedures, compatible with standard
///        backup & restore mechanism.
/// 
/// \author Mariusz Podlesny : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------


#include "backup.hpp"
#include "error.hpp"
#include "process.hpp"
#include "process_ebtables.hpp"
#include "process_iptables.hpp"
#include "process_service.hpp"
#include "regex.hpp"
#include "system.hpp"
#include "xmlhlp.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>




namespace wago
{


#ifndef DEBUGPC
    static const std::string bkp_tmpl = "/etc/firewall";
    static const std::string bkp_path = "/etc/firewall";
//    static const std::string bkp_path = "/tmp/firewall.restore";
#else
    static const std::string bkp_tmpl = "test";
    static const std::string bkp_path = "test";
#endif



void perform_backup(void)
{
#ifndef DEBUGPC
    int ret_status;
    const std::string result(exe_cmd("sh /etc/firewall/fwbackup.sh", ret_status));
#else
    const std::string result(exe_cmd("cd test/; sh fwbackup.sh local"));
#endif
    std::cout << result;
}

class sline
{
public:
    std::string line;
    std::string key;
    std::string value;
    bool processed;

    sline(const std::string& _line);
    ~sline();
};

sline::sline(const std::string& _line)
    : line(_line), key(), value(), processed(false) {}

sline::~sline()
{
}

class file
{
public:
    std::string name;   // Name of configuration part, e.g. ebtables, iptables, or name of a service.
    std::string tmpl;   // Path of a template file, which should be loaded before configuration is changed.
    std::string path;   // Destination path, where the file should finally be stored.
    xmldoc conf;        // Configuration object.

    file();
    file(const std::string& _name, const std::string& _tmpl, const std::string& _path,
         const bool load_tmpl = true);
    file(file&&) = default;
    ~file();

    // Load configuration from a given path.
    void load(const std::string& _path = "");

    // Store the configuration objecto to its destination file.
    void store(const std::string& _path = "");
};

file::file()
    : name(), tmpl(), path(), conf()
{
}

file::file(const std::string& _name, const std::string& _tmpl, const std::string& _path,
           const bool load_tmpl)
    : name(_name), tmpl(_tmpl), path(_path), conf()
{
    if (load_tmpl)
        load();
}

file::~file()
{
}

void file::load(const std::string& _path)
{
    conf = read_file(0 == _path.size() ? tmpl : _path);
}

void file::store(const std::string& _path)
{
    store_file(0 == _path.size() ? path : _path, conf);
}

class files
{
public:
    file ebtables;
    file iptables;
    std::vector<file> services;

    files();
    ~files();
};

files::files()
    : ebtables(), iptables(), services()
{
}

files::~files()
{
}

static std::vector<sline> read_source(void)
{
    regex::regex source_match("^\\s*firewall-[a-zA-Z0-9-].+\\s*=\\s*.+");
    regex::regex exclude_match("^\\s*firewall-config\\s*=\\s*.+");

    std::vector<sline> source;

    source.reserve(1000);

    std::string line;
    while (std::getline(std::cin, line))
    {
        if(regex::is_match(source_match, line) && !regex::is_match(exclude_match, line))
            source.push_back(line);
    }

    return source;
}

static void split_key_value(std::vector<sline>& source)
{
    // Expectation: line is pre-checked and deemed firewall related.
    // If match will not be found an exception is thrown.

    regex::regex source_match("^\\s*firewall-([a-zA-Z0-9-].+)\\s*=\\s*(\\S*)\\s*");

    for (sline& line : source)
    {
        std::vector<std::string> match(regex::get_match(source_match, line.line));

        if (3 != match.size())
            throw invalid_config_error();

        line.key = match.at(1);
        line.value = match.at(2);
    }
}

static std::vector<std::string> split_string(const std::string& line, const char delim)
{
    std::vector<std::string> tokens;

    if (0 == line.size())
        return tokens;

    std::stringstream ss(line);
    std::string item;

    while (std::getline(ss, item, delim))
        tokens.push_back(item);

    if (delim == line[line.size() - 1])
        tokens.push_back(std::string());

    return tokens;
}

static void prepare_files(files& fs, std::vector<sline>& source)
{
    fs.ebtables.name = "ebtables";
    fs.ebtables.tmpl = bkp_tmpl + "/templates/ebwlist.xml";
    fs.ebtables.path = bkp_path + "/ebtables/ebwlist.xml";

    fs.iptables.name = "iptables";
    fs.iptables.tmpl = bkp_tmpl + "/templates/ipcmn.xml";
    fs.iptables.path = bkp_path + "/iptables/ipcmn.xml";

    for (sline& line : source)
    {
        if (line.processed)
            continue;
        if ("service" != line.key)
            continue;
        if (0 == line.value.size())
            throw invalid_config_error();
        if (!regex::is_match(regex::rex_name, line.value))
            throw invalid_param_error();

        file service;

        service.name = line.value;
        service.tmpl = bkp_tmpl + "/templates/service.xml";
        service.path = bkp_path + "/services/" + service.name + ".xml";

        fs.services.push_back(std::move(service));

        line.processed = true;
    }

    fs.ebtables.load();
    fs.iptables.load();

    for (file& f : fs.services)
    {
        f.load();

        xmlctx ctx(get_ctx(f.conf));
        xmlnode rule(get_node(ctx, "/f:firewall/f:ipv4/f:service", false));
        update_attribute(rule, "name", f.name);
    }
}

static void store_files(files& fs)
{
    int ret;
    fs.ebtables.store();
    fs.iptables.store();

    for (file& f : fs.services)
        f.store();

    (void)exe_cmd("sh /etc/firewall/permissions.sh", ret);


// The code below is remarked because in the light of how change of configuration
// is done, in normal cases, restore also may not provide atomicity.
// In case this shall be performed please look at the bkp_path path on top of
// the file.

//    exe_cmd("rm -rf " + bkp_path);
//    exe_cmd("mkdir -p " + bkp_path);
//    const std::string result(exe_cmd("sh /etc/firewall/fwrestore.sh " + bkp_path));
//    exe_cmd("rm -rf " + bkp_path);
//    if (0 != result.size())
//        throw invalid_config_error(result);
}

static void ebtables(xmldoc& doc, std::vector<sline>& source)
{
    regex::regex keyrex("^ebtables-([a-zA-Z0-9-].+)");

    for (sline& line : source)
    {
        if (line.processed)
            continue;

        const std::string key(get_match(keyrex, line.key, 1));

        if (0U == key.size())
            continue;

        std::string cmd;
        std::vector<std::string> values(split_string(line.value, '|'));

        for (std::string& value : values)
            if (0 == value.size())
                value = "-";

        if ("mode" == key)
            cmd = "--set-mode";
        else if ("log" == key)
            cmd = "--set-log";
        else if ("interface" == key)
            cmd = "--set-if";
        else if ("protocols-state" == key)
            cmd = "--toggle-eproto";
        else if ("protocol" == key)
            cmd = "--add-eproto";
        else if ("wlhost" == key)
            cmd = "--add-host";

//        std::cout << "cmd: " << cmd;
//        if (0 < values.size())
//        {
//            std::cout << ", values ";
//            for (std::string& value : values)
//                std::cout << "[" << value << "] ";
//        }
//        std::cout << std::endl;

        if (0 != cmd.size())
        {
            process_ebtables(doc, cmd, values);
            line.processed = true;
        }
        else
            throw invalid_param_error("Unrecognized ebtables restore configuration line.");
    }
}

static void iptables(xmldoc& doc, std::vector<sline>& source)
{
    regex::regex keyrex("^iptables-([a-zA-Z0-9-].+)");

    for (sline& line : source)
    {
        if (line.processed)
            continue;

        const std::string key(get_match(keyrex, line.key, 1));

        if (0U == key.size())
            continue;

        std::string cmd;
        std::vector<std::string> values(split_string(line.value, '|'));

        for (std::string& value : values)
            if (0 == value.size())
                value = "-";

        if ("climits" == key)
            cmd = "--set-climits";
        else if ("echo" == key)
            cmd = "--set-echo";
        else if ("echo-request" == key)
            cmd = "--set-echo-if";
        else if ("forward" == key)
            cmd = "--set-forward";
        else if ("fwlink" == key)
            cmd = "--set-forward-link";
        else if ("snatmasq" == key)
            cmd = "--set-masq";
        else if ("dnatfwp" == key)
            cmd = "--add-pfw";
        else if ("input-openif" == key)
            cmd = "--set-open-if";
        else if ("input-filter" == key)
            cmd = "--add-filter";

//        std::cout << "cmd: " << cmd;
//        if (0 < values.size())
//        {
//            std::cout << ", values ";
//            for (std::string& value : values)
//                std::cout << "[" << value << "] ";
//        }
//        std::cout << std::endl;

        if (0 != cmd.size())
        {
            process_iptables(doc, cmd, values);
            line.processed = true;
        }
        else
            throw invalid_param_error("Unrecognized iptables restore configuration line.");
    }
}

static void service(const std::string& name, xmldoc& doc, std::vector<sline>& source)
{
    regex::regex keyrex("^service-" + name + "-([a-zA-Z0-9-].+)");

    for (sline& line : source)
    {
        if (line.processed)
            continue;

        const std::string key(get_match(keyrex, line.key, 1));

        if (0U == key.size())
            continue;

        std::string cmd;
        std::vector<std::string> values(split_string(line.value, '|'));

        for (std::string& value : values)
            if (0 == value.size())
                value = "-";

        if ("interface" == key)
            cmd = "--set-if";
        else if ("rule" == key)
            cmd = "--add-rule";

//        std::cout << "cmd: " << cmd;
//        if (0 < values.size())
//        {
//            std::cout << ", values ";
//            for (std::string& value : values)
//                std::cout << "[" << value << "] ";
//        }
//        std::cout << std::endl;

        if (0 != cmd.size())
        {
            process_service(doc, cmd, values);
            line.processed = true;
        }
        else
            throw invalid_param_error("Unrecognized " + name + " restore configuration line.");
    }
}

static void services(std::vector<file>& services, std::vector<sline>& source)
{
    for (file& s : services)
        service(s.name, s.conf, source);
}

void perform_restore(void)
{
    std::vector<sline> source(read_source());

    split_key_value(source);

    files fs;

    prepare_files(fs, source);

    ebtables(fs.ebtables.conf, source);
    iptables(fs.iptables.conf, source);
    services(fs.services, source);

    bool complete = true;

    for (const sline& line : source)
    {
        if (!line.processed)
            complete = false;
    }

#ifdef SHOW_ERRORS
    if (!complete)
    {
        std::cout << "Backup restore procedure. Unprocessed backup lines:" << std::endl;
        for (const sline& line : source)
        {
            if (!line.processed)
                std::cout << "Unprocessed: [" << line.line << "] [" << line.key << 
                             "] [" << line.value << "]" << std::endl;
        }
    }
#endif // SHOW_ERRORS

    if (!complete)
        throw invalid_param_error();

    store_files(fs);
}


} // namespace wago

