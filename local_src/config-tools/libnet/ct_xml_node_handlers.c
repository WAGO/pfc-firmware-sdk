// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sys/param.h>

#include "ct_xml_node_handlers.h"
#include "ct_netlink.h"
#include "ct_ethtool.h"
#include "../ct_error_handling.h"

static int ct_xml_update_ipaddr(xmlNode *ipAddrNode, void *data);
static int ct_xml_update_mac(xmlNode *macNode, void *data);
static int ct_xml_update_autoneg(xmlNode *autonegNode, void *data);
static int ct_xml_update_speed(xmlNode *speedNode, void *data);
static int ct_xml_update_duplex(xmlNode *duplexNode, void *data);

struct node_update_function
{
    const char * const nodeName;
    int (*update)(xmlNode*, void *);
};

const struct node_update_function node_update_functions[] = 
{
    {"ipaddr", ct_xml_update_ipaddr},
    {"mac", ct_xml_update_mac},
    {"autoneg", ct_xml_update_autoneg},
    {"speed", ct_xml_update_speed},
    {"duplex", ct_xml_update_duplex},

// last element
    {NULL, NULL},
};

static int ct_xml_update_ipaddr(xmlNode *ipAddrNode, void *data)
{
    int ret = SUCCESS;

    assert(NULL != ipAddrNode);
    assert(NULL != data);
    
    char ipaddr[strlen("255.255.255.255/32") + 1];
    ipaddr[0] = '\0';
    
    char *dev = (char*) data;
    
    ret = ct_netlink_get_ipaddr(dev, ipaddr, sizeof(ipaddr));

    // Now cut off the netmask 
    char *delim = strchr(ipaddr, '/');
    if(NULL != delim)
    {
        *delim = '\0';
    }

    if(SUCCESS == ret)
    {
        
        xmlNodeSetContentLen(ipAddrNode->children, (xmlChar*) ipaddr, strlen(ipaddr));
    }

    return ret;
}

static int ct_xml_update_mac(xmlNode *macNode, void *data)
{
    int ret = SUCCESS;

    assert(NULL != macNode);
    assert(NULL != data);
    
    char mac[strlen("ff:ff:ff:ff:ff:ff") + 1];
    mac[0] = '\0';
    
    char *dev = (char*) data;
    
    ret = ct_netlink_get_macaddr(dev, mac, sizeof(mac));
        
    if(SUCCESS == ret)
    {
        
        xmlNodeSetContentLen(macNode->children, (xmlChar*) mac, strlen(mac));
    }

    return ret;
}

static int ct_xml_update_autoneg(xmlNode *autonegNode, void *data)
{
    int ret = SUCCESS;

    assert(NULL != autonegNode);
    assert(NULL != data);
    
    char autoneg[MAX(strlen("disabled"),strlen("enabled")) + 1];
    autoneg[0] = '\0';
    
    // TODO: currently, father iface's node name is used as dev name!
    // Use port's dev attribute here?
    char *dev = (char*) data;
    
    ret = ct_ethtool_get_autoneg(dev, autoneg, sizeof(autoneg));
        
    if(SUCCESS == ret)
    {
        
        xmlNodeSetContentLen(autonegNode->children, (xmlChar*) autoneg, strlen(autoneg));
    }

    return ret;
}

static int ct_xml_update_duplex(xmlNode *duplexNode, void *data)
{
    int ret = SUCCESS;

    assert(NULL != duplexNode);
    assert(NULL != data);
    
    char duplex[MAX(strlen("half"), strlen("full")) + 1];
    duplex[0] = '\0';
    
    // TODO: currently, father iface's node name is used as dev name!
    // Use port's dev attribute here?
    char *dev = (char*) data;
    
    ret = ct_ethtool_get_duplex(dev, duplex, sizeof(duplex));
        
    if(SUCCESS == ret)
    {
        
        xmlNodeSetContentLen(duplexNode->children, (xmlChar*) duplex, strlen(duplex));
    }

    return ret;
}

static int ct_xml_update_speed(xmlNode *speedNode, void *data)
{
    int ret = SUCCESS;

    assert(NULL != speedNode);
    assert(NULL != data);
    
    char speed[strlen("100000") + 1];
    speed[0] = '\0';
    
    // TODO: currently, father iface's node name is used as dev name!
    // Use port's dev attribute here?
    char *dev = (char*) data;
    
    ret = ct_ethtool_get_speed(dev, speed, sizeof(speed));
        
    if(SUCCESS == ret)
    {
        
        xmlNodeSetContentLen(speedNode->children, (xmlChar*) speed, strlen(speed));
    }

    return ret;
}

int ct_xml_apply_update_function(xmlNode *curNode, void *data)
{
    int ret = NOT_FOUND; 

    assert(NULL != curNode);

    int cnt = 0;
    while(NULL != node_update_functions[cnt].nodeName)
    {
        if(0 == xmlStrcmp((xmlChar*) node_update_functions[cnt].nodeName, curNode->name))
        {
            ret = node_update_functions[cnt].update(curNode, data);
            break;
        }
        ++cnt;
    }

    return ret;
}
