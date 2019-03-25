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
///  \file     set_dsa_mode.c
///
///  \version  $Revision: 22734 $1
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
#include "ct_error_handling.h"

struct options
{
    char *xmlConfigFile;
    char *dsaModeValue;
    bool showHelp;
};

static void ShowHelpText(const char *progName);

static int __ct_getopt(int argc, char **argv, struct options *opts)
{
    int c;

    int status = SUCCESS;

    opts->xmlConfigFile = NULL;
    opts->dsaModeValue = NULL;
    opts->showHelp = false;

    while(1)
    {
        int option_index = 0;

        static struct option long_options[] = 
        {
            {"help", no_argument, 0, 'h'},
            {"value", required_argument, 0, 'v'},

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
        case 'v':
            opts->dsaModeValue = g_uri_unescape_string(optarg, NULL); 
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

    if(NULL != opts->dsaModeValue)
    {
        g_free(opts->dsaModeValue);
    }
}

static void ShowHelpText(const char *progName)
{
    printf( "* Change the DSA mode *\n");
    printf("\n");
    printf( "Usage: %s\n", progName);
    printf( "\t\t-v/--value [value]  [0/1]: DSA mode 1: switch ports behave as different network interfaces.\n");
    printf( "                               DSA mode 0: switch ports are connected.\n");
    printf("\n");
    printf("[Default xml network configuration file: %s\n", NETWORK_INTERFACES_XML);
}

int main(int argc, char **argv)
{
    int status = SUCCESS;

    struct options opts;

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
            
            if(SUCCESS == status)
            {
                status = ct_libnet_start_session(opts.xmlConfigFile, &session);
            }

            if(SUCCESS == status)
            {
                if(NULL == opts.dsaModeValue)
                {
                    status = MISSING_PARAMETER;                   
                }
                else
                {
                    char currentMode[2];
                    currentMode[0] = '\0';
                    // Read current dsa mode from switch hw.
                    status = ct_libnet_get_actual_dsa_state(currentMode, sizeof(currentMode));

                    if(SUCCESS == status)
                    {
                        if(0 == strcmp(currentMode, opts.dsaModeValue))
                        {
                            // Switch hw is already set to the requested dsa mode.
                            currentMode[0] = '\0';

                            // Read dsa mode from configuration file to check if file is consistent with switch hw.
                            status = ct_libnet_get_dsa_state(currentMode, sizeof(currentMode), session);

                            if(SUCCESS == status)
                            {
                                if(0 == strcmp(currentMode, opts.dsaModeValue))
                                {
                                    // nothing to do
                                    goto cleanup;
                                }
                            }
                        }

                        if(SUCCESS == status)
                        {
                            // Update switch hw status and configuration file.
                            status = ct_libnet_set_dsa_state(opts.dsaModeValue, session);
                        }
                    }
                }
            }

cleanup:
            (void) ct_libnet_finish_session(session);
            ct_libnet_permanent_close();
        }
    }

    if(SUCCESS != status)
    {
        const char * lastErrMsg = ct_libnet_get_last_error();

        if(NULL == lastErrMsg)
        {
            lastErrMsg = "Internal error while changing DSA mode.";
        }

        ct_liblog_setLastError(lastErrMsg);
        ct_liblog_reportError(status, "Error in set_dsa_mode.");
    }

    __ct_cleanup_opts(&opts);

    return status;
}
