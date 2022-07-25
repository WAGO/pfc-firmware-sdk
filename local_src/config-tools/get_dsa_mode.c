//------------------------------------------------------------------------------
/// Copyright (c) 2014-2022 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     get_dsa_mode.c
///
///  \version  $Revision: 65689 $1
///
///  \brief    
///
///  \author   AGa : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>

#include <glib.h>

#include "libnet/ct_libnet.h"
#include "ct_error_handling.h"

struct options
{
    char *xmlConfigFile;
    bool showHelp;
};

static void ShowHelpText(const char *progName)
{
    printf( "* Read current DSA mode from configuration file. *\n");
    printf("\n");
    printf( "Usage: %s\n", progName);
    printf( "\t\t 1: switch ports behave as different network interfaces.\n");
    printf( "     0: switch ports are connected.\n");
    printf("\n");
    printf("[Default xml network configuration file: %s\n", NETWORK_INTERFACES_XML);
}

static int __ct_getopt(int argc, char **argv, struct options *opts)
{
    int c;

    int status = SUCCESS;

    opts->xmlConfigFile = NULL;
    opts->showHelp = false;

    while(1)
    {
        int option_index = 0;

        static struct option long_options[] = 
        {
            {"help", no_argument, 0, 'h'},

            // Array terminator
            {0, 0, 0, 0},
        };

        if(-1 == (c = getopt_long(argc, argv, "hv:c:", long_options, &option_index)))
        {
            break;
        }

        switch(c)
        {
        case 'c':
            opts->xmlConfigFile = g_strdup(optarg);
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

    return status;

}

static void __ct_cleanup_opts(struct options *opts)
{
    if(NULL != opts->xmlConfigFile)
    {
        g_free(opts->xmlConfigFile);
    }
}

int main(int argc, char **argv)
{
    int status = SUCCESS;

    struct options opts;

    char value[2];
    *value = '\0';

    libnetSession_t *session = NULL;

    status = __ct_getopt(argc, argv, &opts);

    if(SUCCESS == status)
    {
        if(opts.showHelp)
        {
            ShowHelpText(argv[0]);
        }
        else
        {
            status = ct_libnet_start_session(opts.xmlConfigFile, &session);

            if(SUCCESS == status)
            {
                status = ct_libnet_get_dsa_state(value, sizeof(value), session);

                (void) ct_libnet_finish_session(session);
                ct_libnet_permanent_close();
            }

            if(SUCCESS == status)
            {
                printf("%s", value);
            }
        }
    }

    __ct_cleanup_opts(&opts);
    
    return status;
}
