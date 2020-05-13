//------------------------------------------------------------------------------
/// Copyright (c) 2014 WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     config_dnsmasq.c
///
///  \version  $Revision: 44718 $
///
///  \brief    Configuration tool to configure DNS and DHCP service on PFC200. 
///
///  \author   Hans-Josef Hoffmann, WAGO Kontakttechnik GmbH & Co. KG.
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
#include <unistd.h>
#include <ctype.h>              // for isdigit()
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>           // for flock()
#include <fcntl.h>
#include <net/if.h>
#include <time.h>
#include <libgen.h>             // for basename()

#include <glib.h>

#include "libnet/ct_xml.h"
#include "libnet/ct_libnet.h"
#include "liblog/ct_liblog.h"

// TODO put the following into a common header.
#define NETWORK_SERVICES_XML    "/etc/specific/network-services.xml"
#define XMLFILE_ROOT_NAME       "services_config"
#define ETC_HOSTS               "/etc/hosts"
#define ETC_HOSTS_TMP           "/etc/hosts.dnsmasq"
#define DNSMASQ_CONF            "/etc/dnsmasq.conf"
#define DNSMASQ_CONF_TMP        "/etc/dnsmasq.conf.tmp"
#define DNSMASQ_RESTART_CMD     "/etc/init.d/dnsmasq restart"
#define DNSMASQ_LEASE_FILE	"/var/lib/misc/dnsmasq.leases"
#define SIZEOF_IPADDR           16
#define SIZEOF_MACADDR          18
#define MAX_XPATH_LEN           256
#define DNS_DEFAULT_CACHE       500

// Type of service of config_dnsmasq.
typedef enum {
    UNDEF = 0,
    DHCPD,
    DNS
} service_t;

// Name/value pair structure.
typedef struct {
    char *name;
    void *value;
} nvpair_t;

typedef struct {
    enum eStatusCode code;
    char *text;
} erh_code_to_message_t;

//------------------------------------------------------------------------------
// SECTION Error handling.
//------------------------------------------------------------------------------

// Table to convert error codes to text. TODO put into more general support module.
static erh_code_to_message_t code_to_msg[] = {
    { MISSING_PARAMETER, "MISSING_PARAMETER" },
    { INVALID_PARAMETER, "INVALID_PARAMETER" },
    { FILE_OPEN_ERROR, "FILE_OPEN_ERROR" },
    { FILE_READ_ERROR, "FILE_READ_ERROR" },
    { FILE_CLOSE_ERROR, "FILE_CLOSE_ERROR" },
    { NOT_FOUND, "NOT_FOUND" },
    { SYSTEM_CALL_ERROR, "SYSTEM_CALL_ERROR" },
    { CONFIG_FILE_INCONSISTENT, "CONFIG_FILE_INCONSISTENT" },
    { TIMEOUT, "TIMEOUT" },
    { FILE_WRITE_ERROR, "FILE_WRITE_ERROR" },
    { NARROW_SPACE_WARNING, "NARROW_SPACE_WARNING" },
    { NOT_ENOUGH_SPACE_ERROR, "NOT_ENOUGH_SPACE_ERROR" },
    { SUCCESS, "UNDEFINED" }                   // End marker, don't remove.
};

static char *erh_prgname = "";
static int erh_prgnamesize;

// Set program name.
static void erh_init(char *prgname)
{
    erh_prgname = g_strdup(prgname);
    erh_prgnamesize = strlen(erh_prgname);
}

// Error handling: Set error message for WBM and terminate program.
// The message may be a printf format string with exactly one %s which is substituted by the par string.
static void erh_set_error(enum eStatusCode code, const char *msgfmt, const char *par)
{
    erh_code_to_message_t *p = code_to_msg;
    char message[erh_prgnamesize + strlen(msgfmt) + strlen(par) + 4];

    do
    {
        if(p->code == code)
        {
            break;
        }
        p++;
    } while(p->code != SUCCESS);

    size_t slen = snprintf(message, sizeof(message), "%s: ", erh_prgname);
    slen += snprintf(message + slen, sizeof(message) - slen, msgfmt, par);
    assert(slen <= sizeof(message));
    ct_liblog_setLastError(message);
    ct_liblog_reportError(code, message);

    fprintf(stderr, "%s: %s\n", p->text, message);
    exit(code);
}

// Assert a condition. If it is false, set error and terminate program.
static void erh_assert(bool cond, enum eStatusCode code, const char *msgfmt, const char *par)
{
    if(!cond)
    {
        erh_set_error(code, msgfmt, par);
    }
}

//------------------------------------------------------------------------------
// SECTION String pointer array functions.
//------------------------------------------------------------------------------

// Look up str in an string pointer array like argv.
static int strv_is_in_set(char **set, char *str)
{
    int idx = 0;
    bool found = false;
    for(idx = 0; set[idx] != NULL; idx++)
    {
        if(0 == strcmp(set[idx], str))
        {
            found = true;
            break;
        }
    }
    if(!found)
    {
        idx = -1;
    }
    return idx;
}

//------------------------------------------------------------------------------
// SECTION File handling routines for plain files.
//------------------------------------------------------------------------------

// Plain file handling for limited file size (< INT_MAX).
// Sizes are held in integers
// fh_open() delivers structure with file size, modification time, file descriptor.

#define FH_MAX_FILESIZE         (1024 * 1024 * 1024)	// 1GByte maximum file size.

typedef struct {
  int           fd;
  int           filesize;
  struct stat   stat;
  char          *filepath;
} plainfile_t;

static char fh_errortxt[128];
static int fh_errorcode;
static int fh_errno;

// Opens a file for reading or writing.
// Mode must be a string with one character. The meaning of the character is as
// follows:
//    r = Open file for reading.
//    w = Open file for writing, create a new file if file does not exist,
//        truncate an existing file to length 0.
// Error handling: Returns NULL on errors. Errorcode in fh_errorcode, error message in fh_errortxt and
// system errno in fh_errno.
static plainfile_t *fh_open(char *path, char *mode)
{
    plainfile_t *pfp = g_malloc0(sizeof (plainfile_t));
    int rc;
    int oflags = 0;

    pfp->filepath = path;
    fh_errorcode = 0;
    if(*mode == 'r') oflags = O_RDONLY;
    else if(*mode == 'w') oflags = O_CREAT | O_WRONLY | O_TRUNC;
    pfp->fd = open(path, oflags, 0644);
    if(pfp->fd < 0) {
        fh_errno = errno;
        snprintf(fh_errortxt, sizeof (fh_errortxt), "Error on open %s: %s", path, strerror(errno));
        if(errno == EACCES || errno == ENOENT || errno == EISDIR || errno == ENODEV ||
            errno == ELOOP) {
            fh_errorcode = FILE_OPEN_ERROR;
        } else {
            fh_errorcode = SYSTEM_CALL_ERROR;
        }
        g_free(pfp);
        pfp = NULL;
    }

    if(NULL != pfp && oflags == O_RDONLY) {
        rc = fstat(pfp->fd, &pfp->stat);
        if(rc == 0)
        {
            if(FH_MAX_FILESIZE >= pfp->stat.st_size)
            {
                pfp->filesize = (int)pfp->stat.st_size;
            }
            else
            {
                snprintf(fh_errortxt, sizeof (fh_errortxt), "Maximum file size exceeded on %s", path);
                fh_errno = 0;
                fh_errorcode = SYSTEM_CALL_ERROR;
                close(pfp->fd);
                g_free(pfp);
                pfp = NULL;
            }
        }
        else
        {
            snprintf(fh_errortxt, sizeof (fh_errortxt), "Error on fstat of %s: %s", path, strerror(errno));
            fh_errno = errno;
            fh_errorcode = SYSTEM_CALL_ERROR;
            close(pfp->fd);
            g_free(pfp);
            pfp = NULL;
        }
    }
    return pfp;
}

static int fh_read(plainfile_t *pfp, void *data, int size)
{
    ssize_t rc;

    rc = read(pfp->fd, data, size);
    if(rc < 0) {
        snprintf(fh_errortxt, sizeof(fh_errortxt), "Error on read %s: %s", pfp->filepath, strerror(errno));
        fh_errno = errno;
        fh_errorcode = FILE_READ_ERROR;
    }
    return rc;
}

static int fh_write(plainfile_t *pfp, void *data, int size)
{
    int sz = 0;

    if(size > 0)
    {
        sz = (int)write(pfp->fd, data, size);
        if(sz != size) {
            if(errno == ENOSPC)
            {
                fh_errorcode = NOT_ENOUGH_SPACE_ERROR;
            }
            else if(errno == EIO)
            {
                fh_errorcode = FILE_WRITE_ERROR;
            }
            fh_errno = errno;
            snprintf(fh_errortxt, sizeof(fh_errortxt), "Error on write to %s: %s", pfp->filepath, strerror(errno));
            sz = -1;
        }
    }
    return sz;
}

static void fh_sync(plainfile_t *pfp)
{
    int status = fdatasync(pfp->fd);
    erh_assert(0 == status, SYSTEM_CALL_ERROR, "Sync failed with error %s", strerror(errno));
}

// operation is LOCK_SH or LOCK_EX or LOCK_UN
static int fh_lock(plainfile_t *pfp, int operation)
{
    int rc = flock(pfp->fd, operation);
    if (rc)
    {
        fh_errno = errno;
        snprintf(fh_errortxt, sizeof(fh_errortxt), "Failed to aquire/free lock for %s: %s", pfp->filepath, strerror(errno));
    }
    return rc;
}

static void fh_close(plainfile_t *pfp)
{
    close(pfp->fd);
    g_free(pfp);
}

//------------------------------------------------------------------------------
// SECTION File handling of whole files.
//------------------------------------------------------------------------------

