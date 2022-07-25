// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#ifndef __CT_ETHTOOL_H__
#define __CT_ETHTOOL_H__

#include <sys/cdefs.h>

__BEGIN_DECLS

int ct_ethtool_get_speed(const char *dev, char *speed, size_t speedLen);
int ct_ethtool_get_duplex(const char *dev, char *duplex, size_t duplexLen);
int ct_ethtool_get_autoneg(const char *dev, char *autoneg, size_t autonegLen);

enum ct_ethtoolValues
{
    CT_ETHTOOL_AUTONEG_ON,
    CT_ETHTOOL_AUTONEG_OFF,

    CT_ETHTOOL_SPEED_10M,
    CT_ETHTOOL_SPEED_100M,
    CT_ETHTOOL_SPEED_1000M,

    CT_ETHTOOL_DUPLEX_FULL,
    CT_ETHTOOL_DUPLEX_HALF,
};

int ct_ethtool_set_port_params(const char *dev,
                               enum ct_ethtoolValues tAutoneg,
                               enum ct_ethtoolValues tSpeed,
                               enum ct_ethtoolValues tDuplex);

__END_DECLS

#endif
