// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>

#include <net/if.h>
#include <arpa/inet.h>

#include "ct_libnet.h"
#include "../ct_error_handling.h"
#include "ct_xml.h"
#include "ct_xml_xpath.h"
#include "ct_xslt.h"
#include "ct_netlink.h"
#include "ct_ethtool.h"

#ifdef __ENABLE_DSA
#include "ct_swconfig.h"
#endif

#define NET_IFACES_TMP "/tmp/net-ifaces"

#include <unistd.h>
#include <sys/stat.h>

//WORKAROUND
#include <typelabel_API.h>

// Current session management:
//
// NOTE: Session object is synchronized against the XML file using flock API
//
// libnetSession_t *session = NULL;
// int status = ct_libnet_start_session("file.xml", &session);
// ...
// status = ct_libnet_set_...(..., session);
// ...
// status = ct_libnet_get_...(..., session);
//
// status = ct_libnet_commit_changes_...(..., session);
//
// status = ct_libnet_finish_session(session);
//
// // When no more libnet calls are to follow:
// ct_libnet_permanent_close();
//
// TODO: Maybe use another, transparent idea for session management:
//  start_session(): init xml engine etc.
//  then call API functions
//  finish_session(): commit changes, cleanup everything
//
//  when start_session() not called before API call: every API call is an own session

struct libnetSession {
    xmlSession_t *xml;
    // TODO: netlink session, ethtool session etc.
};

static const char *netmasks[33] = {
    "0.0.0.0",          // 0
    
    "128.0.0.0",        // 1
    "192.0.0.0",        // 2
    "224.0.0.0",        // 3
    "240.0.0.0",        // 4
    "248.0.0.0",        // 5
    "252.0.0.0",        // 6
    "254.0.0.0",        // 7
    "255.0.0.0",        // 8

    "255.128.0.0",      // 9
    "255.192.0.0",      // 10
    "255.224.0.0",      // 11
    "255.240.0.0",      // 12
    "255.248.0.0",      // 13
    "255.252.0.0",      // 14
    "255.254.0.0",      // 15
    "255.255.0.0",      // 16

    "255.255.128.0",      // 17
    "255.255.192.0",      // 18
    "255.255.224.0",      // 19
    "255.255.240.0",      // 20
    "255.255.248.0",      // 21
    "255.255.252.0",      // 22
    "255.255.254.0",      // 23
    "255.255.255.0",      // 24


    "255.255.255.128",      // 25
    "255.255.255.192",      // 26
    "255.255.255.224",      // 27
    "255.255.255.240",      // 28
    "255.255.255.248",      // 29
    "255.255.255.252",      // 30
    "255.255.255.254",      // 31
    "255.255.255.255",      // 32

};

enum ethtoolParamType
{
    ETHTOOL_PARAM_AUTONEG,
    ETHTOOL_PARAM_SPEED,
    ETHTOOL_PARAM_DUPLEX,
};

// The mapping port -> net interface depends from context:
// When using virtual interfaces, ip settings may be configured for example on virtual interface br0
// whereas ethernet settings like port speed or duplex are configured on physical interface ethX1
// That's why we need 2 functions to resolve the mapping

static const char* g_lastError;

static inline int __port2dev_ethernet(const char *port, char *dev, size_t devLen, libnetSession_t *handle);
static inline int __port2dev_ip(const char *port, char *dev, size_t devLen, libnetSession_t *handle);

static inline void __setWBMErrorState(int *status, int errno, const char *errmsg);

static bool __isValidIP(const char *ip);
static bool __isValidMAC(const char *ip);

static bool __isValidIP(const char *ip)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
    return (result != 0);
}

static bool __isValidMAC(const char *mac)
{
    bool result = true;

    int cur = 0;
    while(*mac)
    {
        if((cur + 1) % 3 == 0)
        {
            if(*mac != ':')
            {
                result = false;
                break;
            }
        }
        else
        {
            if(!isxdigit(*mac))
            {
                result = false;
                break;
            }
        }

        ++mac;
        ++cur;
    }

    return (result && cur == 17);
}

// NOTE: only interesting for setter functions. 
// Error message is shown in WBM/CBM -> error in a getter results in an empty field.
// Error messages should have an according granularity (end user relevant)!
static inline void __setWBMErrorState(int *status, int errno, const char *errmsg)
{
    if(NULL != status)
    {
        *status = errno;
    }

    g_lastError = errmsg;
}

static inline int __port2dev_ethernet(const char *port, 
                                      char *dev, 
                                      size_t devLen,
                                      libnetSession_t *sessionHandle)
{

    char *xpathTemplate = "//ethernet_settings[port_name='%s']/../device_name";
    char xpath[strlen(xpathTemplate) + strlen(port) + 1];
    sprintf(xpath, xpathTemplate, port);

    return ct_xml_get_value(xpath, dev, devLen, sessionHandle->xml);
}

static inline int __port2dev_ip(const char *port, 
                                char *dev, 
                                size_t devLen,
                                libnetSession_t *sessionHandle)
{

    char *xpathTemplate = "//ip_settings[port_name='%s']/../device_name";
    char xpath[strlen(xpathTemplate) + strlen(port) + 1];
    sprintf(xpath, xpathTemplate, port);

    return ct_xml_get_value(xpath, dev, devLen, sessionHandle->xml);
}

//TODO: proper, configurable logging
// Advantage of current work-around: cheap logging of ifup/down commands output, small log file size
//#define REDIRECT_OUTPUT " &> /dev/null "
#define REDIRECT_OUTPUT " &> /tmp/log."

static int __validateChanges(const char *dev, libnetSession_t *sessionHandle)
{
    int status = SUCCESS;

    GString *ifdownCmd = g_string_new("ifdown -fv " REDIRECT_OUTPUT "ifdown1 ");
    GString *ifupCmd = g_string_new("ifup -fv -i " REDIRECT_OUTPUT "ifup2 " NET_IFACES_TMP " ");


    if(NULL == dev) // do ifupdown all
    {
        g_string_append(ifdownCmd, " -a");
        g_string_append(ifupCmd, " -a");
    }
    else
    {
        g_string_append(ifdownCmd, (const gchar *) dev);
        g_string_append(ifupCmd, (const gchar *) dev);
    }

    if(SUCCESS == status)
    {
        status = ct_xml_generate_network_config(NET_IFACES_TMP, sessionHandle->xml);
    }

    if(SUCCESS == status)
    {
        (void) system((const char*) ifdownCmd->str);
        status = system((const char*) ifupCmd->str); 

        switch(status)
        {
        case -1:
            // system() failed
            status = SYSTEM_CALL_ERROR;
            break;
        case 0:
            status = SUCCESS;
            break;
        default:
            // return value of ifupdown command
            //TODO: map to ct_error
            status = WEXITSTATUS(status);
            break;
        }

        if(SUCCESS != status)
        {
            __setWBMErrorState(NULL, 0, "Error restarting network interface. Restoring original\nconfiguration.");

            // restore previous config via ifdown/ifup
            g_string_assign(ifdownCmd, "ifdown -fv -i " REDIRECT_OUTPUT "ifdown3 " NET_IFACES_TMP " ");
            g_string_assign(ifupCmd, "ifup -fv " REDIRECT_OUTPUT "ifup4 ");

            if(NULL != dev)
            {
                g_string_append(ifdownCmd, (const gchar*) dev);
                g_string_append(ifupCmd, (const gchar *) dev);
            }

            (void) system((const char*) ifdownCmd->str);
            if(0 != system((const char*) ifupCmd->str))
            {
                __setWBMErrorState(&status, SYSTEM_CALL_ERROR, "Fatal error restarting network interface. Original\nconfiguration could not be restored.");
            }
        }
    }

    g_string_free(ifdownCmd, TRUE);
    g_string_free(ifupCmd, TRUE);

    return status;
}

