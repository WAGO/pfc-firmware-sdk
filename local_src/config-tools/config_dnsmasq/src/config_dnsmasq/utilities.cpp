//------------------------------------------------------------------------------
/// Copyright (c) 2020 WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \brief    Collection of utility functions.
///
///  \author   MSc : WAGO Kontakttechnik GmbH & Co. KG
///  \author   MOe : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include <cstring>
#include <glib.h>
#include <cctype>              // for isdigit()
#include <cassert>

#include "defines.hpp"
#include "utilities.hpp"
#include "error_handling.hpp"

namespace {

  // Character code to character class. Available classes:
  //   0 = Illegal characters
  //   1 = digits
  //   2 = alphabetic
  //   3 = Hyphen
  //   4 = Label terminating '.'
  //   5 = String terminating '\0'
  static char conv_dnchar[] =
  {
      5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 0,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
      0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0,
      0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };


  // State transition table.
  static conv_state_t conv_transitions[] =
  {
      /*             ill. digit    letter   -       .      \0 */
      /* STOP */     ERR, ERR,     ERR,     ERR,    ERR,   ERR,
      /* START */    ERR, ERR,     INLABEL, ERR,    ERR,   ERR,
      /* INLABEL */  ERR, INLABEL, INLABEL, HYPHEN, START, STOP,
      /* HYPHEN */   ERR, INLABEL, INLABEL, HYPHEN, ERR,   ERR,
  };


  // Convert one ascii character '0' to '9' and 'a' - 'f' to decimal 0 - 15.
  // Upper case letters work as well.
  int hexval(int c) {
      static int8_t ktab[] = {
           0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
          -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
          -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
          -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      };
      int rv = -1;

      c -= 0x30;
      if (c >= 0 && c < 0x40) {
          rv = ktab[c];
      }
      return rv;
  }

  // Converts a string of the form mac_addr-ip_addr,... into an array of structures of type dhcpd_host_t
  // Allocates memory for the array of structures which must be freed by g_free().
  int conv_hosts_ascii2bin_(uint32_t netmask, guint *count, dhcpd_host_t **resultp, char **hostlist) {
    int status = SUCCESS;
    guint hcount = g_strv_length(hostlist);
    dhcpd_host_t *hosts = (dhcpd_host_t*)g_malloc(hcount * sizeof(dhcpd_host_t));

    for (guint index = 0; index < hcount; index++) {
      char *host = hostlist[index];
      dhcpd_host_t *hp = &hosts[index];
      char **parts = g_strsplit(host, "_", 0);
      uint32_t ip;

      if (g_strv_length(parts) != 2) {
        status = INVALID_PARAMETER;
        break;
      }
      if (!conv_is_valid_mac(parts[0]) && !conv_is_valid_domain_name(parts[0])) {
        status = INVALID_PARAMETER;
        break;
      }
      hp->mac = g_strdup(parts[0]);
      status = conv_ip_ascii2bin(parts[1], &ip);
      if (SUCCESS != status) {
        break;
      }
      hp->ip_net_part = ip & netmask;
      hp->ip_host_part = ip & ~netmask;
    }
    if (SUCCESS == status) {
      *count = hcount;
      *resultp = hosts;
    } else {
      *count = 0;
      *resultp = NULL;
      g_free(hosts);
    }
    return status;
  }

}  // Anonymous namespace


// Look up str in an string pointer array like argv.
int strv_is_in_set(char **set, const char *str) {
  int idx = 0;
  bool found = false;
  for (idx = 0; set[idx] != NULL; idx++) {
    if (0 == strcmp(set[idx], str)) {
      found = true;
      break;
    }
  }
  if (!found) {
    idx = -1;
  }
  return idx;
}

// Convert IP-Address from dotted ASCII string to uint32_t.
// Did not use inet_aton() in favour of more strict format checks.
// Set result to NULL for checking only.
int conv_ip_ascii2bin(char *ipaddr, uint32_t *resultp)
{
    int i;
    char **decimalparts = g_strsplit(ipaddr, ".", 0);
    int status = SUCCESS;
    uint32_t result = 0;

    if(4 == g_strv_length(decimalparts))
    {
        for(i = 0; i < 4; i++)
        {
            char *p = decimalparts[i];
            uint32_t part;
            if('\0' != *p)
            {
                while(*p != '\0')
                {
                    if(!isdigit(*p))
                    {
                        status = INVALID_PARAMETER;
                        break;
                    }
                    p++;
                }
                if(SUCCESS != status)
                {
                    break;
                }
                part = (uint32_t)strtoul(decimalparts[i], NULL, 10);
                if(part <= 255)
                {
                    result = (result << 8) + part;
                }
                else
                {
                    status = INVALID_PARAMETER;
                    break;
                }
            }
            else
            {
                status = INVALID_PARAMETER;
                break;
            }
        }
    }
    else
    {
        status = INVALID_PARAMETER;
    }
    if(SUCCESS == status && NULL != resultp)
    {
        *resultp = result;
    }

    g_strfreev(decimalparts);
    return status;
}

// Allocatess memory which the caller has to free with g_free().
int conv_ip_bin2ascii(uint32_t ipaddr, char **resultp) {
  char *result = (char*)g_malloc(SIZEOF_IPADDR);
  unsigned int b1 = (ipaddr >> 24) & 255;
  unsigned int b2 = (ipaddr >> 16) & 255;
  unsigned int b3 = (ipaddr >> 8) & 255;
  unsigned int b4 = ipaddr & 255;
  int len = snprintf(result, SIZEOF_IPADDR, "%u.%u.%u.%u", b1, b2, b3, b4);
  assert(len < SIZEOF_IPADDR);
  *resultp = result;
  return SUCCESS;
}

