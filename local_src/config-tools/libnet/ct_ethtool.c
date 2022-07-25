// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include <sys/ioctl.h>

#include <net/if.h>

#include <linux/types.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>


#include "ct_ethtool.h"

#include "../ct_error_handling.h"

enum param
{
    AUTONEG = 0,
    SPEED,
    DUPLEX,
}; 

static int ct_ethtool_init(int *pEthtoolSocket);
static int ct_ethtool_get_param(enum param paramType, const char *dev, char *result, size_t resultLen);
static void ct_ethtool_cleanup(int ethtoolSocket);

static int __validate_autoneg(enum ct_ethtoolValues tVal, unsigned int *pVal);
static int __validate_speed(enum ct_ethtoolValues tVal, unsigned int *pVal);
static int __validate_duplex(enum ct_ethtoolValues tVal, unsigned int *pVal);

static int ct_ethtool_init(int *pEthtoolSocket)
{
    assert(NULL != pEthtoolSocket);

    int status = SUCCESS;

    *pEthtoolSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if (*pEthtoolSocket < 0) {
        status = FILE_OPEN_ERROR;
    }

    return status;
}

static void ct_ethtool_cleanup(int ethtoolSocket)
{
    if(ethtoolSocket > 0)
    {
        close(ethtoolSocket);
    }
}

static int ct_ethtool_get_param(enum param paramType, const char *dev, char *result, size_t resultLen)
{
    assert(NULL != dev);
    assert(NULL != result);

    int status = SUCCESS;


    int ethtoolSocket = 0;
    struct ifreq ifr;
    struct ethtool_cmd ecmd;

    if(SUCCESS == (status = ct_ethtool_init(&ethtoolSocket)) )
    {

        memset(&ifr, 0, sizeof(ifr));
        strcpy(ifr.ifr_name, dev);

        ecmd.cmd = ETHTOOL_GSET;
        ifr.ifr_data = (caddr_t)&ecmd;
        // FIXME: error value!
        status = ioctl(ethtoolSocket, SIOCETHTOOL, &ifr); 
        
    }

    if(SUCCESS == status)
    {

        switch(paramType)
        {
        case AUTONEG:

            snprintf(result, resultLen, "%s", ecmd.autoneg ? "enabled" : "disabled");
            result[resultLen - 1] = '\0';
            break;

        case SPEED:

            snprintf(result, resultLen, "%d", ethtool_cmd_speed(&ecmd));
            result[resultLen - 1] = '\0';
            break;

        case DUPLEX:

            snprintf(result, resultLen, "%s", ecmd.duplex ? "full" : "half");
            result[resultLen - 1] = '\0';
            break;
        default:
            break;
        }

    }

    ct_ethtool_cleanup(ethtoolSocket);

    return status;
}


int ct_ethtool_set_port_params(const char *dev,
                               enum ct_ethtoolValues tAutoneg,
                               enum ct_ethtoolValues tSpeed,
                               enum ct_ethtoolValues tDuplex)
{
    assert(NULL != dev);

    int status = SUCCESS;

    int ethtoolSocket = 0;
    struct ifreq ifr;
    struct ethtool_cmd ecmd;

    unsigned int autoneg = 0;
    unsigned int speed   = 0;
    unsigned int duplex  = 0;

    status =  __validate_autoneg(tAutoneg, &autoneg);

    if(SUCCESS == status)
    {
        status =  __validate_speed(tSpeed, &speed);
    }

    if(SUCCESS == status)
    {
        status =  __validate_duplex(tDuplex, &duplex);
    }

    if(SUCCESS == status)
    {
        status = ct_ethtool_init(&ethtoolSocket);
    }

    if(SUCCESS == status)
    {

        // First, read current values
        memset(&ifr, 0, sizeof(ifr));
        strcpy(ifr.ifr_name, dev);

        ecmd.cmd = ETHTOOL_GSET;
        ifr.ifr_data = (caddr_t)&ecmd;
        // FIXME: error value!
        status = ioctl(ethtoolSocket, SIOCETHTOOL, &ifr); 
        
    }

