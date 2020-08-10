// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#ifndef __CT_NETLINK_H__
#define __CT_NETLINK_H__

#include <stdlib.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

typedef struct netlinkSession netlinkSession_t;

int ct_netlink_init(netlinkSession_t **pSessionHandle);
void ct_netlink_cleanup(netlinkSession_t *sessionHandle);

int ct_netlink_get_link_flags(const char *devStr,
                              unsigned int *flags,
							  netlinkSession_t *sessionHandle);

int ct_netlink_get_ipaddr(const char *devStr,
                          char *strIp,
                          size_t strIpLen,
						  netlinkSession_t *sessionHandle);

int ct_netlink_get_macaddr(const char *devStr,
                           char *strMac,
                           size_t strMacLen,
  						   netlinkSession_t *sessionHandle);

int ct_netlink_get_broadcast(const char *devStr,
                             char *strBroadcast,
                             size_t strBroadcastLen,
						     netlinkSession_t *sessionHandle);

int ct_netlink_get_default_via(const char *devStr,
                               char *strDefaultVia,
                               size_t strDefaultViaLen,
	 						   netlinkSession_t *sessionHandle);

int ct_netlink_enable_link(const char *devStr,
						   netlinkSession_t *sessionHandle);
int ct_netlink_disable_link(const char *devStr,
						    netlinkSession_t *sessionHandle);

#ifdef __ENABLE_DSA
int ct_netlink_set_dsa_state(const char *value);
int ct_netlink_get_dsa_state(char * const szValue,
                             size_t const valueLen);
#endif

__END_DECLS

#endif
