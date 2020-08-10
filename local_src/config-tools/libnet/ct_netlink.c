// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#include "ct_netlink.h"

#include <netlink/netlink.h>
#include <netlink/cache.h>
#include <netlink/route/addr.h>
#include <netlink/route/route.h>

#include <linux/switch.h>

#include <glib.h>

#include <assert.h>

#ifdef __ENABLE_DSA
#include <swlib.h>
#endif

#include "../ct_error_handling.h"

struct netlinkSession
{
	struct nl_sock *sock;
	struct nl_cache *link_cache;
	struct nl_cache *addr_cache;
};

/**
 * @brief
 *
 * @return success/failure
 *         Possible errors: Not enough memory
 *                          Connection error
 *
 */
int ct_netlink_init(netlinkSession_t **pSessionHandle)
{
  int ret = SUCCESS;

	assert(NULL != pSessionHandle);

  *pSessionHandle = g_malloc0(sizeof(netlinkSession_t));

  netlinkSession_t *ressources = *pSessionHandle;

  if(NULL == ressources)
  {
      ret = SYSTEM_CALL_ERROR;
	}
	else
	{

		if(NULL == (ressources->sock = nl_socket_alloc()))
		{
			ret = SYSTEM_CALL_ERROR; //TODO: alloc error
		}
		else
		{
			ret = nl_connect(ressources->sock, NETLINK_ROUTE);
		}

		if(0 == ret)
		{
			ret = rtnl_addr_alloc_cache(ressources->sock, &(ressources->addr_cache));
		}

		if(0 == ret)
		{
			ret = rtnl_link_alloc_cache(ressources->sock, AF_UNSPEC, &(ressources->link_cache));
		}
	}

	return (0 == ret) ? SUCCESS : SYSTEM_CALL_ERROR;
}


/**
 * @brief cleanup acquired internal ressources: a libnl socket, address and link caches.
 */

void ct_netlink_cleanup(netlinkSession_t *sessionHandle)
{
	if(NULL != sessionHandle)
	{

	    if(sessionHandle->link_cache) nl_cache_free(sessionHandle->link_cache);

    	if(sessionHandle->addr_cache) nl_cache_free(sessionHandle->addr_cache);

	    if(sessionHandle->sock) nl_socket_free(sessionHandle->sock);

	    g_free(sessionHandle);
	}
}

// FIXME: Temporary workaround for libnl header mismatch
extern struct nl_object *nl_cache_find(struct nl_cache *cache, struct nl_object *filter);

/**
 * @brief return the first IP address for a given NIC device name (i.e. eth0).
 *
 * NOTE: following KISS principle, no multiple addresses are supported yet
 *
 * @param dev_str         device name string (i.e. eth0, eth1 etc.)
 * @param str_ipaddr      result buffer for the IP address/netmask in string form (i.e. 10.11.12.13/24)
 * @param str_ipaddr_len  length of the result buffer
 *
 * @return success/failure
 *         Possible errors: Not enough memory
 *                          Unknown device
 *                          Buffer too small for the result (FIXME: really?)
 */
//FIXME: always returns 0!!
int ct_netlink_get_ipaddr(const char *devStr,
                          char *strIp,
                          size_t strIpLen,
						  netlinkSession_t *sessionHandle)
{
	assert(NULL != sessionHandle);

	assert(NULL != sessionHandle->sock);
    assert(NULL != sessionHandle->link_cache);
    assert(NULL != sessionHandle->addr_cache);

    assert(NULL != devStr);

    assert(NULL != strIp);
    assert(0 == strlen(strIp));
    assert(strIpLen >= sizeof("255.255.255.255/32"));

    int status = 0;
    struct rtnl_addr *filter = NULL;
    int ival = -1;
    struct nl_object *addr = NULL;

    do{

        if(NULL == (filter = rtnl_addr_alloc()))
        {
            break;
        }

        if((ival = rtnl_link_name2i(sessionHandle->link_cache, devStr)) < 0)
        {
            break;
        }

        rtnl_addr_set_ifindex(filter, ival);

        if(NULL == (addr = nl_cache_find(sessionHandle->addr_cache, OBJ_CAST(filter))))
        {
            break;
        }

// TODO: to support multiple addresses, use foreach_filter/cache_subset

        (void) nl_addr2str(rtnl_addr_get_local((struct rtnl_addr*) addr), strIp, strIpLen);

        nl_object_put(addr);

    }
    while(0);

    if(filter)
    {
        rtnl_addr_put(filter);
    }

    return status;
}

