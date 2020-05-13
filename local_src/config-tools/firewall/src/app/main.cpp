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

#include <file_accessor.hpp>
#include "interface_mapping_provider.hpp"
#include "backup.hpp"
#include "error.hpp"
#include "process_ebtables.hpp"
#include "process_iptables.hpp"
#include "process_params.hpp"
#include "process_service.hpp"
#include "process_services.hpp"
#include "regex.hpp"
#include "system.hpp"
#include "xmlhlp.hpp"
#include "process.hpp"
#include "libxml/parser.h"
#include "rule_file_editor.hpp"
#include <syslog.h>
#include <unistd.h>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>


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

  const std::string  FW_IPR = "/sbin/iptables-restore";
  const std::string  FW_XST = "/usr/bin/xmlstarlet";
  const std::string  GENERAL_CONF = "/etc/firewall/firewall.conf";
  const std::string  FIREWALL_INIT = "/etc/init.d/firewall";

  const FileAccessor file_accessor;
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
  --get-xml                 returns chosen configuration in legacy xml form\n\
  --get-xml-ng              returns chosen configuration in wbn-ng xml form\n\
                            This command is preliminary. It may be changed or\n\
                            removed in future versions.\n\
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
  --add-filter STATE INTERFACE|- PROTOCOL SRC-IP|- SRC-MASK|- SRC-PORT|- DEST-IP|- DEST-MASK|- DEST-PORT|- POLICY\n\
                            adds a filtering rule. At least one of the optional\n\
                            parameters must be present. If SRC-PORT or DST-PORT\n\
                            is given PROTOCOL also must be set. *-MASK can be\n\
                            set only if corresponding *-IP is set as well\n\
  --upd-filter INDEX STATE INTERFACE|- PROTOCOL SRC-IP|- SRC-MASK|- SRC-PORT|- DEST-IP|- DEST-MASK|- DEST-PORT|-\n\
                            updates an existing filtering rule\n\
  --rem-filter INDEX        removes an existing filtering rule\n\
\n\
 CONF: services:\n\
  --get-ifs-status FORMAT   returns a summary of status of all services on\n\
                            all interfaces. FORMAT denotes requested output format\n\
                            and may take one of the two values: xml or json.\n\
  --get-ifs-status-ng FORMAT returns a summary of status of all services on\n\
                            all bridges and interfaces. FORMAT denotes requested output format\n\
                            and may take one of the two values: xml or json.\n\
                            This command is preliminary. It may be changed or\n\
                            removed in future versions.\n\
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
/// \return command to execute
//------------------------------------------------------------------------------
static std::string get_apply_cmd(const std::string& name)
{
    // 'are_apply_options_valid' must check the parameters first.
    std::string cmd = "";

    if ("ebtables" == name)
    {
        cmd = "sh /etc/firewall/ebtables/ebfirewall.sh";
    }
    else if ("iptables" == name)
    {
        cmd = "sh /etc/firewall/iptables/ipfirewall.sh --apply";
    }
    return cmd;
}

//----
//------------------------------------------------------------------------------
/// Processes transformation from xml doc file to iptables rules.
/// \param xmlshema name of XML shema definition file.
/// \param xmlfile  name of XML document file.
/// \param dest name of iptable rule to be changed
//------------------------------------------------------------------------------
static void transform_xmldoc (const std::string& xmlshema,
                              const std::string& xmlfile,
                              const std::string& dest)
{
    if (   file_accessor.check_file(xmlshema)
        && file_accessor.check_file(xmlfile)
        && file_accessor.check_file(FW_XST))
    {
        std::ostringstream oss;

        oss << FW_XST + " tr " << xmlshema  << " " << xmlfile  << " > " << dest;
        const std::string cmd(oss.str());
        int ret;
        (void)exe_cmd(cmd, ret);
        if (ret != 0)
        {
           throw  system_call_error ("call: " + cmd + " failed");
        }
    }
    else
    {
        throw file_open_error();
    }
}