static int __execPostNetconfigScripts(const char *dev)
{
    int status = SUCCESS;
    int rc;
    GString *cmd = g_string_new("/bin/run-parts");

    assert(NULL != dev);
    if('\0' != *dev)
    {
        g_string_append_printf(cmd, " -a %s", dev);
    }
    g_string_append(cmd, " /etc/config-tools/post_netconfig.d");
    rc = system((const char *)cmd->str);
    status = WEXITSTATUS(rc);
    g_string_free(cmd, TRUE);
    return status;
}

const char * ct_libnet_get_last_error(void)
{
    return g_lastError;
}

// Assumption: string is already stripped
static bool __isClassicNetmask(const char *netmask)
{
    bool res = false;

    int i;
    for(i = 0; (i<33) && (false == res); ++i)
    {
        if(0 == strcmp(netmasks[i], netmask))
        {
            res = true;
        }
    }
                
    return res;
}

// Assumption: string is already stripped
static bool __isCidrNetmask(const char *netmask)
{
    bool res = false;

    if(0 == strcmp("0", netmask)) // strtol returns 0 in error case
    {
        res = true;
    }
    else
    {
        int nmAsInt = strtol(netmask, NULL, 10);

        if( (nmAsInt > 0) && (nmAsInt < 33) )
        {
            res = true;
        }
    }

    return res;
}

int ct_libnet_netmask_to_cidr(const char *from, char *res, size_t resLen)
{
    assert(NULL != from);
    assert(NULL != res);
// allow overlapping from and to (as in netmask_to_cidr(netmask,netmask...)    
//    assert(0 == strlen(res));

    assert(resLen > strlen("32"));

    int status = INVALID_PARAMETER;

    if(__isCidrNetmask(from))
    {
        (void) g_strlcpy(res, from, resLen);
        status = SUCCESS;
    }
    else
    {
        int i;
        for(i = 0; (i<33) && (SUCCESS != status); ++i)
        {
            if(0 == strcmp(netmasks[i], from))
            {
                (void) snprintf(res, resLen, "%d", i);
                status = SUCCESS;
            }
        }
    }

    return status;
}

int ct_libnet_start_session(const char *xmlConfigFile, libnetSession_t **sessionHandle)
{
    int status = SUCCESS;

    assert(NULL != sessionHandle);

    *sessionHandle = g_malloc(sizeof(libnetSession_t));

    libnetSession_t *ressources = *sessionHandle;

    if(NULL == ressources)
    {
        status = SYSTEM_CALL_ERROR;
    }
    else
    {
        (*sessionHandle)->xml = NULL;
        status = ct_xml_start_session(xmlConfigFile, &((*sessionHandle)->xml));
    } 

    return status;
}

int ct_libnet_finish_session(libnetSession_t *sessionHandle)
{
    int status = SUCCESS;

    if(NULL != sessionHandle)
    {
        if(NULL != sessionHandle->xml)
        {
            status = ct_xml_finish_session(sessionHandle->xml);
        }
        g_free(sessionHandle);
    }

    return status; 
}

void ct_libnet_permanent_close(void)
{
    ct_xml_permanent_close();
}

int ct_libnet_netmask_to_classic(const char *from, char *res, size_t resLen)
{
    assert(NULL != from);
    assert(NULL != res);

// allow overlapping from and to (as in netmask_to_classic(netmask,netmask...)    
//    assert(0 == strlen(res));

    assert(resLen > strlen("255.255.255.255"));

    int status = INVALID_PARAMETER;

    if(__isClassicNetmask(from))
    {
        (void) g_strlcpy(res, from, resLen);
        status = SUCCESS;
    }
    else
    {
        // strtol returns 0 in error case
        if(0 == strcmp("0", from))
        {
            (void) snprintf(res, resLen, "0.0.0.0");
            status = SUCCESS;
        }
        else
        {
            int nmAsInt = strtol(from, NULL, 10);

            if( (nmAsInt > 0) && (nmAsInt < 33) )
            {
                (void) snprintf(res, resLen, "%s", netmasks[nmAsInt]);
                status = SUCCESS;
            }
        }
    }

    return status;
}

int ct_libnet_get_eth_port_state_from_hw(const char *port, 
                                         char *strState, 
                                         size_t strStateLen,
                                         libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != strState);
    assert(strStateLen > strlen("disabled"));
    assert(NULL != sessionHandle);

    int status = SUCCESS;
    unsigned int flags = 0;

    char dev[IFNAMSIZ + 1];
    dev[0] = '\0';

    status = __port2dev_ethernet(port, dev, sizeof(dev), sessionHandle);
   
    // FIXME: find a better place for init/cleanup!
    // User should be able to chain several ct_libnet_ functions without
    // reinitializing the underlying stuff.
    
	netlinkSession_t *nlSessionHandle = NULL;
	
	if(SUCCESS == status)
  {
      status = ct_netlink_init(&nlSessionHandle);
  }

  if(SUCCESS == status)
  {
      status = ct_netlink_get_link_flags(dev, &flags, nlSessionHandle);
  }

  if(SUCCESS == status)
  {
      if(flags & IFF_UP)
      {
          snprintf(strState, strStateLen, "enabled");
      }
      else
      {
          snprintf(strState, strStateLen, "disabled");
      }
  }

  ct_netlink_cleanup(nlSessionHandle);

  return status;
}

int ct_libnet_get_cable_state(const char *port, 
                              enum cableState_t *state, 
                              libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != state);
    assert(NULL != sessionHandle);

    int status = SUCCESS;
    unsigned int flags = 0;

    char dev[IFNAMSIZ + 1];
    dev[0] = '\0';

    status = __port2dev_ethernet(port, dev, sizeof(dev), sessionHandle);
   
    // FIXME: find a better place for init/cleanup!
    // User should be able to chain several ct_libnet_ functions without
    // reinitializing the underlying stuff.
	
	netlinkSession_t *nlSessionHandle = NULL;

    if(SUCCESS == status)
    {
        status = ct_netlink_init(&nlSessionHandle);
    }

    if(SUCCESS == status)
    {
        status = ct_netlink_get_link_flags(dev, &flags, nlSessionHandle);
    }

    if(SUCCESS == status)
    {

        if(flags & IFF_RUNNING)
        {
            *state = CT_CABLE_STATE_CONNECTED;
        }
        else
        {
            if(flags & IFF_UP)
            {
                *state = CT_CABLE_STATE_DISCONNECTED;
            }
            else
            {
                *state = CT_CABLE_STATE_UNKNOWN;
            }
        }
    }

    ct_netlink_cleanup(nlSessionHandle);

    return status;
}

/**
 * @brief Returns the IPv4 address currently configured for the given device
 *
 * @param dev   Device name (i.e. eth0)
 * @param ip    Pointer to char array where the ip address will be stored. Should be at least 20 bytes long
 *             to store string like "255.255.255.255/32"
 * @param ipLen Length of the ip char array
 * @param mode  IP_FORMAT_MODE_NO_NETMASK/IP_FORMAT_MODE_NETMASK expected. The former modifier may be used
 *             to cut the CIDR netmask part (i.e. /24) from the IP address.
 *
 * @return SUCCESS if no errors happened, ct_netlink_get_ipaddr's errors 
 */
