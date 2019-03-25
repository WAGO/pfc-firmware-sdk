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
///  \file     ipdatacheck.c
///
///  \version  $Revision: 25225 $
///
///  \brief    Check functions of network parameters.
///
///  Implemented so far:
///     --subnet-check:     Checks an ip address against the subnet of
///                         given interface names
///
///  \author   Hoffmann, Hans-Josef
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
#include <libgen.h>             // for basename()

#include <glib.h>               // Gnome GLib interfaces

#include "libnet/ct_xml.h"
#include "libnet/ct_libnet.h"
#include "liblog/ct_liblog.h"   // for error codes
#include "config_tool_lib.h"

#define SIZEOF_IPADDR           16

//------------------------------------------------------------------------------
// SECTION Error handling.
//------------------------------------------------------------------------------

typedef struct {
    enum eStatusCode code;
    char *text;
} erh_code_to_message_t;

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
// SECTION Misc functions.
//------------------------------------------------------------------------------

// String to uint32_t.
// If err != NULL, *err is set to 1 on errors.
static uint32_t ascto32(char *str, int *err)
{
    uint32_t res = 0;
    uint32_t newres;
    uint32_t dig;
    int idx = 0;
    int c;
    int rc = 1;

    if (*str) {
        while ((c = *str++) != '\0' && idx++ < 10) {
            if (c >= '0' && c <= '9') {
                newres = res * 10;
                if (newres / 10 != res) break;
                res = newres + (c - '0');
                if (res < newres) break;
            } else break;
        }
        if (c == '\0') rc = 0;
        else res = 0;
    }
    if (err != NULL) *err = rc;
    return res;
}

//------------------------------------------------------------------------------
// SECTION Network interface configuration data.
//------------------------------------------------------------------------------

#define MAX_PORTS  8

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
    port_data_t *port_data_list[MAX_PORTS];
} ip_config_t;