// Allocs a dhcpd_range_t structure which the caller has to free with g_free().
int conv_range_ascii2bin(char *rangestr, uint32_t netmask, dhcpd_range_t **resultp) {
  int status = SUCCESS;
  uint32_t ip1;
  uint32_t ip2;
  char **ipaddrs = g_strsplit(rangestr, "_", 0);

  if (g_strv_length(ipaddrs) != 2) {
    status = INVALID_PARAMETER;
  }

  if (SUCCESS == status) {
    status = conv_ip_ascii2bin(ipaddrs[0], &ip1);
  }

  if (SUCCESS == status) {
    status = conv_ip_ascii2bin(ipaddrs[1], &ip2);
  }

  g_strfreev(ipaddrs);

  if (SUCCESS == status) {
    if( (ip1 & netmask) != (ip2 & netmask) || ip1 >= ip2) {
      status = INVALID_PARAMETER;
    }
  }

  if (SUCCESS == status) {
    dhcpd_range_t *range = (dhcpd_range_t*)g_malloc(sizeof(dhcpd_range_t));
    range->ip_net_part1 = ip1 & netmask;
    range->ip_net_part2 = ip2 & netmask;
    range->host_part1 = ip1 & ~netmask;
    range->host_part2 = ip2 & ~netmask;
    *resultp = range;
  }

  return status;
}

// Converts a dhcpd_range_t structure to the ASCII representation like 192.168.3.100-192.168.3.200
// Allocs memory which the caller has to free with g_free().
int conv_range_bin2ascii(dhcpd_range_t *range, char **resultp) {
  char *ip1;
  char *ip2;

  int status = conv_ip_bin2ascii(range->ip_net_part1 | range->host_part1, &ip1);
  if (status != SUCCESS) {
    return status;
  }

  status = conv_ip_bin2ascii(range->ip_net_part2 | range->host_part2, &ip2);
  if (status != SUCCESS) {
    return status;
  }

  char *result = (char*)g_malloc(2 * SIZEOF_IPADDR);  // SIZEOF_IPADDR counts for the terminating \0.
  int len = snprintf(result, 2 * SIZEOF_IPADDR, "%s_%s", ip1, ip2);
  assert(len <= 2 * SIZEOF_IPADDR);
  g_free(ip1);
  g_free(ip2);
  if (NULL != resultp) {
    *resultp = result;
  }

  return status;
}

int conv_hosts_ascii2bin(char *hostsstr, uint32_t netmask, guint *count, dhcpd_host_t **resultp) {
  char **hostlist = g_strsplit(hostsstr, ",", 0);
  int status = conv_hosts_ascii2bin_(netmask, count, resultp, hostlist);
  g_strfreev(hostlist);
  return status;
}

// Allocates memory which must be freed with g_free().
int conv_hosts_bin2ascii(dhcpd_host_t *hosts, guint count, char **resultp) {
  int status = SUCCESS;
  guint strsize = count * (SIZEOF_MACADDR + SIZEOF_IPADDR) + 1;
  char *result = (char*)g_malloc0(strsize);
  guint len;
  char *bufp = result;
  char const *fmt = "%s_%s";

  for (guint index = 0; index < count; index++) {
    dhcpd_host_t *hp = &hosts[index];
    char *value;
    status = conv_ip_bin2ascii(hp->ip_net_part | hp->ip_host_part, &value);
    if (status != SUCCESS) {
      break;
    }
    len = (guint)snprintf(bufp, strsize, fmt, hp->mac, value);
    g_free(value);
    bufp += len;
    strsize -= len;
    assert(strsize > 0);
    fmt = ",%s_%s";
  }

  *resultp = status == SUCCESS ? result : NULL;
  if (status != SUCCESS) {
    g_free(result);
  }

  return status;
}

// Statemachine to check domain/host name according to RFC 1034.
bool conv_is_valid_domain_name(const char *name) {
  bool isname = true;
  int labelidx = 0;
  int charidx = 0;
  conv_state_t state = START;
  while (state > 0 && isname && charidx <= 255) {
    unsigned int c = *name & 255;
    int ccl = conv_dnchar[c];
    conv_state_t nstate = conv_transitions[state * 6 + ccl];
    switch (state) {
      case START:
        isname = (ccl == 2);
        labelidx = 0;
        break;
      case INLABEL:
        isname = (ccl > 0);
        break;
      case HYPHEN:
        isname = (ccl == 1 || ccl == 2 || ccl == 3);
        break;
      default:
        break;
    }
    state = nstate;
    name++;
    labelidx++;
    charidx++;
  }
  return isname && state == STOP && labelidx <= 64;
}

// Returns true if mac address is valid. Checks also for broadcast and multicast.
bool conv_is_valid_mac(const char *mac_in) {
    static char states[] = {1, 1, 2, 1, 1, 2, 1, 1 ,2, 1, 1, 2, 1, 1, 2, 1, 1, 0};
    bool result = false;
    const char *mac = mac_in;
    int idx = 0;

    for (;;) {
        if (states[idx] == 0) {
            if (*mac == 0) {
                result = true;
            }
            break;
        } else if (states[idx] == 1) {
            if (hexval(*mac) < 0) {
                break;
            }
        } else if (states[idx] == 2) {
            if (*mac != ':') {
                break;
            }
        }
        mac++;
        idx++;
    }

    // Check for broadcast or multicast MAC address.
    if (hexval(mac_in[1]) & 1) {
        result = false;
    }
    return result;
}
