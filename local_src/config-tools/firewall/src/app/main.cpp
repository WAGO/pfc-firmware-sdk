//------------------------------------------------------------------------------ 
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG 
// 
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All 
// manufacturing, reproduction, use, and sales rights pertaining to this 
// subject matter are governed by the license agreement. The recipient of this 
// software implicitly accepts the terms of the license. 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file main.cpp
/// 
/// \brief Main module of firewall configuration tool.
/// 
/// \author Mariusz Podlesny : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------


#include "backup.hpp"
#include "error.hpp"
#include "file_access.hpp"
#include "process_ebtables.hpp"
#include "process_iptables.hpp"
#include "process_params.hpp"
#include "process_service.hpp"
#include "process_services.hpp"
#include "regex.hpp"
#include "system.hpp"
#include "xmlhlp.hpp"

#include <libxml/parser.h>
#include <syslog.h>
#include <unistd.h>
#include <cassert>
#include <iostream>
#include <sstream>

//
//  BEFORE YOU START
//
//  The 'main' function should contain mainly C initialization/deinitialization
//  procedures. All C++ code, except for exception handling, should be executed
//  in the 'execute' function. Rationale: this should help to avoid omitting
//  C code deinitialization and spilling exceptions onto console in a situation
//  when an exception is thrown. This will also make sure that all C++ objects
//  which rely on C frameworks get deinitialized before these frameworks are.
//
//  Entire code in the file should be agnostic in regard to specific
//  configuration file. Whole code dedicated to a given configuration file
//  should be contained in process*.cpp files.
//