int ct_libnet_get_ip_addr_from_hw(const char *port, 
                                  char *ip, 
                                  size_t ipLen, 
                                  enum ipAddrFormatModes_t mode,
                                  libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != ip);

    assert(ipLen > strlen("255.255.255.255/32"));
    assert(NULL != sessionHandle);


    int status = SUCCESS;


    char dev[IFNAMSIZ + 1];
    dev[0] = '\0';

    status = __port2dev_ip(port, dev, sizeof(dev), sessionHandle);

	netlinkSession_t *nlSessionHandle = NULL;

    if(SUCCESS == status)
    {
        status = ct_netlink_init(&nlSessionHandle);
    }

    if(SUCCESS == status)
    {
        status = ct_netlink_get_ipaddr(dev, ip, ipLen, nlSessionHandle);
    }

    if( (SUCCESS == status) && (IP_FORMAT_MODE_NO_NETMASK == mode) )
    {
        char *delim = strchr(ip, '/');
        if(NULL != delim)
        {
            *delim = '\0';
        }
    }

    ct_netlink_cleanup(nlSessionHandle);

    return status;
}

/**
 * @brief TODO
 *
 * @param port
 * @param netmask
 * @param netmaskLen
 *
 * @return 
 */
int ct_libnet_get_netmask_from_hw(const char *port, 
                                  char *netmask, 
                                  size_t netmaskLen, 
                                  enum netmaskFormatModes_t mode,
                                  libnetSession_t *sessionHandle)
{

    assert(NULL != port);
    assert(NULL != netmask);
    assert(NULL != sessionHandle);

    switch(mode)
    {
        case NETMASK_FORMAT_MODE_CIDR:
            assert(netmaskLen > 2);
            break;
        case NETMASK_FORMAT_MODE_CLASSIC:
            assert(netmaskLen > strlen("255.255.255.255"));
            break;
        default:
            break;
    }

    int status = SUCCESS;

    char ip[strlen("255.255.255.255/32") + 1];
    ip[0] = '\0';

    char dev[IFNAMSIZ + 1];
    dev[0] = '\0';

    status = __port2dev_ip(port, dev, sizeof(dev), sessionHandle);

	netlinkSession_t *nlSessionHandle = NULL;

    if(SUCCESS == status)
    {
        status = ct_netlink_init(&nlSessionHandle);
    }

    if(SUCCESS == status)
    {
        status = ct_netlink_get_ipaddr(dev, ip, sizeof(ip), nlSessionHandle);
    }

    if(SUCCESS == status)
    {
        char *delim = strchr(ip, '/');
        if(NULL != delim)
        {
            strncpy(netmask, (delim + 1), netmaskLen - 1);
            netmask[netmaskLen - 1] = '\0';

            if(NETMASK_FORMAT_MODE_CLASSIC == mode)
            {
                status = ct_libnet_netmask_to_classic(delim + 1, netmask, netmaskLen);
            }
            else // CIDR mode is the netlink layer default
            {
                g_strlcpy(netmask, delim + 1, netmaskLen);
            }
        }
    }

    ct_netlink_cleanup(nlSessionHandle);

    return status;
}

/**
 * @brief TODO
 *
 * @param port
 * @param mac
 * @param macLen
 *
 * @return 
 */
int ct_libnet_get_mac_addr_from_hw(const char *port, 
                                   char *mac, 
                                   size_t macLen,
                                   libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != mac);
    assert(NULL != sessionHandle);

    int status = SUCCESS;
    
    char dev[IFNAMSIZ + 1];
    dev[0] = '\0';

    status = __port2dev_ip(port, dev, sizeof(dev), sessionHandle);

	netlinkSession_t *nlSessionHandle = NULL;

    if(SUCCESS == status)
    {
        status = ct_netlink_init(&nlSessionHandle);
    }

    if(SUCCESS == status)
    {
        status = ct_netlink_get_macaddr(dev, mac, macLen, nlSessionHandle);
    }
    
    ct_netlink_cleanup(nlSessionHandle);

    return status; 
}


/**
 * @brief TODO
 *
 * @param port
 * @param broadcast
 * @param broadcastLen
 *
 * @return 
 */
int ct_libnet_get_broadcast_from_hw(const char *port, 
                                    char *broadcast, 
                                    size_t broadcastLen,
                                    libnetSession_t *sessionHandle)
{

    assert(NULL != port);
    assert(NULL != broadcast);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    char dev[IFNAMSIZ + 1];
    dev[0] = '\0';

    status = __port2dev_ip(port, dev, sizeof(dev), sessionHandle);

	netlinkSession_t *nlSessionHandle = NULL;

	status = ct_netlink_init(&nlSessionHandle);

    if(SUCCESS == status)
    {
        status = ct_netlink_get_broadcast(dev, broadcast, broadcastLen, nlSessionHandle);
    }

    ct_netlink_cleanup(nlSessionHandle);
    
	return status;
}

/**
 * @brief TODO
 *
 * @param port
 * @param configType
 * @param configTypeLen
 *
 * @return 
 */
int ct_libnet_get_config_type(const char *port, 
                              char *configType, 
                              size_t configTypeLen, 
                              libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != configType);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    char *xpathTemplate = "//ip_settings[port_name='%s']/type";
    char xpath[strlen(xpathTemplate) + strlen(port) + 1];
    sprintf(xpath, xpathTemplate, port);

    status = ct_xml_get_value(xpath, 
                              configType, 
                              configTypeLen, 
                              sessionHandle->xml);

    return status;
}

/**
 * @brief TODO
 *
 * @param param
 * @param port
 * @param result
 * @param resultLen
 *
 * @return 
 */
static int ct_libnet_get_ethtool_param(enum ethtoolParamType param, 
                                       const char *port, 
                                       char *result, 
                                       size_t resultLen,
                                       libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != result);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    char dev[IFNAMSIZ + 1];
    dev[0] = '\0';

    status = __port2dev_ethernet(port, dev, sizeof(dev), sessionHandle);

    if(SUCCESS == status)
    {
        switch(param)
        {
        case ETHTOOL_PARAM_SPEED:
            status = ct_ethtool_get_speed(dev, result, resultLen);
            break;
        case ETHTOOL_PARAM_DUPLEX:
            status = ct_ethtool_get_duplex(dev, result, resultLen);
            break;
        case ETHTOOL_PARAM_AUTONEG:
            status = ct_ethtool_get_autoneg(dev, result, resultLen);
            break;
        default:
            status = INVALID_PARAMETER;
        }
    }

    return status;
}

/**
 * @brief TODO
 *
 * @param port
 * @param speed
 * @param speedLen
 *
 * @return 
 */
int ct_libnet_get_speed_from_hw(const char *port, 
                                char *speed, 
                                size_t speedLen, 
                                libnetSession_t *sessionHandle)
{
    return ct_libnet_get_ethtool_param(ETHTOOL_PARAM_SPEED, 
                                       port, 
                                       speed, 
                                       speedLen, 
                                       sessionHandle);
}

int ct_libnet_get_duplex_from_hw(const char *port, 
                                 char *duplex, 
                                 size_t duplexLen,
                                 libnetSession_t *sessionHandle)
{
    return ct_libnet_get_ethtool_param(ETHTOOL_PARAM_DUPLEX, 
                                       port, 
                                       duplex, 
                                       duplexLen, 
                                       sessionHandle);
}

int ct_libnet_get_autoneg_from_hw(const char *port, 
                                  char *autoneg, 
                                  size_t autonegLen,
                                  libnetSession_t *sessionHandle)
{
    return ct_libnet_get_ethtool_param(ETHTOOL_PARAM_AUTONEG, 
                                       port, 
                                       autoneg, 
                                       autonegLen,
                                       sessionHandle);
}

