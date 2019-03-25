// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#include "error.hpp"
#include "logs.hpp"
#include "vpncfg.hpp"
#include <iostream>
#include <string>
#include <vector>


namespace wago {


//------------------------------------------------------------------------------
/// Prints help description.
//------------------------------------------------------------------------------
static void print_help(void)
{
    std::cout << 
"\
Usage: vpncfg -h|--help\n\
       vpncfg vpn CMD\n\
       vpncfg VPN CMD\n\
       vpncfg crt CMD\n\
       vpncfg file CMD [OPTIONS]\n\
\n\
 Basic commands:\n\
  -h, --help                prints this help\n\
\n\
 vpn - general commands:\n\
  vpncfg vpn CMD\n\
    --get-full-status       returns all status information using json format,\n\
                            including the list of certificates from --get-cert-command\n\
\n\
 VPN - OpenVPN/IPsec common commands:\n\
  vpncfg VPN CMD            executs command for a selected type of vpn framework.\n\
                            VPN can be set to either ipsec or ovpn and there are\n\
                            the following types of CMD commands:\n\
    --is-enabled            returns enabled|disabled if VPN is started on boot\n\
    --enable                enables VPN on boot\n\
    --disable               disables VPN on boot\n\
    --is-running            returns running|stopped status of the VPN\n\
    --start                 starts VPN service\n\
    --stop                  stops VPN service\n\
    --restart               restarts VPN service\n\
    --reload                orders VPN to reload configuration files\n\
\n\
 crt - certificate commands:\n\
  vpncfg crt CMD\n\
    --get-list              returns a list of certificates using json format\n\
\n\
 key - private keys commands:\n\
  vpncfg key CMD\n\
    --get-list              returns a list of private keys using json format\n\
\n\
 file - file commands:\n\
  vpncfg file CMD [OPTIONS]\n\
    --store FILE SRC_FILE\n\
                            copies content of a file designated by SRC_FILE to its\n\
                            correct name. If the file already exists it will be\n\
                            overwritten. FILE parameter names a type of a file\n\
                            which might be one of: cert, key, ipsec.conf, ipsec.secrets\n\
                            openvpn.conf. Source files shall be stored inside\n\
                            /tmp/vpncfg/ directory.\n\
    --read FILE             drops content of a file on std out. Allowed are the following\n\
                            FILE values: ipsec.conf, openvpn.conf.\n\
    --remove FILE [NAME]    removes a file completely. In case of the FILE argument\n\
                            allowed are all values from --store command, but in case\n\
                            of the cert and key files also NAME of the file must be given.\n\
    --copy FILE USER GROUP  copies content of a FILE to a /tmp/vpncfg-out/FILE location.\n\
                            FILE values: ipsec.conf, openvpn.conf.\n\
                            The copied file will be owned by USER:GROUP with access\n\
                            rights set to 0600.\n\
\n\
Example:\n\
  vpncfg ipsec --is-enabled\n\
  vpncfg file --store cert user_certificate.pem\n\
                            The full source file path is:\n\
                            /tmp/vpncfg/user_certificate.pem\n\
"
              << std::endl;
}


void execute(int argc, char** argv)
{
    if (argc < 2)
        throw missing_param_error();

    const std::string conf(argv[1]);

    if ("--help" == conf || "-h" == conf)
    {
        print_help();
        return;
    }

    std::vector<std::string> args;

    for (int i = 0; i < argc; ++i)
        args.push_back(nullptr != argv[i] ? argv[i] : "");

    process_request(args);
}


} // namespace wago


int main(int argc, char** argv)
{
    wago::openlog("vpncfg");
    wago::log_args(argc, argv);

    int result = 0;
    std::string result_msg;

    try
    {
        wago::execute(argc, argv);
    }
    catch (const wago::missing_param_error& ex)
    {
        result = ex.code();
        result_msg = ex.msg();
        #ifdef SHOW_ERRORS
        std::cout << "Excecution failed - missing/invalid parameter(s)." << std::endl
                  << "Exception: " << ex.what() << std::endl;
        #endif
    }
    catch (const wago::execution_error& ex)
    {
        result = ex.code();
        result_msg = ex.msg();
        #ifdef SHOW_ERRORS
        std::cout << "Excecution failed." << std::endl
                  << "Exception: " << ex.what() << std::endl;
        #endif
    }
    catch (const std::exception& ex)
    {
        result = SYSTEM_CALL_ERROR;
        result_msg = wago::error_msg_system_call_error;
        #ifdef SHOW_ERRORS
        std::cout << "Excecution failed." << std::endl
                  << "Exception: " << ex.what() << std::endl;
        #endif
    }
    catch (...)
    {
        result = SYSTEM_CALL_ERROR;
        result_msg = wago::error_msg_unknown;
        #ifdef SHOW_ERRORS
        std::cout << "Excecution failed." << std::endl
                  << "Unrecognized type of exception has been thrown." << std::endl;
        #endif
    }

    if (0 != result && 0 != result_msg.size())
        wago::log_error_message(result_msg);

    wago::log_exit(result);

    return result;

}