namespace wago
{


//------------------------------------------------------------------------------
/// Prints help description.
//------------------------------------------------------------------------------
static void print_help(void)
{
    std::cout << 
"\
Usage: firewall -h|--help\n\
       firewall CONF --get-xml\n\
       firewall CONF --apply [up|down]\n\
       firewall CONF [--stdio] CMD [OPTIONS] [--apply [up|down]]\n\
\n\
  -h, --help                prints this help\n\
\n\
 Configuration type (CONF) determines allowed commands (CMD) and their options\n\
 (OPTIONS). The following configurations are allowed:\n\
\n\
  firewall                  general firewall settings\n\
  ebtables                  link layer, ethernet settings\n\
  iptables                  network layer, IPv4 settings\n\
  services                  configuration files of all services\n\
  SERVICE                   a standard service settings, e.g. ftp, ssh, http, etc.\n\
\n\
 Each option which is marked with |- sequence at its end, e.g. LIMIT|-, is optional\n\
 and instead of real value '-' character can be set in its place. This means that\n\
 the option should not be used, and it will get erased from the configuration.\n\
 Please note that whenever BURST option is used it is only allowed if its LIMIT\n\
 counterpart is also present, i.e. is not set to '-' value.\n\
 For exact definitions of allowed values please refer //fiewall/patterns.xsd file.\n\
 In some case the name of an option directly translates into a name of a rule\n\
 with the following exceptions:\n\
 - TOTAL     -> conn_count\n\
 - TCP/UDP   -> limit (for limits)\n\
 - STATE     -> onoff\n\
 - INTERFACE -> ifname\n\
 - MAC       -> ifmac\n\
 - MASK      -> ifmac_mask (for mac addresses)\n\
 - *-IP      -> ip4\n\
 - *-PORT    -> port\n\
\n\
 Common command:\n\
  --get-xml                 returns chosen configuration in xml form\n\
  --apply [up|down]         applies current configuration. Please note that\n\
                            setting values doesn't mean their immediate application.\n\
                            Several changes can be done to the configuration before\n\
                            whole set is applied.\n\
                            Options 'up' and 'down' may be used only for services,\n\
                            where 'up' means application of service related rules\n\
                            and 'down' their removal.\n\
\n\
 Common option:\n\
  --stdio                   toggles configuration source to the standard input\n\
                            and prints results on the standard ouput. Doesn't\n\
                            work with --get-xml and --apply commands. By default\n\
                            configuration is taken from a standard, predefined\n\
                            location and in place edited\n\
\n\
 CONF: firewall:\n\
  --is-enabled              returns 'enabled' or 'disabled' depending on the state\n\
                            of the entire firewall.\n\
  --enable                  enables firewall.\n\
  --disable                 disables entire firewall.\n\
  --backup                  prints on stdio configuration of the entire firewall\n\
                            in format compatible with the standard backup tool.\n\
  --restore                 restores configuration of the entire firewall from\n\
                            the stdio.\n\
\n\
 CONF: ebtables:\n\
  --set-mode MODE           set mode of work: all-allow|whitelist\n\
\n\
  --set-log STATE TAG LIMIT|- BURST|- LEVEL\n\
                            set logging\n\
\n\
  --set-if STATE INTERFACE  change interface state, fully open or filtered.\n\
                            If there is no existing entry for an interface it is\n\
                            by default considered closed. Setting a state for\n\
                            an interface previously not on the list adds it to\n\
                            the list\n\
  --rem-if INTERFACE        remove an interface from the list altogether. This\n\
                            will have the same effect as setting its state to\n\
                            'filtered'.\n\
\n\
  --toggle-eproto STATE     turn on/off filtering of protocols\n\
\n\
  --add-eproto EPROTO       add protocol to the allowed list\n\
  --rem-eproto EPROTO       remove protocols from the allowed list\n\
\n\
  --add-host STATE MAC MASK|-\n\
                            add a new whitelist entry\n\
  --upd-host INDEX STATE MAC MASK|-\n\
                            update an existing whitelist entry\n\
  --rem-host INDEX          remove an existing whitelist entry\n\
\n\
 CONF: iptables:\n\
  --set-climits TOTAL|- LIMIT|- BURST|- TCP|- UDP|-\n\
                            sets limitations on incoming connections\n\
\n\
  --set-echo POLICY LIMIT|- BURST|- BROADCAST_PROTECTION\n\
                            sets global ping policy and limitations, for all\n\
                            interfaces. BROADCAST_PROTECTION is an on/off\n\
                            parameter, active indeptendently of global policy.\n\
                            It triggers global echo broadcat protection\n\
                            implemented the Linux kernel\n\
\n\
  --set-echo-if POLICY INTERFACE LIMIT|- BURST|-\n\
                            sets a policy and limitation for ping requests for\n\
                            a given interface. Please note that these policies\n\
                            are active only if default policy for all interfaces\n\
                            is set to drop\n\
  --rem-echo-if INTERFACE   removes an existing policy and limitation\n\
\n\
  --set-forward STATE       toggles global forwarding, between all interfaces\n\
  --set-forward-link STATE INTERFACE INTERFACE\n\
                            add/update forwarding rule for two interfaces. This\n\
                            rule will have an effect only if global forwarding\n\
                            is disabled (see --set-forward option)\n\
  --rem-forward-link INTERFACE INTERFACE\n\
                            remove forwarding rule\n\
\n\
  --set-masq INTERFACE      applies masquerading to a given interface\n\
  --rem-masq INTERFACE      removes an existing masquerading setting\n\
  --rem-masq all            removes all existing masquerading settings\n\
\n\
  --add-pfw STATE INTERFACE|- PROTOCOL DEST-IP|- DEST-PORT|- FW-IP|- FW-PORT|-\n\
                            adds a port forwarding rule. For each of\n\
                            the following pairs one of values must be present:\n\
                            - DST-IP or DST-PORT\n\
                            - FW-IP or FW-PORT\n\
  --upd-pfw INDEX STATE INTERFACE|- PROTOCOL DEST-IP|- DEST-PORT|- FW-IP|- FW-PORT|-\n\
                            updates an existing port forwarding rule\n\
  --rem-pfw INDEX           removes an existing port forwarding rule\n\
  --rem-pfw all             removes all existing port forwarding rules\n\
\n\
  --set-open-if STATE INTERFACE\n\
                            add/update a fully openned interface entry\n\
  --rem-open-if INTERFACE   remove a fully openned interface entry\n\
\n\
  --add-filter STATE INTERFACE|- PROTOCOL SRC-IP|- SRC-MASK|- SRC-PORT|- DEST-IP|- DEST-MASK|- DEST-PORT|-\n\
                            adds a filtering rule. At least one of the optional\n\
                            parameters must be present. If SRC-PORT or DST-PORT\n\
                            is given PROTOCOL also must be set. *-MASK can be\n\
                            set only if corresponding *-IP is set as well\n\
  --upd-filter INDEX STATE INTERFACE|- PROTOCOL SRC-IP|- SRC-MASK|- SRC-PORT|- DEST-IP|- DEST-MASK|- DEST-PORT|-\n\
                            updates an existing filtering rule\n\
  --rem-filter INDEX        removes an existing filtering rule\n\
\n\
 CONF: services:\n\
  --get-ifs-status FORMAT   returns a summary of statuses of all services on\n\
                            all interfaces. FORMAT denotes requested output format\n\
                            and may take one of the two values: xml or json.\n\
\n\
 CONF: SERVICE:\n\
  --set-if STATE INTERFACE  enables/disables application of service filtering\n\
                            rules to a given interface. If an interface is not\n\
                            on the list of enabled/disabled interfaces rules are\n\
                            not applied to it.\n\
  --rem-if INTERFACE        removes interface entry\n\
\n\
  --add-rule STATE PROTO SRC-PORT|- DST-PORT\n\
                            adds a new rule\n\
  --upd-rule INDEX STATE PROTO SRC-PORT|- DST-PORT\n\
                            updates an existing rule\n\
  --rem-rule INDEX          removes an existing rule\n\
"
              << std::endl;
}

//------------------------------------------------------------------------------
/// Checks validity of --apply parameters.
///
/// \param name name of type of configuration, e.g. ebtables, iptables, wbm, etc.
/// \param updown (up|down) or empty value
/// \return true if parameters are valid
//------------------------------------------------------------------------------
static bool are_apply_options_valid(const std::string& name, const std::string& updown)
{
    if ("ebtables" == name || "iptables" == name)
    {
        return 0 == updown.size();
    }
    else
    {
        return ("up" == updown || "down" == updown) && regex::is_match(regex::rex_name, name);
    }
}

//------------------------------------------------------------------------------
/// Returns a command which applies firewall rules.
/// \param name name of type of configuration, e.g. ebtables, iptables, wbm, etc.
/// \param updown 'up' or 'down' for services, empty for eb-/ip-tables
/// \return command to execute
//------------------------------------------------------------------------------
static std::string get_apply_cmd(const std::string& name, const std::string& updown)
{
    // 'are_apply_options_valid' must check the parameters first.

    if ("ebtables" == name)
    {
        assert(0 == updown.size());
        return "sh /etc/firewall/ebtables/ebfirewall.sh";
    }
    else if ("iptables" == name)
    {
        assert(0 == updown.size());
        return "sh /etc/firewall/iptables/ipfirewall.sh --apply";
    }
    else
    {
        assert("up" == updown || "down" == updown);

        std::ostringstream oss;

        oss << "sh /etc/firewall/services/service.sh ";
        oss << updown << " " << name;

        return oss.str();
    }
}

//------------------------------------------------------------------------------
/// Processes configuration change request.
/// \param conf name of type of configuration, e.g. ebtables, iptables, wbm, etc.
/// \param cmd command type, depends on type of configuration
/// \param doc xml document to be changed
/// \param argc number of parameters (as in main function)
/// \param argv parameters of processed command
//------------------------------------------------------------------------------
static void process_configuration(const std::string& conf,
                                  const std::string& cmd,
                                  xmldoc& doc,
                                  int argc, char** argv)
{
    std::vector<std::string> args;

    for (int i = 0; i < argc; ++i)
        args.push_back(nullptr != argv[i] ? argv[i] : std::string());

    if ("ebtables" == conf)
        process_ebtables(doc, cmd, args);
    else if ("iptables" == conf)
        process_iptables(doc, cmd, args);
    else
        process_service(doc, cmd, args);
}

static void apply_conf(const std::string& conf, const std::string& updown)
{
    if (!are_apply_options_valid(conf, updown))
        throw invalid_param_error();

    update_network_interface_name_mapping();

    // TODO Result control should be applied.
    const std::string apply(get_apply_cmd(conf, updown));
    (void)exe_cmd(apply);
}

static void firewall_change(const std::string& command)
{
    if ("--is-enabled" == command)
    {
        const std::string result(exe_cmd("sh /etc/firewall/firewall_ed.sh --is-enabled"));
        std::cout << result;
    }
    else if ("--enable" == command)
    {
        update_network_interface_name_mapping();
        (void)exe_cmd("sh /etc/firewall/firewall_ed.sh --enable");
    }
    else if ("--disable" == command)
    {
        update_network_interface_name_mapping();
        (void)exe_cmd("sh /etc/firewall/firewall_ed.sh --disable");
    }
    else if ("--backup" == command)
    {
        perform_backup();
    }
    else if ("--restore" == command)
    {
        perform_restore();
    }
    else
    {
        throw invalid_param_error();
    }
}

//------------------------------------------------------------------------------
/// Executes proper action based on program's parameters.
/// \param argc number of parameters
/// \param argv parameters of main function call
//------------------------------------------------------------------------------
static void execute(int argc, char** argv)
{
    // TODO This is a one really ugly function - refactore it!

    if (argc < 2)
        throw missing_param_error();

    const std::string conf(argv[1]);

    if ("--help" == conf || "-h" == conf)
    {
        print_help();
        return;
    }
    else if ("firewall" == conf)
    {
        if (argc < 3)
            throw invalid_param_error("Too few arguments.");

        const std::string cmd(argv[2]);

        firewall_change(cmd);
    }
    else if ("services" == conf)
    {
        if (4 != argc)
            throw invalid_param_error("Invalid number of arguments.");

        const std::string cmd(argv[2]);
        const std::string dir(get_config_fname("services"));
        std::vector<std::string> av;

        av.push_back(argv[3]);

        process_services(cmd, dir, av);
    }
    else
    {
        if (!regex::is_match(regex::rex_name, conf))
            throw invalid_param_error();

        if (argc < 3)
            throw invalid_param_error("Too few arguments.");

        std::string cmd(argv[2]);

        if ("--get-xml" == cmd)
        {
            if (3 < argc)
                throw invalid_param_error("To many arguments.");

            print_file(get_config_fname(conf));
        }
        else if ("--apply" == cmd)
        {
            if (4 < argc)
                throw invalid_param_error("To many arguments.");

            apply_conf(conf, 3 < argc && NULL != argv[3] ? argv[3] : "");
        }
        else
        {
            bool stdio = false;

            if ("--stdio" == cmd)
            {
                if (argc < 4)
                    throw invalid_param_error("Too few arguments.");

                cmd = argv[3];
                stdio = true;
            }

            int optc = argc - (stdio ? 4 : 3);
            char** const optv = argv + (stdio ? 4 : 3);
            bool apply = false;
            std::string updown;

            if (1 < optc)
            {
                if (std::string("--apply") == optv[optc - 1])
                {
                    apply = true;
                    --optc;
                }
                else if (2 < optc && std::string("--apply") == optv[optc - 2])
                {
                    updown = optv[optc - 1];

                    if (!are_apply_options_valid(conf, updown))
                        throw invalid_param_error();

                    apply = true;
                    optc -= 2;
                }
            }

            xmldoc doc = read_configuration(conf, stdio);
            if (!doc.is_empty())
            {
                process_configuration(conf, cmd, doc, optc, optv);
                store_configuration(conf, stdio, doc);
            }

            if (apply)
            {
                apply_conf(conf, updown);
            }
        }
    }
}


//------------------------------------------------------------------------------
/// Log all parameters to syslog.
/// \param argc number of parameters
/// \param argv parameters of main function call
//------------------------------------------------------------------------------
static void log_args(int argc, char** argv)
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


//------------------------------------------------------------------------------
/// Log program exit value.
/// \param result program exit value.
//------------------------------------------------------------------------------
static void log_exit(int const result)
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


//------------------------------------------------------------------------------
/// Open syslog.
//------------------------------------------------------------------------------
static void openlog(void)
{
    static char prefix[30];

    snprintf(prefix, 30, "firewall[%i]", getpid());
    ::openlog(prefix, LOG_ODELAY, LOG_USER);
}


} // namespace wago


int main(int argc, char** argv)
{
    wago::openlog();
    wago::log_args(argc, argv);

    LIBXML_TEST_VERSION

    int result = 0;
    std::string result_msg;

    // TODO: Below in error catch blocks add logging "what" strings.

    try
    {
        wago::execute(argc, argv);
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

    xmlCleanupParser();
    xmlMemoryDump();

    if (0 != result && 0 != result_msg.size())
        wago::log_error_message(result_msg);

    wago::log_exit(result);

    return result;
}