int ct_libnet_get_autoneg_from_config(const char *port, 
                                      char *autoneg, 
                                      size_t autonegLen,
                                      libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != autoneg);
    assert(autonegLen > strlen("disabled"));
    assert(NULL != sessionHandle);
 
    char *xpathTemplate = "//ethernet_settings[port_name='%s']/autoneg";
    char xpath[strlen(xpathTemplate) + strlen(port) + 1];
    sprintf(xpath, xpathTemplate, port);
 
    return ct_xml_get_value(xpath, autoneg, autonegLen, sessionHandle->xml);
}

int ct_libnet_get_port_state_from_config(const char *port, 
                                         char *state, 
                                         size_t stateLen,
                                         libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != state);
    assert(stateLen > strlen("disabled"));
    assert(NULL != sessionHandle);
 
    int status = SUCCESS;

    char dev[IFNAMSIZ + 1];
    dev[0] = '\0';

    status = __port2dev_ethernet(port, dev, sizeof(dev), sessionHandle);

    if(SUCCESS == status)
    {
        char *xpathTemplate = "//iface[device_name='%s']/state";
        char xpath[strlen(xpathTemplate) + strlen(dev) + 1];
        sprintf(xpath, xpathTemplate, dev);

        status = ct_xml_get_value(xpath, state, stateLen, sessionHandle->xml);
    }

    return status;
}

int ct_libnet_get_speed_from_config(const char *port, 
                                    char *speed, 
                                    size_t speedLen,
                                    libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != speed);
    assert(speedLen > strlen("1000M"));
    assert(NULL != sessionHandle);


    char *xpathTemplate = "//ethernet_settings[port_name='%s']/speed";
    char xpath[strlen(xpathTemplate) + strlen(port) + 1];
    sprintf(xpath, xpathTemplate, port);
 
    return ct_xml_get_value(xpath, speed, speedLen, sessionHandle->xml);
}

int ct_libnet_get_duplex_from_config(const char *port, 
                                     char *duplex, 
                                     size_t duplexLen,
                                     libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != duplex);
    assert(duplexLen > strlen("half"));
    assert(NULL != sessionHandle);

 
    char *xpathTemplate = "//ethernet_settings[port_name='%s']/duplex";
    char xpath[strlen(xpathTemplate) + strlen(port) + 1];
    sprintf(xpath, xpathTemplate, port);
 
    return ct_xml_get_value(xpath, duplex, duplexLen, sessionHandle->xml);
}

int ct_libnet_get_default_via_from_hw(const char *port, char *strDefaultVia, size_t strDefaultViaLen)
{
    assert(NULL != port); // TODO: do we need the port at all?
    assert(NULL != strDefaultVia);
    assert(strDefaultViaLen > strlen("255.255.255.255"));

	netlinkSession_t *nlSessionHandle = NULL;

    int status = ct_netlink_init(&nlSessionHandle);
    
    if(SUCCESS == status)
    {
        status = ct_netlink_get_default_via("dontcare", strDefaultVia, strDefaultViaLen, nlSessionHandle);
    }

    ct_netlink_cleanup(nlSessionHandle);

    return status;
}

int ct_libnet_get_ip_addr_from_config(const char *port, 
                                      char *ip, 
                                      size_t ipLen,
                                      libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != ip);
    assert(NULL != sessionHandle);
 
    char *xpathTemplate = "//ip_settings[port_name='%s']/static_ipaddr";
    char xpath[strlen(xpathTemplate) + strlen(port) + 1];
    sprintf(xpath, xpathTemplate, port);
 
    return ct_xml_get_value(xpath, ip, ipLen, sessionHandle->xml);
}

int ct_libnet_get_netmask_from_config(const char *port, 
                                      char *netmask, 
                                      size_t netmaskLen, 
                                      enum netmaskFormatModes_t mode,
                                      libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != netmask);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    switch(mode)
    {
        case NETMASK_FORMAT_MODE_CIDR:
            assert(netmaskLen > 2);
            break;
        case NETMASK_FORMAT_MODE_CLASSIC:
            assert(netmaskLen > strlen("255.255.255.255"));
            break;
        default:
            status = INVALID_PARAMETER;
            break;
    }

    if(SUCCESS == status)
    {
        char *xpathTemplate = "//ip_settings[port_name='%s']/static_netmask";
        char xpath[strlen(xpathTemplate) + strlen(port) + 1];
        sprintf(xpath, xpathTemplate, port);
    
        status = ct_xml_get_value(xpath, netmask, netmaskLen, sessionHandle->xml);
    }
 

    if(SUCCESS == status && NETMASK_FORMAT_MODE_CLASSIC == mode)
    {
        status = ct_libnet_netmask_to_classic(netmask, netmask, netmaskLen);
    }

    return status;
}

int ct_libnet_get_broadcast_from_config(const char *port, 
                                        char *broadcast, 
                                        size_t broadcastLen, 
                                        libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != broadcast);
    assert(broadcastLen > strlen("255.255.255.255"));
    assert(NULL != sessionHandle);
 
    char *xpathTemplate = "//ip_settings[port_name='%s']/static_broadcast";
    char xpath[strlen(xpathTemplate) + strlen(port) + 1];
    sprintf(xpath, xpathTemplate, port);
 
    return ct_xml_get_value(xpath, broadcast, broadcastLen, sessionHandle->xml);
}

static int ct_libnet_get_port_names(const char *xpath,
                                     char *result, 
                                     size_t resultLen, 
                                     const char *delim, 
                                     libnetSession_t *sessionHandle)
{
    assert(NULL != result);
    assert(strlen(result) == 0);
    assert(NULL != delim); 
    assert(resultLen > 0);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    GString *gstringResult = g_string_new(""); 

    status = ct_xml_get_multiple_values(xpath, gstringResult, delim, sessionHandle->xml);

    if(SUCCESS == status)
    {
        size_t written = snprintf(result, resultLen, "%s", gstringResult->str);

        if(written < gstringResult->len)
        {
            status = SYSTEM_CALL_ERROR; 
        }
    }

    g_string_free(gstringResult, TRUE);

    return status;
}

/* Return all active ports */
int ct_libnet_get_ports_list(char *result, 
                             size_t resultLen, 
                             const char *delim, 
                             libnetSession_t *sessionHandle)
{
    const char *xpath = "//ip_settings[show_in_wbm='1']/port_name";
    return ct_libnet_get_port_names(xpath, result, resultLen, delim, sessionHandle);
}

/* Return all possible ports */
int ct_libnet_get_all_ports_list(char *result, 
                                 size_t resultLen, 
                                 const char *delim, 
                                 libnetSession_t *sessionHandle)
{
    const char *xpath = "//ip_settings/port_name";
    return ct_libnet_get_port_names(xpath, result, resultLen, delim, sessionHandle);
}

/**
 * @brief TODO
 *
 * @param dev
 * @param ip
 *
 * @return 
 */

// Could be optimized by avoiding ifdown/ifup
int ct_libnet_set_ip_addr_to_config(const char *port, 
                                    const char *ip, 
                                    libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != ip);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    if(__isValidIP(ip))
    {

        char *xpathTemplate = "//ip_settings[port_name='%s']/static_ipaddr";
        char xpath[strlen(xpathTemplate) + strlen(port) + 1];
        sprintf(xpath, xpathTemplate, port);

        status = ct_xml_set_value(xpath, ip, sessionHandle->xml);
    }
    else
    {
        __setWBMErrorState(&status, INVALID_PARAMETER, "Invalid IP address.");
    }

    return status;
}

/**
 * @brief TODO
 *
 * @param dev
 * @param netmask
 *
 * @return 
 */