// Read file, consider raise condition where file size changed.
// Read buffer is one byte longer then the file size. A terminating '\0' is appended to the file
// data.
// Returns the file buffer and write the file size to *size.
// Returns NULL on open or read errors.
static char *cfh_readfile(char *path, int *size)
{
    char *filebuf = NULL;
    int readsize = 0;
    int retries = 3;
    plainfile_t *pfh;

    do
    {
        pfh = fh_open(path, "r");
        if(NULL == pfh)
        {
            g_free(filebuf);
            filebuf = NULL;
            readsize = 0;
            break;
        }
        int bufsize = pfh->filesize + 1;
        if(filebuf == NULL)
        {
            filebuf = g_malloc(bufsize);
        }
        readsize = fh_read(pfh, filebuf, bufsize);
        if(readsize != pfh->filesize)
        {
            // File size changed. Read again.
            fh_close(pfh);
            retries--;
            if(retries <= 0)
            {
                g_free(filebuf);
                readsize = 0;
                filebuf = NULL;
                break;
            }
        }
        filebuf[readsize] = '\0';
    } while(readsize != pfh->filesize);
    *size = readsize;
    return filebuf;
}

//------------------------------------------------------------------------------
// SECTION Conversion and check routines for DHCPD / DNS configuration parameters.
//------------------------------------------------------------------------------

typedef struct
{
    uint32_t ip_net_part1;
    uint32_t ip_net_part2;
    uint32_t host_part1;
    uint32_t host_part2;
} dhcpd_range_t;

typedef struct
{
    char *mac;
    uint32_t ip_net_part;
    uint32_t ip_host_part;
} dhcpd_host_t;

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

// States of the domain name check routine.
typedef enum
{
    ERR = -1,
    STOP,
    START,
    INLABEL,
    HYPHEN                         // Input character was a hyphen
} conv_state_t;

// State transition table.
static conv_state_t conv_transitions[] =
{
    /*             ill. digit    letter   -       .      \0 */
    /* STOP */     ERR, ERR,     ERR,     ERR,    ERR,   ERR,
    /* START */    ERR, ERR,     INLABEL, ERR,    ERR,   ERR,
    /* INLABEL */  ERR, INLABEL, INLABEL, HYPHEN, START, STOP,
    /* HYPHEN */   ERR, INLABEL, INLABEL, HYPHEN, ERR,   ERR,
};

