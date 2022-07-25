//------------------------------------------------------------------------------
/// Copyright (c) 2000 - 2022 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     get_eth_config.c
///
///  \version  $Revision: 65689 $1
///
///  \brief    
///
///  \author   StM, AGa : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>

#include "libnet/ct_libnet.h"
#include "ct_error_handling.h"

#include "config_tool_lib.h"

#include <glib.h>


//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------
struct options
{
    bool showHelp;
    char *switchName;
    char *param;
};


//static int __ct_getXpathTemplate(const char *param, const char **template);
static int __ct_getopt(int argc, char **argv, struct options *opts);
static int PrintParameter(struct options *opts);

// array of all possible requested parameters
static const char*  inputParams[] =
{
    "bcast-protect",
    "rate-limit",
    "port-mirror",
    "fast-aging",
    // this line must always be the last one - don't remove it!
    NULL,
};


static int PrintParameter(struct options *opts)
{
    int status = INVALID_PARAMETER;

    if(0 == strcmp(opts->param, "bcast-protect"))
    {
        char bcastProtect[strlen("20") + 1];
        bcastProtect[0] = '\0';
        status = ct_libnet_get_bcast_protect(opts->switchName, bcastProtect, sizeof(bcastProtect));

        if(SUCCESS == status)
        {
            printf("%s", bcastProtect);
        }
    }
    else if(0 == strcmp(opts->param, "rate-limit"))
    {
        char rateLimit[strlen("960.kbps") + 1];
        rateLimit[0] = '\0';
        status = ct_libnet_get_rate_limit(opts->switchName, rateLimit, sizeof(rateLimit));

        if(SUCCESS == status)
        {
            printf("%s", rateLimit);
        }
    }        
    else if(0 == strcmp(opts->param, "port-mirror"))
    {
        char portMirror[2];
        portMirror[0] = '\0';
        status = ct_libnet_get_port_mirror(opts->switchName, portMirror, sizeof(portMirror));

        if(SUCCESS == status)
        {
            printf("%s", portMirror);
        }
    }
    else if(0 == strcmp(opts->param, "fast-aging"))
    {
        char fastAging[2];
        fastAging[0] = '\0';
        status = ct_libnet_get_fast_aging(opts->switchName, fastAging, sizeof(fastAging));

        if(SUCCESS == status)
        {
            printf("%s", fastAging);
        }
    }

    return status;
}

static void ShowHelpText(const char *progName)
//
// Show description and usage of program on stdout
//
{
  int parameterIndex = 0;

  printf("\n* Get advanced switch configuration parameters *\n\n");
  printf("Usage: <parameter> [-s switch name]\n\n", progName);

  // read the possible strings for parameter from array and show them like this: "param1 | param2 | param3"
  while(NULL != inputParams[parameterIndex])
  {
    if(parameterIndex != 0) printf("| ");
    printf("%s ", inputParams[parameterIndex]);
    ++parameterIndex;
  }

  printf("\n");
  printf("[Default switch name: %s\n", SWCONFIG_SWITCH_NAME);
  
}

static int __ct_getopt(int argc, char **argv, struct options *opts)
{
    int status = SUCCESS;

    int c;

    while(1)
    {
        int option_index = 0;

        static struct option long_options[] = 
        {
            {"help", no_argument, 0, 'h'},
            {"switch-name", required_argument, 0, 's'},

            // Array terminator
            {0, 0, 0, 0},

        };

        if(-1 == (c = getopt_long(argc, argv, "s:h", long_options, &option_index)))
        {
            break;
        }

        switch(c)
        {
        case 's':
            opts->switchName = strdup(optarg);
            break;
        case 'h':
            ShowHelpText(argv[0]);
            opts->showHelp = true;
            break;
        default:
            status = INVALID_PARAMETER;
            break;
        }
    }

    if(NULL == opts->switchName)
    {
       opts->switchName = strdup(SWCONFIG_SWITCH_NAME);
    } 

    if(SUCCESS == status)
    {
        if(optind < argc )
        {
            opts->param = strdup(argv[optind]);
            status = SUCCESS;
        }
        else
        {
            status = INVALID_PARAMETER;
        }
    }

    return status;
}

void __ct_cleanup_opt(struct options *opts)
{
    if(NULL != opts->switchName)
    {
        free(opts->switchName);
    }

    if(NULL != opts->param)
    {
        free(opts->param);
    }
}

//
// NOTE: the 'unorthodox' parameter parsing method is used here to
// avoid breaking the old config-tools command-line calling convention
// which had been used in bash get_eth_config.
//
int main(int argc, char **argv)
{
    int status = SUCCESS;

    struct options opts;
    memset(&opts, 0, sizeof(struct options));

    if(SUCCESS == (status =__ct_getopt(argc, argv, &opts)))
    {
        status = PrintParameter(&opts);
    }
    else
    {
        ShowHelpText(argv[0]);
    }

    __ct_cleanup_opt(&opts);

    return status;
}