int ct_libnet_set_netmask_to_config(const char *port,
                                    const char *netmask, 
                                    libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != netmask);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    char netmaskCidr[3];

    status = ct_libnet_netmask_to_cidr(netmask, netmaskCidr, sizeof(netmaskCidr));

    if(SUCCESS == status)
    {
        // FIXME: static ip address?
        char *xpathTemplate = "//ip_settings[port_name='%s']/static_netmask";
        char xpath[strlen(xpathTemplate) + strlen(port) + 1];
        sprintf(xpath, xpathTemplate, port);
        status = ct_xml_set_value(xpath, netmaskCidr, sessionHandle->xml);
    }

		if(SUCCESS == status)
    {
        char *xpathTemplate = "//ip_settings[port_name='%s']/static_netmask_long";
        char xpath[strlen(xpathTemplate) + strlen(port) + 1];
        sprintf(xpath, xpathTemplate, port);

        // Check if netmask long xml entry exists.
        bool netmaskLongEntryExists = false;
        size_t const netmaskLen = strlen("255.255.255.255") + 1;
        char netmaskTmp[netmaskLen];
        memset(netmaskTmp, 0, sizeof(netmaskTmp));
        status = ct_xml_has_key(xpath, &netmaskLongEntryExists, sessionHandle->xml);

        if(SUCCESS == status)
        {
          	// Ensure netmask has classic format.
          	status = ct_libnet_netmask_to_classic(netmask, netmaskTmp, sizeof(netmaskTmp));
        }

        if(SUCCESS == status)
        {
          	if(netmaskLongEntryExists)
          	{
            	status = ct_xml_set_value(xpath, netmaskTmp, sessionHandle->xml);
          	}
          	else
          	{
            	status = ct_xml_add_value(xpath, netmaskTmp, sessionHandle->xml);
          	}
        }
    }

    return status;
}

/**
 * @brief 
 *
 * @param port
 * @param broadcast
 *
 * @return 
 */
int ct_libnet_set_broadcast_to_config(const char *port, 
                                      const char *broadcast, 
                                      libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != broadcast);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    if(!__isValidIP(broadcast))
    {
        __setWBMErrorState(&status, INVALID_PARAMETER, "Invalid broadcast address.");
    }

    if(SUCCESS == status)
    {
        // FIXME: static ip address?
        char *xpathTemplate = "//ip_settings[port_name='%s']/static_broadcast";
        char xpath[strlen(xpathTemplate) + strlen(port) + 1];
        sprintf(xpath, xpathTemplate, port);
        status = ct_xml_set_value(xpath, broadcast, sessionHandle->xml);
    }

    return status;
}

static inline int __disable_all_default_vias(libnetSession_t *sessionHandle)
{
    return ct_xml_set_multiple_values("//ip_settings/gw[state='enabled']/state", 
                                      "disabled", 
                                      sessionHandle->xml);
}

static inline bool __hasStaticConfig(const char *port, int *status, libnetSession_t *sessionHandle)
{
    bool result = false;

    char configType[strlen("static") + 1];
    configType[0] = '\0';
    
    *status = ct_libnet_get_config_type(port, configType, sizeof(configType), sessionHandle);

    if(SUCCESS == *status && (0 == strcmp("static", configType)) )
    {
        result = true;
    }

    return result;
}

/**
 * @brief TODO
 *
 * @param port
 * @param mac
 *
 * @return 
 */
int ct_libnet_set_mac_addr(const char *port, const char *mac, libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != mac);
    assert(NULL != sessionHandle);

    int status = SUCCESS;
   
    if(!__isValidMAC(mac))
    {
        __setWBMErrorState(&status, INVALID_PARAMETER, "Invalid MAC address.");
    }

    if(SUCCESS == status)
    {
        char *xpathTemplate = "//ethernet_settings[port_name='%s']/mac";
        char xpath[strlen(xpathTemplate) + strlen(port) + 1];
        sprintf(xpath, xpathTemplate, port);
        status = ct_xml_set_value(xpath, mac, sessionHandle->xml);
    }

    return status;
}

/**
 * @brief 
 *
 * @param port
 * @param state
 *
 * @return 
 */
int ct_libnet_set_port_state_to_config(const char *port, 
                                       const char *state, 
                                       libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != state);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    if( 0 != strcmp(state, "enabled") &&
        0 != strcmp(state, "disabled")) 
    {
        // No need to __setWBMErrorState: can only happen for console/C user 
        status = INVALID_PARAMETER;
    }

    if(SUCCESS == status)
    {
        char *xpathTemplate = "//ethernet_settings[port_name='%s']/../state";
        char xpath[strlen(xpathTemplate) + strlen(port) + 1];
        sprintf(xpath, xpathTemplate, port);
        status = ct_xml_set_value(xpath, state, sessionHandle->xml);
    }

    return status;
}

/**
 * @brief TODO
 *
 * @param port
 * @param configType
 *
 * @return 
 */
int ct_libnet_set_config_type(const char *port, 
                              const char *configType, 
                              libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != configType);
    assert(NULL != sessionHandle); 

    int status = SUCCESS;

    if( 0 != strcmp(configType, "static") &&
        0 != strcmp(configType, "dhcp") &&
        0 != strcmp(configType, "bootp"))
    {
        // No need to __setWBMErrorState: can only happen for console/C user 
        status = INVALID_PARAMETER;
    }

    if(SUCCESS == status)
    {
        char *xpathTemplate = "//ip_settings[port_name='%s']/type";
        char xpath[strlen(xpathTemplate) + strlen(port) + 1];
        sprintf(xpath, xpathTemplate, port);
        status = ct_xml_set_value(xpath, configType, sessionHandle->xml);
    }

    return status;
}

int ct_libnet_set_port_state_to_hw(const char *port,
                                   const char *state,
                                   libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != state);
    assert(NULL != sessionHandle);
    
    int status = SUCCESS;

    char dev[IFNAMSIZ + 1];
    dev[0] = '\0';

	netlinkSession_t *nlSessionHandle = NULL;

    status = ct_netlink_init(&nlSessionHandle);

    if(SUCCESS == status)
    {
        status = __port2dev_ip(port, dev, sizeof(dev), sessionHandle);
    }

    if(0 == strcmp("enabled", state))
    {
        status = ct_netlink_enable_link(dev, nlSessionHandle);
    }
    else if(0 == strcmp("disabled", state))
    {
        status = ct_netlink_disable_link(dev, nlSessionHandle);
    }
    else
    {
        // No need to __setWBMErrorState: can only happen for console/C user 
        status = INVALID_PARAMETER;
    }

    ct_netlink_cleanup(nlSessionHandle);

    return status;
}