// TODO should be in an config tool specific library.
// Statemachine to check domain/host name according to RFC 1034.
static bool conv_is_valid_domain_name(const char *name)
{
    bool isname = true;
    int labelidx = 0;
    int charidx = 0;
    conv_state_t state = START;
    while(state > 0 && isname && charidx <= 255)
    {
        unsigned int c = *name & 255;
        int ccl = conv_dnchar[c];
        conv_state_t nstate = conv_transitions[state * 6 + ccl];
        switch(state)
        {
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

// Convert one ascii character '0' to '9' and 'a' - 'f' to decimal 0 - 15.
// Upper case letters work as well.
static int hexval(int c) {
    static char ktab[] = {
         0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1, 
        -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
        -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    };
    int rv = -1;

    c -= 0x30;
    if (c >= 0 && c < 0x40)
    {
        rv = ktab[c];
    }
    return rv;
}

// TODO should be in an config tool specific library.
// Returns true if mac address is valid. Checks also for broadcast and multicast.
static bool conv_is_valid_mac(const char *mac_in)
{
    static char states[] = {1, 1, 2, 1, 1, 2, 1, 1 ,2, 1, 1, 2, 1, 1, 2, 1, 1, 0};
    bool result = false;
    const char *mac = mac_in;
    int idx = 0;

    for (;;)
    {
        if (states[idx] == 0)
        {
            if (*mac == 0)
            {
                result = true;
            }
            break;
        }
        else if (states[idx] == 1)
        {
            if (hexval(*mac) < 0)
            {
                break;
            }
        }
        else if (states[idx] == 2)
        {
            if (*mac != ':')
            {
                break;
            }
        }
        mac++;
        idx++;
    }

    // Check for broadcast or multicast MAC address.
    if (hexval(mac_in[1]) & 1)
    {
        result = false;
    }
    return result;
}

// Convert IP-Address from dotted ASCII string to uint32_t.
// Did not use inet_aton() in favour of more strict format checks.
// Set result to NULL for checking only.
static int conv_ip_ascii2bin(char *ipaddr, uint32_t *resultp)
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
    return status;
}

// Allocatess memory which the caller has to free with g_free().
static int conv_ip_bin2ascii(uint32_t ipaddr, char **resultp)
{
    char *result = g_malloc(SIZEOF_IPADDR);
    unsigned int b1 = (ipaddr >> 24) & 255;  
    unsigned int b2 = (ipaddr >> 16) & 255;
    unsigned int b3 = (ipaddr >> 8) & 255;
    unsigned int b4 = ipaddr & 255;
    int len = snprintf(result, SIZEOF_IPADDR, "%d.%d.%d.%d", b1, b2, b3, b4);
    assert(len < SIZEOF_IPADDR);
    *resultp = result;
    return SUCCESS;
}

// Allocs a dhcpd_range_t structure which the caller has to free with g_free().
static int conv_range_ascii2bin(char *rangestr, uint32_t netmask, dhcpd_range_t **resultp)
{
    int status = SUCCESS;
    dhcpd_range_t *range = g_malloc(sizeof(dhcpd_range_t));
    char **ipaddrs = g_strsplit(rangestr, "_", 0);

    if(g_strv_length(ipaddrs) == 2)
    {
        uint32_t ip1;
        uint32_t ip2;

        status = conv_ip_ascii2bin(ipaddrs[0], &ip1);
        if(SUCCESS == status)
        {
            status = conv_ip_ascii2bin(ipaddrs[1], &ip2);
        }
        if(SUCCESS == status)
        {
            if((ip1 & netmask) == (ip2 & netmask) && ip1 < ip2)
            {
                range->ip_net_part1 = ip1 & netmask;
                range->ip_net_part2 = ip2 & netmask;
                range->host_part1 = ip1 & ~netmask;
                range->host_part2 = ip2 & ~netmask;
                *resultp = range;
            }
            else
            {
                status = INVALID_PARAMETER;
            }
        }
    }
    else
    {
        status = INVALID_PARAMETER;
    }
    return status;
}

// Converts a dhcpd_range_t structure to the ASCII representation like 192.168.3.100-192.168.3.200
// Allocs memory which the caller has to free with g_free().
static int conv_range_bin2ascii(dhcpd_range_t *range, char **resultp)
{
    int status;
    char *result = g_malloc(2 * SIZEOF_IPADDR); // SIZEOF_IPADDR counts for the terminating \0.
    char *ip1;
    char *ip2;

    status = conv_ip_bin2ascii(range->ip_net_part1 | range->host_part1, &ip1);
    if(SUCCESS == status)
    {
        status = conv_ip_bin2ascii(range->ip_net_part2 | range->host_part2, &ip2);
    }

    if(SUCCESS == status)
    {
        int len = snprintf(result, 2 * SIZEOF_IPADDR, "%s_%s", ip1, ip2);
        assert(len <= 2 * SIZEOF_IPADDR);
        g_free(ip1);
        g_free(ip2);
        if(NULL != resultp)
        {
            *resultp = result;
        }
    }
    return status;
}

// Converts a string of the form mac_addr-ip_addr,... into an array of structures of type dhcpd_host_t
// Allocates memory for the array of structures which must be freed by g_free().
static int conv_hosts_ascii2bin(char *hostsstr, uint32_t netmask, int *count, dhcpd_host_t **resultp)
{
    int status = SUCCESS;
    char **hostlist = g_strsplit(hostsstr, ",", 0);
    int hcount = g_strv_length(hostlist);
    int index;
    dhcpd_host_t *hosts = g_malloc(hcount * sizeof(dhcpd_host_t));

    for(index = 0; index < hcount; index++)
    {
        char *host = hostlist[index];
        dhcpd_host_t *hp = &hosts[index];
        char **parts = g_strsplit(host, "_", 0);
        uint32_t ip;

        if(g_strv_length(parts) != 2)
        {
            status = INVALID_PARAMETER;
            break;
        }
        if(!conv_is_valid_mac(parts[0]) && !conv_is_valid_domain_name(parts[0]))
        {
            status = INVALID_PARAMETER;
            break;
        }
        hp->mac = g_strdup(parts[0]);
        status = conv_ip_ascii2bin(parts[1], &ip);
        if(SUCCESS != status)
        {
            break;
        }
        hp->ip_net_part = ip & netmask;
        hp->ip_host_part = ip & ~netmask;
    }
    if(SUCCESS == status)
    {
        *count = hcount;
        *resultp = hosts;
    }
    else
    {
        *count = 0;
        *resultp = NULL;
        g_free(hosts);
    }
    g_strfreev(hostlist);
    return status;
}

// Allocates memory which must be freed with g_free().
static int conv_hosts_bin2ascii(dhcpd_host_t *hosts, int count, char **resultp)
{
    int status = SUCCESS;
    int strsize = count * (SIZEOF_MACADDR + SIZEOF_IPADDR) + 1;
    char *result = g_malloc0(strsize);
    int len;
    int index;
    char *bufp = result;
    char *fmt = "%s_%s";

    for(index = 0; index < count; index++)
    {
        dhcpd_host_t *hp = &hosts[index];
        char *value;
        status = conv_ip_bin2ascii(hp->ip_net_part | hp->ip_host_part, &value);
        if(SUCCESS == status)
        {
            len = snprintf(bufp, strsize, fmt, hp->mac, value);
            g_free(value);
            bufp += len;
            strsize -= len;
            assert(strsize > 0);
            fmt = ",%s_%s";
        }
        else
        {
            break;
        }
    }
    if(SUCCESS == status)
    {
        *resultp = result;
    }
    else
    {
        *resultp = NULL;
        g_free(result);
    }
    return status;
}

//------------------------------------------------------------------------------
// SECTION Check routines for XML file related strings.
//------------------------------------------------------------------------------

// Structure of an entry in the parameter name list.
typedef struct {
    char      *pname;           // Parameter name.
    char      *pjname;          // Parameter name for JSON output.
    service_t pservice;         // Kind of service.
    bool      pext;             // Name visible on command line.
    bool      pmulti;           // Multiple values possible.
    char      *pxmlname;        // Name in XML file.
    void      (*pcheckfct)(char *v); // Function to check a config parameter.
} xml_param_t;

// Forward declarations of check function names, used in xml_param_name_list.
static void xml_check_dnsstate(char *value);
static void xml_check_dnsmode(char *value);
static void xml_check_dnshost(char *value);
static void xml_check_dhcpdstate(char *value);
static void xml_check_dhcpdrange(char *value);
static void xml_check_dhcpdlease(char *value);
static void xml_check_dhcpdhost(char *value);
static void xml_check_nop(char *value);

// This table defines all dnsmasq configuration parameter names as they can be get/set by xml_*().
// Forth column true means that name is used on the command line.
// Each parameter name is associated with name in the XML config file.
static xml_param_t xml_param_name_list[] = {
    { "dns-state",        "dnsState",       DNS,   true,  false, "state",        xml_check_dnsstate },
    { "dns-mode",         "dnsMode",        DNS,   true,  false, "mode",         xml_check_dnsmode },
    { "dns-fix-host",     "dnsFixHost",     DNS,   true,  true,  "fix_hostname", xml_check_dnshost },
    { "dhcpd-state",      "dhcpdState",     DHCPD, true,  false, "state",        xml_check_dhcpdstate },
    { "dhcpd-range",      "dhcpdRange",     DHCPD, true,  false, "range",        xml_check_dhcpdrange },
    { "dhcpd-lease-time", "dhcpdLeaseTime", DHCPD, true,  false, "lease_time",   xml_check_dhcpdlease },
    { "dhcpd-fix-host",   "dhcpdFixHost",   DHCPD, true,  true,  "fix_host",     xml_check_dhcpdhost },
    { "ip_addr",          "",               DHCPD, false, false, "ip_addr",      xml_check_nop },
    { "netmask",          "",               DHCPD, false, false, "netmask",      xml_check_nop },
    { "port-name",        "",               DHCPD, false, false, "port_name",    xml_check_nop },
    { NULL, NULL, UNDEF, false, false, NULL, xml_check_nop }              // Endmarker, do not remove.
};

static xml_param_t *xml_param_name_list_end = 
                   &xml_param_name_list[sizeof(xml_param_name_list) / sizeof(xml_param_name_list[0])];

static bool xml_check_condition(bool cond, int status)
{
    return cond && status;
}

static void xml_check_dnsstate(char *value)
{
    bool result = (0 == strcmp(value, "enabled") || 0 == strcmp(value, "disabled"));
    erh_assert(result, INVALID_PARAMETER, "Invalid DNS state.", "");
}

static void xml_check_dnsmode(char *value)
{
    bool result = (0 == strcmp(value, "relay") || 0 == strcmp(value, "proxy"));
    erh_assert(result, INVALID_PARAMETER, "Invalid DNS mode.", "");
}

// value example: 192.168.0.2:hostname,192.168.0.4:hostname2_hostname3
static void xml_check_dnshost(char *value)
{
    bool result = true;
    char **entries = g_strsplit(value, ",", 0);
    int count = g_strv_length(entries);
    int i;

    erh_assert(count <= 15, INVALID_PARAMETER, "Max number of dns static hosts exceeded.", "");
    for(i = 0; i < count; i++)
    {
        // Check for double entries.
        int k;
        for(k = 0; k < i; k++)
        {
            erh_assert(strcmp(entries[k], entries[i]) != 0, 
                INVALID_PARAMETER, "Double DNS static host value %s", entries[k]);
        }

        char **ip_hosts = g_strsplit(entries[i], ":", 0);
        erh_assert(g_strv_length(ip_hosts) == 2, INVALID_PARAMETER, "Illegal dns-fix-host format", "");
        char **hostlist = g_strsplit(ip_hosts[1], "_", 0);
        int n_hostnames = g_strv_length(hostlist);
        result = xml_check_condition(n_hostnames > 0, result);
        result = xml_check_condition(SUCCESS == conv_ip_ascii2bin(ip_hosts[0], NULL), result);
        if(result)
        {
            int j;
            for(j = 0; j < n_hostnames; j++)
            {
                result = xml_check_condition(conv_is_valid_domain_name(hostlist[j]), result);
            }
        }
        g_strfreev(hostlist);
        g_strfreev(ip_hosts);
        erh_assert(result, INVALID_PARAMETER, "Invalid DNS fix host value.", "");
    }
    g_strfreev(entries);
}

static void xml_check_dhcpdstate(char *value)
{
    bool result;
    result = (0 == strcmp(value, "enabled") || 0 == strcmp(value, "disabled"));
    erh_assert(result, INVALID_PARAMETER, "Invalid DHCPD state.", "");
}

// Check the format of an ip address range.
// Example value: 192.168.3.50_192.168.3.150
static void xml_check_dhcpdrange(char *value)
{
    bool result = true;
    char **ip_addrs = g_strsplit(value, "_", 0);
    result = xml_check_condition(g_strv_length(ip_addrs) == 2, result);
    result = xml_check_condition(SUCCESS == conv_ip_ascii2bin(ip_addrs[0], NULL), result);
    result = xml_check_condition(SUCCESS == conv_ip_ascii2bin(ip_addrs[1], NULL), result);
    g_strfreev(ip_addrs);
    erh_assert(result, INVALID_PARAMETER, "Invalid DHCP range.", "");
}

// Check DHCPD lease time value.
// Example value: 150
static void xml_check_dhcpdlease(char *value)
{
    bool result = false;
    int vlen = strlen(value);
    if(0 < vlen && 8 >= vlen)
    {
        result = true;
        while('\0' != *value)
        {
            if(!isdigit(*value))
            {
                result = false;
                break;
            }
            value++;
        }
    }
    erh_assert(result, INVALID_PARAMETER, "Invalid DHCPD lease time value.", "");
}

// Example value: 01:02:03:04:05:06_192.168.0.44,hostname_192.168.0.44.
// Value may be the empty string.
static void xml_check_dhcpdhost(char *value)
{
    if('\0' != *value)
    {
        int count;
        char **entries = g_strsplit(value, ",", 0);
        count = g_strv_length(entries);
        int i;

        erh_assert(count <= 15, INVALID_PARAMETER, "Max number of dhcp static hosts exceeded.", "");
        for(i = 0; i < count; i++)
        {
            bool result = true;
            char **mac_ip = g_strsplit(entries[i], "_", 0);
            result = xml_check_condition(g_strv_length(mac_ip) == 2, result);
            
            if(result && !conv_is_valid_mac(mac_ip[0]))
            {
                result = xml_check_condition(conv_is_valid_domain_name(mac_ip[0]), result);
            }
            if(result)
            {
                result = xml_check_condition(SUCCESS == conv_ip_ascii2bin(mac_ip[1], NULL), result);
            }
            g_strfreev(mac_ip);
            erh_assert(result, INVALID_PARAMETER, "Invalid DHCP fix host config parameter.", "");
        }
    }
}

static void xml_check_nop(char *value)
{
    (void) value;
}

// Find parameter structure for name parname, service serv and external mode ext.
// Failed if pp->name == NULL.
static xml_param_t *xml_find_param(char *parname, service_t serv, bool extonly)
{
    xml_param_t *pp = xml_param_name_list;

    while(NULL != pp->pname)
    {
        if(0 == strcmp(parname, pp->pname) && serv == pp->pservice && (extonly ? pp->pext : true))
        {
            break;
        }
        pp++;
    }
    return pp;
}

// Check if config parameter name is valid, given a service type and the external mode.
static void xml_check_name(char *parname, service_t serv, bool ext)
{
    xml_param_t *pp = xml_find_param(parname, serv, ext);
    erh_assert(pp->pname != NULL, INVALID_PARAMETER, "Unknown config parameter name '%s'", parname);
}

// Check config parameter values.
// Calls one of the check functions xml_check_dnsmode(), xml_check_dnshost(), xml_check_dhcpdstate(),
// xml_check_dhcpdrange(), xml_check_dhcpdlease(), xml_check_dhcpdhost(),  xml_check_nop() via function pointer pcheckfct().
// Returns false on error.
static void xml_check_values(char *parname, char *value, service_t service)
{
    xml_param_t *pp = xml_find_param(parname, service, true);
    assert(pp->pname != NULL);
    pp->pcheckfct(value);
}

// Iterate over the parameter name list.
// Usage: for(par = xml_get_next_par(NULL, serv, true); par->pname != NULL; par = xml_get_next_par(par, port, serv) { ... }
static xml_param_t *xml_get_next_par(xml_param_t *par, service_t serv, bool extmode)
{
    if(NULL == par)
    {
        par = xml_param_name_list - 1;
    }
    do
    {
        par++;
        if(par->pservice == serv && par->pext == extmode)
        {
            break;
        }
    } while(par < xml_param_name_list_end - 1);
    assert(par >= xml_param_name_list && par < xml_param_name_list_end);
    return par;
}

//------------------------------------------------------------------------------
// SECTION XML file and XML memory data handling.
//------------------------------------------------------------------------------

// XML file session handle.
typedef struct {
    xmlSession_t *xml_session;
    bool modified;                      // XML data modified, need to write back.
} dnsmasq_handle_t;

// Initialize xml file handling. This function and the following ones accept configuration parameter names
// as defined in the table xml_param_name_list.
static dnsmasq_handle_t *ct_dnsmasq_start_session(char *xml_file)
{
    int status;

    dnsmasq_handle_t *handle = g_malloc(sizeof(dnsmasq_handle_t));
    handle->xml_session = NULL;
    handle->modified = false;
    if(SUCCESS != (status = ct_xml_start_session(xml_file, &(handle->xml_session))))
    {
        erh_set_error(status, "XML config file missing or wrong format", "");
    }
    return handle;
}

static void ct_dnsmasq_finish_session(dnsmasq_handle_t *handle)
{
    (void) ct_xml_finish_session(handle->xml_session);
    ct_xml_permanent_close();
    g_free(handle);
}

// Save changes to disk. ct_xml_commit_xml_changes first writes to a temporary file which
// is renamed to the original file afterwards.
static void ct_dnsmasq_save_xml_file(dnsmasq_handle_t *handle, char *new_filename)
{
    (void) new_filename;
    if(handle->modified)
    {
        int status = ct_xml_commit_xml_changes(handle->xml_session);
        erh_assert(SUCCESS == status, status, "Error on writing XML file to disk.", "");
    }
    handle->modified = false;
}

// Builds an xpath of the config parameter.
static char *xml_make_xpath(xml_param_t *pp, service_t service, char *port)
{
    int len;
    char *xpath = g_malloc(MAX_XPATH_LEN);

    if(service == DNS)
    {
        len = snprintf(xpath, MAX_XPATH_LEN, "//dns/%s", pp->pxmlname);
    }
    else
    {
        len = snprintf(xpath, MAX_XPATH_LEN, "//dhcpd[port_name = '%s']/%s", port, pp->pxmlname);
    }
    assert(len < MAX_XPATH_LEN);
    return xpath;
}

// Get a value from the XML config file. If defaultval is not NULL, return that value in case
// the parameter is not available. Otherwise terminate with an error message.
static char *ct_dnsmasq_get_value(dnsmasq_handle_t *handle, service_t service, char *port, char *parname, char *defaultval)
{
    int status = SUCCESS;
    char *retval;
    xml_param_t *pp = xml_find_param(parname, service, false);
    char *xpath = xml_make_xpath(pp, service, port);

    erh_assert(NULL != pp->pname, INVALID_PARAMETER, "Unknown config parameter name %s.", parname);
    if(pp->pmulti)
    {
        GString *parlist = g_string_new("");
        status = ct_xml_get_multiple_values((const char *)xpath, parlist, ",", handle->xml_session);
        retval = g_strdup(parlist->str);
        g_string_free(parlist, true);
    }
    else
    {
        status = ct_xml_dup_value((const char *)xpath, &retval, handle->xml_session);
    }

    // Start error handling on missing parameters. Return defaultval if given.
    if(SUCCESS != status)
    {
        if(NULL != defaultval)
        {
            g_free(retval);
            retval = g_strdup(defaultval);
        }
        else
        {
            erh_set_error(status, "XML config file error. Missing config parameter '%s'", parname);
        }
    }
    g_free(xpath);
    return retval;
}

// Set a configuration parameter (or a list of parameters in case of fix-host lists).
static void ct_dnsmasq_set_value(dnsmasq_handle_t *handle, service_t service, char *port, char *parname, char *value)
{
    // Check if parameter is changed and set flag in handle.
    char *current_value = ct_dnsmasq_get_value(handle, service, port, parname, "");
    if(0 != strcmp(current_value, value))
    {
        handle->modified = true;
        int status = SUCCESS;
        xml_param_t *pp = xml_find_param(parname, service, false);
        char *xpath = xml_make_xpath(pp, service, port);

        if(pp->pmulti)
        {
            char **parts = g_strsplit(value, ",", 0);
            status = ct_xml_del_multiple_values((const char *)xpath, handle->xml_session);
            while(NULL != *parts && SUCCESS == status)
            {
                status = ct_xml_add_value((const char *)xpath, *parts, handle->xml_session);
                parts++;
            }
        }
        else
        {
            status = ct_xml_set_value(xpath, value, handle->xml_session);
        }
        erh_assert(SUCCESS == status, status, "Cannot write to XML data.", "");
    }
}

//------------------------------------------------------------------------------
// SECTION Network interface configuration data.
//------------------------------------------------------------------------------

#define MAX_DHCP_PORTS  8

typedef struct {
    char *port_name;
    char *state;
    char *type;
    char *ip_addr;
    char *netmask;
    uint32_t ip_addr_bin;
    uint32_t netmask_bin;
    uint32_t network_bin;
} port_data_t;

typedef struct {
    int dsa_state;
    char *host_name;
    int  no_of_ports;
    char **port_name_list;
    port_data_t *port_data_list[MAX_DHCP_PORTS];
} ip_config_t;

// Read some configuration parameters of all network interfaces into memory data structures.
static void netcfg_read_settings(ip_config_t *data, char *interfaces_xml_path, char **legal_ports, int debugmode)
{
    char buf[256];
    int rc;
    libnetSession_t *libnet_session = NULL; 
    int status;

    status = ct_libnet_start_session(interfaces_xml_path, &libnet_session);
    erh_assert(SUCCESS == status, status, "No access to net interface config xml file %s.", interfaces_xml_path);
    if(debugmode)
    {
        // Simplify testing, take a constant host name.
        strcpy(buf, "debughostname");
    }
    else
    {
        if(0 != gethostname(buf, sizeof(buf)))
        {
            erh_set_error(SYSTEM_CALL_ERROR, "gethostname failed.", "");
        }
    }
    data->host_name = g_strdup(buf);

    // Get DSA state.
    buf[0] = '\0';
    status = ct_libnet_get_dsa_state(buf, sizeof(buf), libnet_session);
    data->dsa_state = buf[0] - '0';
    erh_assert(SUCCESS == status && strlen(buf) == 1 && (data->dsa_state == 0 || data->dsa_state == 1), status,
              "Error in net interface config xml file %s.", interfaces_xml_path);

    // Get list of ports configured from network-interfaces.xml.
    buf[0] = '\0';
    status = ct_libnet_get_ports_list(buf, sizeof(buf), ",", libnet_session);
    erh_assert(SUCCESS == status, status, "Error in net interface config xml file %s.", interfaces_xml_path);
    char **ip_port_list = g_strsplit(buf, ",", 0);
    int ip_port_count = g_strv_length(ip_port_list);
    data->port_name_list = g_malloc0((MAX_DHCP_PORTS + 1) * sizeof(char *));
    if(MAX_DHCP_PORTS < data->no_of_ports)
    {
        snprintf(buf, sizeof(buf), "%d", MAX_DHCP_PORTS);
        erh_set_error(CONFIG_FILE_INCONSISTENT, "Found more than %s ports in network-interfaces.xml", buf);
            data->no_of_ports++;
    }

    // Read port specific parameters, IP-Address, Netmask.
    int i;
    for(i = 0; i < ip_port_count; i++)
    {
        if(0 <= strv_is_in_set(legal_ports, ip_port_list[i]))
        {
            int portindex = data->no_of_ports;
            data->port_name_list[data->no_of_ports] = ip_port_list[i];
            data->no_of_ports++;

            // Copy port name.
            port_data_t *pd = g_malloc(sizeof(port_data_t));
            data->port_data_list[portindex] = pd;
            pd->port_name = g_strdup(data->port_name_list[portindex]);

            // Get port state
            buf[0] = '\0';
            status = ct_libnet_get_port_state_from_config(pd->port_name, buf, sizeof(buf), libnet_session);
            erh_assert(SUCCESS == status, status, "Error in net interface config xml file %s.", interfaces_xml_path);
            pd->state = g_strdup(buf);

            // Get port type (static | dhcp | bootp)
            buf[0] = '\0';
            status = ct_libnet_get_config_type(pd->port_name, buf, sizeof(buf), libnet_session);
            erh_assert(SUCCESS == status, status, "Error in net interface config xml file %s.", interfaces_xml_path);
            pd->type = g_strdup(buf);

            // Get IP address from interface config
            char ipaddr[SIZEOF_IPADDR];
            ipaddr[0] = '\0';
            rc = ct_libnet_get_ip_addr_from_config(pd->port_name, ipaddr, sizeof(ipaddr), libnet_session); 
            if(SUCCESS == rc)
            {
                pd->ip_addr = g_strdup(ipaddr);
            }
            else
            {
                pd->ip_addr = g_strdup("0.0.0.0");
            }

            // Get netmask from interface config
            char netmask[SIZEOF_IPADDR];
            netmask[0] = '\0';
            rc = ct_libnet_get_netmask_from_config(pd->port_name, netmask, sizeof(netmask), NETMASK_FORMAT_MODE_CLASSIC,
                                                   libnet_session);
            if(SUCCESS == rc)
            {
                pd->netmask = g_strdup(netmask);
            }
            else
            {
                pd->netmask = g_strdup("0.0.0.0");
            }
            conv_ip_ascii2bin(pd->ip_addr, &pd->ip_addr_bin);
            conv_ip_ascii2bin(pd->netmask, &pd->netmask_bin);
            pd->network_bin = pd->ip_addr_bin & pd->netmask_bin;
        }
    }

    // Terminate session to libnet.
    ct_libnet_finish_session(libnet_session);
}

// Return the list of port names as get from network interface config.
static char **netcfg_get_port_names(ip_config_t *data, int *count)
{
    *count = data->no_of_ports;
    return data->port_name_list;
}

// Check for a valid configured network port name
static bool netcfg_is_valid_port_name(ip_config_t *data, char *portname)
{
    bool isvalid = false;
    int i;
    for(i = 0; i < data->no_of_ports; i++)
    {
        if(0 == strcmp(data->port_name_list[i], portname))
        {
            isvalid = true;
            break;
        }
    }
    return isvalid;
}

// Find the index of port with the given name.
static port_data_t *netcfg_get_port_data(ip_config_t *data, char *port)
{
    int idx;
    port_data_t *retval = NULL;
    for(idx = 0; idx < data->no_of_ports; idx++)
    {
        if(0 == strcmp(port, data->port_data_list[idx]->port_name))
        {
            retval = data->port_data_list[idx];
            break;
        }
    }
    erh_assert(retval != NULL, INVALID_PARAMETER, "Invalid port specification.", "");
    return retval;
}

//------------------------------------------------------------------------------
// SECTION Plausibility checks of DHCP / DNS configuration parameter against network interface data.
//------------------------------------------------------------------------------

// Adjust netmask part of all ip addresses of one port.
// Only if DHCPD is enabled on the specified port.
static void ct_dnsmasq_adjust_net(dnsmasq_handle_t *xmlhandle, port_data_t *pdt)
{
    int status;
    char *port = pdt->port_name;
    char *state = ct_dnsmasq_get_value(xmlhandle, DHCPD, port, "dhcpd-state", NULL);

    if(0 == strcmp(state, "enabled"))
    {
        uint32_t newnetmask;
        uint32_t oldnetmask;

        status = conv_ip_ascii2bin(pdt->netmask, &newnetmask);
        erh_assert(SUCCESS == status, status, "Invalid netmask from network interface configuration.", "");
        status = conv_ip_ascii2bin(ct_dnsmasq_get_value(xmlhandle, DHCPD, port, "netmask", NULL), &oldnetmask);
        erh_assert(SUCCESS == status, status, "Invalid netmask from DHCPD configuration.", "");

        if(newnetmask <= oldnetmask)
        {
            // New netmask does not shrink host range.
            // Adjust net part of dhcp range and fix ip addresses for port, if netmask or ip address changed.
            uint32_t interface_net;
            status = conv_ip_ascii2bin(pdt->ip_addr, &interface_net);
            erh_assert(SUCCESS == status, status, "Invalid ip address from network interface configuration.", "");
            interface_net &= newnetmask;

            dhcpd_range_t *range;
            status = conv_range_ascii2bin(ct_dnsmasq_get_value(xmlhandle, DHCPD, port, "dhcpd-range", NULL),
                                          oldnetmask, &range);
            erh_assert(SUCCESS == status, status, "Invalid dhcpd range config parameter.", "");
            if(interface_net != range->ip_net_part1)
            {
                // Adjust DHCP range.
                range->ip_net_part1 = interface_net;
                range->ip_net_part2 = interface_net;
                char *value;
                status = conv_range_bin2ascii(range, &value);
                erh_assert(SUCCESS == status, status, "Invalid dhcpd range config parameter.", "");
                ct_dnsmasq_set_value(xmlhandle, DHCPD, port, "dhcpd-range", value);
                g_free(value);

                // Adjust net part of fix ip addresses.
                int index;
                int count;
                dhcpd_host_t *ethers;
                status = conv_hosts_ascii2bin(ct_dnsmasq_get_value(xmlhandle, DHCPD, port, "dhcpd-fix-host", ""), 
                                                            oldnetmask, &count, &ethers);
                erh_assert(SUCCESS == status, status, "Invalid dhcpd fix host config parameter.", "");
                for(index = 0; index < count; index++)
                {
                    ethers[index].ip_net_part = interface_net;
                }
                status = conv_hosts_bin2ascii(ethers, count, &value);
                erh_assert(SUCCESS == status, status, "Invalid dhcpd fix host config parameter.", "");
                ct_dnsmasq_set_value(xmlhandle, DHCPD, port, "dhcpd-fix-host", value);
                g_free(value);
                g_free(ethers);
            }
            g_free(range);
        }
    }
}

// Check if DSA mode or netmask part of ip address changed.
// If required, disables ports and adapt ip adresses.
static void ct_dnsmasq_check_ip_dependencies(ip_config_t *data, dnsmasq_handle_t *xmlhandle, char **legal_ports)
{
    char *port;
    for(port = *legal_ports; port != NULL; legal_ports++, port = *legal_ports)
    {
        int pidx = strv_is_in_set(data->port_name_list, port);
        if(0 > pidx)
        {
            ct_dnsmasq_set_value(xmlhandle, DHCPD, port, "dhcpd-state", "disabled");
        }
        else
        {
            port_data_t *pdt = netcfg_get_port_data(data, port);
            if(0 == strcmp(pdt->state, "disabled") || 0 != strcmp(pdt->type, "static"))
            {
                ct_dnsmasq_set_value(xmlhandle, DHCPD, port, "dhcpd-state", "disabled");
            }
            else
            {
                ct_dnsmasq_adjust_net(xmlhandle, pdt);
            }
            ct_dnsmasq_set_value(xmlhandle, DHCPD, port, "ip_addr", pdt->ip_addr);
            ct_dnsmasq_set_value(xmlhandle, DHCPD, port, "netmask", pdt->netmask);
        }
    }
}

// Check that dhcp range is in local subnet.
static void ct_dnsmasq_check_dhcp_range(char *rangestr, ip_config_t *ipcfg, char *port)
{
    uint32_t netmask;
    uint32_t ifip;
    int status;
    dhcpd_range_t *range;

    if(netcfg_is_valid_port_name(ipcfg, port))
    {
        port_data_t *pdt = netcfg_get_port_data(ipcfg, port);
        status = conv_ip_ascii2bin(pdt->netmask, &netmask);
        erh_assert(SUCCESS == status, status, "Invalid netmask from network interface configuration.", "");
        status = conv_ip_ascii2bin(pdt->ip_addr, &ifip);
        erh_assert(SUCCESS == status, status, "Invalid ip address from network interface configuration.", "");
        status = conv_range_ascii2bin(rangestr, netmask, &range);
        erh_assert(SUCCESS == status, status, "Invalid dhcpd range.", "");
        erh_assert((ifip & netmask) == range->ip_net_part1, INVALID_PARAMETER,
                    "dhcpd range not in local subnet", "");
        g_free(range);
    }
}

// Check if each MAC address is unique.
// macaddr-ipaddr,macaddr-ipaddr,...
static void ct_dnsmasq_check_dhcp_hosts(char *hoststr)
{
    int count;
    uint32_t dummy_netmask = 0xFFFFFFFF;
    dhcpd_host_t *ethers;
    int status;

    status = conv_hosts_ascii2bin(hoststr, dummy_netmask, &count, &ethers);
    erh_assert(SUCCESS == status, status, "Invalid DHCPD fix host parameter", "");
    if(count > 1)
    {
        int i;
        for(i = 0; i < count - 1; i++)
        {
            int j;
            for(j = i + 1; j < count; j++)
            {
                char *mac1 = ethers[i].mac;
                char *mac2 = ethers[j].mac;
                erh_assert(0 != strcmp(mac1, mac2), INVALID_PARAMETER, "Duplicate MAC addresses", "");
            }
        }
    }
    g_free(ethers);
}

// Check one name/value pair of a config parameter name and a config parameter value.
// For example "dhcpd-range=192.168.3.100-192.168.3.200"
// nvpair_t structure as well as its components are allocated memory and have to be set free by the caller.
static nvpair_t *ct_dnsmasq_check_input(char *nvpair, service_t service, char *port, ip_config_t *ipcfg)
{
    nvpair_t *nvp = g_malloc(sizeof(nvpair_t));

    // Split name value pair.
    char **nvalue = g_strsplit(nvpair, "=", 0);
    erh_assert(g_strv_length(nvalue) == 2, INVALID_PARAMETER, "Incorrect name=value format.", "");

    // Check if name is a valid argument name
    xml_check_name(nvalue[0], service, true);

    // Check format of config parameter value
    xml_check_values(nvalue[0], nvalue[1], service);

    // Additional checks
    if(0 == strcmp(nvalue[0], "dhcpd-state"))
    {
        // dhcp-state: Network interface must be in mode static if enabling dhcpd.
        port_data_t *pdt = netcfg_get_port_data(ipcfg, port);
        if(0 == strcmp(nvalue[1], "enabled"))
        {
            erh_assert(0 == strcmp(pdt->type, "static"), INVALID_PARAMETER, "No static IP on network port %s", port);
        }
    }
    else if(0 == strcmp(nvalue[0], "dhcpd-range"))
    {
        // dhcp-range: Smaller ip first, in local subnet
        ct_dnsmasq_check_dhcp_range(nvalue[1], ipcfg, port);
    }
    else if(0 == strcmp(nvalue[0], "dhcpd-fix-host"))
    {
        // fix-host dhcp: check if MAC unique:
        ct_dnsmasq_check_dhcp_hosts(nvalue[1]);
    }
    else if(0 == strcmp(nvalue[0], "dhcpd-lease-time"))
    {
        // lease time: check if greater 2.
        int32_t val = (int32_t)strtol(nvalue[1], NULL, 10);
        erh_assert(val >= 2, INVALID_PARAMETER, "Incorrect dhcpd lease time value.", "");
    }

    // Build up the nvpair structure.
    nvp->name = g_strdup(nvalue[0]);
    nvp->value = g_strdup(nvalue[1]);
    g_strfreev(nvalue);    
    return nvp;
}

//------------------------------------------------------------------------------
// SECTION Config file generation and editing and dnsmasq restarts.
//------------------------------------------------------------------------------

#define ETC_HOST_MARKER  "# CONFIG_TOOL_SPECIFIC\n"
static char *cfgtool_header = 
    ETC_HOST_MARKER
    "# This part of /etc/hosts starting and ending with the content of the line above\n"
    "# is written by WAGO config tools for dnsmasq DNS and DHCP service. Do not edit\n"
    "# this part. Especially do not remove the markers.\n"
    "# Add your entries above or below the markers.\n"
    "#\n"
    "";

// Edit /etc/hosts
// Remember /etc/hosts content up to marker ETC_HOST_MARKER and content from second marker to end.
// Finally write first part, generated part with markers, and second part to a temporary file.
static void ct_dnsmasq_edit_etc_hosts(dnsmasq_handle_t *xmlhandle, ip_config_t *data, char *etchosts)
{
    char marker[] = ETC_HOST_MARKER;
    int filesize;
    int lenfirst;
    int lensecond;
    char *second;
    int rc;

    // Lock /etc/hosts, use extra file descriptor
    plainfile_t *lockfh = fh_open(etchosts, "r");
    erh_assert(NULL != lockfh, FILE_OPEN_ERROR, fh_errortxt, "");
    rc = fh_lock(lockfh, LOCK_EX);
    erh_assert(0 == rc, SYSTEM_CALL_ERROR, fh_errortxt, "");

    // Read /etc/hosts
    char *filebuf = cfh_readfile(etchosts, &filesize);
    erh_assert(NULL != filebuf, FILE_READ_ERROR, "Failed to read /etc/hosts.", "");

    // Find config tools part, if present, save position and length of the parts to leave alone.
    char *p = strstr(filebuf, marker);
    if(NULL != p)
    {
        lenfirst = (int)(p - filebuf);
        p += sizeof(marker) - 1;
        second = strstr(p, marker);
        erh_assert(NULL != second, CONFIG_FILE_INCONSISTENT, "Inconsistent /etc/hosts.", "");
        second += sizeof(marker) - 1;
        lensecond = &filebuf[filesize] - second;
    }
    else
    {
        lenfirst = filesize;
        lensecond = 0;
        second = NULL;
    }

    // Build config tool part.
    bool data_added = false;
    GString *cfg_host = g_string_sized_new(1024);
    g_string_append(cfg_host, cfgtool_header);

    int pidx;
    for(pidx = 0; pidx < data->no_of_ports; pidx++)
    {
        port_data_t *pdt = data->port_data_list[pidx];
        if(0 == strcmp(pdt->state, "enabled") && 0 == strcmp(pdt->type, "static"))
        {
            g_string_append_printf(cfg_host, "%-16s %s %s\n", pdt->ip_addr, data->host_name, "");
            data_added = true;
        }
    }

    char **hosts = g_strsplit(ct_dnsmasq_get_value(xmlhandle, DNS, NULL, "dns-fix-host", ""), ",", 0);
    int idx;
    for(idx = 0; hosts[idx] != NULL; idx++)
    {
        char **ip_names = g_strsplit(hosts[idx], ":", 0);
        char **names = g_strsplit(ip_names[1], "_", 0);
        int namecount = g_strv_length(names);
        int i;
        g_string_append_printf(cfg_host, "%-16s", ip_names[0]);
        for(i = 0; i < namecount; i++)
        {
            g_string_append_printf(cfg_host, " %s", names[i]);
        }
        g_string_append(cfg_host, "\n");
        data_added = true;
        g_strfreev(ip_names);
        g_strfreev(names);
    }
    g_string_append(cfg_host, marker);

    // Write header, config tool part, trailer to temporary file.
    int status;
    plainfile_t *pfh = fh_open(etchosts, "w");
    erh_assert(pfh != NULL, FILE_OPEN_ERROR, fh_errortxt, "");
    status = fh_write(pfh, filebuf, lenfirst);
    erh_assert(status >= 0, FILE_WRITE_ERROR, fh_errortxt, "");
    if(data_added)
    {
        status = fh_write(pfh, cfg_host->str, cfg_host->len);
        erh_assert(status >= 0, FILE_WRITE_ERROR, fh_errortxt, "");
    }
    status = fh_write(pfh, second, lensecond);
    erh_assert(status >= 0, FILE_WRITE_ERROR, fh_errortxt, "");
    fh_sync(pfh);
    fh_close(pfh);

    // unlock /etc/hosts
    rc = fh_lock(lockfh, LOCK_UN);
    erh_assert(0 == rc, SYSTEM_CALL_ERROR, fh_errortxt, "");

    g_string_free(cfg_host, true);
    g_free(filebuf);
}

#define DNSMASQ_DONT_CHANGE_MARKER  "# KEEP\n"

static char *cfg_dnsmasq_start =
    "# This is a generated file by WAGO configuration tools for DNS and DHCP service.\n"
    "# To prevent from overwriting this file, replace the first line with # KEEP.\n"
    "# Read /etc/dnsmasq.d/dnsmasq_default.conf for meaning of configuration parameters.\n"
    "\n"
    "# DNS part\n"
    "\n"
    "";

static char *cfg_dnsmasq_end =
    "\n"
    "# Include default config file\n"
    "conf-file=/etc/dnsmasq.d/dnsmasq_default.conf\n"
    "conf-file=/etc/dnsmasq.d/dnsmasq_gadget.conf\n"
    "";

// Check if DNSMASQ_MARKER line is present in dnsmasq.conf.
// Accept missing dnsmasq.conf for healing purposes.
static void ct_dnsmasq_assert_conf_not_compromised(char *dnsmasq_conf)
{
    // Read dnsmasq.conf and look for valid start marker in dnsmasq.conf.
    int filesize;
    char *filebuf = cfh_readfile(dnsmasq_conf, &filesize);

    // Look for valid start marker in dnsmasq.conf. Otherwise don't touch the file.
    if(NULL != filebuf)
    {
        char *p = strstr(filebuf, DNSMASQ_DONT_CHANGE_MARKER);
        g_free(filebuf);
        if(NULL != p)
        {
            const char *msg = "Detected manually edited dnsmasq.conf.";
            (void) ct_liblog_reportError(CONFIG_FILE_INCONSISTENT, msg);
            erh_set_error(CONFIG_FILE_INCONSISTENT, msg, "");
        }
    }
}

// Add DHCPD config values of one port to the GString for dnsmasq.conf.
static void add_dhcpd_port_config(dnsmasq_handle_t *xmlhandle, char *port, GString *config, ip_config_t *ip_config_data)
{
    if(0 == strcmp(ct_dnsmasq_get_value(xmlhandle, DHCPD, port, "dhcpd-state", NULL), "enabled"))
    {
        g_string_append_printf(config, "\n# Interface port %s\n", port);
        char **range = g_strsplit(ct_dnsmasq_get_value(xmlhandle, DHCPD, port, "dhcpd-range", NULL), "_", 0);
        char *lease = ct_dnsmasq_get_value(xmlhandle, DHCPD, port, "dhcpd-lease-time", NULL);
        g_string_append_printf(config, "dhcp-range=%s,%s,%sm\n", range[0], range[1], lease);
        g_strfreev(range);

        if(ip_config_data->dsa_state == 0) {
            g_string_append_printf(config, "interface=br0\n");
        } else {
            if(0 == strcmp("X1", port)) {
                g_string_append_printf(config, "interface=br0\n");
            } else if(0 == strcmp("X2", port)) {
                g_string_append_printf(config, "interface=br1\n");
            }
        }

        char **hosts = g_strsplit(ct_dnsmasq_get_value(xmlhandle, DHCPD, port, "dhcpd-fix-host", ""), ",", 0);
        int idx;
        for(idx = 0; hosts[idx] != NULL; idx++)
        {
            char **mac_ip = g_strsplit(hosts[idx], "_", 0);
            g_string_append_printf(config, "dhcp-host=%s,%s\n", mac_ip[0], mac_ip[1]);
            g_strfreev(mac_ip);
        }
        g_strfreev(hosts);
    } else {
        if(0 == strcmp("X1", port)) {
            g_string_append_printf(config, "no-dhcp-interface=br0\n");
        }
        else if(0 == strcmp("X2", port)) {
            g_string_append_printf(config, "no-dhcp-interface=br1\n");
        }
    }
}
 
// Write dnsmasq.conf
static void ct_dnsmasq_generate_conf(dnsmasq_handle_t *xmlhandle, char *dnsmasq_conf_tmp, char **legal_ports, ip_config_t *ip_config_data)
{
    int rc;
    plainfile_t *pfp = fh_open(dnsmasq_conf_tmp, "w");
    erh_assert(NULL != pfp, FILE_OPEN_ERROR, fh_errortxt, "");
    GString *config = g_string_sized_new(1024);
    g_string_append(config, cfg_dnsmasq_start);

    char *dnsstate = ct_dnsmasq_get_value(xmlhandle, DNS, NULL, "dns-state", NULL);
    if(0 == strcmp(dnsstate, "enabled"))
    {
        if(ip_config_data->dsa_state == 0) {
            g_string_append_printf(config, "interface=br0\n");
        } else {
            g_string_append_printf(config, "interface=br0\ninterface=br1\n");
        }
    } else {
        g_string_append(config, "port=0\n");
    }
    char *dnsmode = ct_dnsmasq_get_value(xmlhandle, DNS, NULL, "dns-mode", NULL);
    int dnscache = DNS_DEFAULT_CACHE;
    if(0 == strcmp(dnsmode, "relay"))
    {
        dnscache = 0;
    }
    g_string_append_printf(config, "cache-size=%d\n", dnscache);

    g_string_append(config, "\n# DHCP part\n");
    char *port;
    for(port = *legal_ports; port != NULL; legal_ports++, port = *legal_ports)
    {
        add_dhcpd_port_config(xmlhandle, port, config, ip_config_data);
    }

    g_string_append(config, cfg_dnsmasq_end);
    rc = fh_write(pfp, config->str, config->len);
    erh_assert(rc >= 0, FILE_WRITE_ERROR, fh_errortxt, "");
    fh_sync(pfp);
    fh_close(pfp);
    g_string_free(config, true);
}

// Call /etc/init.d/dnsmasq restart
static void restart_dnsmasqd(char *restart_cmd)
{
    char exitcodestr[8];
    int rc = system(restart_cmd);
    int ecode = WEXITSTATUS(rc);
    snprintf(exitcodestr, sizeof(exitcodestr), "%d", ecode);
    erh_assert(0 == ecode, SYSTEM_CALL_ERROR, "Restarting dnsmasq failed with exit code %s.", exitcodestr);
}

// Rename temporary file to original file name. Rename minimizes the time the original file is
// not available.
static void rename_file(char *filepath, char *tmpfilepath)
{
    int rc;

    rc = rename(tmpfilepath, filepath);
    erh_assert(0 == rc, SYSTEM_CALL_ERROR, "Rename %s failed.", tmpfilepath);
}

//------------------------------------------------------------------------------
// SECTION Main function, command line option handling.
//------------------------------------------------------------------------------

// Main function command codes of config_dnsmasq.
typedef enum {
    NONE = 0,
    GENERATE,
    GET_CONFIG,
    GET_JSON_CONFIG,
    SET_CONFIG
} command_t;

// Program configuration data.
typedef struct {
    command_t command;          // Function to perform.
    char *dbg_root;             // Root directory for testing and debugging.
    char *interfaces_xml_path;  // Path to network-interfaces.xml.
    char *services_xml_path;	// Path to network-services.xml.
    char *etchosts;             // Path to /etc/hosts.
    char *dnsmasq_conf;         // Path to /etc/dnsmasq.conf.
    char *dnsmasq_conf_tmp;     // Path to temporary /etc/dnsmasq.conf.
    char *restart_command;      // Path to /etc/init.d/dnsmasq.
    char *lease_file;           // Path to /var/lib/misc/dnsmasq.leases.
    char *selected_port;        // Port selected for getting/setting DHCPD parameters.
    service_t service_type;     // Configuration type DNS or DHCPD.
    int nodmasqrestart;         // Do not call dnsmasq init script after config setting.
} prgconf_t;

static char *usage_text = 
    "* Get or change the configuration of DHCP or DNS server *\n"
    "\n"
    "Usage:\n"
    "  config_dnsmasq -h                      # Print this usage message.\n"
    "  config_dnsmasq [options] -c            # Generate config files.\n"
    "  config_dnsmasq [options] -g [params]   # Get one or more or all configuration\n"
    "                                         #  parameters.\n"
    "  config_dnsmasq [options] -s param ...  # Set one or more config parameters.\n"
    "\n"
    "Functional command line switches:\n"
    "  -h/--help                  Print this usage message.\n"
    "  -c/--gen-config            Generate dnsmasq config file and /etc/hosts part.\n"
    "  -g/--get-config [param]    Get named configuration parameter.\n"
    "  -s/--set-config param ...  Set one or more of the following config parameters.\n"
    "\n"
    "These are the configuration parameter names and possible values.\n"
    "For the DHCP server\n"
    "  dhcpd-state=<state>        state = disabled | enabled.\n"
    "  dhcpd-range=<range>        range = ip-addr_ip-addr.\n"
    "  dhcpd-lease-time=<minutes> Minimal 2 minutes, default is 120 minutes.\n"
    "  dhcpd-fix-host=<hosts>     hosts = comma sep. list of mac-addr_ip-addr or\n"
    "                             hostname_ip-addr.\n"
    "\n"
    "For the DNS server\n"
    "  dns-state=<state>          state for DNS = disabled | enabled. Default is disabled.\n"
    "  dns-mode=<mode>            mode = relay | proxy. Default is proxy.\n"
    "  dns-fix-host=<dns-hosts>   dns-hosts = comma sep. list ip-addr:hostname[_hostname]\n"
    "\n"
    "Options:\n"
    "  -d/--dhcp                  Configure DHCP server.\n"
    "  -n/--dns                   Configure name service (DNS)\n"
    "  -p/--port=<port-value>     Port name X1 or X2 for DHCP server configuration.\n"
    "  -j/--json                  For get-config: put all output into one JSON format string.\n"
    "                             Parameter names are converted to camel case representation.\n"
    "                             Additionally prints current leases.\n"
    "  -x/--no-dnsmasq-restart    After setting a value do not run /etc/init.d/dnsmasq restart.\n"
    "  -r/dbg-root <path>         Prepend path to config file paths for debugging.\n"
    "  -C/--services-config>      Alternative network-services.xml configuration file.\n"
    "                             Default xml network configuration file is\n"
    "                             " NETWORK_SERVICES_XML "\n"
    "  -o/--dnsmasq-config>       Alternative dnsmasq configuration file.\n"
    "                             Default dnsmasq configuration file is\n"
    "                             " DNSMASQ_CONF "\n"
    "Examples\n"
    "  config_dnsmasq -d -p X2 -s dhcpd-state=enabled dhcpd-range=192.168.2.100_192.168.2.200\n"
    "    Enables DHCP server on port X2 and sets an address range.\n"
    "\n"
    "  config_dnsmasq --dns --get-config\n"
    "    Prints all DNS server configuration parameters as name=value pairs, one per line.\n"
    "    Same parameter names as required for -s.\n"
    "\n"
    "";

static struct option long_options[] = 
{
    { "services-config", required_argument, NULL, 'C' },
    { "interfaces-config", required_argument, NULL, 'i' },
    { "dnsmasq-config", required_argument, NULL, 'o' },
    { "help", no_argument, NULL, 'h' },
    { "port", required_argument, NULL, 'p' },
    { "dns", no_argument, NULL, 'n' },
    { "dhcp", no_argument, NULL, 'd' },
    { "get-config", no_argument, NULL, 'g' },
    { "set-config", no_argument, NULL, 's' },
    { "gen-config", no_argument, NULL, 'c' },
    { "dbg-root", required_argument, NULL, 'r' },
    { "json", no_argument, NULL, 'j' },
    { "no-dnsmasq-restart", no_argument, NULL, 'x' },
};

static char *legal_port_names[] = {
    "X1",
    "X2",
    NULL
};

static bool is_legal_port_name(char *pname)
{
    int i;
    bool valid = false;

    for(i = 0; legal_port_names[i]; i++)
    {
        if(0 == strcmp(legal_port_names[i], pname))
        {
            valid = true;
            break;
        }
    }
    return valid;
}

// Return JSON string with list of DHCPD lease times.
// Format of one line of the leases file:
//   1397224102 00:30:de:40:0d:b6 192.168.2.199 PFC200-400DB6 01:00:30:de:40:0d:b6
static char *json_get_lease_file(uint32_t network, uint32_t netmask, char *lease_file)
{
    int filesize;
    char *retval = "";
    char *filebuf = cfh_readfile(lease_file, &filesize);

    (void) network;
    if(NULL != filebuf)
    {
        GString *result = g_string_new("\"dhcpdLeases\": [");
        gsize empty_len = result->len;
        char **lines = g_strsplit(filebuf, "\n", 0);
        int nlines = g_strv_length(lines);
        int i;
        char *delim = "";
        for(i = 0; i < nlines; i++)
        {
            if(0 < strlen(lines[i]))
            {
                char **fields = g_strsplit(lines[i], " ", 0);
                int nfields =  g_strv_length(fields);
                erh_assert(4 <= nfields, SYSTEM_CALL_ERROR, "Invalid dhcpd lease file.", "");
                uint32_t lease_net;
                int status  = conv_ip_ascii2bin(fields[2], &lease_net);
                erh_assert(SUCCESS == status, SYSTEM_CALL_ERROR, "Invalid ip address in dhcpd lease file.", "");
                if((lease_net & netmask) == network)
                {
                    char timestr[20];
                    struct tm *timestruct;
                    time_t lt = (time_t)strtoul(fields[0], NULL, 10);
                    if(time(NULL) < lt)
                    {
                        timestr[0] = '\0';
                        timestruct = localtime(&lt);
                        strftime(timestr, sizeof(timestr), "%F %T", timestruct);
                        g_string_append_printf(result, "%s{\"ipAddr\": \"%s\", \"macAddr\": \"%s\", ", delim, fields[2], fields[1]);
                        g_string_append_printf(result, "\"hostName\": \"%s\", \"endOfLease\": \"%s\"}", fields[3], timestr);
                        delim = ", ";
                    }
                }
                g_strfreev(fields);
            }
        }
        g_strfreev(lines);
        g_string_append(result, "]");
        if(result->len > empty_len + 1)
        {
            retval = result->str;
        }
    }
    return retval;
}

static void json_print_dhcpd_conf(dnsmasq_handle_t *handle, port_data_t *pd, char *lease_file)
{
    xml_param_t *par;
    char *delim = "";
    printf("\"%s\": {", pd->port_name);
    for(par = xml_get_next_par(NULL, DHCPD, true); par->pname != NULL; par = xml_get_next_par(par, DHCPD, true))
    {
        char *val = ct_dnsmasq_get_value(handle, DHCPD, pd->port_name, par->pname, "");
        if(0 == strcmp(par->pname, "dhcpd-fix-host"))
        {
            // List of static DHCP MAC or host name to ip address conversions.
            // Example: 11:22:33:44:55:66_192.168.2.22,hostname_192.168.2.35 leads to
            //          "dhcpdFixHost": [{"hostMac": "11:22:33:44:55:66", "ipAddr": "192.168.2.22"}, 
            //                           {"hostMac": "hostname", "ipAddr": "192.168.2.35"}]
            char *subdelim = "";
            char **hlist = g_strsplit(val, ",", 0);
            int hlistlen = g_strv_length(hlist);
            int i;
            printf("%s\"%s\": [", delim, par->pjname);
            for(i = 0; i < hlistlen; i++)
            {
                char **mac_ip = g_strsplit(hlist[i], "_", 0);
                printf("%s{\"hostMac\": \"%s\", \"ipAddr\": \"%s\"}", subdelim, mac_ip[0], mac_ip[1]);
                g_strfreev(mac_ip);
                subdelim = ", ";
            }
            printf("]");
        }
        else
        {
            printf("%s\"%s\": \"%s\"", delim, par->pjname, val);
        }
        delim = ", ";
    }
    char *lease = json_get_lease_file(pd->network_bin, pd->netmask_bin, lease_file);
    if('\0' != *lease)
    {
        printf(", %s", lease);
    }
    printf("}");
}

static void json_print_dns_conf(dnsmasq_handle_t *handle)
{
    xml_param_t *par;
    char *delim = "";
    printf("{");
    for(par = xml_get_next_par(NULL, DNS, true); par->pname != NULL; par = xml_get_next_par(par, DNS, true))
    {
        char *val = ct_dnsmasq_get_value(handle, DNS, "", par->pname, "");
        if(0 == strcmp(par->pname, "dns-fix-host"))
        {
            // List of static DNS ip address / hostnames
            // Example: 192.168.1.22:host1_host2_host3,192.168.2.44:hostname leads to
            //          "dnsFixHost": [{"ipAddr": "192.168.1.22", "names": ["host1", "host2", "host3"]},
            //           {"ipAddr": "192.168.2.44", "names": ["hostname"]}]
            char *subdelim = "";
            char **hlist = g_strsplit(val, ",", 0);
            int hlistlen = g_strv_length(hlist);
            int i;
            printf("%s\"%s\": [", delim, par->pjname);
            for(i = 0; i < hlistlen; i++)
            {
                char *subsubdelim = "";
                char **ip_name = g_strsplit(hlist[i], ":", 0);
                printf("%s{\"ipAddr\": \"%s\", \"names\": [", subdelim, ip_name[0]);

                char **namelist = g_strsplit(ip_name[1], "_", 0);
                int namelistlen = g_strv_length(namelist);
                int j;
                for(j = 0; j < namelistlen; j++)
                {
                    printf("%s\"%s\"", subsubdelim, namelist[j]);
                    subsubdelim = ", ";
                }
                printf("]}");
                g_strfreev(namelist);
                g_strfreev(ip_name);
                subdelim = ", ";
            }
            printf("]");
        }
        else
        {
            printf("%s\"%s\": \"%s\"", delim, par->pjname, val);
        }
        delim = ", ";
    }
    printf("}\n");
}

static void cmd_assert(bool cond, char *emsg)
{
    erh_assert(cond, INVALID_PARAMETER, emsg, "");
}

// Evaluate command line options. Get and set (-g, -s) do not require option arguments because
// these functions will interpret non option arguments as list of settings or list of
// config parameter names to print.
// Returns optind.
static int eval_options(int argc, char **argv, prgconf_t *prgconf)
{
    int c;
    prgconf->dbg_root = NULL;
    prgconf->selected_port = "";
    GString *services_xml = g_string_new(NETWORK_SERVICES_XML);
    GString *interfaces_xml = g_string_new(NETWORK_INTERFACES_XML);
    GString *etchosts = g_string_new(ETC_HOSTS);
    GString *dnsmasq_conf = g_string_new(DNSMASQ_CONF);
    GString *dnsmasq_conf_tmp = g_string_new(DNSMASQ_CONF_TMP);
    GString *restart_command = g_string_new(DNSMASQ_RESTART_CMD);
    GString *lease_file = g_string_new(DNSMASQ_LEASE_FILE);
    int option_index = 0;

    while(-1 != (c = getopt_long(argc, argv, "C:i:o:p:hndgsr:cjx", long_options, &option_index)))
    {
        char *err_servtype = "Double service type command line option.";
        char *err_func = "Double function select command line option.";
        char *err_mport = "Command line: multiple port specification.";
        char *err_moption = "Command line: multiple options detected.";
        switch(c)
        {
        case 'r':
            cmd_assert(prgconf->dbg_root == NULL, err_moption);
            prgconf->dbg_root = optarg;
            break;
        case 'C':
            cmd_assert(0 == strcmp(NETWORK_SERVICES_XML, services_xml->str), err_moption);
            (void) g_string_assign(services_xml, optarg);
            break;
        case 'o':
            cmd_assert(0 == strcmp(DNSMASQ_CONF, dnsmasq_conf->str), err_moption);
            (void) g_string_assign(dnsmasq_conf, optarg);
            cmd_assert(0 == strcmp(DNSMASQ_CONF_TMP, dnsmasq_conf_tmp->str), err_moption);
            (void) g_string_assign(dnsmasq_conf_tmp, optarg);
            (void) g_string_append(dnsmasq_conf_tmp, ".tmp");
            break;
        case 'i':
            cmd_assert(0 == strcmp(NETWORK_INTERFACES_XML, interfaces_xml->str), err_moption);
            (void) g_string_assign(interfaces_xml, optarg);
            break;
        case 'x':
            prgconf->nodmasqrestart = 1;
            break;
        case 'n':
            cmd_assert(prgconf->service_type == UNDEF, err_servtype);
            prgconf->service_type = DNS;
            break;
        case 'd':
            cmd_assert(prgconf->service_type == UNDEF, err_servtype);
            prgconf->service_type = DHCPD;
            break;
        case 'p':
            cmd_assert(*(prgconf->selected_port) == '\0', err_mport);
            prgconf->selected_port = optarg; 
            break;
        case 'j':
            prgconf->command = GET_JSON_CONFIG;
            break;
        case 'g':
            prgconf->command = GET_CONFIG;
            break;
        case 's':
            prgconf->command = SET_CONFIG;
            break;
        case 'c':
            prgconf->command = GENERATE;
            break;
        case 'h':
            printf(usage_text);
            exit(0);
            /*NOTREACHED*/
        default:
            erh_set_error(INVALID_PARAMETER, "Illegal command line option", "");
            break;
        }
    }

    // Check for valid function command code.
    cmd_assert(prgconf->command != NONE, "Missing one of command line options -c, -g, -s, j");

    // Prepend config file paths with debug root path.
    if(NULL != prgconf->dbg_root)
    {
        g_string_prepend(services_xml, prgconf->dbg_root);
        g_string_prepend(interfaces_xml, prgconf->dbg_root);
        g_string_prepend(etchosts, prgconf->dbg_root);
        g_string_prepend(dnsmasq_conf, prgconf->dbg_root);
        g_string_prepend(dnsmasq_conf_tmp, prgconf->dbg_root);
        g_string_prepend(restart_command, prgconf->dbg_root);
        g_string_prepend(lease_file, prgconf->dbg_root);
    }

    // Copy file paths to program config structure.
    prgconf->services_xml_path = g_strdup(services_xml->str);
    prgconf->interfaces_xml_path = g_strdup(interfaces_xml->str);
    prgconf->etchosts = g_strdup(etchosts->str);
    prgconf->dnsmasq_conf = g_strdup(dnsmasq_conf->str);
    prgconf->dnsmasq_conf_tmp = g_strdup(dnsmasq_conf_tmp->str);
    prgconf->restart_command = g_strdup(restart_command->str);
    prgconf->lease_file = g_strdup(lease_file->str);
    g_string_free(services_xml, true);
    g_string_free(interfaces_xml, true);
    g_string_free(etchosts, true);
    g_string_free(dnsmasq_conf, true);
    g_string_free(dnsmasq_conf_tmp, true);
    g_string_free(restart_command, true);
    g_string_free(lease_file, true);
    return optind;
}

// main: call option evaluation and execute selected function.
int main(int argc, char **argv)
{
    ip_config_t ip_config_data;
    dnsmasq_handle_t *dnsmasq_session;
    int debugmode = false;
    prgconf_t prgconf;
    memset(&prgconf, 0, sizeof(prgconf));

    // Check for running as root and evaluate command line.
    erh_init(basename(argv[0]));
    int loptind = eval_options(argc, argv, &prgconf);
    char *port = prgconf.selected_port;
    service_t serv = prgconf.service_type;
    if(NULL == prgconf.dbg_root)
    {
        erh_assert(0 == getuid(), SYSTEM_CALL_ERROR, "Not running as root. Abort.", "");
    }
    else
    {
        debugmode = true;
    }
    
    // Assert unmodified config file and valid port name, if given.
    // Open xml file session to network-services.xml, read all required configuration data.
    ct_dnsmasq_assert_conf_not_compromised(prgconf.dnsmasq_conf);
    dnsmasq_session = ct_dnsmasq_start_session(prgconf.services_xml_path);
    memset(&ip_config_data, 0, sizeof(ip_config_data));
    netcfg_read_settings(&ip_config_data, prgconf.interfaces_xml_path, legal_port_names, debugmode);
    if('\0' != *port)
    {
        erh_assert(is_legal_port_name(port), INVALID_PARAMETER,
                   "Option -p, invalid port name '%s'", port);
    }

    // Start selected program function.
    if(GET_CONFIG == prgconf.command)
    {
        erh_assert(serv != UNDEF, MISSING_PARAMETER, "Missing command line option -d or -n", "");
        if(DHCPD == serv)
        {
            erh_assert('\0' != *port, INVALID_PARAMETER, "Missing port option -p", "");
        }
        if(argc > loptind)
        {
            // Further command line arguments available. Get configuration parameters.
            int idx;
            for(idx = loptind; idx < argc; idx++)
            {
                char *deflt = NULL;
                if(0 == strcmp(argv[idx], "dhcpd-fix-host") || 0 == strcmp(argv[idx], "dns-fix-host"))
                {
                    // Allow for empty parameter.
                    deflt = "";
                }
                printf("%s\n", ct_dnsmasq_get_value(dnsmasq_session, serv, port, argv[idx], deflt));
            }
        }
        else
        {
            // No parameter selection present. Print all config parameters.
            xml_param_t *par;
            for(par = xml_get_next_par(NULL, serv, true); par->pname != NULL; par = xml_get_next_par(par, serv, true))
            {
                printf("%s=%s\n", par->pname, ct_dnsmasq_get_value(dnsmasq_session, serv, port, par->pname, ""));
            }
        }
    }
    else if(GET_JSON_CONFIG == prgconf.command)
    {
        // Print configuration in JSON format for WBM.
        erh_assert(serv != UNDEF, MISSING_PARAMETER, "Missing command line option -d or -n", "");
        if(DHCPD == serv)
        {
            int portcount = 0;
            char **portnames = netcfg_get_port_names(&ip_config_data, &portcount);
            char *delim = "";
            int i;
            printf("{");
            for(i = 0; i < portcount; i++)
            {
                printf(delim);
                port_data_t *pd = netcfg_get_port_data(&ip_config_data, portnames[i]);
                json_print_dhcpd_conf(dnsmasq_session, pd, prgconf.lease_file);
                delim = ", ";
            }
            printf("}\n");
        }
        else
        {
            json_print_dns_conf(dnsmasq_session);
        }
    }
    else if(SET_CONFIG == prgconf.command)
    {
        // First check and adapt configuration parameters to possibly changed network interface settings.
        erh_assert(serv != UNDEF, MISSING_PARAMETER, "Missing command line option -d or -n", "");
        ct_dnsmasq_check_ip_dependencies(&ip_config_data, dnsmasq_session, legal_port_names);

        // Set configuration parameters and restart dnsmasq.
        // Argv holds parameter_name=value pairs to be set.
        if(DHCPD == serv)
        {
            erh_assert('\0' != *port, MISSING_PARAMETER, "Missing port option -p", "");
        }

        int idx;
        for(idx = loptind; idx < argc; idx++)
        {
            nvpair_t *param = ct_dnsmasq_check_input(argv[idx], serv, port, &ip_config_data);
            ct_dnsmasq_set_value(dnsmasq_session, serv, port, param->name, param->value);
        }
        ct_dnsmasq_save_xml_file(dnsmasq_session, NULL);

        if(!prgconf.nodmasqrestart)
        {
            restart_dnsmasqd(prgconf.restart_command);
        }
    }
    else if(GENERATE == prgconf.command)
    {
        // Create the dnsmasq configuration file and edit /etc/hosts.
        // First save xml file. Could be changed due to changes of network interfaces.
        // This function is typically called from the dnsmasq init script on dnsmasq startup.
        ct_dnsmasq_check_ip_dependencies(&ip_config_data, dnsmasq_session, legal_port_names);
        ct_dnsmasq_save_xml_file(dnsmasq_session, NULL);
        ct_dnsmasq_edit_etc_hosts(dnsmasq_session, &ip_config_data, prgconf.etchosts);
        ct_dnsmasq_generate_conf(dnsmasq_session, prgconf.dnsmasq_conf_tmp, legal_port_names, &ip_config_data);
        rename_file(prgconf.dnsmasq_conf, prgconf.dnsmasq_conf_tmp);
    }
    ct_dnsmasq_finish_session(dnsmasq_session);
    return 0;
}