//----
//------------------------------------------------------------------------------
/// Processes applying of iptables rule.
/// \param rule name of iptables rule to be applied
//------------------------------------------------------------------------------
static void apply_rule (const std::string& rule)
{
    if (file_accessor.check_file(rule) && file_accessor.check_file(FW_IPR))
    {
        std::ostringstream oss;
        const int RESOURCE_PROBLEM = 4;
        int ret = 0;

        // Remove duplicate rules
        rulefileeditor::RuleFileEditor rule_file_editor;
        rule_file_editor.remove_duplicate_lines(rule);

        oss << FW_IPR + " -n > /dev/null 2>&1 < " << rule;
        const std::string cmd(oss.str());

        // try to call iptables-restore, repeat by resource conflict
        auto start_time = std::chrono::steady_clock::now();
        while (1)
        {
            (void)exe_cmd(cmd, ret);

            if (ret != RESOURCE_PROBLEM)  break;
            auto current_time =  std::chrono::steady_clock::now();
            if ( timeout < std::chrono::duration_cast<std::chrono::milliseconds>(current_time-start_time).count() )
            {
                throw  system_call_error("Timeout by calling :" + cmd);
                break;
            }
        }
#ifdef SHOW_ERRORS
        syslog(LOG_ERR, "Execute command: %s  status: %i", oss.str().c_str(), ret);
#endif
    }
    else
    {
        throw file_open_error("apply_rule: error check file");
    }
}

//----
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
      ebtables::process(doc, cmd, args);
    else if ("iptables" == conf)
      iptables::process(doc, cmd, args);
    else
      service::process(doc, cmd, args);
}

//----
//------------------------------------------------------------------------------
/// Processes applying of configuration.
/// \param conf  configuration to be applied
/// \param updown (up|down) kind of configuration
//------------------------------------------------------------------------------
static void apply_conf(const std::string& conf, const std::string& updown)
{
    if (!are_apply_options_valid(conf, updown))
    {
        throw invalid_param_error("apply_conf");
    }
    update_network_interface_name_mapping(file_accessor);

    if (conf == "iptables" || conf == "ebtables")
    {
        const std::string apply(get_apply_cmd(conf));

        int ret;
        (void)exe_cmd(apply, ret);
    }
    else
    {
        // process services
        const std::string shema_down = "/etc/firewall/services/service_down.xsl";
        const std::string xmldoc_down = "/etc/firewall/services/" + conf + ".xml";
        const std::string rule_down = "/etc/firewall/services/" + conf + "_down.rls";
        transform_xmldoc (shema_down, xmldoc_down, rule_down);

        if (updown == "up")
        {
            const std::string shema = "/etc/firewall/services/service_up.xsl";
            const std::string xmldoc = "/etc/firewall/services/" + conf +".xml";
            const std::string up_rule = "/etc/firewall/services/"+ conf + "_up.rls";
            transform_xmldoc (shema, xmldoc, up_rule);

            apply_rule (rule_down);
            apply_rule (up_rule);
        }
        else if (updown == "down")
        {
            apply_rule (rule_down);
        }
        else
        {
            throw invalid_param_error( "apply_conf failed:", updown.c_str());
        }
    }
}

//----
//------------------------------------------------------------------------------
/// Gets configuration value from general configuration file.
/// \params  none
/// \return configurations value
//------------------------------------------------------------------------------
static std::string get_config_value()
{
    std::string value = "";
    std::ifstream config_file(GENERAL_CONF, std::ifstream::in);

    if (config_file)
    {
        std::string line = "";

        if (getline(config_file, line))
        {
            size_t pos = line.find("=");
            value = line.substr(pos + 1);
        }
        else throw file_read_error();

        if(config_file.is_open())
        {
            config_file.close();
        }
    }
    else
    {
        throw file_open_error();
    }
    return (value);
}