// TODO: list of accepted autoneg, speed and duplex values
// TODO: currently unused: eth params are written to config and updated
// via ifdown/ifup
int ct_libnet_set_port_params_to_hw(const char *port, 
                                    const char *autoneg,
                                    const char *speed,
                                    const char *duplex,
                                    libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != autoneg);
    assert(NULL != speed);
    assert(NULL != duplex);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    enum ct_ethtoolValues tAutoneg;
    enum ct_ethtoolValues tSpeed;
    enum ct_ethtoolValues tDuplex;

    char dev[IFNAMSIZ + 1];
    dev[0] = '\0';

    status = __port2dev_ethernet(port, dev, sizeof(dev), sessionHandle);
 
    if(SUCCESS == status)
    {

        if(0 == strcmp("enabled", autoneg))
        {
            tAutoneg = CT_ETHTOOL_AUTONEG_ON;
        }
        else if(0 == strcmp("disabled", autoneg))
        {
            tAutoneg = CT_ETHTOOL_AUTONEG_OFF;
        }
        else
        {
            // No need to __setWBMErrorState: can only happen for console/C user 
            status = INVALID_PARAMETER;
        }
    }

    if(SUCCESS == status)
    {

        if(0 == strcmp("10M", speed))
        {
            tSpeed = CT_ETHTOOL_SPEED_10M;
        }
        else if(0 == strcmp("100M", speed))
        {
            tSpeed = CT_ETHTOOL_SPEED_100M;
        }
        else if(0 == strcmp("1000M", speed))
        {
            tSpeed = CT_ETHTOOL_SPEED_1000M;
        }
        else
        {
            // No need to __setWBMErrorState: can only happen for C user 
            status = INVALID_PARAMETER;
        }
    }

    if(SUCCESS == status)
    {

        if(0 == strcmp("full", duplex))
        {
            tDuplex = CT_ETHTOOL_DUPLEX_FULL;
        }
        else if(0 == strcmp("half", duplex))
        {
            tDuplex = CT_ETHTOOL_DUPLEX_HALF;
        }
        else
        {
            // No need to __setWBMErrorState: can only happen for C user 
            status = INVALID_PARAMETER;
        }
    }

    if(SUCCESS == status)
    {
        status = ct_ethtool_set_port_params(dev, tAutoneg, tSpeed, tDuplex);
    }

    return status;
}

int ct_libnet_set_speed_to_config(const char *port, 
                                  const char *speed, 
                                  libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != speed);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    // TODO: refactor value checks
    if( (0 != strcmp("10M", speed))  &&
        (0 != strcmp("100M", speed)) &&
        (0 != strcmp("1000M", speed)))
    {
        // No need to __setWBMErrorState: can only happen for console/C user 
        status = INVALID_PARAMETER;
    } 

    if(SUCCESS == status)
    {

        char *xpathTemplate = "//ethernet_settings[port_name='%s']/speed";
        char xpath[strlen(xpathTemplate) + strlen(port) + 1];
        sprintf(xpath, xpathTemplate, port);
    
        status = ct_xml_set_value(xpath, speed, sessionHandle->xml);
    }

    return status;
}

int ct_libnet_set_autoneg_to_config(const char *port, 
                                    const char *autoneg, 
                                    libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != autoneg);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    if( (0 != strcmp("enabled", autoneg)) &&
        (0 != strcmp("disabled", autoneg)))
    {
        status = INVALID_PARAMETER;
    } 

    if(SUCCESS == status)
    {

        char *xpathTemplate = "//ethernet_settings[port_name='%s']/autoneg";
        char xpath[strlen(xpathTemplate) + strlen(port) + 1];
        sprintf(xpath, xpathTemplate, port);
    
        status = ct_xml_set_value(xpath, autoneg, sessionHandle->xml);
    }

    return status;
}

int ct_libnet_set_duplex_to_config(const char *port, const char *duplex, libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != duplex);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    if( (0 != strcmp("half", duplex)) &&
        (0 != strcmp("full", duplex)))
    {
        // No need to __setWBMErrorState: can only happen for console/C user 
        status = INVALID_PARAMETER;
    } 

    if(SUCCESS == status)
    {
        char *xpathTemplate = "//ethernet_settings[port_name='%s']/duplex";
        char xpath[strlen(xpathTemplate) + strlen(port) + 1];
        sprintf(xpath, xpathTemplate, port);
    
        status = ct_xml_set_value(xpath, duplex, sessionHandle->xml);
    }

    return status;
}

int ct_libnet_commit_changes_all(libnetSession_t *sessionHandle)
{
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    if(SUCCESS == status)
    {
        status = __validateChanges(NULL, sessionHandle);
    }

    if(SUCCESS == status)
    {
       status = ct_xml_commit_changes(sessionHandle->xml); 
    }

    if(SUCCESS == status)
    {
       status = __execPostNetconfigScripts("");
    }

    return status;
}

int ct_libnet_commit_changes_eth(const char *port, libnetSession_t *sessionHandle)
{
    assert(NULL != port);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    char dev[IFNAMSIZ + 1];
    dev[0] = '\0';

    status = __port2dev_ethernet(port, dev, sizeof(dev), sessionHandle);
    
    if(SUCCESS == status)
    {
        status = __validateChanges(dev, sessionHandle);
    }

    if(SUCCESS == status)
    {
       status = ct_xml_commit_changes(sessionHandle->xml);
    }

    if(SUCCESS == status)
    {
       status = __execPostNetconfigScripts(dev);
    }

    return status;
}

int ct_libnet_commit_changes_ip(const char *port, libnetSession_t *sessionHandle)
{
    int status = SUCCESS;

    assert(NULL != port);
    assert(NULL != sessionHandle);

    char dev[IFNAMSIZ + 1];
    dev[0] = '\0';

    status = __port2dev_ip(port, dev, sizeof(dev), sessionHandle);
    
    if(SUCCESS == status)
    {
        status = __validateChanges(dev, sessionHandle);
    }

    if(SUCCESS == status)
    {
       status = ct_xml_commit_changes(sessionHandle->xml);
    }

    if(SUCCESS == status)
    {
       status = __execPostNetconfigScripts(dev);
    }

    return status;
}

int ct_libnet_save_xml_file(libnetSession_t *sessionHandle)
{
    int status;

    assert(NULL != sessionHandle);

    status = ct_xml_commit_xml_changes(sessionHandle->xml);

    return status;
}

int ct_generate_network_config(const char *outFilename, libnetSession_t *sessionHandle)
{
    assert(NULL != sessionHandle);      

    return ct_xml_generate_network_config(outFilename, sessionHandle->xml);
}

int ct_libnet_calculate_broadcast(const char *ipAddr,
                                  const char *netmask, 
                                  char *broadcast,
                                  size_t broadcastLen)
{
    int status = SUCCESS;

    assert(NULL != ipAddr);
    assert(NULL != netmask);
    assert(NULL != broadcast);
    assert(broadcastLen > strlen("255.255.255.255"));

    char classicNetmask[strlen("255.255.255.255") + 1];

    struct sockaddr_in ipRepr;
    struct sockaddr_in netmaskRepr;
    struct sockaddr_in bcastRepr;

    if(1 == inet_pton(AF_INET, ipAddr, &(ipRepr.sin_addr)))
    {
        status = SUCCESS;
    }
    else
    {
        status = INVALID_PARAMETER;
    }
    
    if(SUCCESS == status)
    {
        status = ct_libnet_netmask_to_classic(netmask, classicNetmask, sizeof(classicNetmask));
    }

    if(SUCCESS == status)
    {
        // netmask_to_classic already checks for validity
        (void) inet_pton(AF_INET, classicNetmask, &(netmaskRepr.sin_addr));
    }

    if(SUCCESS == status)
    {
        bcastRepr.sin_addr.s_addr = ipRepr.sin_addr.s_addr | ~(netmaskRepr.sin_addr.s_addr);

        if(NULL == inet_ntop(AF_INET, &(bcastRepr.sin_addr), broadcast, broadcastLen))
        {
            //  EAFNOSUPPORT - actually impossible (address family not supported)
            status = SYSTEM_CALL_ERROR;
        }
    }

    return status;
}

#ifdef __ENABLE_DSA

bool ct_libnet_swconfig_is(const char* alias)
{
  swconfigSession_t *sessionHandle = NULL;
  int status = ct_swconfig_start_session(SWCONFIG_SWITCH_NAME, &sessionHandle);
  const char *found_alias;

  if(SUCCESS == status)
  {
    found_alias = ct_swconfig_get_switch_alias(sessionHandle);
  }

  if(NULL != sessionHandle)
  {
      ct_swconfig_finish_session(sessionHandle);
  }

  return strcmp(alias, found_alias) == 0;
}

