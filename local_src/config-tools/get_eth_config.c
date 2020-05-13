//------------------------------------------------------------------------------
/// Copyright (c) 2000 - 2014 WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     get_eth_config.c
///
///  \version  $Revision: 44697 $1
///
///  \brief    
///
///  \author   StM, AGa : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>

#include "libnet/ct_netlink.h"
#include "libnet/ct_libnet.h"
#include "libnet/../ct_error_handling.h"

#include "config_tool_lib.h"

//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------
struct options
{
    char *xmlConfigFile;
    bool showHelp;
    bool printPorts;
    bool printAllPorts;
    bool printXml;
};


//static int __ct_getXpathTemplate(const char *param, const char **template);
static int __ct_getopt(int argc, char **argv, struct options *opts);
static int PrintParameter(const char *pPortString, char *pParamString, libnetSession_t *session);

// array of all possible requested parameter (input-strings, strings to search for, processing-function to get them)
static const char*  inputParams[] =
{
    "ip-address",
    "subnet-mask",
    "broadcast",
    "config-type",
    "state",
    //  "device-id",
    "speed",
    "duplex",
    "autoneg",
    "speed-duplex",

//The following options returned HTML code to WBM and are now obsolete
//    "speed-selection",
//    "duplex-selection",
//    "autoneg-selection",
//    "speed-duplex-selection",

    // this line must always be the last one - don't remove it!
    NULL,
};


// TODO: is a hash table worth the initialization overhead here?
static int PrintParameter(const char *pPortString, char *pParamString, libnetSession_t *session)
{
    int status = INVALID_PARAMETER;

    if(0 == strcmp(pParamString, "state"))
    {
        char state[strlen("disabled") + 1];
        state[0] = '\0';
        status = ct_libnet_get_eth_port_state_from_hw(pPortString, state, sizeof(state), session);

        if(SUCCESS == status)
        {
            printf("%s", state);
        }
    }
    else if(0 == strcmp(pParamString, "ip-address"))
    {
        char ip[strlen("255.255.255.255/32") + 1];
        ip[0] = '\0';
        status = ct_libnet_get_ip_addr_from_config(pPortString, ip, sizeof(ip), session);

        if(SUCCESS == status)
        {
            printf("%s", ip);
        }
    }        
    else if(0 == strcmp(pParamString, "subnet-mask"))
    {
        char netmask[strlen("255.255.255.255") + 1];
        netmask[0] = '\0';
        status = ct_libnet_get_netmask_from_config(pPortString, netmask, sizeof(netmask), NETMASK_FORMAT_MODE_CLASSIC, session);

        if(SUCCESS == status)
        {
            printf("%s", netmask);
        }
    }
    else if(0 == strcmp(pParamString, "broadcast"))
    {
        char broadcast[strlen("255.255.255.255") + 1];
        broadcast[0] = '\0';
        status = ct_libnet_get_broadcast_from_config(pPortString, broadcast, sizeof(broadcast), session);

        if(SUCCESS == status)
        {
            printf("%s", broadcast);
        }
    } 
    else if(0 == strcmp(pParamString, "config-type"))
    {
        char configType[strlen("static") + 1];
        configType[0] = '\0';
        status = ct_libnet_get_config_type(pPortString, configType, sizeof(configType), session);

        if(SUCCESS == status)
        {
            printf("%s", configType);
        }
    }
    else if(0 == strcmp(pParamString, "autoneg"))
    {
        char autoneg[strlen("disabled") + 1];
        autoneg[0] = '\0';
        status = ct_libnet_get_autoneg_from_hw(pPortString, autoneg, sizeof(autoneg), session);

        if(SUCCESS == status)
        {
            printf("%s", autoneg);
        }

    }
    else if(0 == strcmp(pParamString, "speed"))
    {
        char speed[strlen("10000M") + 1];
        speed[0] = '\0';
        status = ct_libnet_get_speed_from_hw(pPortString, speed, sizeof(speed), session);

        if(SUCCESS == status)
        {
            printf("%s", speed);
        }
    }
    else if(0 == strcmp(pParamString, "duplex"))
    {
        char duplex[strlen("full") + 1];
        duplex[0] = '\0';
        status = ct_libnet_get_duplex_from_hw(pPortString, duplex, sizeof(duplex), session);

        if(SUCCESS == status)
        {
            printf("%s", duplex);
        }
    }
    else if(0 == strcmp(pParamString, "speed-duplex"))
    {
        char speed[strlen("10000M") + 1];
        char duplex[strlen("full") + 1];
        speed[0] = '\0';
        duplex[0] = '\0';
       
        status = ct_libnet_get_speed_from_hw(pPortString, speed, sizeof(speed), session);

        if(SUCCESS == status)
        {
            status = ct_libnet_get_duplex_from_hw(pPortString, duplex, sizeof(duplex), session);
        }

        if(SUCCESS == status)
        {
            printf("%s/%s", speed, duplex);
        }
    }

    return status;
}

static int PrintPorts(struct options *opts)
{
    int status = SUCCESS;

    char portsList[32];
    portsList[0] = '\0';
    libnetSession_t *session = NULL;

    status = ct_libnet_start_session(opts->xmlConfigFile, &session);


    if(SUCCESS == status)
    {
        status = ct_libnet_get_ports_list(portsList, sizeof(portsList), " ", session);
    }

    if(SUCCESS == status)
    {
        printf("%s", portsList);
    }

    (void) ct_libnet_finish_session(session);
    ct_libnet_permanent_close();

    return status;
}

