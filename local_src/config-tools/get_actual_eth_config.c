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
///  \file     get_actual_eth_config.c
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
#include <fcntl.h>

// includes for netlink routines to read the network routing table
#include <sys/types.h>
#include <strings.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <bits/sockaddr.h>
#include <linux/rtnetlink.h>
#include <glib.h>

// config tool specific includes
#include "libnet/ct_libnet.h"
#include "ct_error_handling.h"
#include "config_tool_lib.h"


//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------

static int PrintParameter(char *pPortString, char *pParamString, libnetSession_t *session);

// Pointer to funktion to search the requested parameter
typedef int (*tFktPrintParameter)(char* pPortString,
                                  char* pParamString,
                                  libnetSession_t *session);

// struct to join the possible input-strings with the processing-function to get them
typedef struct
{
  // input-string from command-line
  char               *inputString;

  // processing function to get the requested parameter
  tFktPrintParameter   pFktPrintParameter;

} tParameterJumptab;


// array of all possible requested parameter (input-strings, strings to search for, processing-function to get them)
static tParameterJumptab astParameterJumptab[] =
{
  { "config-type",              PrintParameter },
  { "ip-address",               PrintParameter },
  { "mac-address",              PrintParameter },
  { "subnet-mask",              PrintParameter },
  { "default-gateway",          PrintParameter },
  { "default-gateway-nofail",   PrintParameter },
  { "cable-state",              PrintParameter },
  { "state",                    PrintParameter }, // duplicate of "get_eth_config state", needed for compatibility reasons

  // this line must always be the last one - don't remove it!
  { "", NULL }
};

struct options
{
    char *xmlConfigFile;
    bool showHelp;
};

//------------------------------------------------------------------------------
// netlink routines to read system routing table
//------------------------------------------------------------------------------

// structure for netlink routing table access
struct nlink_if
{
    int sock;
    struct sockaddr_nl laddr;
    char *recbuf;
    int recbuflen;
    int datalen;
};

// data of a netlink request message
struct request
{
    struct nlmsghdr nl;
    struct rtmsg rt;
};

// forward function declaration
static void nlroute_close(struct nlink_if *nlp);

// initialise routing table access via netlink, open and bind socket
static struct nlink_if *nlroute_init()
{
    struct nlink_if *nlp = g_malloc0(sizeof (struct nlink_if));

    nlp->recbuflen = 4096;
    nlp->recbuf = g_malloc(nlp->recbuflen);
    if ((nlp->sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) < 0)
    {
        nlroute_close(nlp);
        return NULL;
    }
    nlp->laddr.nl_family = AF_NETLINK;
    nlp->laddr.nl_pid = getpid();
    if (bind(nlp->sock, (struct sockaddr *)&nlp->laddr, sizeof(nlp->laddr)) < 0)
    {
        nlroute_close(nlp);
        return NULL;
    }
    return nlp;
}

// close netlink communication
static void nlroute_close(struct nlink_if *nlp)
{
    if (nlp->sock >= 0)
    {
        close(nlp->sock);
    }
    g_free(nlp->recbuf);
    g_free(nlp);
}