static bool ct_libnet_swconfig_is_unsupported_switch(void)
{
  return ct_libnet_swconfig_is("mv88e6321");
}

bool ct_libnet_swconfig_default_for_unsupported_switch(const char* default_value, char *value, size_t valueLen)
{
  if (ct_libnet_swconfig_is_unsupported_switch())
  {
    g_strlcpy(value, default_value, valueLen);
    return true;
  }
  return false;
}

// Set requested dsa state to hw switch AND configuration file.
int ct_libnet_set_dsa_state(const char *value, libnetSession_t *sessionHandle)
{
    assert(NULL != value);
    assert(NULL != sessionHandle);

    int status = SUCCESS;
    char oldValue[2];
    oldValue[0]='\0';

    if(0 != strcmp("0", value) && 0 != strcmp("1", value))
    {
        // No need to __setWBMErrorState: can only happen for console/C user
        status = INVALID_PARAMETER;
    }

    if(SUCCESS == status)
    {
        // Get current dsa state of switch hw.
        // Note: We prefer dsa state of switch hw over configuration file settings.
        status = ct_netlink_get_dsa_state(oldValue, sizeof(oldValue));
    }

    // Only change switch hw if old and new dsa state differ.
    if(SUCCESS == status && 0 != strcmp(value, oldValue))
    {
        status = ct_netlink_set_dsa_state(value);
    }

    if(SUCCESS == status)
    {
        status = ct_xml_set_value("//interfaces/dsa_mode", value, sessionHandle->xml);
    }

    if(SUCCESS == status)
    {
        //FIXME: this change to XML file is configuration dependent and
        // has to be reworked: c program should have no knowledge about configuration
        // details!
        status = ct_xml_set_value("/interfaces/iface/ip_settings[port_name='X2']/show_in_wbm",
                                  value,
                                  sessionHandle->xml);
    }

    if(SUCCESS == status)
    {
        status = ct_xml_commit_xml_changes(sessionHandle->xml);
    }

    // Restore old state in error case
    if(SUCCESS != status && (0 == strcmp("0", oldValue) || 0 == strcmp("1", oldValue)) )
    {
        (void) ct_netlink_set_dsa_state(oldValue);
    }

    return status;
}


// Read dsa state from configuration file.
int ct_libnet_get_dsa_state(char *value, size_t valueLen, libnetSession_t *sessionHandle)
{
    assert(NULL != value);
    assert(NULL != sessionHandle);

    int status = SUCCESS;
    status = ct_xml_get_value("/interfaces/dsa_mode",
                              value,
                              valueLen,
                              sessionHandle->xml);

    return status;
}


// Read current dsa state of hw switch.
int ct_libnet_get_actual_dsa_state(char * const value, size_t const valueLen)
{
    assert(NULL != value);
    assert(1 < valueLen);

    int status = ct_netlink_get_dsa_state(value, valueLen);

    return status;
}


static int __swconfig_set_attr(const char *dev, const char *attr, const char *value, const char *valid_arg_list[])
{
    assert(NULL != value);

    int status = SUCCESS;

    swconfigSession_t *sessionHandle = NULL;

    status = isValidArg(value, valid_arg_list);

    if(SUCCESS == status)
    {
        status = ct_swconfig_start_session(dev, &sessionHandle);
    }

    if(SUCCESS == status)
    {
        status = ct_swconfig_set_attr(attr, value, -1, sessionHandle);
    }

    if(NULL != sessionHandle)
    {
        ct_swconfig_finish_session(sessionHandle);
    }

    return status;
}

static int __swconfig_get_attr(const char *dev, const char *attr, char *value, size_t valueLen)
{
    int status = SUCCESS;

    swconfigSession_t *sessionHandle = NULL;
    
    status = ct_swconfig_start_session(dev, &sessionHandle);

    if(SUCCESS == status)
    {
        status = ct_swconfig_get_attr(attr, value, valueLen, sessionHandle);
    }

    if(NULL != sessionHandle)
    {
        ct_swconfig_finish_session(sessionHandle);
    }

    return status;
}

#define BCAST_DEFAULT "0"
static const char *bcast_protect_arg_list[] = {"0","1","2","3","4","5",NULL};

int ct_libnet_set_bcast_protect(const char *dev, const char *value)
{
    if (ct_libnet_swconfig_is_unsupported_switch())
      return strcmp(BCAST_DEFAULT, value);
    return __swconfig_set_attr(dev, "bcast_protect", value, bcast_protect_arg_list);
}

void ct_libnet_print_bcast_protect_args(void)
{
    printValidArgs(bcast_protect_arg_list);
}

int ct_libnet_get_bcast_protect(const char *dev, char *value, size_t valueLen)
{
    assert(NULL != value);
    assert(valueLen > 2);

    if (ct_libnet_swconfig_default_for_unsupported_switch(BCAST_DEFAULT, value, valueLen))
      return 0;

    return __swconfig_get_attr(dev, "bcast_protect", value, valueLen);
}


static int ct_libnet_is_pfc100(void)
{
    int retVal = 0;
    tTypeLabel typelabel;

    typelabel = typelabel_OBJECT_New();
    if(TYPELABEL_RETURN_OK == typelabel_OBJECT_Open(typelabel))
    {
        typelabel_OBJECT_Sync(typelabel);
        char *orderNr=typelabel_OBJECT_GetValue(typelabel,"ORDER");
        if (orderNr != NULL)
        {
            //printf("Get OrderNumber: %s\n", orderNr);

            //Compare only the beginning of the order-number
            //all PFC100 start with "750-81"
            if (strncmp(orderNr, "750-81xx", 6) == 0)
            {
                //printf("PFC100 Series\n");
                retVal = 1;
            }
            //else
            //{
            //    printf("PFCXXX Series\n");
            //}

            free(orderNr);
        }
        typelabel_OBJECT_Destroy(typelabel);
    }
    return retVal;
}

#define PORT_MIRROR_DEFAULT "0"
static const char *port_mirror_arg_list[] = {"0","1","2", NULL};

int ct_libnet_set_port_mirror(const char *dev, const char *value)
{
    //A workaround is needed here because on PFC100 the ports X1 and X2
    //are crosswise connected as on PFC200
    //---BEGIN WORKAROUND---
    // If hostname starts with "PFC100" we have to swap the values 1 and 2
    int iValue = atoi(value);
    char newValue[2];
    newValue[0]='0';
    newValue[1]='\0';

    if (ct_libnet_swconfig_is_unsupported_switch())
      return strcmp(PORT_MIRROR_DEFAULT, value);

    if (ct_libnet_is_pfc100())
    {
        //printf("WORKAROUND FOR PFC100 - VALUE: %s - SETTO:", value);
        switch (iValue)
        {
         case 1:
          newValue[0]='2';
          break;
         case 2:
          newValue[0]='1';
          break;
         default:
          break;
        }
        //printf("%s\n", newValue);
        return __swconfig_set_attr(dev, "port_mirror", (const char*) newValue, port_mirror_arg_list);
    }
    //---END WORKAROUND---


    return __swconfig_set_attr(dev, "port_mirror", value, port_mirror_arg_list);
}

void ct_libnet_print_port_mirror_args(void)
{
    printValidArgs(port_mirror_arg_list);
}