// Convert IP-Address from dotted ASCII string to uint32_t.
// Did not use inet_aton() in favour of more strict format checks.
// Set resultp to NULL for checking only.
static int conv_ip_ascii2bin(char *ipaddr, uint32_t *resultp)
{
    int i;
    char **decimalparts = g_strsplit(ipaddr, ".", 0);
    int status = SUCCESS;
    uint32_t result = 0;

    if (g_strv_length(decimalparts) == 4)
    {
        for(i = 0; i < 4; i++)
        {
            char *p = decimalparts[i];
            uint32_t part;
            if (*p != 0)
            {
                while (*p != '\0')
                {
                    if (!isdigit(*p))
                    {
                        status = INVALID_PARAMETER;
                        break;
                    }
                    p++;
                }
                if (status != SUCCESS)
                {
                    break;
                }
                part = (uint32_t)strtoul(decimalparts[i], NULL, 10);
                if (part <= 255)
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
    if (status == SUCCESS && resultp != NULL)
    {
        *resultp = result;
    }
    return status;
}

// Check netmask from dotted ASCII string to uint32_t.
// valid if netmask is a group of 'ones' (MSB) and following bits are zero
static int conv_netmask2dec(char *netmask, uint32_t *mask_dec)
{
    char **decimalparts = g_strsplit(netmask, ".", 0);
    int status = SUCCESS;
    int i=0;
    bool found_first_zero = false;
    uint32_t return_decimal_mask = 0;

    if (g_strv_length(decimalparts) == 4)
    {
        for (i=0; i<4; i++)
        {
            char *n = decimalparts[i];
            uint32_t part;
            if (*n != 0)
            {
                while(*n != '\0')
                {
                    if (!isdigit(*n))
                    {
                        status = INVALID_PARAMETER;
                        break;
                    }
                    n++;
                }

                if (status != SUCCESS)
                {
                    break;
                }

                part = (uint32_t) strtoul(decimalparts[i], NULL, 10);
                if (part <=255) // check if in range
                {

                    if ((part == 255) && (found_first_zero == false))
                    {
                        //keep going
                        return_decimal_mask += 8;
                    }
                    else if (found_first_zero == false) 
                    {
                        //check each bit
                        int bit = 7;
                        for (bit; bit >=0; bit--)
                        {
                            if (( (part >> bit) & 0x01) == 0x01)
                            {
                                if (found_first_zero == false)
                                {
                                    return_decimal_mask++;
                                }
                                else
                                {
                                    status = INVALID_PARAMETER;
                                    break;
                                }
                            }
                            else // from this point only 0x00 are allowed
                            {
                                found_first_zero = true;
                            }
                        }
                    }
                    else if ((found_first_zero == true) && (part != 0))
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

    if (status == SUCCESS && mask_dec !=NULL)
    {
        *mask_dec = return_decimal_mask;
    }

    g_strfreev(decimalparts);
    return status;
}


// Read some configuration parameters of all network interfaces into memory data structures.
static void netcfg_read_settings(ip_config_t *data, char *interfaces_xml_path, int debugmode)
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
        if (gethostname(buf, sizeof(buf)) != 0)
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
    data->port_name_list = g_malloc0((MAX_PORTS + 1) * sizeof(char *));
    if (data->no_of_ports >= MAX_PORTS)
    {
        snprintf(buf, sizeof(buf), "%d", MAX_PORTS);
        erh_set_error(CONFIG_FILE_INCONSISTENT, "Found more than %s ports in network-interfaces.xml", buf);
        data->no_of_ports++;
    }

    // Read port specific parameters, IP-Address, Netmask.
    int i;
    for (i = 0; i < ip_port_count; i++)
    {
        int portindex = data->no_of_ports;
        data->port_name_list[data->no_of_ports] = ip_port_list[i];
        data->no_of_ports++;

        // Copy port name.
        port_data_t *pd = g_malloc(sizeof(port_data_t));
        data->port_data_list[portindex] = pd;
        pd->port_name = g_strdup(data->port_name_list[portindex]);

        // Get ethernet port state
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
        buf[0] = '\0';
        rc = ct_libnet_get_ip_addr_from_hw(pd->port_name, buf, sizeof(buf), IP_FORMAT_MODE_NO_NETMASK, libnet_session);
        if (rc == SUCCESS)
        {
            pd->ip_addr = g_strdup(buf);
        }
        else
        {
            pd->ip_addr = g_strdup("0.0.0.0");
        }

        // Get netmask from interface config
        char netmask[SIZEOF_IPADDR];
        netmask[0] = '\0';
        rc = ct_libnet_get_netmask_from_hw(pd->port_name, netmask, sizeof(netmask), NETMASK_FORMAT_MODE_CLASSIC, libnet_session);
        if (rc == SUCCESS)
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

    // Terminate session to libnet.
    ct_libnet_finish_session(libnet_session);
}

// Debug print ip data structure
static void netcfg_print_settings(ip_config_t *ipd)
{
    port_data_t *pd;
    int i;

    printf("Network config data:\n");
    printf("  DSA state = %d\n", ipd->dsa_state);
    printf("  Hostname = %s\n", ipd->host_name);
    for (i = 0; i < ipd->no_of_ports; i++)
    {
        pd = ipd->port_data_list[i];
        printf("\n  Portname   = %s\n", pd->port_name);
        printf("    State      = %s\n", pd->state);
        printf("    Type       = %s\n", pd->type);
        printf("    IP address = %s\n", pd->ip_addr);
        printf("    Netmask    = %s\n", pd->netmask);
        printf("    IP address hex = 0x%08X\n", pd->ip_addr_bin);
        printf("    Netmask hex    = 0x%08X\n", pd->netmask_bin);
        printf("    Network hex    = 0x%08X\n", pd->network_bin);
    }
}

// Return the list of port names as get from network interface config.
static char **netcfg_get_port_names(ip_config_t *data, int *count)
{
    *count = data->no_of_ports;
    return data->port_name_list;
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
// SECTION Main function, command line option handling.
//------------------------------------------------------------------------------

// Main function command codes.
typedef enum {
    NONE = 0,
    CHECKSUBNETS,
    VERIFY_IP,
    VERIFY_UNSIGNED,
    VERIFY_HOSTNAME,
    VERIFY_DOMAIN,
    VERIFY_NETMASK,
    VERIFY_ROUTE
} command_t;

// Program configuration data.
typedef struct {
    command_t command;          // Function to perform.
    char *dbg_root;             // Root directory for testing and debugging.
    char *interfaces_xml_path;  // Path to network-interfaces.xml.
} prgconf_t;

static char *usage_text = 
    "* Support checks on network parameters *\n"
    "\n"
    "Usage:\n"
    "  ipdatacheck -h                                            Print this usage message.\n"
    "  ipdatacheck -s/--subnet-check <gw-ip>                     Check if IP fits at least to one interface.\n"
    "  ipdatacheck -i/--verify-ip <ip-address>                   Check for valid ip address.\n"
    "  ipdatacheck -u/--verify-unsigned <v>                      Check for valid unsigned 32 bit number.\n"
    "  ipdatacheck -n/--verify-hostname <host>                   Check for valid hostname.\n"
    "  ipdatacheck -d/--verify-domain <domain>                   Check for valid dns domain name.\n"
    "  ipdatacheck -m/--verify-netmask <mask>                    Check for valid netmask.\n"
    "  ipdatacheck -r/--verify-route-dest <dest-ip> <dest-mask>  Check for valid route destination ip and mask.\n"
    "\n"
    "  Exit code 1 if a check does not match\n"
    "";

static struct option long_options[] = 
{
    { "subnet-check", no_argument, NULL, 's' },
    { "verify-ip", no_argument, NULL, 'i' },
    { "verify-unsigned", no_argument, NULL, 'u' },
    { "verify-hostname", no_argument, NULL, 'n' },
    { "verify-domain", no_argument, NULL, 'd' },
    { "verify-netmask", no_argument, NULL, 'm'},
    { "verify-route-dest", no_argument, NULL, 'r'},
    { "help", no_argument, NULL, 'h' }
};

// Evaluate command line options.
// Returns optind.
static int eval_options(int argc, char **argv, prgconf_t *prgconf)
{
    int c;
    prgconf->dbg_root = NULL;
    int option_index = 0;
    GString *interfaces_xml = g_string_new(NETWORK_INTERFACES_XML);

    while((c = getopt_long(argc, argv, "siundmrh", long_options, &option_index)) != -1)
    {
        switch(c)
        {
        case 's':
            prgconf->command = CHECKSUBNETS;
            break;
        case 'i':
            prgconf->command = VERIFY_IP;
            break;
        case 'u':
            prgconf->command = VERIFY_UNSIGNED;
            break;
        case 'n':
            prgconf->command = VERIFY_HOSTNAME;
            break;
        case 'd':
            prgconf->command = VERIFY_DOMAIN;
            break;
        case 'm':
            prgconf->command = VERIFY_NETMASK;
            break;
        case 'r':
            prgconf->command = VERIFY_ROUTE;
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

    // Prepend config file paths with debug root path.
    if(NULL != prgconf->dbg_root)
    {
        g_string_prepend(interfaces_xml, prgconf->dbg_root);
    }

    // Copy file paths to program config structure.
    prgconf->interfaces_xml_path = g_strdup(interfaces_xml->str);
    g_string_free(interfaces_xml, true);
    return optind;
}

// main: call option evaluation and execute selected function.
int main(int argc, char **argv)
{
    static ip_config_t ip_config_data;
    static prgconf_t prgconf;
    int debugmode = false;
    ip_config_t *ipd = &ip_config_data;
    int exitcode = 0;

    // Check for running as root and evaluate command line.
    erh_init(basename(argv[0]));
    int loptind = eval_options(argc, argv, &prgconf);
    argc -= loptind;
    argv += loptind;
    if(NULL == prgconf.dbg_root)
    {
        erh_assert(0 == getuid(), SYSTEM_CALL_ERROR, "Not running as root. Abort.", "");
    }
    else
    {
        debugmode = true;
    }
    
    memset(&ip_config_data, 0, sizeof(ip_config_data));
    netcfg_read_settings(ipd, prgconf.interfaces_xml_path, debugmode);

    // Start selected program function. argc holds the count of the remaining arguments. argv[] the
    // arguments, starting at index 0.
    if (prgconf.command == CHECKSUBNETS)
    {
        uint32_t gw;
        int i;
        int found = 0;
        int ifcount;
        char **ifnamelist;

        erh_assert(argc >= 1, MISSING_PARAMETER, "Missing gateway address.", "");
        erh_assert(conv_ip_ascii2bin(argv[0], &gw) == 0, INVALID_PARAMETER, "Invalid gateway ip address", "");
        ifnamelist = netcfg_get_port_names(ipd, &ifcount);
        for (i = 0; i < ifcount; i++)
        {
            port_data_t *pd = netcfg_get_port_data(ipd, ifnamelist[i]);
            if ((gw & pd->netmask_bin) == pd->network_bin)
            {
                found = 1;
            }
        }
        exitcode = found ^ 1;
    }
    else if (prgconf.command == VERIFY_IP)
    {
        int rc;

        erh_assert(argc >= 1, MISSING_PARAMETER, "Missing ip address.", "");
        rc = conv_ip_ascii2bin(argv[0], NULL);
        exitcode = (rc == 0 ? 0 : 1);
    }
    else if (prgconf.command == VERIFY_UNSIGNED)
    {
        erh_assert(argc >= 1, MISSING_PARAMETER, "Missing value.", "");
        (void)ascto32(argv[0], &exitcode);
    }
    else if (prgconf.command == VERIFY_DOMAIN)
    {
        exitcode = 1;
        erh_assert(argc >= 1, MISSING_PARAMETER, "Missing dns domain name.", "");
        if (ctlib_VerifyDomainName(argv[0]))
        {
            exitcode = 0;
        }
    }
    else if (prgconf.command == VERIFY_HOSTNAME)
    {
        exitcode = 1;
        erh_assert(argc >= 1, MISSING_PARAMETER, "Missing hostname.", "");
        if (strstr(argv[0], ".") == NULL && ctlib_VerifyDomainName(argv[0]))
        {
            exitcode = 0;
        }
    }
    else if (prgconf.command == VERIFY_NETMASK)
    {
        int returncode;
        erh_assert(argc >= 1, MISSING_PARAMETER, "Missing netmask." , "");
        returncode = conv_netmask2dec(argv[0], NULL);
        exitcode = (returncode == 0 ? 0 : 1);
    }
    else if (prgconf.command == VERIFY_ROUTE)
    {
        int returncode;
        erh_assert(argc >= 2, MISSING_PARAMETER, "Missing destination ip and destination mask." , "");

        uint32_t net_ip;
        uint32_t netmask;
        returncode = conv_ip_ascii2bin(argv[0], &net_ip);
        erh_assert(SUCCESS == returncode , INVALID_PARAMETER, "Invalid destination ip." , "");
        returncode = conv_ip_ascii2bin(argv[1], &netmask);
        erh_assert(SUCCESS == returncode , INVALID_PARAMETER, "Invalid destination mask." , "");
        exitcode = ((net_ip & netmask) == net_ip) ? 0 : 1;
    }
    return exitcode;
}
