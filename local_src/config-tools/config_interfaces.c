//------------------------------------------------------------------------------
/// Copyright (c) 2014 WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     config_interfaces.c
///
///  \version  $Revision: 13503 $1
///
///  \brief    
///
///  \author   AGa : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>

#include <net/if.h>

#include <glib.h>

#include "libnet/ct_libnet.h"
#include "liblog/ct_liblog.h"
#include "config_tool_lib.h"

struct options
{
    char *xmlConfigFile;
    char *port;
    char *state; // TODO: needed? already implemented in config_ethernet!

    char *configType; // static/dhcp/bootp

    // for config-type static only:
    char *ipAddr;
    char *netmask;
    char *broadcast;

    bool showHelp;
    
    libnetSession_t *libnetSession;
};

static void ShowHelpText(const char *progName);
static int __ct_getopt(int argc, char **argv, struct options *opts);
static void __ct_cleanup_opts(struct options *opts);
static int __fill_missing_params(struct options *opts);

static void ShowHelpText(const char *progName)
//
// Show description and usage of program on stdout
//
{
    printf("* Change the configuration of the interfaces *\n");
    printf("\n");
    printf("Usage %s\n", progName);
    printf( "\t-p/--port\t\t<port-value>\n"); //TODO: get ports?
    printf( "\t-s/--state\t\t<state-value>\t\t[enabled/disabled]\n");
    printf( "\t-t/--config-type\t<config-type>\t\t[static/dhcp/bootp]\n");
    printf( "\t-i/--ip\t\t\t<ip address>\t\t[for config-type \'static\']\n");
    printf( "\t-m/--subnet-mask\t<subnet mask>\t\t[for config-type \'static\']\n");
    printf( "\t-b/--broadcast\t\t<broadcast>\t\t[for config-type \'static\']\n");
    printf( "\t-I/--template-ip\t\t\t<ip address>\t\t[for ip address via dip switch]\n");
    printf( "\t-M/--template-subnet-mask\t<subnet mask>\t\t[for ip address via dip switch]\n");

    printf("\n");
    printf("\t-c/--xml-config\t<alternative xml configuration file>\n");
    printf("[Default xml network configuration file: %s\n", NETWORK_INTERFACES_XML);
}

static int __fill_missing_params(struct options *opts)
{
    int status = SUCCESS;
    int ipLen = strlen("255.255.255.255") + 1;

    // a netmask value is needed for broadcast calculation and has to be retrieved if only ip or only netmask is provided
    if( (SUCCESS == status) &&
        ((NULL != opts->ipAddr && NULL == opts->netmask)) )
    {
        opts->netmask = g_malloc(sizeof(char) * ipLen);
        opts->netmask[0] = '\0';

        status = ct_libnet_get_netmask_from_config(opts->port, 
                                                   opts->netmask, ipLen, NETMASK_FORMAT_MODE_CLASSIC, opts->libnetSession);
    }

    if( (SUCCESS == status) &&
        ((NULL == opts->ipAddr && NULL != opts->netmask)) )
    {
        opts->ipAddr = g_malloc(sizeof(char) * ipLen);
        opts->ipAddr[0] = '\0';

        status = ct_libnet_get_ip_addr_from_config(opts->port, 
                                                   opts->ipAddr, ipLen, opts->libnetSession);
    }


    // broadcast address has to be recalculated if either the ip or the netmask has changed
    if( (SUCCESS == status) &&
        ((NULL != opts->ipAddr) || (NULL != opts->netmask)) )
    {
        opts->broadcast = g_malloc(sizeof(char) * ipLen);
        opts->broadcast[0] = '\0';

        status = ct_libnet_calculate_broadcast(opts->ipAddr, opts->netmask, opts->broadcast, ipLen);

    }

    return status;
}