// send netlink request to read routing table
static int nlroute_send_request(struct nlink_if *nlp)
{
    struct msghdr msg;
    struct iovec iov;
    struct sockaddr_nl dstaddr;
    struct request *rqp = g_malloc0(sizeof (struct request));
    int status = SUCCESS;

    // prepare netlink request message
    rqp->nl.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    rqp->nl.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    rqp->nl.nlmsg_type = RTM_GETROUTE;

    rqp->rt.rtm_family = AF_INET;
    rqp->rt.rtm_table = RT_TABLE_MAIN;

    // initialize sendmsg() data structures for a one element message
    memset(&dstaddr, 0, sizeof (dstaddr));
    dstaddr.nl_family = AF_NETLINK;
    iov.iov_base = (void *)rqp;
    iov.iov_len = rqp->nl.nlmsg_len;

    memset(&msg, 0, sizeof(msg));
    msg.msg_name = (void *)&dstaddr;
    msg.msg_namelen = sizeof (dstaddr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // send the rtnetlink message to kernel
    if (sendmsg(nlp->sock, &msg, 0) < 0)
    {
        status = SYSTEM_CALL_ERROR;
    }
    g_free(rqp);
    return status;
}

// read routing table from kernel
static int nlroute_get_response(struct nlink_if *nlp)
{
    char *p;
    int   status = SUCCESS;

    // read from the socket until the NLMSG_DONE is returned
    p = nlp->recbuf;
    nlp->datalen = 0;
    for (;;)
    {
        int recsize = recv(nlp->sock, p, nlp->recbuflen - nlp->datalen, 0);
        if (recsize >= 0)
        {
            if (((struct nlmsghdr *)p)->nlmsg_type == NLMSG_DONE)
            {
                break;
            }
            struct rtmsg *rtmp = (struct rtmsg *)NLMSG_DATA((struct nlmsghdr *)p);
            if (rtmp->rtm_table == RT_TABLE_MAIN)
            {
                p += recsize;
                nlp->datalen += recsize;
            }
        }
        else
        {
            status = SYSTEM_CALL_ERROR;
            break;
        }
    }
    return status;
}

static int nlroute_table_read(struct nlink_if *nlp)
{
    int status = SUCCESS;

    if ((status = nlroute_send_request(nlp)) == SUCCESS)
    {
        status = nlroute_get_response(nlp);
    }
    return status;
}

// provide default gateway that fits to the subnet defined by ip and netmask
static void nlroute_default_gw_of_if(struct nlink_if *nlp, uint32_t ipaddr, uint32_t netmask, char *result, int result_size)
{
    struct nlmsghdr *nlmp;
    for (nlmp = (struct nlmsghdr *)nlp->recbuf; NLMSG_OK(nlmp, nlp->datalen); nlmp = NLMSG_NEXT(nlmp, nlp->datalen))
    {
        uint32_t gateway;
        struct rtmsg *rtdp = (struct rtmsg *)NLMSG_DATA(nlmp);
        struct rtattr *rtap = (struct rtattr *)RTM_RTA(rtdp);
        int rtlen = RTM_PAYLOAD(nlmp);

        for (; RTA_OK(rtap, rtlen); rtap = RTA_NEXT(rtap, rtlen)) {
            if (rtap->rta_type == RTA_GATEWAY)
            {
                gateway = *(uint32_t *)RTA_DATA(rtap);
                break;
            }
        }

        if (rtdp->rtm_dst_len == 0)
        {
            // this is a default gateway. Check if it fits to the subnet
            uint32_t subnet = ipaddr & netmask;
            if ((ntohl(gateway) & netmask) == subnet)
            {
                inet_ntop(AF_INET, RTA_DATA(rtap), result, result_size);
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------
// config tool functions
//------------------------------------------------------------------------------

enum netvalue {
    NETMASK,
    IPADDR
};

// get port specific network parameter value
static int NetValueGet(const char *pPortString, uint32_t *value, enum netvalue type, libnetSession_t *session)
{
    char buffer[sizeof ("255.255.255.255/32")];
    int status = SUCCESS;

    buffer[0] = '\0';
    switch (type)
    {
    case NETMASK:
        status = ct_libnet_get_netmask_from_hw(pPortString, buffer, sizeof (buffer), NETMASK_FORMAT_MODE_CLASSIC, session);
        if (SUCCESS == status)
        {
            status = ctlib_IpaddrToInt((const char *)buffer, value);
        }
        break;
    case IPADDR:
        status = ct_libnet_get_ip_addr_from_hw(pPortString, buffer, sizeof (buffer), IP_FORMAT_MODE_NO_NETMASK, session);
        if (SUCCESS == status)
        {
            status = ctlib_IpaddrToInt((const char *)buffer, value);
        }
        break;
    }
    return status;
}

static int read_dipvalue()
{
    int dipval = 0;
    int fd = open("/etc/specific/dipvalue-on-reset", O_RDONLY);

    if (fd >= 0)
    {
        char buf[1024];
        char id[] = "DIP_SWITCH_VALUE=";
        char *p;
        int len = read(fd, buf, sizeof (buf));
        close(fd);
        if (len < sizeof (buf))
        {
            buf[len] = '\0';
            p = strstr(buf, id);
            if (p)
            {
                dipval = atoi(p + sizeof (id) - 1);
            }
        }
    }
    return dipval;
}

static int PrintDefaultGateway(const char *pPortString, libnetSession_t *session)
{
    int status = SUCCESS;
    char defaultVia[sizeof ("255.255.255.255")];
    uint32_t ipaddr;
    uint32_t netmask;
    struct nlink_if *nlk;

    defaultVia[0] = '\0';

    if ((status = NetValueGet(pPortString, &ipaddr, IPADDR, session)) == SUCCESS)
    {
        status = NetValueGet(pPortString, &netmask, NETMASK, session);
    }

    if(SUCCESS == status)
    {
        nlk = nlroute_init();
        status = nlroute_table_read(nlk);

        if(SUCCESS == status)
        {
            nlroute_default_gw_of_if(nlk, ipaddr, netmask, defaultVia, sizeof (defaultVia));
        }
        nlroute_close(nlk);
    }

    printf("%s", defaultVia);
    return status;
}

static int PrintParameter(char *pPortString, char *pParamString, libnetSession_t *session)
{
    int status = INVALID_PARAMETER;

    if(0 == strcmp(pParamString, "state")) // duplicate of "get_eth_config state", needed for compatibility reasons
    {
        char state[strlen("disabled") + 1];
        state[0] = '\0';
        status = ct_libnet_get_eth_port_state_from_hw(pPortString, state, sizeof(state), session);

        if(SUCCESS == status)
        {
            printf("%s", state);
        }
    }
    else if(0 == strcmp(pParamString, "cable-state"))
    {
        enum cableState_t state = CT_CABLE_STATE_UNKNOWN;

        status = ct_libnet_get_cable_state(pPortString, &state, session);

        if(SUCCESS == status)
        {
            switch(state)
            {
            case CT_CABLE_STATE_CONNECTED:
                printf("connected");
                break;
            case CT_CABLE_STATE_DISCONNECTED:
                printf("disconnected");
                break;
            case CT_CABLE_STATE_UNKNOWN:
                printf("unknown");
                break;
            //default:
                // TODO: error?
            }

        }
    }
    else if(0 == strcmp(pParamString, "mac-address"))
    {
        char mac[strlen("ff:ff:ff:ff:ff:ff") + 1];
        mac[0] = '\0';
        status = ct_libnet_get_mac_addr_from_hw(pPortString, mac, sizeof(mac), session);

        if(SUCCESS == status)
        {
            printf("%s", mac);
        }
    }
    else if(0 == strcmp(pParamString, "ip-address"))
    {
        char ip[strlen("255.255.255.255/32") + 1];
        ip[0] = '\0';
        status = ct_libnet_get_ip_addr_from_hw(pPortString, ip, sizeof(ip), IP_FORMAT_MODE_NO_NETMASK, session);

        if(SUCCESS == status)
        {
            printf("%s", ip);
        }
    }
    else if(0 == strcmp(pParamString, "config-type"))
    {
        char configtype[16];
        int dipvalue = read_dipvalue();

        if (strcmp(pPortString, "X1") != 0)
        {
            dipvalue = 0;
        }

        switch (dipvalue)
        {
        case 0:
            *configtype = '\0';
            status = ct_libnet_get_config_type(pPortString, configtype, sizeof (configtype), session);
            if(SUCCESS == status)
            {
                printf("%s", configtype);
            }
            break;
        case 255:
            printf("dhcp dip-switch");
            status = SUCCESS;
            break;
        default:
            printf("dip-switch");
            status = SUCCESS;
        }

    }
    else if(0 == strcmp(pParamString, "subnet-mask"))
    {
        char netmask[strlen("255.255.255.255") + 1];
        netmask[0] = '\0';
        status = ct_libnet_get_netmask_from_hw(pPortString, netmask, sizeof(netmask), NETMASK_FORMAT_MODE_CLASSIC, session);

        if(SUCCESS == status)
        {
            printf("%s", netmask);
        }
    }
    else if(0 == strcmp(pParamString, "default-gateway"))
    {
        status = PrintDefaultGateway(pPortString, session);
    } else if(0 == strcmp(pParamString, "default-gateway-nofail"))
    {
        PrintDefaultGateway(pPortString, session);
        status = SUCCESS;
    }

    return status;
}

//
// Show description and usage of program on stdout
//
static void ShowHelpText(const char *progName)
{
  int parameterIndex = 0;

  printf("\n* Get currently used Interface-Parameters *\n\n");
  printf("Usage: %s <interface> <parameter> [-c custom xml network config]\n\n", progName);
  printf("Interface: e.g. X1, X2\n");
  printf("parameter: ");

  // read the possible strings for parameter from array and show them like this: "param1 | param2 | param3"
  while(strlen(astParameterJumptab[parameterIndex].inputString) > 0)
  {
    if(parameterIndex != 0) printf("| ");
    printf("%s ", astParameterJumptab[parameterIndex].inputString);
    ++parameterIndex;
  }

  printf("\n");
  printf("[Default XML network configuration file: %s]\n", NETWORK_INTERFACES_XML);

}

static int GetCurrentEthSettings(int argc, char **argv, struct options *opts)
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
        };

        if(-1 == (c = getopt_long(argc, argv, "c:h", long_options, &option_index)))
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

    struct options opts;

    int nrArgsInterpreted = __ct_getopt(argc, argv, &opts);

    switch(argc - nrArgsInterpreted)
    {
    case 0:
    case 1:
        status = MISSING_PARAMETER;
        break;

    case 2:
        if(!opts.showHelp)
        {
            status = MISSING_PARAMETER;
        }
        break;

    case 3:
        // number of command line arguments - nr of getopt args  - program name
        status = GetCurrentEthSettings(argc - nrArgsInterpreted - 1, &argv[optind], &opts);
        break;

    default:
        status = INVALID_PARAMETER;
        break;
    }

    // cleanup
    free(opts.xmlConfigFile);

    return status;
}
