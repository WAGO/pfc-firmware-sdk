// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#include "ct_swconfig.h"

#include <assert.h>
#include <glib.h>
#include <stdlib.h>

#include <linux/types.h>
#include <linux/netlink.h>
#include <linux/genetlink.h>
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <linux/switch.h>
#include "swlib.h"


#include "../ct_error_handling.h"

struct swconfigSession
{
    struct switch_dev *dev;
};


static struct switch_attr *__swlib_get_attr_ptr(const char *attrName, swconfigSession_t *sessionHandle);


static struct switch_attr *__swlib_get_attr_ptr(const char *attrName, swconfigSession_t *sessionHandle)
{

    swlib_scan(sessionHandle->dev);
    
    // hide enum swlib_attr_group from user by subsequently trying all groups

    struct switch_attr *attr = swlib_lookup_attr(sessionHandle->dev, SWLIB_ATTR_GROUP_GLOBAL, attrName);

    if(NULL == attr)
    {
        attr = swlib_lookup_attr(sessionHandle->dev, SWLIB_ATTR_GROUP_VLAN, attrName);
    }

    if(NULL == attr)
    {
        attr = swlib_lookup_attr(sessionHandle->dev, SWLIB_ATTR_GROUP_PORT, attrName);
    }

    return attr;
}

int ct_swconfig_start_session(const char *switchName, swconfigSession_t **sessionHandle)
{
    assert(NULL != switchName);
    assert(NULL == *sessionHandle);
    
    int status = SUCCESS;

    *sessionHandle = g_malloc(sizeof(swconfigSession_t));

    (*sessionHandle)->dev = swlib_connect(switchName);

    if(NULL == (*sessionHandle)->dev)
    {
        status = SYSTEM_CALL_ERROR;
    }

    return status;
}

const char *ct_swconfig_get_switch_alias(swconfigSession_t *sessionHandle)
{
  assert(NULL != sessionHandle);
  assert(NULL != sessionHandle->dev);

  return sessionHandle->dev->alias;
}

void ct_swconfig_finish_session(swconfigSession_t *sessionHandle)
{
    assert(NULL != sessionHandle);
    
    swlib_free_all(sessionHandle->dev);
    g_free(sessionHandle);
}
// set portNr to -1 if not applicable!
int ct_swconfig_set_attr(const char *attrName, const char *attrValue, int portNr, swconfigSession_t *sessionHandle)
{
    int status = SUCCESS;

    struct switch_attr *attr = __swlib_get_attr_ptr(attrName, sessionHandle);

    if(NULL == attr)
    {
        status = SYSTEM_CALL_ERROR;
    }

    if(SUCCESS == status)
    {
        if(0 != swlib_set_attr_string(sessionHandle->dev, attr, portNr, attrValue))
        {
            status = SYSTEM_CALL_ERROR;
        }

    }

    return status;
}

int ct_swconfig_get_attr(const char *attrName, char *attrValue, size_t attrValueLen, swconfigSession_t *sessionHandle)
{
    assert(NULL != attrName);
    assert(NULL != sessionHandle);

    int status = SUCCESS;

    struct switch_val val;

    struct switch_attr *attr = __swlib_get_attr_ptr(attrName, sessionHandle);

    if(NULL == attr)
    {
        status = INVALID_PARAMETER;
    }

    if(SUCCESS == status)
    {
        if(-1 == swlib_get_attr(sessionHandle->dev, attr, &val))
        {
            status = SYSTEM_CALL_ERROR;
        }
    }

    if(SUCCESS == status)
    {
        switch(attr->type)
        {
	    case SWITCH_TYPE_INT:
            (void) g_snprintf(attrValue, attrValueLen, "%d", val.value.i);
            break;
	    case SWITCH_TYPE_STRING:
            (void) g_snprintf(attrValue, attrValueLen, "%s", val.value.s);
            free((void *)val.value.s);
            break;
        default:
            status = INVALID_PARAMETER;
            break;
        }            
    }

    return status;
}
