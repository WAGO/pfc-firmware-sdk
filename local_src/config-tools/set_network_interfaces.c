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
///  \file     set_network_interfaces.c
///
///  \version  $Revision: 11087 $1
///
///  \brief    
///
///  \author   KNü : WAGO Kontakttechnik GmbH & Co. KG
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

#define NETWORK_INTERFACES_OUT  "/etc/network/interfaces"

struct options
{
    char *xmlConfigFile;
    char *outputFile;
    bool showHelp;
};

static void ShowHelpText(const char *progName);
static int __ct_getopt(int argc, char **argv, struct options *opts);
static void __ct_cleanup_opts(struct options *opts);

static void ShowHelpText(const char *progName)
//
// Show description and usage of program on stdout
//
{
    printf("* Generate network interface configuration *\n");
    printf("\n");
    printf("Usage %s\n", progName);
    printf( "\t-o/--output-file\t<alternative output file>\n");
    printf("[Default output file: %s\n", NETWORK_INTERFACES_OUT);
    printf("\n");
    printf("\t-c/--xml-config\t<alternative xml configuration file>\n");
    printf("[Default xml network configuration file: %s\n", NETWORK_INTERFACES_XML);
}

static int __ct_getopt(int argc, char **argv, struct options *opts)
{
    int status = SUCCESS;

    opts->xmlConfigFile = NULL;
    opts->outputFile = NULL;
    opts->showHelp = false;
    while(1)
    {
        int c;
        int option_index = 0;
        static struct option long_options[] = 
        {
            {"xml-config", required_argument, 0, 'c'},
            {"output-file", required_argument, 0, 'o'},
            {"help", no_argument, 0, 'h'},
            // Array terminator
            {0, 0, 0, 0},
        };

        if(-1 == (c = getopt_long(argc, argv, "c:o:h", long_options, &option_index)))
        {
            break;
        }

        switch(c)
        {
        case 'c':
            opts->xmlConfigFile = g_strdup(optarg);
            break;
        case 'o':
            opts->outputFile = g_strdup(optarg);
            break;
        case 'h':
            opts->showHelp = true;
            break;
        default:
            status = INVALID_PARAMETER;
            break;
        }
    }

    if(NULL == opts->xmlConfigFile)
    {
        opts->xmlConfigFile = g_strdup(NETWORK_INTERFACES_XML);
    }

    if(NULL == opts->outputFile)
    {
        opts->outputFile = g_strdup(NETWORK_INTERFACES_OUT);
    }

    return status;
}

static void __ct_cleanup_opts(struct options *opts)
{
    if(NULL != opts->xmlConfigFile)
    {
        g_free(opts->xmlConfigFile);
    }

    if(NULL != opts->outputFile)
    {
        g_free(opts->outputFile);
    }
}

int main(int argc, char **argv)
{
    struct options opts;
    int status = __ct_getopt(argc, argv, &opts);
    if(SUCCESS == status)
    {
        if(opts.showHelp)
        {
            ShowHelpText(argv[0]);
        }
        else
        {
            libnetSession_t *libnetSession = NULL;
            status = ct_libnet_start_session(opts.xmlConfigFile, &libnetSession);
            if(SUCCESS == status)
            {
                status = ct_generate_network_config(opts.outputFile, libnetSession);
            }
            (void) ct_libnet_finish_session(libnetSession);
            ct_libnet_permanent_close();
        }
    }

    if(SUCCESS != status)
    {
        const char * lastErrMsg = ct_libnet_get_last_error();
        if(NULL == lastErrMsg)
        {
            lastErrMsg = "Internal error while setting network interfaces.";
        }
        ct_liblog_setLastError(lastErrMsg);
        ct_liblog_reportError(status, "Error in set_network_interfaces.");
    }
    __ct_cleanup_opts(&opts);

    return status;
}
