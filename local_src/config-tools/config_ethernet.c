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
///  \file     config_ethernet.c
///
///  \version  $Revision: 14749 $1
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

#include <glib.h>

#include "libnet/ct_libnet.h"
#include "liblog/ct_liblog.h"
#include "config_tool_lib.h"


struct options
{
    char *xmlConfigFile;
    char *port;
    char *state;
    char *autoneg;
    char *speed;
    char *duplex;
    bool showHelp;
    bool validParamNr;
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
  printf( "* Change the ethernet port settings *\n");
  printf("\n");
  printf( "Usage: %s\n", progName);
  printf( "\t\t-p port-value\n");
  printf( "\t\t[-S port state (enabled/disabled) ]\n");
  printf( "\t\t[-a autoneg-value ]\n");
  printf( "\t\t[-s speed-value ]\n");
  printf( "\t\t[-d duplex-value ]\n"); 
  printf( "\t\t[-c custom xml network config]\n");
  printf( "With autoneg=on, no speed or duplex values have to be provided.\n" );
  printf("\n");
  printf( "port-value: ");
  //TODO: port names!
  printf( "autoneg-value: on/off\n" );
  printf( "speed-value: 10M/100M\n" );
  printf( "duplex-value: full/half\n" );
  printf("\n");
  printf("[Default xml network configuration file: %s\n", NETWORK_INTERFACES_XML);
  
}

static int __fill_missing_params(struct options *opts)
{
    int status = SUCCESS;
    
   
    if(SUCCESS == status && NULL == opts->state)
    {
        opts->state = g_malloc(sizeof(char) * (strlen("disabled") + 1));
        opts->state[0] = '\0';
        status = ct_libnet_get_port_state_from_config(opts->port, 
                                                      opts->state, 
                                                      strlen("disabled") + 1, 
                                                      opts->libnetSession);
    }

    if(SUCCESS == status && NULL == opts->autoneg)
    {
        opts->autoneg = g_malloc(sizeof(char) * (strlen("disabled") + 1));
        opts->autoneg[0] = '\0';
        status = ct_libnet_get_autoneg_from_config(opts->port, 
                                                   opts->autoneg, 
                                                   strlen("disabled") + 1,
                                                   opts->libnetSession);
    }

    if(SUCCESS == status && NULL == opts->speed)
    {
        opts->speed = g_malloc(sizeof(char) * (strlen("10000M") + 1));
        opts->speed[0] = '\0';
        status = ct_libnet_get_speed_from_config(opts->port, 
                                                 opts->speed, 
                                                 strlen("10000M") + 1,
                                                 opts->libnetSession);
    }

    if(SUCCESS == status && NULL == opts->duplex)
    {
        opts->duplex = g_malloc(sizeof(char) * (strlen("full") + 1));
        opts->duplex[0] = '\0';
        status = ct_libnet_get_duplex_from_config(opts->port, 
                                                  opts->duplex, 
                                                  strlen("full") + 1,
                                                  opts->libnetSession);
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
    opts->autoneg = NULL;
    opts->speed = NULL;
    opts->duplex = NULL;
    opts->showHelp = false;
    opts->validParamNr = false;
    opts->libnetSession = NULL;

    while(1)
    {
        int option_index = 0;

        static struct option long_options[] = 
        {
            {"help", no_argument, 0, 'h'},
            {"port", required_argument, 0, 'p'},
            {"state", required_argument, 0, 'S'},
            {"autoneg", required_argument, 0, 'a'},
            {"speed", required_argument, 0, 's'},
            {"duplex", required_argument, 0, 'd'},

            // Array terminator
            {0, 0, 0, 0},
        };

        if(-1 == (c = getopt_long(argc, argv, "c:p:S:a:s:d:h", long_options, &option_index)))
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
        case 'S':
            opts->state = g_uri_unescape_string(optarg, NULL);
            opts->validParamNr = true;
            break;
        case 'a':
            opts->autoneg = g_uri_unescape_string(optarg, NULL);
            opts->validParamNr = true;
            break;
        case 's':
            opts->speed = g_uri_unescape_string(optarg, NULL);
            opts->validParamNr = true;
            break;
        case 'd':
            opts->duplex = g_uri_unescape_string(optarg, NULL);
            opts->validParamNr = true;
            break;
        case 'h':
            opts->showHelp = true;
            break;
        default:
            status = INVALID_PARAMETER;
            break;
        }
    }

    // do it before __fill_missing_params because we need a valid xml configuration
    // to fill the other missing parameters.
    if(NULL == opts->xmlConfigFile)
    {
        opts->xmlConfigFile = g_strdup(NETWORK_INTERFACES_XML);
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

    if(NULL != opts->autoneg)
    {
        g_free(opts->autoneg);
    }

    if(NULL != opts->speed)
    {
        g_free(opts->speed);
    }

    if(NULL != opts->duplex)
    {
        g_free(opts->duplex);
    }
}

/**
 * @brief    Change ethernet settings (port status, autoneg, speed, duplex).
 *          Each setting is first changed in HW. If the value could be changed
 *          successfully, it is updated in the XML config representation in
 *          memory.
 *
 * @param    opts: pointer to struct options with the string representations of
 *          the according values
 *
 * @return SUCCESS
 *         TODO: set_current_* errors
 *               set_configured_* errors
 */

//TODO: only do sth on state change? actually there are checks in frontends
int SetEthSettings(struct options *opts)
{
    assert(NULL != opts);
    assert(NULL != opts->state);
    assert(NULL != opts->autoneg);
    assert(NULL != opts->speed);
    assert(NULL != opts->duplex);
    assert(NULL != opts->libnetSession);

    bool settingFound = false;

    int status = SUCCESS;

//    if(SUCCESS == status)
//    {
//        status = ct_libnet_set_port_params_to_hw(opts->port,
//                                                  opts->autoneg,
//                                                  opts->speed,
//                                                  opts->duplex);
//    }
// 
       
    if(SUCCESS == status)
    {
        status = ct_libnet_set_autoneg_to_config(opts->port, opts->autoneg, opts->libnetSession);
    }


    if(SUCCESS == status)
    {
        status = ct_libnet_set_speed_to_config(opts->port, opts->speed, opts->libnetSession);
    }

    if(SUCCESS == status)
    {
        status = ct_libnet_set_duplex_to_config(opts->port, opts->duplex, opts->libnetSession);
    }

    // now, when port params have been set, change the port status
    
//    if(SUCCESS == status)
//    {
//        status = ct_libnet_set_port_state_to_hw(opts->port,
//                                                 opts->state);
//    }

    if(SUCCESS == status)
    {
        status = ct_libnet_set_port_state_to_config(opts->port, opts->state, opts->libnetSession);
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

            if(!opts.validParamNr || NULL == opts.port)
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
                status = SetEthSettings(&opts);
            }

            if(SUCCESS == status)
            {
                status = ct_libnet_save_xml_file(opts.libnetSession); 
            }

            (void) ct_libnet_finish_session(opts.libnetSession); 
            ct_libnet_permanent_close(); // cleans up global libxml2 data
        }
    }
 
    if(SUCCESS != status)
    {
        const char * lastErrMsg = ct_libnet_get_last_error();

        if(NULL == lastErrMsg)
        {
            lastErrMsg = "Internal error while setting ethernet parameters.";
        }

        ct_liblog_setLastError(lastErrMsg);
        ct_liblog_reportError(status, "Error in config_ethernet.");
    }
 
    __ct_cleanup_opts(&opts);

    return status;
}
