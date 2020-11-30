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
///  \file     config_switch.c
///
///  \version  $Revision: 50494 $1
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

#define SWITCH_SETTINGS_CONF "/etc/switch_settings.conf"


struct options
{
    char *switchName;
    char *bcastProtect;
    char *rateLimit;
    char *portMirror;
    char *fastAging;
    bool showHelp;
    bool paramFound;
};

static void ShowHelpText(const char *progName);
static int __ct_getopt(int argc, char **argv, struct options *opts);
static void __ct_cleanup_opts(struct options *opts);

static void ShowHelpText(const char *progName)
//
// Show description and usage of program on stdout
//
{
    printf( "* Change advanced ethernet switch settings *\n");
    printf("\n");
    printf( "Usage: %s\n", progName);
    printf( "\t\t-b|--bcast-protect broadcast protection\n");
    printf( "\t\t-r|--rate-limit    ingress rate limit\n");
    printf( "\t\t-p|--port-mirror   port mirror\n");
    printf( "\t\t-f|--fast-aging    fast aging\n");
    printf( "\n");
    printf( "\t\t-s|--switch-name   switch device name (as in swconfig dev xxx)\n");
    printf( "\t\t   [default switch device name if omitted: %s]\n", SWCONFIG_SWITCH_NAME);
    printf("=====================================================\n");
    printf("Valid arguments for broadcast protection: ");
    ct_libnet_print_bcast_protect_args();
    printf("\n");
    printf("Valid arguments for rate limit: ");
    ct_libnet_print_rate_limit_args();
    printf("\n");
    printf("Valid arguments for port mirror: ");
    ct_libnet_print_port_mirror_args();
    printf("\n");
    printf("Valid arguments for fast aging: ");
    ct_libnet_print_fast_aging_args();
    printf("\n");

}

static int __ct_get_missing_opts(struct options *opts)
{
    int status = SUCCESS;

    if(NULL == opts->switchName)
    {
        return INVALID_PARAMETER;
    }

    if(SUCCESS == status && NULL == opts->portMirror)
    {
        char portMirror[2];
        portMirror[0] = '\0';
        status = ct_libnet_get_port_mirror(opts->switchName, portMirror, sizeof(portMirror));

        if(SUCCESS == status)
        {
            opts->portMirror = g_strdup(portMirror);
        }
    } 

    if(SUCCESS == status && NULL == opts->bcastProtect)
    {
        char bcastProtect[strlen("20") + 1];
        bcastProtect[0] = '\0';
        status = ct_libnet_get_bcast_protect(opts->switchName, bcastProtect, sizeof(bcastProtect));
    
        if(SUCCESS == status)
        {
            opts->bcastProtect = g_strdup(bcastProtect);
        }
    } 

    if(SUCCESS == status && NULL == opts->rateLimit)
    {
        // strlen("disabled") == strlen("XXX.kbps")
        char rateLimit[strlen("960.kbps") + 1];
        rateLimit[0] = '\0';
        status = ct_libnet_get_rate_limit(opts->switchName, rateLimit, sizeof(rateLimit));

        if(SUCCESS == status)
        {
            //workaround: set-rate-limit: off == get-rate-limit: disabled
            if(0 == strcmp("disabled", rateLimit))
            {
                opts->rateLimit = g_strdup("off");
            }
            else
            {
                opts->rateLimit = g_strdup(rateLimit);
            }
        }
    }
    
    if(SUCCESS == status && NULL == opts->fastAging)
    {
        char fastAging[2];
        fastAging[0] = '\0';
        status = ct_libnet_get_fast_aging(opts->switchName, fastAging, sizeof(fastAging));

        if(SUCCESS == status)
        {
            opts->fastAging = g_strdup(fastAging);
        }
    }   

    return status;
}