static int __ct_getopt(int argc, char **argv, struct options *opts)
{
    int c;

    int status = SUCCESS;

    opts->xmlConfigFile = NULL;
    opts->port = NULL;
    opts->state = NULL;
    opts->configType = NULL;
    opts->ipAddr = NULL;
    opts->netmask = NULL;
    opts->broadcast = NULL;
    opts->showHelp = false;

    bool settingFound = false;

    while(1)
    {
        int option_index = 0;

        static struct option long_options[] = 
        {
            {"xml-config", required_argument, 0, 'c'},
            
            {"help", no_argument, 0, 'h'},
            {"port", required_argument, 0, 'p'},
            {"state", required_argument, 0, 's'},

            {"config-type", required_argument, 0, 't'},

            {"ip", required_argument, 0, 'i'},
            {"subnet-mask", required_argument, 0, 'm'},
           
            {"broadcast", required_argument, 0, 'b'},

            // Array terminator
            {0, 0, 0, 0},

        };

        if(-1 == (c = getopt_long(argc, argv, "c:p:s:t:i:m:b:g:h", long_options, &option_index)))
        {
            break;
        }

        switch(c)
        {
        case 'c':
            opts->xmlConfigFile = g_strdup(optarg);
            break;
        case 'p':
            opts->port = g_uri_unescape_string(optarg, NULL); 
            break;
        case 's':
            opts->state = g_uri_unescape_string(optarg, NULL);
            settingFound = true;
            break;
        case 't':
            opts->configType = g_uri_unescape_string(optarg, NULL);
            settingFound = true;
            break;
        case 'i':
            opts->ipAddr = g_uri_unescape_string(optarg, NULL);
            settingFound = true;
            break;
        case 'm':
            opts->netmask = g_uri_unescape_string(optarg, NULL);
            settingFound = true;
            break;
        case 'b':
            opts->broadcast = g_uri_unescape_string(optarg, NULL);
            settingFound = true;
            break;
        case 'h':
            opts->showHelp = true;
            settingFound = true;
            break;

        default:
            status = INVALID_PARAMETER;
            break;
        }
    }

    // do it before __fill_missing_params because we need a valid xml configuration
    // in order to fill the other missing parameters (TODO: move xml initialization
    // to libnet).
    if(NULL == opts->xmlConfigFile)
    {
        opts->xmlConfigFile = g_strdup(NETWORK_INTERFACES_XML);
    }

    if(!settingFound)
    {
        status = MISSING_PARAMETER;
    }

    return status;
}

static void __ct_cleanup_opts(struct options *opts)
{
    if(NULL != opts->xmlConfigFile)
    {
        g_free(opts->xmlConfigFile);
    }

    if(NULL != opts->port)
    {
        g_free(opts->port);
    }

    if(NULL != opts->state)
    {
        g_free(opts->state);
    }

    if(NULL != opts->configType)
    {
        g_free(opts->configType);
    }

    if(NULL != opts->ipAddr)
    {
        g_free(opts->ipAddr);
    }

    if(NULL != opts->netmask)
    {
        g_free(opts->netmask);
    }

    if(NULL != opts->broadcast)
    {
        g_free(opts->broadcast);
    }

}

/**
 * @brief    Change network interface settings (ip address, netmask, link status...)
 *          First, all settings are written to a temporary XML config file, then
 *          we create a temporary /etc/network/interfaces and try to set them in HW
 *          using it. If everything worked, we rename the temporary files.
 *
 * @param    opts: pointer to struct options with the string representations of
 *          the according values
 *
 * @return SUCCESS
 *         TODO: set_current_* errors
 *               set_configured_* errors
 */

//TODO: only do sth on state change? actually there are checks in frontends
int SetIfaceSettings(struct options *opts)
{
    assert(NULL != opts);

    int status = SUCCESS;

    if(opts->configType)
    {
        status = ct_libnet_set_config_type(opts->port, opts->configType, opts->libnetSession);
    }

    if(SUCCESS == status && opts->state)
    {
        status = ct_libnet_set_port_state_to_config(opts->port, opts->state, opts->libnetSession);
    }

    if(SUCCESS == status && opts->ipAddr)
    {
        status = ct_libnet_set_ip_addr_to_config(opts->port, opts->ipAddr, opts->libnetSession);
    }

    if(SUCCESS == status && opts->netmask)
    {
        status = ct_libnet_set_netmask_to_config(opts->port, opts->netmask, opts->libnetSession);
    }

    if(SUCCESS == status && opts->broadcast)
    {
        status = ct_libnet_set_broadcast_to_config(opts->port, opts->broadcast, opts->libnetSession);
    }

    return status;
}

int main(int argc, char **argv)
{
    int status = SUCCESS;

    struct options opts;

    if(SUCCESS == status)
    {
       status = __ct_getopt(argc, argv, &opts);
    }

    if(SUCCESS == status)
    {
        if(opts.showHelp)
        {
            ShowHelpText(argv[0]);
        }
        else
        {

            if(NULL == opts.port)
            {
                status = MISSING_PARAMETER;
            }
            
            // Actual work starts here

            if(SUCCESS == status)
            {
                status = ct_libnet_start_session(opts.xmlConfigFile, &(opts.libnetSession));
            }

            if(SUCCESS == status)
            {
                status = __fill_missing_params(&opts); // needs xml engine to be initialized
            }

            if(SUCCESS == status)
            {
                status = SetIfaceSettings(&opts);
            }

            if(SUCCESS == status)
            {
            	status = ct_libnet_save_xml_file(opts.libnetSession);
            }

            (void) ct_libnet_finish_session(opts.libnetSession);
            ct_libnet_permanent_close();
        }
    }

    if(SUCCESS != status)
    {
        const char * lastErrMsg = ct_libnet_get_last_error();

        if(NULL == lastErrMsg)
        {
            lastErrMsg = "Internal error while setting network parameters.";
        }

        ct_liblog_setLastError(lastErrMsg);
        ct_liblog_reportError(status, "Error in config_interfaces.");
    }

    __ct_cleanup_opts(&opts);
    
    return status;
}
