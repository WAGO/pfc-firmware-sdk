// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#include "vpncfg.hpp"
#include "error.hpp"
#include "filesys.hpp"
#include "services.hpp"
#include "system.hpp"
#include "x509.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#include "json/json.h"
#pragma GCC diagnostic pop

#include <grp.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <cassert>
#include <iostream>


namespace wago {


static const std::string type_source = "source";
static const std::string type_out = "out";
static const std::string type_cert = "cert";
static const std::string type_key = "key";
static const std::string type_ipsec_conf = "ipsec.conf";
static const std::string type_ipsec_secrets = "ipsec.secrets";
static const std::string type_openvpn_conf = "openvpn.conf";

static const std::string path_source = "/tmp/vpncfg/";
static const std::string path_out = "/tmp/vpncfg-out/";
static const std::string path_cert = "/etc/certificates/";
static const std::string path_key = "/etc/certificates/keys/";
static const std::string path_ipsec_conf = "/etc/ipsec.conf";
static const std::string path_ipsec_secrets = "/etc/ipsec.secrets";
static const std::string path_openvpn_conf = "/etc/openvpn/openvpn.conf";

#ifndef DEBUGPC
static const std::string cmd_cp = "/usr/bin/cp";
#else
static const std::string cmd_cp = "/bin/cp";
#endif

static const std::string cmd_rm = "/bin/rm";
static const std::string cmd_mkd = "/bin/mkdir";

static const std::string cert_dir = "/etc/certificates/";
static const std::string keys_dir = "/etc/certificates/keys/";


static std::string get_access_rights(const std::string& type)
{
    if (type_cert == type)
        return "0644";
    else
        return "0600";
}


static std::string get_file_path(const std::string& type)
{
    if (type_cert == type)
        return path_cert;
    else if (type_key == type)
        return path_key;
    else if (type_ipsec_conf == type)
        return path_ipsec_conf;
    else if (type_ipsec_secrets == type)
        return path_ipsec_secrets;
    else if (type_openvpn_conf == type)
        return path_openvpn_conf;
    else if (type_source == type)
        return path_source;
    else if (type_out == type)
        return path_out;
    else
        throw invalid_param_error();
}


static std::string secure_file_path(const std::string& type, const std::string& fname)
{
    const std::string bpath = get_file_path(type);
    std::string fpath = bpath;

    if (type_source == type || type_out == type || type_cert == type || type_key == type)
    {
        if (0 == fname.length())
            throw missing_param_error();

        fpath += fname;
        fpath = get_real_path(fpath);

        if (fpath.length() < bpath.length())
            throw invalid_param_error();

        if (0 != fpath.compare(0, bpath.length(), bpath))
            throw invalid_param_error();
    }

    return fpath;
}


static void process_services(const std::vector<std::string>& args)
{
    assert(2 <= args.size());
    assert("ipsec" == args[1] || "ovpn" == args[1]);

    if (args.size() != 3)
        throw missing_param_error();

    const services::service s = "ipsec" == args[1] ? services::service::ipsec :
                                                     services::service::ovpn;
    const std::string& cmd = args[2];

    if ("--is-enabled" == cmd)
    {
        const bool state = services::is_enabled(s);
        std::cout << (state ? "enabled" : "disabled") << std::endl;
    }
    else if ("--enable" == cmd)
        services::enable(s);
    else if ("--disable" == cmd)
        services::disable(s);
    else if ("--is-running" == cmd)
    {
        const bool state = services::is_running(s);
        std::cout << (state ? "running" : "stopped") << std::endl;
    }
    else if ("--start" == cmd)
        services::start(s);
    else if ("--stop" == cmd)
        services::stop(s);
    else if ("--restart" == cmd)
        services::restart(s);
    else if ("--reload" == cmd)
        services::reload(s);
}


static void store_file(const std::string& dfile, const std::string& sfile)
{
    if (type_source == dfile)
        throw invalid_param_error();

    const std::string ar = get_access_rights(dfile);
    const std::string dfname = get_file_path(dfile);
    const std::string sfname = secure_file_path(type_source, sfile);

    (void)exe_cmd("/bin/chown root:root " + sfname);
    (void)exe_cmd("/bin/chmod " + ar + " " + sfname);

    const std::string cmdc = cmd_cp + " -p " + sfname + " " + dfname;
    (void)exe_cmd(cmdc);

    const std::string cmdr = cmd_rm + " " + sfname;
    (void)exe_cmd(cmdr);

    (void)exe_cmd("/bin/sync");
}


static void read_file(const std::string& dfile)
{
    if (type_ipsec_conf != dfile && type_openvpn_conf != dfile)
        throw invalid_param_error();

    const std::string fname = get_file_path(dfile);

    print_file(fname);
}


static void remove_file(const std::string& dfile, const std::string& sfile)
{
    // The rest of allowed combinations are checked by secure_file_path.
    // This one is here explicitly dissallowed.
    if (type_source == dfile)
        throw invalid_param_error();

    const std::string fname = secure_file_path(dfile, sfile);

    remove_file(fname, false);
}


static std::string get_user_name(const std::string& uname)
{
    struct passwd pwd;
    struct passwd *result;
    static const size_t buflen = 16384;
    char buf[buflen];

    const int ret = getpwnam_r(uname.c_str(), &pwd, buf, buflen, &result);

    if (0 != ret || nullptr == result || nullptr == pwd.pw_name)
        throw system_call_error();

    return std::string(pwd.pw_name);
}


static std::string get_group_name(const std::string& gname)
{
    struct group grp;
    struct group *result;
    static const size_t buflen = 16384;
    char buf[buflen];

    const int ret = getgrnam_r(gname.c_str(), &grp, buf, buflen, &result);

    if (0 != ret || nullptr == result || nullptr == grp.gr_name)
        throw system_call_error();

    return std::string(grp.gr_name);
}


static void copy_file(const std::string& sfile,
                      const std::string& uname,
                      const std::string& gname)
{
    if (type_ipsec_conf != sfile && type_openvpn_conf != sfile)
        throw invalid_param_error();

    const std::string sfname = get_file_path(sfile);
    const std::string dfname = path_out + sfile;

    struct stat st;
    const int exists = stat(sfname.c_str(), &st);

    if (0 != exists || S_IFREG != (S_IFMT & st.st_mode))
        throw file_open_error("File not exists");

    (void)exe_cmd(cmd_mkd + " -p " + path_out);
    const std::string cmdc = cmd_cp + " -p " + sfname + " " + path_out;
    (void)exe_cmd(cmdc);

    (void)exe_cmd("/bin/chmod 0600 " + dfname);
    const std::string cmdo = "/bin/chown " + get_user_name(uname) + ":" + get_group_name(gname) + " " + dfname;
    (void)exe_cmd(cmdo);
}


static void process_files(const std::vector<std::string>& args)
{
    assert(2 <= args.size());
    assert("file" == args[1]);

    if (args.size() < 4)
        throw missing_param_error();

    const std::string& cmd = args[2];
    const std::string& dfile = args[3];
    const std::string  sfile = 5 <= args.size() ? args[4] : "";
    const std::string  gname = 6 <= args.size() ? args[5] : "";

    if ("--store" == cmd)
        store_file(dfile, sfile);
    else if ("--read" == cmd)
        read_file(dfile);
    else if ("--remove" == cmd)
        remove_file(dfile, sfile);
    else if ("--copy" == cmd)
        copy_file(dfile, sfile, gname);
    else
        throw missing_param_error();
}


static std::vector<std::string> get_file_names(const std::string& dir_name)
{
    std::vector<std::string> fnames;

    DIR* dir = opendir(dir_name.c_str());

    if (nullptr == dir)
        return fnames;

    struct dirent* dent = nullptr;

    while (nullptr != (dent = readdir(dir)))
    {
        const std::string fname = dent->d_name;

        if (file_exists(dir_name + fname))
            fnames.push_back(fname);
    }

    closedir(dir);

    return fnames;
}


static void add_crt(Json::Value& root, const std::string dname, const std::string& fname)
{
    const x509 cert = parse_x509_file(dname + fname);

    if (cert.is_empty())
        return;

    std::string notBefore = get_notBefore(cert);
    std::string notAfter = get_notAfter(cert);
    std::vector<rdname> issuer_name = get_issuer_name(cert);
    std::vector<rdname> subject_name = get_subject_name(cert);

    auto& c = root["vpn"]["certs"][fname];

    c["notBefore"] = notBefore;
    c["notAfter"] = notAfter;

    for (auto name : issuer_name)
    {
        auto& n = c["issuerName"];
        std::string oin = name.get_oi_name();
        n[oin] = name.get_name();
    }

    for (auto name : subject_name)
    {
        auto& n = c["subjectName"];
        std::string oin = name.get_oi_name();
        n[oin] = name.get_name();
    }
}


static void add_crt_list(Json::Value& root)
{
    std::vector<std::string> fnames = get_file_names(cert_dir);

    for (auto fname : fnames)
        add_crt(root, cert_dir, fname);
}


static void add_key_list(Json::Value& root)
{
    std::vector<std::string> fnames = get_file_names(keys_dir);

    auto& k = root["vpn"]["keys"];

    for (int i = 0; (size_t)i < fnames.size(); ++i)
        k[i] = fnames.at(i);
}


static void process_vpn(const std::vector<std::string>& args)
{
    assert(2 <= args.size());
    assert("vpn" == args[1]);

    if (3 != args.size())
        throw missing_param_error();

    if ("--get-full-status" != args[2])
        throw invalid_param_error();

    Json::Value root;

    const bool istate = services::is_enabled(services::service::ipsec);
    root["vpn"]["ipsec"]["status"] = istate ? "enabled" : "disabled";
    const bool iruntime = services::is_running(services::service::ipsec);
    root["vpn"]["ipsec"]["runtime"] = iruntime ? "running" : "stopped";

    const bool ostate = services::is_enabled(services::service::ovpn);
    root["vpn"]["ovpn"]["status"] = ostate ? "enabled" : "disabled";
    const bool oruntime = services::is_running(services::service::ovpn);
    root["vpn"]["ovpn"]["runtime"] = oruntime ? "running" : "stopped";

    add_crt_list(root);
    add_key_list(root);

    std::cout << root << std::endl;
}


static void process_crt(const std::vector<std::string>& args)
{
    assert(2 <= args.size());
    assert("crt" == args[1]);

    if (3 != args.size())
        throw missing_param_error();

    if ("--get-list" != args[2])
        throw invalid_param_error();

    Json::Value root;

    add_crt_list(root);

    std::cout << root << std::endl;
}


static void process_key(const std::vector<std::string>& args)
{
    assert(2 <= args.size());
    assert("key" == args[1]);

    if (3 != args.size())
        throw missing_param_error();

    if ("--get-list" != args[2])
        throw invalid_param_error();

    Json::Value root;

    add_key_list(root);

    std::cout << root << std::endl;
}


void process_request(const std::vector<std::string>& args)
{
    if (args.size() < 2)
        throw missing_param_error();

    const std::string& part(args[1]);

    if ("ipsec" == part || "ovpn" == part)
        process_services(args);
    else if ("file" == part)
        process_files(args);
    else if ("vpn" == part)
        process_vpn(args);
    else if ("crt" == part)
        process_crt(args);
    else if ("key" == part)
        process_key(args);
    else
        throw missing_param_error();
}


} // namespace wago