static int PrintAllPorts(struct options *opts)
{
    int status = SUCCESS;

    char portsList[32];
    portsList[0] = '\0';
    libnetSession_t *session = NULL;

    status = ct_libnet_start_session(opts->xmlConfigFile, &session);

    if(SUCCESS == status)
    {
        status = ct_libnet_get_all_ports_list(portsList, sizeof(portsList), " ", session);
    }

    if(SUCCESS == status)
    {
        printf("%s", portsList);
    }

    (void) ct_libnet_finish_session(session);
    ct_libnet_permanent_close();

    return status;
}



static void ShowHelpText(const char *progName)
//
// Show description and usage of program on stdout
//
{
  int parameterIndex = 0;

  printf("\n* Get the interface-parameters configured by file interfaces *\n\n");
  printf("Usage: %s <interface> <parameter> [-c custom xml network config]\n\n", progName);
  printf("Interface: X1 | X2\n");
  printf("Parameter: ");

  // read the possible strings for parameter from array and show them like this: "param1 | param2 | param3"
  while(NULL != inputParams[parameterIndex])
  {
    if(parameterIndex != 0) printf("| ");
    printf("%s ", inputParams[parameterIndex]);
    ++parameterIndex;
  }

  printf("\n");
  printf("%s --print-ports: print active network port names\n", progName);
  printf("%s --print-all-ports: print all network port names\n", progName);
  printf("%s --xml: print complete XML configuration\n", progName);
  printf("\n");
  printf("[Default xml network configuration file: %s\n", NETWORK_INTERFACES_XML);
  
}

static void PrintNetworkInterfacesXml(void)
//
// Show network interfaces xml file on stdout
//
{
  char  completeSystemCallString[MAX_LENGTH_SYSTEM_CALL]  = "";

  snprintf(completeSystemCallString, MAX_LENGTH_SYSTEM_CALL, "cat %s", NETWORK_INTERFACES_XML);
  system(completeSystemCallString);

  return;
}

static int GetConfiguredEthSettings(int argc, char **argv, struct options *opts)
{
    assert(2 == argc);

    char *port = argv[0];
    char *param = argv[1];

    int status = SUCCESS;

    libnetSession_t *session = NULL;

    status = ct_libnet_start_session(opts->xmlConfigFile, &session);

    if(SUCCESS == status)
    {
        status = PrintParameter(port, param, session);
    }

    // FIXME: don't ignore return value:
    // currently nothing user-relevant can fail here, but this may change
    // in future
    (void) ct_libnet_finish_session(session);
    ct_libnet_permanent_close();

    return status;
}

static int __ct_getopt(int argc, char **argv, struct options *opts)
{
    int nrOptsFound = 0;
    
    int c;

    opts->xmlConfigFile = strdup(NETWORK_INTERFACES_XML);

    while(1)
    {
        int option_index = 0;

        static struct option long_options[] = 
        {
            {"help", no_argument, 0, 'h'},
            {"print-ports", no_argument, 0, 'p'},
            {"print-all-ports", no_argument, 0, 'P'},
            {"config", required_argument, 0, 'c'},
            {"xml", no_argument, 0, 'x'},

            // Array terminator
            {0, 0, 0, 0},

        };

        if(-1 == (c = getopt_long(argc, argv, "pPc:h", long_options, &option_index)))
        {
            break;
        }

        switch(c)
        {
        case 'c':
            nrOptsFound += 2;
            opts->xmlConfigFile = strdup(optarg);
            break;
        case 'h':
            ++nrOptsFound;
            ShowHelpText(argv[0]);
            opts->showHelp = true;
            break;
        case 'p':
            ++nrOptsFound;
            opts->printPorts = true;
            break;
        case 'P':
            ++nrOptsFound;
            opts->printAllPorts = true;
            break;
        case 'x':
            ++nrOptsFound;
            opts->printXml = true;
            opts->printPorts = false;
            opts->printAllPorts = false;
            PrintNetworkInterfacesXml();
            break;
        default:
            break;
        }
    }

    return nrOptsFound;
}

//
// NOTE: the 'unorthodox' parameter parsing method is used here to
// avoid breaking the old config-tools command-line calling convention
// which had been used in bash get_eth_config.
//
int main(int argc, char **argv)
{
    int status = SUCCESS;

    static struct options opts;

    int nrArgsInterpreted = __ct_getopt(argc, argv, &opts);

    // BUGME: rework this switch-case as it is nonsense
    switch(argc - nrArgsInterpreted)
    {
    case 1:

        if(!opts.showHelp && !opts.printPorts && !opts.printAllPorts && !opts.printXml)
        {
            status = MISSING_PARAMETER;
        }

        if(opts.printPorts)
        {
            status = PrintPorts(&opts);
        }

        if(opts.printAllPorts)
        {
            status = PrintAllPorts(&opts);
        }
        break;

    case 2:
        status = MISSING_PARAMETER;
        break;

    case 3:
        //TODO: accept list of parameters (X1 ip-address netmask mac-address ...)
        // number of command line arguments - nr of getopt args  - program name
        status = GetConfiguredEthSettings(argc - nrArgsInterpreted - 1, &argv[optind], &opts);
        break;

    default:
        status = INVALID_PARAMETER;
        break;
    }

    // cleanup
    free(opts.xmlConfigFile);

    return status;
}