static int __ct_getopt(int argc, char **argv, struct options *opts)
{
    int c;

    int status = SUCCESS;

    opts->switchName = NULL;

    opts->portMirror = NULL;
    opts->bcastProtect = NULL;
    opts->fastAging = NULL;
    opts->rateLimit = NULL;
    opts->showHelp = false;

    opts->paramFound = false;

    while(1)
    {
        int option_index = 0;

        static struct option long_options[] = 
        {
            {"help", no_argument, 0, 'h'},
            {"port-mirror", required_argument, 0, 'p'},
            {"bcast-protect", required_argument, 0, 'b'},
            {"rate-limit", required_argument, 0, 'r'},
            {"fast-aging", required_argument, 0, 'f'},
            {"switch-name", required_argument, 0, 's'},


            // Array terminator
            {0, 0, 0, 0},
        };

        if(-1 == (c = getopt_long(argc, argv, "p:b:r:f:s:h", long_options, &option_index)))
        {
            break;
        }

        switch(c)
        {
        case 'c':
            opts->switchName = g_uri_unescape_string(optarg, NULL);
            opts->paramFound = true;
            break;
        case 'p':
            opts->portMirror = g_uri_unescape_string(optarg, NULL); 
            opts->paramFound = true;
            break;
        case 'b':
            opts->bcastProtect = g_uri_unescape_string(optarg, NULL);
            opts->paramFound = true;
            break;
        case 'r':
            opts->rateLimit = g_uri_unescape_string(optarg, NULL);

            //workaround: set-rate-limit: off == get-rate-limit: disabled
            if(0 == strcmp("disabled", opts->rateLimit))
            {
                g_free(opts->rateLimit);
                opts->rateLimit = g_strdup("off");
            }

            opts->paramFound = true;
            break;
        case 'f':
            opts->fastAging = g_uri_unescape_string(optarg, NULL);
            opts->paramFound = true;
            break;
        case 'h':
            opts->showHelp = true;
            break;
        default:
            status = INVALID_PARAMETER;
            break;
        }
    }


    if(NULL == opts->switchName)
    {
        opts->switchName = g_strdup(SWCONFIG_SWITCH_NAME);
    }

    return status;
}

static void __ct_cleanup_opts(struct options *opts)
{

    if(NULL != opts->switchName)
    {
        g_free(opts->switchName);
    }

    if(NULL != opts->portMirror)
    {
        g_free(opts->portMirror);
    }

    if(NULL != opts->bcastProtect)
    {
        g_free(opts->bcastProtect);
    }

    if(NULL != opts->rateLimit)
    {
        g_free(opts->rateLimit);
    }

    if(NULL != opts->fastAging)
    {
        g_free(opts->fastAging);
    }
}

int SetEthSettings(struct options *opts)
{
    assert(NULL != opts);

    int status = SUCCESS;

    if(SUCCESS == status && (NULL != opts->bcastProtect) )
    {
        status = ct_libnet_set_bcast_protect(opts->switchName, opts->bcastProtect);
    }

    if(SUCCESS == status && (NULL != opts->portMirror) )
    {
        status = ct_libnet_set_port_mirror(opts->switchName, opts->portMirror);
    }

    if(SUCCESS == status && (NULL != opts->rateLimit) )
    {
        status = ct_libnet_set_rate_limit(opts->switchName, opts->rateLimit);
    }

    if(SUCCESS == status && (NULL != opts->fastAging) )
    {
        status = ct_libnet_set_fast_aging(opts->switchName, opts->fastAging);
    }

    return status;
}

int main(int argc, char **argv)
{
    int status = SUCCESS;

    struct options opts;

    (void) memset(&opts, 0, sizeof(opts));

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
        {   // main work starts here
            if(opts.paramFound)
            {
                status = SetEthSettings(&opts);
            }
            else
            {
                status = MISSING_PARAMETER;
            }

        
            if(SUCCESS == status)
            {
                status = __ct_get_missing_opts(&opts);
            }

            if(SUCCESS == status)
            {
                ct_switch_settings_t settings;

                settings.bcastProtect = opts.bcastProtect;
                settings.portMirror = opts.portMirror;
                settings.fastAging = opts.fastAging;
                settings.rateLimit = opts.rateLimit;

                status = ct_libnet_save_switch_settings_to_config(SWITCH_SETTINGS_CONF, &settings);
            }
        }
    }
    else
    {
        ShowHelpText(argv[0]);
    }

    if(SUCCESS != status)
    {
        const char * lastErrMsg = ct_libnet_get_last_error();

        if(NULL == lastErrMsg)
        {
            lastErrMsg = "Internal error while setting ethernet switch parameters.";
        }

        ct_liblog_setLastError(lastErrMsg);
    }
 
    __ct_cleanup_opts(&opts);

    return status;
}