int ct_netlink_get_macaddr(const char *devStr,
                           char *strMac,
                           size_t strMacLen,
						  netlinkSession_t *sessionHandle)
{
	assert(NULL != sessionHandle);

	assert(NULL != sessionHandle->sock);
    assert(NULL != sessionHandle->link_cache);
    assert(NULL != sessionHandle->addr_cache);

    struct rtnl_link *filter = NULL;
    int ival = -1;
    struct nl_object *link = NULL;

    int status = SUCCESS;

    do
    {
        if(NULL == (filter = rtnl_link_alloc()))
        {
            break;
        }

        if((ival = rtnl_link_name2i(sessionHandle->link_cache, devStr)) < 0)
        {
            break;
        }

        rtnl_link_set_ifindex(filter, ival);

        if(NULL == (link = nl_cache_find(sessionHandle->link_cache, OBJ_CAST(filter))))
        {
            break;
        }

        // TODO: to support multiple addresses, use foreach_filter/cache_subset

        (void) nl_addr2str( rtnl_link_get_addr((struct rtnl_link*) link),
                            strMac,
                            strMacLen);

        nl_object_put(link);
    }
    while(0);

    if(filter)
    {
        rtnl_link_put(filter);
    }

    return status;
}

//TODO: refactor: get_broadcast is very similar to get_ipaddr
int ct_netlink_get_broadcast(const char *devStr,
                             char *strBroadcast,
                             size_t strBroadcastLen,
						  	 netlinkSession_t *sessionHandle)
{
	assert(NULL != sessionHandle);

	assert(NULL != sessionHandle->sock);
    assert(NULL != sessionHandle->link_cache);
    assert(NULL != sessionHandle->addr_cache);

    struct rtnl_link *filter = NULL;
    int ival = -1;
    struct nl_object *addr = NULL;

    int status = SUCCESS;

    do
    {
        if(NULL == (filter = rtnl_link_alloc()))
        {
            break;
        }

        if((ival = rtnl_link_name2i(sessionHandle->link_cache, devStr)) < 0)
        {
            break;
        }

        rtnl_link_set_ifindex(filter, ival);

        if(NULL == (addr = nl_cache_find(sessionHandle->addr_cache, OBJ_CAST(filter))))
        {
            break;
        }

        // TODO: to support multiple addresses, use foreach_filter/cache_subset

        (void) nl_addr2str( rtnl_addr_get_broadcast((struct rtnl_addr*) addr),
                            strBroadcast,
                            strBroadcastLen);

        nl_object_put(addr);
    }
    while(0);

    if(filter)
    {
        rtnl_link_put(filter);
    }

    return status;
}