    if(SUCCESS == status)
    {
        ecmd.autoneg = autoneg;
        ethtool_cmd_speed_set(&ecmd, speed);
        ecmd.duplex = duplex;

        ecmd.cmd = ETHTOOL_SSET;
        ifr.ifr_data = (caddr_t)&ecmd;
        // FIXME: error value!
        status = ioctl(ethtoolSocket, SIOCETHTOOL, &ifr); 
    }

    ct_ethtool_cleanup(ethtoolSocket);

    return status;
}



int ct_ethtool_get_autoneg(const char *dev, char *result, size_t resultLen)
{
    assert(NULL != dev);
    assert(NULL != result);
    assert(resultLen > strlen("disabled"));

    return ct_ethtool_get_param(AUTONEG, dev, result, resultLen);
}

int ct_ethtool_get_speed(const char *dev, char *result, size_t resultLen)
{
    assert(NULL != dev);
    assert(NULL != result);
    assert(resultLen > strlen("10000M"));

    return ct_ethtool_get_param(SPEED, dev, result, resultLen);
}

int ct_ethtool_get_duplex(const char *dev, char *result, size_t resultLen)
{
    assert(NULL != dev);
    assert(NULL != result);
    assert(resultLen > strlen("full"));

    return ct_ethtool_get_param(DUPLEX, dev, result, resultLen);
}

static inline int __validate_autoneg(enum ct_ethtoolValues tVal, unsigned int *pVal)
{
    int status = SUCCESS;

    switch(tVal)
    {
    case CT_ETHTOOL_AUTONEG_ON:
        *pVal = AUTONEG_ENABLE;
        break;

    case CT_ETHTOOL_AUTONEG_OFF:
        *pVal = AUTONEG_DISABLE;
        break;
   
    case CT_ETHTOOL_SPEED_10M:
    case CT_ETHTOOL_SPEED_100M:
    case CT_ETHTOOL_SPEED_1000M:
    case CT_ETHTOOL_DUPLEX_HALF:
    case CT_ETHTOOL_DUPLEX_FULL:

    default:
        status = INVALID_PARAMETER;
        break;
    }
   
    return status;
}

static inline int __validate_speed(enum ct_ethtoolValues tVal, unsigned int *pVal)
{
    int status = SUCCESS;

    switch(tVal)
    {
    case CT_ETHTOOL_SPEED_10M:
        *pVal = SPEED_10;
        break;

    case CT_ETHTOOL_SPEED_100M:
        *pVal = SPEED_100;
        break;

    case CT_ETHTOOL_SPEED_1000M:
        *pVal = SPEED_1000;
        break;

    case CT_ETHTOOL_AUTONEG_ON:
    case CT_ETHTOOL_AUTONEG_OFF:
    case CT_ETHTOOL_DUPLEX_HALF:
    case CT_ETHTOOL_DUPLEX_FULL:

    default:
        status = INVALID_PARAMETER;
        break;
    }
   
    return status;
}

static inline int __validate_duplex(enum ct_ethtoolValues tVal, unsigned int *pVal)
{
    int status = SUCCESS;

    switch(tVal)
    {
    case CT_ETHTOOL_DUPLEX_HALF:
        *pVal = DUPLEX_HALF;
        break;

    case CT_ETHTOOL_DUPLEX_FULL:
        *pVal = DUPLEX_FULL;
        break;

    case CT_ETHTOOL_AUTONEG_ON:
    case CT_ETHTOOL_AUTONEG_OFF:
    case CT_ETHTOOL_SPEED_10M:
    case CT_ETHTOOL_SPEED_100M:
    case CT_ETHTOOL_SPEED_1000M:

    default:
        status = INVALID_PARAMETER;
        break;
    }

    return status;
}