//----
//------------------------------------------------------------------------------
/// Set configuration value from general configuration file.
/// \params  configuration value
//------------------------------------------------------------------------------
static void set_config_value(const std::string& value)
{
    std::ofstream config_file (GENERAL_CONF);

    if (config_file)
    {
        config_file << "FIREWALL_GENERAL_STATE=" + value;
    }
    else
    {
        throw file_open_error();
    }
}

//----
//------------------------------------------------------------------------------
/// Check if firewall enabled on general configuration file.
/// \return true if firewall enabled, otherwise false
//------------------------------------------------------------------------------
static bool is_enabled (void)
{
    return (get_config_value() == "enabled");
}

//----
//------------------------------------------------------------------------------
/// Request, backup, restore and change firewall configuration.
/// \param  command (--is-enabled | --enable | --disable | --backup | --restore)
//------------------------------------------------------------------------------
static void firewall_change(const std::string& command)
{
    int ret;
    if (file_accessor.check_file(FIREWALL_INIT))
    {
        if ("--is-enabled" == command)
        {
            std::cout << get_config_value();
        }
        else if ("--enable" == command)
        {
            if (false == is_enabled())
            {
                update_network_interface_name_mapping(file_accessor);
                set_config_value("enabled");
                sync();
                (void)exe_cmd("sh "+ FIREWALL_INIT + " restart", ret);
            }
        }
        else if ("--disable" == command)
        {
            if (true == is_enabled())
            {
                update_network_interface_name_mapping(file_accessor);
                set_config_value("disabled");
                sync();
                (void)exe_cmd("sh "+ FIREWALL_INIT + " stop", ret);
            }
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
            throw invalid_param_error("firewall_change: ", command.c_str());
        }
    }
    else
    {
        throw file_open_error();
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
        throw missing_param_error("test_test");

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
        const std::string dir(file_accessor.get_config_fname("services"));
        std::vector<std::string> av;

        av.push_back(argv[3]);

        process_services(cmd, dir, av);
    }
    else
    {
        if (!regex::is_match(regex::rex_name, conf))
            throw invalid_param_error(conf.c_str());

        if (argc < 3)
            throw invalid_param_error("Too few arguments.");

        std::string cmd(argv[2]);

        if ("--get-xml" == cmd)
        {
            if (3 < argc)
                throw invalid_param_error("To many arguments.");

            file_accessor.print_file(file_accessor.get_config_fname(conf));
        }
        else if ("--get-xml-ng" == cmd)
        {
            if (3 < argc)
                throw invalid_param_error("To many arguments.");

            file_accessor.print_file_ng(file_accessor.get_config_fname(conf));
        }
        else if ("--apply" == cmd)
        {
            if (4 < argc)
                throw invalid_param_error("To many arguments.");

            std::string updown_cmd((3 < argc && NULL != argv[3]) ? argv[3] : "");
            apply_conf(conf, updown_cmd);
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
                    {
                      std::string str = "apply_options_valid:" + conf + "  " + updown;
                      throw invalid_param_error(str);
                    }

                    apply = true;
                    optc -= 2;
                }
            }

            xmldoc doc = file_accessor.read_configuration(conf, stdio);
            if (!doc.is_empty())
            {
                process_configuration(conf, cmd, doc, optc, optv);
                file_accessor.store_configuration(conf, stdio, doc);
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

//--------------------------------------------------------------------------
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
    std::string result_what;

    // TODO: Below in error catch blocks add logging "what" strings.

    try
    {
        wago::execute(argc, argv);
    }
    catch (const wago::execution_error& ex)
    {
        result = ex.code();
        result_msg = ex.msg();
        result_what = ex.what();

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
    {
        syslog(LOG_ERR, "log_error_message: %s %s", result_msg.c_str(), result_what.c_str());
        wago::log_error_message(result_msg);
    }

    wago::log_exit(result);

    return result;
}