int ct_netlink_get_default_via(const char *devStr,
                               char *strDefaultVia,
                               size_t strDefaultViaLen,
							   netlinkSession_t *sessionHandle)
{
	assert(NULL != devStr);
    assert(NULL != strDefaultVia);
    assert(strDefaultViaLen > strlen("255.255.255.255"));

	assert(NULL != sessionHandle);

    assert(NULL != sessionHandle->sock);
    assert(NULL != sessionHandle->link_cache);
    assert(NULL != sessionHandle->addr_cache);

    int status = SUCCESS;

    struct nl_cache *route_cache = NULL;
    struct rtnl_route *filter = NULL;
    struct nl_object *viaObj = NULL;
    struct rtnl_route *via = NULL;

    do
    {

        if(SUCCESS != (status = rtnl_route_alloc_cache(sessionHandle->sock, AF_UNSPEC, 0, &route_cache)))
        {
            break;
        }

        if(NULL == (filter = rtnl_route_alloc()))
        {
            break;
        }

        rtnl_route_set_family(filter, AF_INET);
        rtnl_route_set_scope(filter, RT_SCOPE_UNIVERSE);
        rtnl_route_set_table(filter, RT_TABLE_MAIN);

        if(NULL == (viaObj = nl_cache_find(route_cache, OBJ_CAST(filter))))
        {
            break;
        }

        via = (struct rtnl_route*) viaObj;

        // The default via address is stored in the first nexthop structure in the static route description
        if(1 == rtnl_route_get_nnexthops((via)))
        {
            struct rtnl_nexthop *nexthop = rtnl_route_nexthop_n(via, 0);

            (void) nl_addr2str( rtnl_route_nh_get_gateway(nexthop),
                                strDefaultVia,
                                strDefaultViaLen);


        }

    }
    while(0);

    if(filter)
    {
        rtnl_route_put(filter);
    }
    if(viaObj)
    {
        nl_object_put(viaObj);
    }

    if(route_cache)
    {
        nl_cache_free(route_cache);
    }

    return status;
}

int ct_netlink_get_link_flags(const char *devStr,
                              unsigned int *flags,
							  netlinkSession_t *sessionHandle)
{
    assert(NULL != devStr);
    assert(NULL != flags);

	assert(NULL != sessionHandle);

    assert(NULL != sessionHandle->sock);
    assert(NULL != sessionHandle->link_cache);

    int status = SUCCESS;
    int ival = -1;

    struct rtnl_link *filter = NULL;
    struct nl_object *link = NULL;

    do
    {
        if(NULL == (filter = rtnl_link_alloc()))
        {
            break;
        }

        if((ival = rtnl_link_name2i(sessionHandle->link_cache, devStr)) < 0)
        {
            break;
        }

        rtnl_link_set_ifindex(filter, ival);

        if(NULL == (link = nl_cache_find(sessionHandle->link_cache, OBJ_CAST(filter))))
        {
            break;
        }

        *flags = rtnl_link_get_flags((struct rtnl_link*) link);
    }
    while(0);

    if(filter)
    {
        rtnl_link_put(filter);
    }
    if(link)
    {
        nl_object_put(link);
    }

    return status;
}

enum ct_netlink_link_states
{
    CT_NL_LINK_STATE_ENABLED,
    CT_NL_LINK_STATE_DISABLED,
};

static int __set_link_state(const char *devStr, enum ct_netlink_link_states state, netlinkSession_t *sessionHandle)
{
    assert(NULL != devStr);

	assert(NULL != sessionHandle);

    assert(NULL != sessionHandle->sock);
    assert(NULL != sessionHandle->link_cache);

    int status = SUCCESS;
    int ival = -1;

    struct rtnl_link *filter = NULL;
    struct nl_object *link = NULL;
    struct rtnl_link *changes = NULL;

    do
    {
        if(NULL == (changes = rtnl_link_alloc()))
        {
            break;
        }

        if(NULL == (filter = rtnl_link_alloc()))
        {
            break;
        }

        if((ival = rtnl_link_name2i(sessionHandle->link_cache, devStr)) < 0)
        {
            break;
        }

        rtnl_link_set_ifindex(filter, ival);

        if(NULL == (link = nl_cache_find(sessionHandle->link_cache, OBJ_CAST(filter))))
        {
            break;
        }

       switch(state)
        {
        case CT_NL_LINK_STATE_DISABLED:
             rtnl_link_unset_flags(changes, IFF_UP);
//           rtnl_link_set_operstate(changes, IF_OPER_DOWN);
           break;
        case CT_NL_LINK_STATE_ENABLED:
             rtnl_link_set_flags(changes, IFF_UP);
//            rtnl_link_set_operstate(changes, IF_OPER_UP);
            break;
        default:
            status = INVALID_PARAMETER;
            break;
        }

        if(SUCCESS == status)
        {
            status = rtnl_link_change(sessionHandle->sock, (struct rtnl_link*) link, changes, 0);
        }

        if(0 != status)
        {
            //TODO: generalize logging
            printf("rtnl_link_change: %s\n", nl_geterror(status));
            status = SYSTEM_CALL_ERROR;
        }

    }
    while(0);

    if(filter)
    {
        rtnl_link_put(filter);
    }
    if(link)
    {
        nl_object_put(link);
    }
    if(changes)
    {
        rtnl_link_put(changes);
    }

    return status;
}