int ct_libnet_get_port_mirror(const char *dev, char *value, size_t valueLen)
{
    assert(NULL != value);
    assert(valueLen > 1);

    if (ct_libnet_swconfig_default_for_unsupported_switch(PORT_MIRROR_DEFAULT,
                                                          value, valueLen))
      return 0;

    //Original has to be commented out!
    //return __swconfig_get_attr(dev, "port_mirror", value, valueLen);
    //---BEGIN WORKAROUND---
    int status = __swconfig_get_attr(dev, "port_mirror", value, valueLen);

    if (SUCCESS == status)
    {
        if (ct_libnet_is_pfc100())
        {
            //If value is "0" we have to do nothing
            if (strncmp(value, "0",1) != 0)
            {
                if (strncmp(value, "1", 1) == 0)
                {
                    g_strlcpy(value, "2", 2);
                }
                else if (strncmp(value, "2", 1) == 0)
                {
                    g_strlcpy(value, "1", 2);
                }
                else
                {
                    status = ERROR;
                }
            }
        }
    }

    return status;
    //---END WORKAROUND---

}

#define RATE_LIMIT_DEFAULT_GET "disabled"
#define RATE_LIMIT_DEFAULT_SET "off"
static const char *rate_limit_arg_list[] = {    "off",
                                                "64.kbps", "128.kbps", "192.kbps", "256.kbps", "320.kbps", "384.kbps", "448.kbps", "512.kbps", 
                                                "576.kbps", "640.kbps", "704.kbps", "768.kbps", "832.kbps", "896.kbps", "960.kbps", 
                                                "1.mbps", "2.mbps", "3.mbps", "4.mbps", "5.mbps", "6.mbps","7.mbps", "8.mbps", "9.mbps", "10.mbps", 
                                                "11.mbps", "12.mbps", "13.mbps", "14.mbps", "15.mbps", "16.mbps", "17.mbps", "18.mbps", "19.mbps", "20.mbps", 
                                                "21.mbps", "22.mbps", "23.mbps", "24.mbps", "25.mbps", "26.mbps", "27.mbps", "28.mbps", "29.mbps", "30.mbps", 
                                                "31.mbps", "32.mbps", "33.mbps", "34.mbps", "35.mbps", "36.mbps", "37.mbps", "38.mbps", "39.mbps", "40.mbps", 
                                                "41.mbps", "42.mbps", "43.mbps", "44.mbps", "45.mbps", "46.mbps", "47.mbps", "48.mbps", "49.mbps", "50.mbps", 
                                                "51.mbps", "52.mbps", "53.mbps", "54.mbps", "55.mbps", "56.mbps", "57.mbps", "58.mbps", "59.mbps", "60.mbps", 
                                                "61.mbps", "62.mbps", "63.mbps", "64.mbps", "65.mbps", "66.mbps", "67.mbps", "68.mbps", "69.mbps", "70.mbps", 
                                                "71.mbps", "72.mbps", "73.mbps", "74.mbps", "75.mbps", "76.mbps", "77.mbps", "78.mbps", "79.mbps", "80.mbps", 
                                                "81.mbps", "82.mbps", "83.mbps", "84.mbps", "85.mbps", "86.mbps", "87.mbps", "88.mbps", "89.mbps", "90.mbps", 
                                                "91.mbps", "92.mbps", "93.mbps", "94.mbps", "95.mbps", "96.mbps", "97.mbps", "98.mbps", "99.mbps", 

                                                NULL};

int ct_libnet_set_rate_limit(const char *dev, const char *value)
{
    if (ct_libnet_swconfig_is_unsupported_switch())
      return strcmp(RATE_LIMIT_DEFAULT_SET, value);
    
    return __swconfig_set_attr(dev, "rate_limit", value, rate_limit_arg_list);
}

void ct_libnet_print_rate_limit_args(void)
{
    printValidArgs(rate_limit_arg_list);
}

int ct_libnet_get_rate_limit(const char *dev, char *value, size_t valueLen)
{
    assert(NULL != value);
    assert(valueLen > 2);

    if (ct_libnet_swconfig_default_for_unsupported_switch(RATE_LIMIT_DEFAULT_GET, value, valueLen))
      return 0;

    return __swconfig_get_attr(dev, "rate_limit", value, valueLen);
}

#define FAST_AGING_DEFAULT "0"
static const char *fast_aging_arg_list[] = {"0","1", NULL};

void ct_libnet_print_fast_aging_args(void)
{
   printValidArgs(fast_aging_arg_list); 
}

int ct_libnet_set_fast_aging(const char *dev, const char *value)
{
    if (ct_libnet_swconfig_is_unsupported_switch())
        return strcmp(FAST_AGING_DEFAULT, value);
    
    return __swconfig_set_attr(dev, "fast_aging", value, fast_aging_arg_list);
}

int ct_libnet_get_fast_aging(const char *dev, char *value, size_t valueLen)
{
    assert(NULL != value);
    assert(valueLen > 1);

    if (ct_libnet_swconfig_default_for_unsupported_switch(FAST_AGING_DEFAULT, value, valueLen))
      return 0;

    return __swconfig_get_attr(dev, "fast_aging", value, valueLen);
}

int ct_libnet_save_switch_settings_to_config(const char *configFile, ct_switch_settings_t *settings)
{
    // TODO: this function has code copied from ct_xml.c::__reliably_save_to_disk()
    // This should be refactored. AGa, 2014-11-24

    int status = SUCCESS;

    char tmpFilename[strlen(configFile) + strlen(".XXXXXX") + 1];
    snprintf(tmpFilename, sizeof(tmpFilename), "%s.XXXXXX", configFile);

    int tmpFd = mkstemp(tmpFilename);

    if(-1 == tmpFd)
    {
        perror("mkstemp");
        status = FILE_OPEN_ERROR;
    }

    if(SUCCESS == status)
    {
        // the file created by mkstemp has 600 permissions, change it to 644
        if(-1 == fchmod(tmpFd, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH))
        {
            perror("fchmod");
            status = FILE_OPEN_ERROR;
        }
    }

    if(SUCCESS == status)
    {
        const char *configFileContents = "#Auto-generated configuration file. Do not change!\n\nPORT_MIRROR=%s\nBCAST_PROTECT=%s\nRATE_LIMIT=%s\nFAST_AGING=%s\n";

        status = dprintf(tmpFd, 
                         configFileContents, 
                         settings->portMirror, 
                         settings->bcastProtect, 
                         settings->rateLimit, 
                         settings->fastAging);

        if(-1 == status)
        {
            perror("dprintf");
            status = FILE_WRITE_ERROR;
        }
        else
        {
            // status contains number of written bytes
            status = SUCCESS;
        }
    }

    if(SUCCESS == status)
    {
        status = fsync(tmpFd);

        if(-1 == status)
        {
            // possible errnos: EIO: sync error (i.e. not enough free space)
            //                  EROFS, EINVAL: sync not supported (unlikely)
            //                  EBADF: Not a valid writable fd    (impossible, because it has already been written to above)

            perror("fsync");
            status = FILE_WRITE_ERROR; 
        }
    }

    if(-1 != tmpFd)
    {
        if(-1 == close(tmpFd))
        {
        // At this point we cannot do anything, but our config might have been destroyed
        // TODO: Ultima ratio: reset to factory settings?
            perror("close");
            status = FILE_WRITE_ERROR;
        }
    }

    if(SUCCESS == status)
    {
        status = rename(tmpFilename, configFile);
        if(-1 == status)
        {
            perror("rename");
            status = FILE_WRITE_ERROR;
        }
        // rename fails: old config not touched
    }

    if(SUCCESS != status)
    {
        __setWBMErrorState(NULL, 0, "Error saving switch settings.\nChanges will not be restored after reboot.");
    }

    return status;
}


#else

// Provide value '0' if platform has no DSA feature.
int ct_libnet_get_dsa_state(char *value, size_t valueLen, libnetSession_t *sessionHandle)
{
    assert(NULL != value);
    assert(NULL != sessionHandle);
    assert(1 < valueLen);

    value[0] = '0';
    value[1] = '\0';
    return SUCCESS;
}

#endif