int ct_netlink_enable_link(const char *devStr, netlinkSession_t *sessionHandle)
{
    assert(NULL != devStr);

	assert(NULL != sessionHandle);

    assert(NULL != sessionHandle->sock);
    assert(NULL != sessionHandle->link_cache);

    return __set_link_state(devStr, CT_NL_LINK_STATE_ENABLED, sessionHandle);
}

int ct_netlink_disable_link(const char *devStr, netlinkSession_t *sessionHandle)
{
    assert(NULL != devStr);

	assert(NULL != sessionHandle);

    assert(NULL != sessionHandle->sock);
    assert(NULL != sessionHandle->link_cache);

    return __set_link_state(devStr, CT_NL_LINK_STATE_DISABLED, sessionHandle);
}

#ifdef __ENABLE_DSA
int ct_netlink_set_dsa_state(const char *value)
{
    int status = SUCCESS;

    struct switch_dev *dev = NULL;
    struct switch_attr *a = NULL;

    dev = swlib_connect("switch0");

    if(NULL == dev)
    {
        status = SYSTEM_CALL_ERROR;
    }


    if(SUCCESS == status)
    {
        swlib_scan(dev);
        a = swlib_lookup_attr(dev, SWLIB_ATTR_GROUP_GLOBAL, "dsa_enable");

        if(NULL == a)
        {
            status = SYSTEM_CALL_ERROR;
        }
    }

    if(SUCCESS == status)
    {
        if(0 > swlib_set_attr_string(dev, a, -1, value))
        {
            status = SYSTEM_CALL_ERROR;
        }
    }

   if(NULL != dev)
   {
       swlib_free_all(dev);
   }

    return status;
}


int ct_netlink_get_dsa_state(char * const szValue,
                             size_t const valueLen)
{
    int status = SUCCESS;

    struct switch_dev *dev = NULL;
    struct switch_attr *attr = NULL;

    dev = swlib_connect("switch0");

    if(NULL == dev)
    {
        status = SYSTEM_CALL_ERROR;
    }

    if( 2 > valueLen)
    {
        status = SYSTEM_CALL_ERROR;
    }

    szValue[0] = '\0';

    if(SUCCESS == status)
    {
        swlib_scan(dev);
        attr = swlib_lookup_attr(dev, SWLIB_ATTR_GROUP_GLOBAL, "dsa_enable");

        if(NULL == attr)
        {
            status = SYSTEM_CALL_ERROR;
        }
    }

    if(SUCCESS == status)
    {
        struct switch_val val;
        if(0 > swlib_get_attr(dev, attr, &val))
        {
            status = SYSTEM_CALL_ERROR;
        }
        else
        {
            if(attr->type == SWITCH_TYPE_INT)
            {
                if(0 == val.value.i)
                {
                    szValue[0] = '0';
                    szValue[1] = '\0';
                }
                else if(1 == val.value.i)
                {
                    szValue[0] = '1';
                    szValue[1] = '\0';
                }
                else
                {
                    status = SYSTEM_CALL_ERROR;
                }
            }
            else
            {
              status = SYSTEM_CALL_ERROR;
            }
        }
    }

    if(NULL != dev)
    {
       swlib_free_all(dev);
    }

    return status;
}
#endif
