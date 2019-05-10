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
///  \file     config_mdmd.c
///
///  \version  $Revision: 38420 $
///
///  \brief    Configuration tool for WAGO Modem Management Daemon. 
///
///  \author   KNu : WAGO Kontakttechnik GmbH & Co. KG
///  \author   PEn : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include <assert.h>
#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <libgen.h>             // for basename()
#include <glib.h>
#include <gio/gio.h>
#include "liblog/ct_liblog.h"
#include "ct_dbus.h"
#include <typelabel_API.h>
#include <diagnostic/diagnostic_API.h>
#include <diagnostic/mdmd_diag.h>

#define MODEM_VERSION_FROM_FILE_LENGTH 15
#define MODEM_VERSION_FIRST_PART_LENGTH 7
#define MODEM_VERSION_SECOND_PART_LENGTH 6
#define MODEM_VERSION_COMPARE_LENGTH MODEM_VERSION_FIRST_PART_LENGTH + MODEM_VERSION_SECOND_PART_LENGTH

//------------------------------------------------------------------------------
// SECTION Common objects and functions.
//------------------------------------------------------------------------------
typedef struct MdmDBusClient
{
  GDBusProxy *gDBusProxy;
  guint32    timeout;
  gchar      *lastError;
} tMdmDBusClient;

static tMdmDBusClient _mdmDBusClient = {NULL,0,NULL};

static void MdmDBusClientInit(const gchar     *name,
                              const gchar     *objectPath,
                              const gchar     *interfaceName,
                              guint32         timeout)
{
  _mdmDBusClient.gDBusProxy = g_dbus_proxy_new_for_bus_sync( G_BUS_TYPE_SYSTEM,
                                                             G_DBUS_PROXY_FLAGS_NONE,
                                                             0,
                                                             name,
                                                             objectPath,
                                                             interfaceName,
                                                             0,
                                                             0);
  _mdmDBusClient.timeout = timeout;
}

static void MdmDBusClientSetLastError(const char *errmsg)
{
  if (_mdmDBusClient.lastError)
  {
    g_free(_mdmDBusClient.lastError);
  }
  _mdmDBusClient.lastError = g_strdup(errmsg);
}

static gint MdmDBusClientCallSync(const char     *methodName,
                                  GVariant       *parameters,
                                  GVariant       **result)
{
  GError *err = NULL;
  *result = g_dbus_proxy_call_sync(_mdmDBusClient.gDBusProxy,
                                   methodName,
                                   parameters,
                                   G_DBUS_CALL_FLAGS_NONE,
                                   _mdmDBusClient.timeout,
                                   0,
                                   &err);

  if (err)
  {
    MdmDBusClientSetLastError(err->message);
    g_error_free(err);
    return -1;
  }
  return 0;
}

static void MdmDBusClientFree()
{
  if (_mdmDBusClient.gDBusProxy)
  {
    g_object_unref(_mdmDBusClient.gDBusProxy);
  }
  if (_mdmDBusClient.lastError)
  {
    g_free(_mdmDBusClient.lastError);
  }
}


typedef struct MdmOper
{
  gint selmode;
  gint id;
  gint act;
  gint state;
  gint quality;
  gchar *name;
  gchar *lac;
  gchar *cid;
} tMdmOper;

static void MdmOperInit(tMdmOper *oper)
{
  oper->selmode = 0; /*AUTOMATIC*/
  oper->id = 0; /*NOT DEFINED*/
  oper->act = 0; /*GSM*/
  oper->state = 0;  /*STOPPED / UNKNOWN*/
  oper->quality = 0;
  oper->name = NULL;
  oper->lac = NULL;
  oper->cid = NULL;
}

static void MdmOperFree(tMdmOper *oper)
{
  if (oper->name) 
  {
    g_free(oper->name);
  }
  if (oper->lac)
  {
    g_free(oper->lac);
  }
  if (oper->cid)
  {
    g_free(oper->cid);
  }
}


typedef struct MdmGprsAccess
{
  gchar  *apn;
  gint   auth;
  gchar  *user;
  gchar  *pass;
  gint   state;
  gint   connectivity;
  gchar  *pdp_addr;
} tMdmGprsAccess;

static void MdmGprsAccessInit(tMdmGprsAccess *gprsAccess)
{
  gprsAccess->apn = NULL;
  gprsAccess->auth = 0; /*NONE*/
  gprsAccess->user = NULL;
  gprsAccess->pass = NULL;
  gprsAccess->state = 0; /*STOPPED*/
  gprsAccess->connectivity = 1; /*ENABLED*/
  gprsAccess->pdp_addr = NULL;
}

static void MdmGprsAccessFree(tMdmGprsAccess *gprsAccess)
{
  if (gprsAccess->apn) 
  {
    g_free(gprsAccess->apn);
  }
  if (gprsAccess->user)
  {
    g_free(gprsAccess->user);
  }
  if (gprsAccess->pass)
  {
    g_free(gprsAccess->pass);
  }
  if (gprsAccess->pdp_addr)
  {
    g_free(gprsAccess->pdp_addr);
  }
}


typedef struct IntToStringMap
{
  gint id;
  const gchar *name;
} tIntToStringMap;

static void MapIntToString (const tIntToStringMap *map,
                            gint id,
                            GString *name)
{
  while((map->id != -1) && (map->id != id))
  {
    map++;
  }
  g_string_assign(name, map->name);
}

static void MapStringToInt (const tIntToStringMap *map,
                            const char *name,
                            gint *id)
{
  while((map->id != -1) && (g_strcmp0(name, map->name)))
  {
    map++;
  }
  *id = map->id;
}

//------------------------------------------------------------------------------
// SECTION Error handling.
//------------------------------------------------------------------------------
typedef struct {
  enum eStatusCode code;
  char *text;
} tCodeToErrorMessage;

typedef struct SmsStorageConfig 
{
  gchar  *mem1;
  gint   user1;
  gint   total1;
  gchar  *mem2;
  gint   user2;
  gint   total2;
  gchar  *mem3;
  gint   user3;
  gint   total3;
} tSmsStorageConfig;

typedef struct SmsReportingConfig
{
  gint   mode;
  gint   mt;
  gint   bm;
  gint   ds;
  gint   bfr;
} tSmsReportingConfig;

static void SmsStorageConfigInit(tSmsStorageConfig* storageConfig)
{
  storageConfig->mem1 = NULL;
  storageConfig->user1 = 0;
  storageConfig->total1 = 0;
  storageConfig->mem2 = NULL;
  storageConfig->user2 = 0;
  storageConfig->total2 = 0;
  storageConfig->mem3 = NULL;
  storageConfig->user3 = 0;
  storageConfig->total3 = 0;
}

static void SmsStorageConfigFree(tSmsStorageConfig* storageConfig)
{
  g_free(storageConfig->mem1);
  g_free(storageConfig->mem2);
  g_free(storageConfig->mem3);
}

static void SmsEventReportingConfigInit(tSmsReportingConfig* reportingConfig)
{
  reportingConfig->mode = 0;
  reportingConfig->mt = 0;
  reportingConfig->bm = 0;
  reportingConfig->ds = 0;
  reportingConfig->bfr = 0;
}

static gchar *_programmName = NULL;

// Table to convert error codes to text. TODO put into more general support module.
static tCodeToErrorMessage _codeToMessage[] = {
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

static void ExitProgramm(gint code)
{
  if (_programmName) 
  {
    g_free(_programmName);
  }
  MdmDBusClientFree();
  exit(code);
}

static void ExitWithError(enum eStatusCode code, const char *msg)
{
  GString *message;
  tCodeToErrorMessage *p = _codeToMessage;
  while((p->code != SUCCESS) && (p->code != code))
  {
    p++;
  }

  message = g_string_new(_programmName);
  
  if (msg)
  {
    const char *gDBusErrorPrefix = "GDBus.Error:de.wago.mdmdError: ";
    int offset = g_str_has_prefix(msg, gDBusErrorPrefix) ? strlen(gDBusErrorPrefix) : 0;
    g_string_append_printf(message, ": %s", msg+offset);
  }

  ct_liblog_setLastError((const char *)message->str);
  ct_liblog_reportError(code, (const char *)message->str);
  fprintf(stderr, "%s: %s\n", p->text, message->str);

  g_string_free(message, TRUE);
  ExitProgramm(code);
}

static void AssertCondition(bool cond, enum eStatusCode code, const char *msg)
{
  if(!cond)
  {
    ExitWithError(code, msg);
  }
}

//------------------------------------------------------------------------------
// SECTION Configuration get routines
//------------------------------------------------------------------------------
static gint GetModemInfo(gchar **ppManufacturer, gchar **ppType, gchar **ppFirmware)
{
  GVariant *result = NULL;
  gint retVal = MdmDBusClientCallSync("GetModemInfo",
                                      NULL, /* no method parameters */
                                      &result);
  if (result)
  {
    g_variant_get(result,"(sss)", ppManufacturer, ppType, ppFirmware);
    g_variant_unref(result);
  }
  return retVal;
}


static gint GetModemIdentity(gchar **ppImei)
{
  GVariant *result = NULL;
  gint retVal = MdmDBusClientCallSync("GetModemIdentity",
                                      NULL, /* no method parameters */
                                      &result);
  if (result)
  {
    g_variant_get(result,"(s)", ppImei);
    g_variant_unref(result);
  }
  return retVal;
}


static gint GetSignalQuality(gint *rssi,
                             gint *ber)
{
  GVariant *result = NULL;
  gint retVal = MdmDBusClientCallSync("GetSignalQuality",
                                      NULL, /* no method parameters */
                                      &result);
  if (result)
  {
    g_variant_get(result,"(ii)", rssi, ber);
    g_variant_unref(result);
  }
  return retVal;
}

static gint GetSimState(gint *state, 
                        gint *attempts)
{
  GVariant *result = NULL;
  gint retVal = MdmDBusClientCallSync("GetSimState",
                                      NULL, /* no method parameters */
                                      &result);
  if (result)
  {
    g_variant_get(result,"(ii)", state, attempts);
    g_variant_unref(result);
  }
  return retVal;
}

static gint GetOperState(tMdmOper *oper)
{
  GVariant *result = NULL;
  gint retVal = MdmDBusClientCallSync("GetOperState",
                                      NULL, /* no method parameters */
                                      &result);
  if (result)
  {
    g_variant_get(result,"(iiiiisss)", &oper->state, &oper->selmode, &oper->id, &oper->act, &oper->quality, &oper->name, &oper->lac, &oper->cid);
    g_variant_unref(result);
  }
  return retVal;
}

static gint GetOperList(gint *size,
                        tMdmOper **operList)
{
  GVariant *result = NULL;
  gint retVal = MdmDBusClientCallSync("GetOperList",
                                      NULL, /* no method parameters */
                                      &result);
  if (result && (retVal == 0))
  {
    GVariant *gvarOper = g_variant_get_child_value( result, 0 );
    if (gvarOper == NULL)
    {
      MdmDBusClientSetLastError("Invalid DBus result");
      retVal = -1;
    }
    else
    {
      gint n = g_variant_n_children(gvarOper);
      tMdmOper *oper = (tMdmOper*)calloc(n, sizeof(tMdmOper));
      if (oper == NULL)
      {
        MdmDBusClientSetLastError("No memory for DBus result");
        retVal = -1;
      }
      else
      {
        gint i;
        *size = n;
        *operList = oper;
        for (i = 0; i < n; i++)
        {
          MdmOperInit(oper);
          g_variant_get_child( gvarOper, i, "(iiiis)", &oper->state, &oper->id, &oper->act, &oper->quality, &oper->name);
          oper++;
        }
      }
      g_variant_unref(gvarOper);
    }
  }
  if (result)
  {
    g_variant_unref(result);
  }
  return retVal;
}

static gint GetGprsAccess(tMdmGprsAccess *gprsAccess)
{
  GVariant *result = NULL;
  gint retVal = MdmDBusClientCallSync("GetGprsAccess2",
                                      NULL, /* no method parameters */
                                      &result);
  if (result)
  {
    g_variant_get(result,"(isissis)",
                  &gprsAccess->state, &gprsAccess->apn, &gprsAccess->auth, &gprsAccess->user, &gprsAccess->pass,
                  &gprsAccess->connectivity, &gprsAccess->pdp_addr);
    g_variant_unref(result);
  }
  return retVal;
}

static gint GetPortState(gint *state, gint * open)
{
  GVariant *result = NULL;
  gint retVal = MdmDBusClientCallSync("GetPortState2",
                                      NULL, /* no method parameters */
                                      &result);
  if (result)
  {
    g_variant_get(result,"(ii)", state, open);
    g_variant_unref(result);
  }
  return retVal;
}

static gint GetVersion(gchar **ppVersion)
{
  GVariant *result = NULL;
  gint retVal = MdmDBusClientCallSync("GetVersion",
                                      NULL, /* no method parameters */
                                      &result);
  if (result)
  {
    g_variant_get(result,"(s)", ppVersion);
    g_variant_unref(result);
  }
  return retVal;
}

static gint GetLogLevel(gint *loglevel)
{
  GVariant *result = NULL;
  gint retVal = MdmDBusClientCallSync("GetLogLevel",
                                      NULL, /* no method parameters */
                                      &result);
  if (result)
  {
    g_variant_get(result,"(i)", loglevel);
    g_variant_unref(result);
  }
  return retVal;
}

static gint GetSmsStorageConfig(tSmsStorageConfig* storageConfig)
{
  GVariant *result = NULL;
  gint retVal = MdmDBusClientCallSync("GetSmsStorage",
                                      NULL, /* no method parameters */
                                      &result);
  if (result)
  {
    g_variant_get(result,"(siisiisii)",
                  &storageConfig->mem1,
                  &storageConfig->user1,
                  &storageConfig->total1,
                  &storageConfig->mem2,
                  &storageConfig->user2,
                  &storageConfig->total2,
                  &storageConfig->mem3,
                  &storageConfig->user3,
                  &storageConfig->total3);
    g_variant_unref(result);
  }
  return retVal;
}

static gint GetSmsEventReporting(tSmsReportingConfig* reportingConfig)
{
  GVariant *result = NULL;
  gint retVal = MdmDBusClientCallSync("GetSmsReportConfig",
                                      NULL, /* no method parameters */
                                      &result);
  if (result)
  {
    g_variant_get(result,"(iiiii)",
                  &reportingConfig->mode,
                  &reportingConfig->mt,
                  &reportingConfig->bm,
                  &reportingConfig->ds,
                  &reportingConfig->bfr);
    g_variant_unref(result);
  }
  return retVal;
}

//------------------------------------------------------------------------------
// SECTION Configuration set routines
//------------------------------------------------------------------------------
static gint SetSimPin(const gchar *pin)
{
  GVariant *result = NULL;
  gint retVal = MdmDBusClientCallSync("SetSimPin",
                                      g_variant_new("(s)", pin),
                                      &result);
  if (result)
  {
    g_variant_unref(result);
  }
  return retVal;
}

static gint SetSimPuk(const gchar *puk,
                        const gchar *pin)
{
  GVariant *result = NULL;
  gint retVal = MdmDBusClientCallSync("SetSimPuk",
                                      g_variant_new("(ss)", puk, pin),
                                      &result);
  if (result)
  {
    g_variant_unref(result);
  }
  return retVal;
}

static gint SetOper(gint selmode, gint id, gint act)
{
  GVariant *result = NULL;
  gint retVal = MdmDBusClientCallSync("SetOper",
                                      g_variant_new("(iii)", selmode, id, act),
                                      &result);
  if (result)
  {
    g_variant_unref(result);
  }
  return retVal;
}

static gint SetGprsAccess(const gchar* apn, gint auth, const gchar* user, const gchar* pass, gint connectivity)
{
  GVariant *result = NULL;
  gint retVal = MdmDBusClientCallSync("SetGprsAccess2",
                                      g_variant_new("(sissi)", apn, auth, user, pass, connectivity),
                                      &result);
  if (result)
  {
    g_variant_unref(result);
  }
  return retVal;
}

static gint SetSmsStorageConfig(const gchar* mem1, const gchar* mem2, const gchar* mem3) 
{
  GVariant *result = NULL;
  gint retVal = MdmDBusClientCallSync("SetSmsStorage",
                                      g_variant_new("(sss)", mem1, mem2, mem3),
                                      &result);
  if (result)
  {
    g_variant_unref(result);
  }
  return retVal;
}

static gint SetSmsEventReportingConfig(const gint mode, const gint mt, const gint bm, const gint ds, const gint bfr) 
{
  GVariant *result = NULL;
  gint retVal = MdmDBusClientCallSync("SetSmsReportConfig",
                                      g_variant_new("(iiiii)", mode, mt, bm, ds, bfr),
                                      &result);
  if (result)
  {
    g_variant_unref(result);
  }
  return retVal;
}

static gint ModemReset(void)
{
  GVariant *result = NULL;
  gint retVal = MdmDBusClientCallSync("ModemReset",
                                      NULL, /* no method parameters */
                                      &result);
  if (result)
  {
    g_variant_unref(result);
  }
  return retVal;
}

static gint SetPortState(gint state)
{
  GVariant *result = NULL;
  gint retVal = MdmDBusClientCallSync("SetPortState",
                                      g_variant_new("(i)", state),
                                      &result);
  if (result)
  {
    g_variant_unref(result);
  }
  return retVal;
}

static gint SetLogLevel(gint loglevel)
{
  GVariant *result = NULL;
  gint retVal = MdmDBusClientCallSync("SetLogLevel",
                                      g_variant_new("(i)", loglevel),
                                      &result);
  if (result)
  {
    g_variant_unref(result);
  }
  return retVal;
}

//------------------------------------------------------------------------------
// SECTION Main function, command line option handling.
//------------------------------------------------------------------------------
static const gchar *info_text = 
  "* Configuration tool for MDMD modem management daemon *\n"
  "For detailed information use:\n"
  "  config_mdmd --help\n"
  "\n"
  "";


static const gchar *usage_text = 
  "* Configuration tool for modem management daemon *\n"
  "\n"
  "Usage:\n"
  "  config_mdmd <command> [<option>] [<param>, ...]\n"
  "\n"
  "Commands:\n"
  "  -h/--help                  Print this usage message.\n"
  "  -g/--get                   Query parameters and print as name=value pairs, one per line.\n"
  "  -j/--get-json              Query parameters and print as JSON format string.\n"
  "  -s/--set [param] ...       Set parameters.\n"
  "  -r/--reset                 Trigger a modem hard reset.\n"
  "     --reset-all             Trigger a modem hard reset and restart modem software environment.\n"
  "  -v/--version               Print version of modem manager daemon (mdmd)\n"
  "  -c/--check                 Check the compatibility between the PFC firmware and the modem firmware.\n"
  "\n"
  "Options:\n"
  "  -d/--device                Modem device information.\n"
  "  -i/--identity              Mobile equipment identity\n"
  "  -n/--network               Configuration of mobile network.\n"
  "  -l/--network-list          List of detected mobile networks.\n"
  "  -G/--gprsaccess            Configuration of packet data service.\n"
  "  -S/--SIM                   Subscriber identity authentication.\n"
  "  -p/--port                  Configuration of modem control port\n"
  "  -L/--logging               Configuration of log output\n"
  "  -q/--signal-quality        Get measured signal quality values.\n"
  "  -T/--smsstorage            Configuration of sms storage.\n"
  "  -R/--smseventreporting     Configuration of sms event reporting.\n"

/*
 * Added functionality to fetch message list from command line.
 * While this functionality was only for bugfixing purpose it is not properly tested.
 * So these functions have to remain undocumented, so customers do not know about this functionality.
 * It is not known, if these functions have unknown side-effects.
 * 
 * "  -m/--message-list          List of received messages\n" 
 */
  "\n"
  "Parameter names and possible values:\n"
  "  PortState                  type: enum\n"
  "                             access: read/write\n"
  "                             - state of modem control port: DISABLED | ENABLED\n"
  "  PortOpen                   type: bool\n"
  "                             access: read\n"
  "                             - state of modem control port:\n"
  "                               TRUE:  PortState is ENABLED\n"
  "                               FALSE: PortState is DISABLED\n"

  "  GprsRegistrationState      type: enum\n"
  "                             access: read only\n"
  "                             - registration state:\n"
  "                               STOPPED: not registered, the manually selected network is not available.\n"
  "                               HOMENET: registered with home network.\n"
  "                               STARTED: not registered, automatic network selection ongoing or no network available\n"
  "                               DENIED:  not registered, the manually selected network allows no registration.\n"
  "                               ROAMING: registered with roaming network.\n"
  "  GprsAccessPointName        type: string\n"
  "                             access: read/write\n"
  "                             - logical name of GGSN or provider specific data network\n"
  "  GprsAuthType               type: enum\n"
  "                             access: read/write\n"
  "                             - type of APN authentication: NONE | PAP | CHAP | PAP_OR_CHAP\n"
  "  GprsUserName               type: string\n"
  "                             access: read/write\n"
  "                             - user name for APN authentication type PAP\n"
  "  GprsPassword               type: string\n"
  "                             access: read/write\n"
  "                             - password for APN authentication type PAP\n"
  "  GprsConnectivity           type: enum\n"
  "                             access: read/write\n"
  "                             - state of packet radio service: DISABLED | ENABLED\n"
  "  PdpAddress                 type: string\n"
  "                             access: read only\n"
  "                             - packet data protocol address\n"

  "  SmsStorageMem1             type: string\n"
  "                             access: read/write\n"
  "                             - messages to be read and deleted from this memory storage: SM(SIM message storage), ME(MobileEquipment message storage), MT(same as ME)\n"
  "  SmsStorageUser1            type: numeric\n"
  "                             access: read only\n"
  "  SmsStorageTotal1            type: numeric\n"
  "                             access: read only\n"
  "  SmsStorageMem2             type: string\n"
  "                             access: read/write\n"
  "                             - messages will be written and sent to this memory storage: SM(SIM message storage), ME(MobileEquipment message storage), MT(same as ME)\n"
  "  SmsStorageUser2            type: numeric\n"
  "                             access: read onlye\n"
  "  SmsStorageTotal2           type: numeric\n"
  "                             access: read only\n"
  "  SmsStorageMem3             type: string\n"
  "                             access: read/write\n"
  "                             - received messages will be placed in this memory storage if routing is not set: SM(SIM message storage), ME(MobileEquipment message storage), MT(same as ME)\n"
  "  SmsStorageUser3            type: numeric\n"
  "                             access: read only\n"
  "  SmsStorageTotal3           type: numeric\n"
  "                             access: read only\n"

  "  SmsEventReportingMode      type: integer\n"
  "                             access: read/write\n"
  "                             - event reporting mode (see 3GPP TS 27.005)\n"
  "                             - possible values for setting: 0|1|2\n"
  "  SmsEventReportingMT        type: integer\n"
  "                             access: read/write\n"
  "                             - rules for storing received SMS (see 3GPP TS 27.005)\n"
  "                             - possible values for setting: 0|1\n"
  "  SmsEventReportingBM        type: integer\n"
  "                             access: read/write\n"
  "                             - rules for storing received cell broadcast messages (see 3GPP TS 27.005)\n"
  "                             - possible values for setting: 0\n"
  "  SmsEventReportingDS        type: integer\n"
  "                             access: read/write\n"
  "                             - status routed to the terminal equipment (see 3GPP TS 27.005)\n"
  "                             - possible values for setting: 0\n"
  "  SmsEventReportingBFR       type: integer\n"
  "                             access: read/write\n"
  "                             - buffer of unsolicited result codes (see 3GPP TS 27.005)\n"
  "                             - possible values for setting: 0|1\n"

  "  IMEI                       type string\n"
  "                             access: read only\n"
  "                             - International Mobile Equipment Identity of the modem\n"

  "  LogLevel                   type: enum\n"
  "                             access: read/write\n"
  "                             - detail level of log output: DISABLED | ERROR | WARNING | INFO | DEBUG1 | DEBUG2\n"

  "  ModemDevice                type: string\n"
  "                             access: read only\n"
  "                             - modem device manufacturer and type\n"
  "  ModemFirmware              type: string\n"
  "                             access: read only\n"
  "                             - modem firmware revision\n"

  "  NetworkSelection           type: enum\n"
  "                             access: read/write\n"
  "                             - strategy of network selection:\n"
  "                               MANUAL:      select the given network fix.\n"
  "                               AUTOMATIC:   automatic network selection without priorisation.\n"
  "                               ONLY_GSM:    automatic network selection with restriction to GSM networks.\n"
  "                               ONLY_UMTS:   automatic network selection with restriction to UMTS networks.\n"
  "                               PREFER_GSM:  automatic network selection with preference for GSM networks.\n"
  "                               PREFER_UMTS: automatic network selection with preference for UMTS networks.\n"
  "  NetworkRegistrationState   type: enum\n"
  "                             access: read only\n"
  "                             - status of network registration:\n"
  "                               STOPPED: not registered, the manually selected network is not available.\n"
  "                               HOMENET: registered with home network.\n"
  "                               STARTED: not registered, automatic network selection ongoing or no network available\n"
  "                               DENIED:  not registered, the manually selected network allows no registration.\n"
  "                               ROAMING: registered with roaming network.\n"
  "  NetworkId                  type: numeric\n"
  "                             access: read/write\n"
  "                             - unique identifier of the mobile network as 5- or 6 digit number (MCC + MNC)\n"
  "  NetworkType                type: enum\n"
  "                             access: read/write\n"
  "                             - underlying technology of the mobile network:\n"
  "                               GSM|GSM_COMPACT|UMTS|GSM_EGPRS|UMTS_HSDPA|UMTS_HSUPA|UMTS_HSPA|LTE\n"
  "  NetworkName                type: read only\n"
  "                             access: read/write\n"
  "                             - name of the mobile network\n"
  "  SignalStrength             type: numeric\n"
  "                             access: read only\n"
  "                             - received signal strength in percent\n"
  "  NetworkState               type: enum\n"
  "                             access: read only\n"
  "                             - availability of the the listed network:\n"
  "                               AVAILABLE: registration is possible.\n"
  "                               CURRENT:   currently registered network.\n"
  "                               FORBIDDEN: registration is not allowed\n"

  "  SimPin                     type: numeric\n"
  "                             access: write only\n"
  "                             - password for SIM authentication when SimState SIM_PIN or SIM_PUK, commonly it is a 4 digit number\n"
  "  SimPuk                     type: numeric\n"
  "                             access: write only\n"
  "                             - password for SIM authentication when SimState SIM_PUK, commonly it is a 8 digit number\n"

  "  SimState                   type: enum\n"
  "                             access: read only\n"
  "                             - sim card status\n"
  "                               SIM_PIN:      authentication with PIN necessary.\n"
  "                               SIM_PUK:      authentication with PUK necessary.\n"
  "                               READY:        authentication successful.\n"
  "                               ERROR:        sim card is not valid.\n"
  "                               NOT_INSERTED: sim card is not inserted.\n"
  "  SimAttempts                type: numeric\n"
  "                             access: read only\n"
  "                             - the remaining attempts for SIM authentication when SimState SIM_PIN or SIM_PUK\n"

  "  RSSI                       type: integer\n"
  "                             access: read only\n"
  "                             - The Received Signal Strength Indicator gives information about the signal strength.\n"
  "  BER                        type: integer\n"
  "                             access: read only\n"
  "                             - The Bit Error Rate gives information about the number of bit errors per unit time.\n"

  "  MdmdVersion                type: string\n"
  "                             access: read only\n"
  "                             - version of the modem manager\n"

  "  Firmware compatibility     type: string\n"
  "                             access: read only\n"
  "                             - compatibility between the PFC firmware and the modem firmware\n"
  "                               compatible:    PFC firmware and the modem firmware are compatible.\n"
  "                               incompatible:  PFC firmware and the modem firmware are incompatible.\n"

  "\n"
  "Examples\n"
  "  config_mdmd --get --SIM\n"
  "    Prints current SIM parameters as name=value pairs, one per line.\n"
  "\n"
  "  config_mdmd --set --SIM SimPin=1234\n"
  "    Does SIM authentication when SimState=SIM_PIN.\n"
  "\n"
  "  config_mdmd --set --SIM SimPuk=12345678 SimPin=1234\n"
  "    Does SIM authentication when SimState=SIM_PUK.\n"
  "\n"
  "  config_mdmd --get --network\n"
  "    Prints all current network parameters as name=value pairs, one per line.\n"
  "\n"
  "  config_mdmd --get --network-list\n"
  "    Prints network parameters for each detected network separated by an empty line.\n"
  "\n"
  "  config_mdmd --set --network NetworkSelection=MANUAL NetworkId=26201 NetworkType=UMTS\n"
  "    Sets mobile network fix to T-Mobile Germany UMTS.\n"
  "\n"
  "  config_mdmd --set --network NetworkSelection=AUTOMATIC\n"
  "    Sets automatic network registration mode.\n"
  "\n"
  "  config_mdmd --set --gprsaccess GprsAccessPointName=\"internet.t-mobile\" GprsAuthType=NONE GprsUserName=\"\" GprsPassword=\"\"\n"
  "    Sets specific packet data network parameter where authentication is not used.\n"
  "\n"
  "  config_mdmd --set --smsstorage SmsStorageMem1=SM SmsStorageMem2=ME SmsStorageMem3=MT\n"
  "    Sets sms memory storage to be used for reading, writing, etc.\n"
  "\n"
  "  config_mdmd --set --smseventreporting SmsEventReportingMode=0 SmsEventReportingMT=0 SmsEventReportingBM=0 SmsEventReportingDS=0 SmsEventReportingBFR=0\n"
  "    Sets the procedure on how the received new messages from the network are indicated to the terminal equipment.\n"
  "\n"
  "";

static tIntToStringMap _mapSimState[] = {
  { 0, "UNKNOWN" },
  { 1, "SIM_PIN" },
  { 2, "SIM_PUK" },
  { 3, "READY" },
  { 4, "NOT_INSERTED" },
  { 5, "ERROR" },
  { -1, "INVALID" }        // End marker, don't remove.
};

static tIntToStringMap _mapOperAct[] = {
  { 0, "GSM" },
  { 1, "GSM_COMPACT" },
  { 2, "UMTS" },
  { 3, "GSM_EGPRS" },
  { 4, "UMTS_HSDPA" },
  { 5, "UMTS_HSUPA" },
  { 6, "UMTS_HSPA" },
  { 7, "LTE" },
  { -1, "INVALID" }        // End marker, don't remove.
};

static tIntToStringMap _mapRegState[] = {
  { 0, "STOPPED" },
  { 1, "HOMENET" },
  { 2, "STARTED" },
  { 3, "DENIED" },
  { 4, "UNKNOWN" },
  { 5, "ROAMING" },
  { -1, "INVALID" }        // End marker, don't remove.
};

static tIntToStringMap _mapOperState[] = {
  { 0, "UNKNOWN" },
  { 1, "AVAILABLE" },
  { 2, "CURRENT" },
  { 3, "FORBIDDEN" },
  { -1, "INVALID" }        // End marker, don't remove.
};

static tIntToStringMap _mapSelectMode[] = {
  { 0, "AUTOMATIC" },
  { 1, "MANUAL" },
  { 2, "ONLY_GSM" },
  { 3, "ONLY_UMTS" },
  { 4, "PREFER_GSM" },
  { 5, "PREFER_UMTS" },
  { -1, "INVALID" }        // End marker, don't remove.
};

static tIntToStringMap _mapAuthType[] = {
  { 0, "NONE" },
  { 1, "PAP" },
  { 2, "CHAP" },
  { 3, "PAP_OR_CHAP" },
  { -1, "INVALID" }        // End marker, don't remove.
};

static tIntToStringMap _mapState[] = {
  { 0, "DISABLED" },
  { 1, "ENABLED" },
  { -1, "INVALID" }        // End marker, don't remove.
};

static tIntToStringMap _mapBoolean[] = {
  { 0, "FALSE" },
  { 1, "TRUE" },
  { -1, "INVALID" }        // End marker, don't remove.
};

static tIntToStringMap _mapLoglevel[] = {
  { 0, "DISABLED" },
  { 1, "ERROR" },
  { 2, "WARNING" },
  { 3, "INFO" },
  { 4, "DEBUG1" },
  { 5, "DEBUG2" },
  { -1, "INVALID" }        // End marker, don't remove.
};

static tIntToStringMap _mapMessageState[] = {
  { 0, "RECV UNREAD" },
  { 1, "RECV READ" },
  { 2, "STOR UNSENT" },
  { 3, "STOR SENT" },
  { 4, "ALL" },
  { -1, "INVALID" }        // End marker, don't remove.
};

typedef enum
{
  OPT_DEVICE = 0,
  OPT_NETWORK,
  OPT_NETWORKLIST,
  OPT_GPRSACCESS,
  OPT_SIM,
  OPT_PORT,
  OPT_IDENTITY,
  OPT_SIGNAL_QUALITY,
  OPT_SMS_STORAGE,
  OPT_SMS_EVENT_REPORTING,
  OPT_MESSAGE_LIST,
  OPT_LOGGING,
  OPT_NONE,
} tOption;

static struct option _commandOptions[] =
{
  { "device", no_argument, NULL, 'd' },
  { "network", no_argument, NULL, 'n' },
  { "network-list", no_argument, NULL, 'l' },
  { "gprsaccess", no_argument, NULL, 'G' },
  { "SIM", no_argument, NULL, 'S' },
  { "port", no_argument, NULL, 'p' },
  { "identity", no_argument, NULL, 'i' },
  { "signal-quality", no_argument, NULL, 'q' },
  { "smsstorage", no_argument, NULL, 'T' },
  { "smseventreporting", no_argument, NULL, 'R' },
  { "message-list", no_argument, NULL, 'm' },
  { "logging", no_argument, NULL, 'L' },
  { NULL, 0, NULL, 0 }, // End marker, don't remove.
};

typedef enum
{
  CMD_GET = 0,
  CMD_GET_JSON,
  CMD_SET,
  CMD_RESET,
  CMD_RESETALL,
  CMD_HELP,
  CMD_VERSION,
  CMD_CHECK,
  CMD_NONE,
} tCommand;

static struct option _commands[] =
{
  { "get", no_argument, NULL, 'g' },
  { "get-json", no_argument, NULL, 'j' },
  { "set", no_argument, NULL, 's' },
  { "reset", no_argument, NULL, 'r' },
  { "reset-all", no_argument, NULL, '9' },
  { "help", no_argument, NULL, 'h' },
  { "version", no_argument, NULL, 'v' },
  { "check", no_argument, NULL, 'c' },
  { NULL, 0, NULL, 0 }, // End marker, don't remove.
};


static const gchar *_parameterNames[] =
{
  "SimPin",
  "SimPuk",
  "GprsAccessPointName",
  "GprsAuthType",
  "GprsConnectivity",
  "GprsUserName",
  "GprsPassword",
  "NetworkId",
  "NetworkType",
  "NetworkSelection",
  "SmsStorageMem1",
  "SmsStorageMem2",
  "SmsStorageMem3",
  "SmsEventReportingMode",
  "SmsEventReportingMT",
  "SmsEventReportingBM",
  "SmsEventReportingDS",
  "SmsEventReportingBFR",
  "PortState",
  "LogLevel",
  NULL,        // End marker, don't remove.
};

typedef enum
{
  FMT_DEFAULT = 0,
  FMT_JSON,
} tPrintFormat;

// Name/value pair structure.
typedef struct
{
  const gchar *name;
  const gchar *value;
} tNameValuePair;

static gint GetOption(char *arg, 
                      struct option *options)
{
  gint i = 0;

  AssertCondition(arg[0] == '-', INVALID_PARAMETER, "");
  AssertCondition(arg[1] != '\0', INVALID_PARAMETER, "");

  if (arg[1] == '-') //long option
  {
    while((options[i].name != NULL) && (g_strcmp0(options[i].name, &arg[2])))
    {
      i++;
    }
  }
  else //short option
  {
    AssertCondition(arg[2] == '\0', INVALID_PARAMETER, "");
    while((options[i].val != 0) && (options[i].val != (int)arg[1]))
    {
      i++;
    }
  }
  return i;
}

static bool IsValidParameter(const gchar *pname)
{
  const gchar **pp = _parameterNames;
  while((*pp != NULL) && (g_strcmp0(*pp, pname)))
  {
    pp++;
  }
  return (*pp != NULL);
}

static void GetCommandParams(gint argc, char **argv, tNameValuePair *nvpairs)
{
  gint i;
  for (i = 0; i < argc; i++)
  {
    char* c = NULL;

    // decode uri sequeces and write decoded string back to argv variable
    // (no problem with length, because uri decoded string is always shorter than encoded string)
    // note: we can't set c to acUriDecodedParam, because nvpairs.name will directly point to c and
    // we must free acUriDecodedParam afterwards
    char* acUriDecodedParam = g_uri_unescape_string(argv[i], NULL);

    if(NULL != acUriDecodedParam)
    {
      gint  maxParamLength = strlen(argv[i]);
      strncpy(argv[i], acUriDecodedParam, maxParamLength);
      g_free(acUriDecodedParam);
    }

    // Split name value pair.
    c = argv[i];

    nvpairs[i].name = c;
    nvpairs[i].value = NULL;
    while (*c != '\0')
    {
      if (*c == '=')
      {
        *c = '\0';
        c++;
        if (*c == '\"')
        { //strip quotation
          c++;
          nvpairs[i].value = c;
          while (*c != '\0')
          {
            if (*c == '\"')
            {
              *c = '\0';
              break;
            }
            c++;
          }
        }
        else
        {
          nvpairs[i].value = c;
        }
        break;
      }
      c++;
    }
    AssertCondition(IsValidParameter(nvpairs[i].name), INVALID_PARAMETER, nvpairs[i].name);
    AssertCondition(nvpairs[i].value, INVALID_PARAMETER, nvpairs[i].name);
  }
}

static void PrintDeviceParams(tPrintFormat format)
{
  gchar *manufacturer = NULL;
  gchar *type = NULL;
  gchar *firmware = NULL;

  gint result = GetModemInfo(&manufacturer, &type, &firmware);
  AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);

  switch (format)
  {
    case FMT_JSON:
      printf("{");
      if ((manufacturer != NULL) && (type != NULL))
      {
        printf("\"ModemDevice\": \"%s %s\"", manufacturer, type);
      }
      else
      {
        printf("\"ModemDevice\": \"unknown\"");
      }
      printf(", ");
      if (firmware != NULL)
      {
        printf("\"ModemFirmware\": \"%s\"", firmware);
      }
      else
      {
        printf("\"ModemFirmware\": \"unknown\"");
      }
      printf("}");
      break;
    case FMT_DEFAULT:
      /*no break*/
    default:
      if ((manufacturer != NULL) && (type != NULL))
      {
        printf("ModemDevice=%s %s\n", manufacturer, type);
      }
      else
      {
        printf("ModemDevice=unknown\n");
      }
      if (firmware != NULL)
      {
        printf("ModemFirmware=%s\n", firmware);
      }
      else
      {
        printf("ModemFirmware=unknown\n");
      }
  }
  if (manufacturer != NULL)
  {
    g_free(manufacturer);
  }
  if (type != NULL)
  {
    g_free(type);
  }
  if (firmware != NULL)
  {
    g_free(firmware);
  }
}


static void PrintMessageList(tPrintFormat format)
{
  GVariant *varResult = 0;
  gint state = 4; //4 = all messages
  gint result = MdmDBusClientCallSync("ListSms",g_variant_new("(i)", state),
                                      &varResult);
  if ((varResult != NULL) && (result == 0))
  {
    GVariant *varMessage = g_variant_get_child_value( varResult, 0 );
    if (format==FMT_JSON)
      printf("[ ");
    if (varMessage != NULL)
    {
      gint nbrOfMessages = g_variant_n_children(varMessage);
      gint i;
      for (i = 0; i < nbrOfMessages; i++)
      {
        gint msgIndex = 0;
        GString *msgState = g_string_new(NULL);
        gint msgLength = 0;
        gchar *msgPdu = NULL;
        g_variant_get_child( varMessage, i, "(iiis)", &msgIndex, &state, &msgLength, &msgPdu);
        MapIntToString(_mapMessageState, state, msgState);
        switch (format)
        {
          case FMT_JSON:
            printf("%s{ \"index\": \"%d\", \"state\": \"%s\", \"length\": \"%d\", \"pdu\": \"%s\" }",
                   (0 == i) ? "" : ", ",
                   msgIndex,
                   msgState->str,
                   msgLength,
                   msgPdu ? msgPdu : "");
            break;

          case FMT_DEFAULT:
            /*no break*/
          default:
            printf("Index=%d\n", msgIndex);
            printf("State=%s\n", msgState->str);
            printf("Length=%d\n", msgLength);
            printf("PDU=%s\n", msgPdu ? msgPdu : "");
            printf("\n");
        }
        if (msgPdu != NULL)
        {
          g_free(msgPdu);
        }
        g_string_free(msgState, TRUE);
      }
      g_variant_unref(varMessage);
    }
    if (format==FMT_JSON) printf("] ");
  }
  if (varResult != NULL)
  {
    g_variant_unref(varResult);
  }
  AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);
}


static void PrintIdentity(tPrintFormat format)
{
  gchar *imei = NULL; //International Mobile Equipment Identity

  gint result = GetModemIdentity(&imei);
  AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);

  switch (format)
  {
    case FMT_JSON:
      printf("{\"IMEI\": \"%s\"}", (imei != NULL) ? imei : "unknown");
      break;
    case FMT_DEFAULT:
      /*no break*/
    default:
      printf("IMEI=%s\n", (imei != NULL) ? imei : "unknown");
  }
  if (imei != NULL) {
    g_free(imei);
  }
}


static void PrintSignalQuality(tPrintFormat format)
{
  gint rssi = 0;
  gint ber = 0;
  gint result = GetSignalQuality(&rssi, &ber);
  AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);

  switch (format)
  {
    case FMT_JSON:
      printf("{");
      printf("\"RSSI\": %d", rssi);
      printf(", ");
      printf("\"BER\": %d", ber);
      printf("}");
      break;
    case FMT_DEFAULT:
      /*no break*/
    default:
      printf("RSSI=%d\n", rssi);
      printf("BER=%d\n", ber);
  }
}


static void PrintNetworkParams(tPrintFormat format)
{
  GString *mode;
  GString *type;
  GString *regstate;
  GString *name;
  gint result;
  tMdmOper oper;
  MdmOperInit(&oper);
  result = GetOperState(&oper);
  AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);

  mode = g_string_new(NULL);
  type = g_string_new(NULL);
  regstate = g_string_new(NULL);
  name = g_string_new(NULL);
  MapIntToString(_mapSelectMode, oper.selmode, mode);
  if (oper.id > 0)
  {
    if (oper.name)
    {
      g_string_assign(name, oper.name);
    }
    else
    {
      g_string_assign(name, "INVALID");
    }
    MapIntToString(_mapOperAct, oper.act, type);
  }
  else
  {
    g_string_assign(name, "NONE");
    g_string_assign(type, "---");
  }
  MapIntToString(_mapRegState, oper.state, regstate);
  switch (format)
  {
    case FMT_JSON:
      printf("{");
      printf("\"NetworkSelection\": \"%s\"", mode->str);
      printf(", ");
      printf("\"NetworkRegistrationState\": \"%s\"", regstate->str);
      printf(", ");
      printf("\"NetworkId\": %d", oper.id);
      printf(", ");
      printf("\"NetworkName\": \"%s\"", name->str);
      printf(", ");
      printf("\"NetworkType\": \"%s\"", type->str);
      printf(", ");
      printf("\"SignalStrength\": %d", oper.quality);
      printf("}");
      break;
    case FMT_DEFAULT:
      /*no break*/
    default:
      printf("NetworkSelection=%s\n", mode->str);
      printf("NetworkRegistrationState=%s\n", regstate->str);
      printf("NetworkId=%d\n", oper.id);
      printf("NetworkName=%s\n", name->str);
      printf("NetworkType=%s\n", type->str);
      printf("SignalStrength=%d\n", oper.quality);
  }
  g_string_free(mode, TRUE);
  g_string_free(type, TRUE);
  g_string_free(regstate, TRUE);
  g_string_free(name, TRUE);
  MdmOperFree(&oper);
}

static void PrintNetworkList(tPrintFormat format)
{
  tMdmOper *operList = NULL;
  gint size = 0;
  gint i;

  gint result = GetOperList(&size, &operList);
  //AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);
  if (result!=0)
  { //print empty list when not available, e.g. when PIN must be set
    if (format==FMT_JSON) printf("[ ] ");
  }
  else
  {
    GString *state = g_string_new(NULL);
    GString *type = g_string_new(NULL);
    switch (format)
    {
      case FMT_JSON:
      {
        printf("[ ");
        for (i = 0; i < size; i++)
        {
          MapIntToString(_mapOperState, operList[i].state, state);
          MapIntToString(_mapOperAct, operList[i].act, type);

          printf("%s{ \"state\": \"%s\", \"id\": \"%d\", \"name\": \"%s\", \"type\": \"%s\" }",
                 (0 == i) ? "" : ", ",
                 state->str,
                 operList[i].id,
                 operList[i].name ? operList[i].name : "",
                 type->str);
        }
        printf("] ");
      }
      break;

      case FMT_DEFAULT:
        /*no break*/
      default:
        for (i = 0; i < size; i++)
        {
            MapIntToString(_mapOperState, operList[i].state, state);
            MapIntToString(_mapOperAct, operList[i].act, type);
            printf("NetworkState=%s\n", state->str);
            printf("NetworkId=%d\n", operList[i].id);
            printf("NetworkName=%s\n", operList[i].name ? operList[i].name : "");
            printf("NetworkType=%s\n", type->str);
            printf("\n");
        }
    }
    g_string_free(state, TRUE);
    g_string_free(type, TRUE);
  }
  for (i = 0; i < size; i++)
  {
    MdmOperFree(&operList[i]);
  }
  free(operList);
}

static void PrintGprsAccessParams(tPrintFormat format)
{
  GString *authtype;
  GString *regstate;
  GString *connectivity;
  gint result;
  tMdmGprsAccess gprsAccess;
  MdmGprsAccessInit(&gprsAccess);
  result = GetGprsAccess(&gprsAccess);
  AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);

  authtype = g_string_new(NULL);
  regstate = g_string_new(NULL);
  connectivity = g_string_new(NULL);
  MapIntToString(_mapAuthType, gprsAccess.auth, authtype);
  MapIntToString(_mapRegState, gprsAccess.state, regstate);
  MapIntToString(_mapState, gprsAccess.connectivity, connectivity);
  switch (format)
  {
    case FMT_JSON:
      printf("{");
      printf("\"GprsRegistrationState\": \"%s\"", regstate->str);
      printf(", ");
      printf("\"GprsAccessPointName\": \"%s\"", gprsAccess.apn ? gprsAccess.apn : "");
      printf(", ");
      printf("\"GprsAuthType\": \"%s\"", authtype->str);
      printf(", ");
      printf("\"GprsUserName\": \"%s\"", gprsAccess.user ? gprsAccess.user : "");
      printf(", ");
      printf("\"GprsPassword\": \"%s\"", gprsAccess.pass ? gprsAccess.pass : "");
      printf(", ");
      printf("\"GprsConnectivity\": \"%s\"", connectivity->str);
      printf(", ");
      printf("\"PdpAddress\": \"%s\"", (gprsAccess.pdp_addr != NULL) ? gprsAccess.pdp_addr : "");
      printf("}");
      break;
    case FMT_DEFAULT:
      /*no break*/
    default:
      printf("GprsRegistrationState=%s\n", regstate->str);
      printf("GprsAccessPointName=\"%s\"\n", gprsAccess.apn ? gprsAccess.apn : "");
      printf("GprsAuthType=%s\n", authtype->str);
      printf("GprsUserName=\"%s\"\n", gprsAccess.user ? gprsAccess.user : "");
      printf("GprsPassword=\"%s\"\n", gprsAccess.pass ? gprsAccess.pass : "");
      printf("GprsConnectivity=%s\n", connectivity->str);
      printf("PdpAddress=\"%s\"\n", (gprsAccess.pdp_addr != NULL) ? gprsAccess.pdp_addr : "");
  }
  g_string_free(authtype, TRUE);
  g_string_free(regstate, TRUE);
  g_string_free(connectivity, TRUE);
  MdmGprsAccessFree(&gprsAccess);
}


static void PrintSmsStorageConfig(tPrintFormat format)
{
  gint result;
  tSmsStorageConfig storageConfig;
  SmsStorageConfigInit(&storageConfig);
  result = GetSmsStorageConfig(&storageConfig);
  AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);

  switch (format)
  {
    case FMT_JSON: 
      printf("{");
      printf("\"SmsStorageMem1\": %s", storageConfig.mem1);
      printf(", ");
      printf("\"SmsStorageUser1\": %d", storageConfig.user1);
      printf(", ");
      printf("\"SmsStorageTotal1\": %d", storageConfig.total1);
      printf(", ");
      printf("\"SmsStorageMem2\": %s", storageConfig.mem2);
      printf(", ");
      printf("\"SmsStorageUser2\": %d", storageConfig.user2);
      printf(", ");
      printf("\"SmsStorageTotal2\": %d", storageConfig.total2);
      printf(", ");
      printf("\"SmsStorageMem3\": %s", storageConfig.mem3);
      printf(", ");
      printf("\"SmsStorageUser3\": %d", storageConfig.user3);
      printf(", ");
      printf("\"SmsStorageTotal3\": %d", storageConfig.total3);
      printf("}");
      break;
    case FMT_DEFAULT:
      /*no break*/
    default:
      printf("SmsStorageMem1=%s\n", storageConfig.mem1);
      printf("SmsStorageUser1=%d\n", storageConfig.user1);
      printf("SmsStorageTotal1=%d\n", storageConfig.total1);
      printf("SmsStorageMem2=%s\n", storageConfig.mem2);
      printf("SmsStorageUser2=%d\n", storageConfig.user2);
      printf("SmsStorageTotal2=%d\n", storageConfig.total2);
      printf("SmsStorageMem3=%s\n", storageConfig.mem3);
      printf("SmsStorageUser3=%d\n", storageConfig.user3);
      printf("SmsStorageTotal3=%d\n", storageConfig.total3);
  }
  SmsStorageConfigFree(&storageConfig);
}

static void PrintSmsEventReportingConfig(tPrintFormat format)
{
  gint result;
  tSmsReportingConfig reportingConfig;
  SmsEventReportingConfigInit(&reportingConfig);
  result = GetSmsEventReporting(&reportingConfig);
  AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);

  switch (format)
  {
    case FMT_JSON: 
      printf("{");
      printf("\"SmsEventReportingMode\": %d", reportingConfig.mode);
      printf(", ");
      printf("\"SmsEventReportingMT\": %d", reportingConfig.mt);
      printf(", ");
      printf("\"SmsEventReportingBM\": %d", reportingConfig.bm);
      printf(", ");
      printf("\"SmsEventReportingDS\": %d", reportingConfig.ds);
      printf(", ");
      printf("\"SmsStorageBFR\": %d", reportingConfig.bfr);
      printf("}");
      break;
    case FMT_DEFAULT:
      /*no break*/
    default:
      printf("SmsEventReportingMode=%d\n", reportingConfig.mode);
      printf("SmsEventReportingMT=%d\n", reportingConfig.mt);
      printf("SmsEventReportingBM=%d\n", reportingConfig.bm);
      printf("SmsEventReportingDS=%d\n", reportingConfig.ds);
      printf("SmsEventReportingBFR=%d\n", reportingConfig.bfr);
  }
}

static void PrintSimParams(tPrintFormat format)
{
  gint state;
  GString *statename;
  gint attempts;

  gint result = GetSimState(&state, &attempts);
  AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);

  statename = g_string_new(NULL);
  MapIntToString(_mapSimState, state, statename);
  switch (format)
  {
    case FMT_JSON:
      printf("{");
      printf("\"SimState\": \"%s\"", statename->str);
      printf(", ");
      printf("\"SimAttempts\": %d", attempts);
      printf("}");
      break;
    case FMT_DEFAULT:
      /*no break*/
    default:
      printf("SimState=%s\n", statename->str);
      printf("SimAttempts=%d\n", attempts);
  }
  g_string_free(statename, TRUE);
}

static void PrintPortParams(tPrintFormat format)
{
  gint state;
  gint open;
  GString *statename;
  GString *openstr;

  gint result = GetPortState(&state, &open);
  AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);

  statename = g_string_new(NULL);
  openstr = g_string_new(NULL);
  
  MapIntToString(_mapState, state, statename);
  MapIntToString(_mapBoolean, open, openstr);

  switch (format)
  {
    case FMT_JSON:
      printf("{");
      printf("\"PortState\": \"%s\"", statename->str);
      printf(", ");
      printf("\"PortOpen\": \"%s\"", openstr->str);
      printf("}");
      break;
    case FMT_DEFAULT:
      /*no break*/
    default:
      printf("PortState=%s\n", statename->str);
      printf("PortOpen=%s\n", openstr->str);
  }
  g_string_free(statename, TRUE);
  g_string_free(openstr, TRUE);
}

static void PrintLogging(tPrintFormat format)
{
  gint loglevel;
  GString *loglevelname;

  gint result = GetLogLevel(&loglevel);
  AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);

  loglevelname = g_string_new(NULL);
  MapIntToString(_mapLoglevel, loglevel, loglevelname);

  switch (format)
  {
    case FMT_JSON:
      printf("{");
      printf("\"LogLevel\": \"%s\"", loglevelname->str);
      printf("}");
      break;
    case FMT_DEFAULT:
      /*no break*/
    default:
      printf("LogLevel=%s\n", loglevelname->str);
  }
  g_string_free(loglevelname, TRUE);
}


static void PrintVersion()
{
  gchar *version = NULL;

  gint result = GetVersion(&version);
  AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);

  printf("MdmdVersion=");
  if (version != NULL)
  {
    printf("%s\n", version);
    g_free(version);
  }
  else
  {
    printf("unknown\n");
  }
}

/**
 * This function checks the compatibility between the PFC firmware and the modem firmware.
 * For this purpose the function compares the required version form the modem-version file with the version,
 * that the function gets from the mdmd daemon.
 */
static void PrintCheck()
{
  bool compareResult = false;

  gchar *manufacturer = NULL;
  gchar *type = NULL;
  gchar *currentModemFirmware = NULL;

  FILE * fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  char expectedModemFirmware[MODEM_VERSION_FROM_FILE_LENGTH];

  gint result = GetModemInfo(&manufacturer, &type, &currentModemFirmware);
  AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);

  fp = fopen("/etc/specific/modem_version", "r");
  AssertCondition(fp != NULL, SYSTEM_CALL_ERROR, "File /etc/specific/modem-version could not be opened");

  //Compare each line, more specifically each firmware version, in the modem-version file, with the current modem firmware
  while ((read = getline(&line, &len, fp)) != -1)
  {
    AssertCondition(read = MODEM_VERSION_FROM_FILE_LENGTH, SYSTEM_CALL_ERROR, "Buffer to small");

    //The firmware version from the modem-version file is split by a comma, that must remove before comparing
    strncpy(expectedModemFirmware,
            line,
            MODEM_VERSION_FIRST_PART_LENGTH);
    strncpy(expectedModemFirmware + MODEM_VERSION_FIRST_PART_LENGTH,
            line + MODEM_VERSION_FIRST_PART_LENGTH + sizeof(char),
            MODEM_VERSION_SECOND_PART_LENGTH);

    //The length of the firmware versions from the both sources is not equal.
    //The version from the modem-version file is only a mask for the supported versions
    if (strncmp(currentModemFirmware, expectedModemFirmware, MODEM_VERSION_COMPARE_LENGTH) == 0)
    {
      compareResult = true;
      break;
    }
  }

  if (true == compareResult)
  {
    printf("Firmware compatibility=compatible\n");
  }
  else
  {
    printf("Firmware compatibility=incompatible\n");
    log_EVENT_LogId(DIAG_3GMM_ERR_MODEM_VERSION, true);
  }

  if (NULL != line)
  {
    free(line);
  }
  fclose(fp);

  if (manufacturer != NULL)
  {
    g_free(manufacturer);
  }
  if (type != NULL)
  {
    g_free(type);
  }
  if (currentModemFirmware != NULL)
  {
    g_free(currentModemFirmware);
  }
}

static void ChangeNetworkParams(tNameValuePair *nvpairs, gint nparams)
{
  gint result;
  gint i;
  gint id;
  gint act;
  gint selmode;
  tNameValuePair *networkId = NULL;
  tNameValuePair *networkType = NULL;
  tNameValuePair *networkSelection = NULL;
  for (i = 0; i < nparams; i++)
  {
    if(0 == strcmp(nvpairs[i].name, "NetworkId"))
    {
      networkId = &nvpairs[i];
    }
    else if(0 == strcmp(nvpairs[i].name, "NetworkType"))
    {
      networkType = &nvpairs[i];
    }
    else if(0 == strcmp(nvpairs[i].name, "NetworkSelection"))
    {
      networkSelection = &nvpairs[i];
    }
  }

  AssertCondition(networkSelection != NULL, MISSING_PARAMETER, "NetworkSelection");
  MapStringToInt(_mapSelectMode, networkSelection->value, &selmode);
  AssertCondition(selmode != -1, INVALID_PARAMETER, "NetworkSelection");
  if (selmode == 1 /*MANUAL*/)
  {
    AssertCondition(networkId != NULL, MISSING_PARAMETER, "NetworkId");  
    AssertCondition(networkType != NULL, MISSING_PARAMETER, "NetworkType");  
    id = (gint)g_ascii_strtoll(networkId->value, NULL, 0);
    MapStringToInt(_mapOperAct, networkType->value, &act);
    AssertCondition(act != -1, INVALID_PARAMETER, "NetworkType");
  }
  else
  {
    id = 0;
    act = 0;
  }
  result = SetOper(selmode, id, act);
  AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);
}

static void ChangeGprsAccessParams(tNameValuePair *nvpairs, gint nparams)
{
  gint result;
  gint i;
  tMdmGprsAccess gprsAccess;
  gint auth;
  tNameValuePair *gprsAccessPointName = NULL;
  tNameValuePair *gprsAuthType = NULL;
  tNameValuePair *gprsUserName = NULL;
  tNameValuePair *gprsPassword = NULL;
  tNameValuePair *gprsConnectivity = NULL;
  const gchar  *apn;
  const gchar  *user;
  const gchar  *pass;
  gint connectivity;

  for (i = 0; i < nparams; i++)
  {
    if(0 == strcmp(nvpairs[i].name, "GprsAccessPointName"))
    {
      gprsAccessPointName = &nvpairs[i];
    }
    else if(0 == strcmp(nvpairs[i].name, "GprsAuthType"))
    {
      gprsAuthType = &nvpairs[i];
    }
    else if(0 == strcmp(nvpairs[i].name, "GprsUserName"))
    {
      gprsUserName = &nvpairs[i];
    }
    else if(0 == strcmp(nvpairs[i].name, "GprsPassword"))
    {
      gprsPassword = &nvpairs[i];
    }
    else if(0 == strcmp(nvpairs[i].name, "GprsConnectivity"))
    {
      gprsConnectivity = &nvpairs[i];
    }
  }

  //no check for mandatory parameters here, all GPRS parameters are optional and missing parameters keep unchanged

  //get current parameter

  MdmGprsAccessInit(&gprsAccess);
  result = GetGprsAccess(&gprsAccess);
  AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);

  apn = (gprsAccessPointName != NULL) ? gprsAccessPointName->value : gprsAccess.apn;
  if (gprsAuthType != NULL)
  {
    MapStringToInt(_mapAuthType, gprsAuthType->value, &auth);
    AssertCondition(auth != -1, INVALID_PARAMETER, "GprsAuthType");
  }
  else
  {
    auth = gprsAccess.auth;
  }
  user = (gprsUserName != NULL) ? gprsUserName->value : gprsAccess.user;
  pass = (gprsPassword != NULL) ? gprsPassword->value : gprsAccess.pass;
  if (gprsConnectivity != NULL)
  {
    MapStringToInt(_mapState, gprsConnectivity->value, &connectivity);
    AssertCondition(auth != -1, INVALID_PARAMETER, "GprsConnectivity");
  }
  else
  {
    connectivity = gprsAccess.connectivity;
  }

  if(    (strcmp(gprsAccess.apn, apn) != 0)
      || (gprsAccess.auth != auth)
      || (strcmp(gprsAccess.user, user) != 0)
      || (strcmp(gprsAccess.pass, pass) != 0)
      || (gprsAccess.connectivity != connectivity))
  {
    result = SetGprsAccess( apn, auth, user, pass, connectivity);
    AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);
  }
}

static void ChangeSimParams(tNameValuePair *nvpairs, gint nparams)
{
  gint result;
  gint i;
  tNameValuePair *simpin = NULL;
  tNameValuePair *simpuk = NULL;
  for (i = 0; i < nparams; i++)
  {
    if(0 == strcmp(nvpairs[i].name, "SimPin"))
    {
      simpin = &nvpairs[i];
    }
    else if(0 == strcmp(nvpairs[i].name, "SimPuk"))
    {
      simpuk = &nvpairs[i];
    }
  }

  AssertCondition(simpin != NULL, MISSING_PARAMETER, "SimPin");
  if (simpuk != NULL)
  {
    result = SetSimPuk(simpuk->value, simpin->value);
  }
  else
  {
    result = SetSimPin(simpin->value);
  }
  AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);
}

static void ChangeSmsStorageConfig(tNameValuePair *nvpairs, gint nparams)
{
  gint result;
  gint i;
  tSmsStorageConfig storageConfig;

  tNameValuePair *smsStorageMem1 = NULL;
  tNameValuePair *smsStorageMem2 = NULL;
  tNameValuePair *smsStorageMem3 = NULL;

  for (i = 0; i < nparams; i++)
  {
    if(0 == strcmp(nvpairs[i].name, "SmsStorageMem1"))
    {
      smsStorageMem1 = &nvpairs[i];
    }
    else if(0 == strcmp(nvpairs[i].name, "SmsStorageMem2"))
    {
      smsStorageMem2 = &nvpairs[i];
    }
    else if(0 == strcmp(nvpairs[i].name, "SmsStorageMem3"))
    {
      smsStorageMem3 = &nvpairs[i];
    }
  }

  // The configuration of each memory storage parameter is optional.
  // If some sms storage memory parameter are not set, the last configuration of the modem is used for this parameters.  
  SmsStorageConfigInit(&storageConfig);
  result = GetSmsStorageConfig(&storageConfig);
  AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);

  const gchar* mem1 = (smsStorageMem1 != NULL) ? smsStorageMem1->value : storageConfig.mem1;
  const gchar* mem2 = (smsStorageMem2 != NULL) ? smsStorageMem2->value : storageConfig.mem2;
  const gchar* mem3 = (smsStorageMem3 != NULL) ? smsStorageMem3->value : storageConfig.mem3;

  //Only set the sms storage memory parameters, if any parameter changed
  if(    (strcmp(storageConfig.mem1, mem1) != 0)
      || (strcmp(storageConfig.mem2, mem2) != 0)
      || (strcmp(storageConfig.mem3, mem3) != 0))
  {
    result = SetSmsStorageConfig(mem1, mem2, mem3);
    AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);
  }
}

static void ChangeSmsEventReporting(tNameValuePair *nvpairs, gint nparams)
{
  gint result;
  gint i;
  tSmsReportingConfig reportingConfig;

  tNameValuePair *smsEventReportingMode = NULL;
  tNameValuePair *smsEventReportingMT = NULL;
  tNameValuePair *smsEventReportingBM = NULL;
  tNameValuePair *smsEventReportingDS = NULL;
  tNameValuePair *smsEventReportingBFR = NULL;

  for (i = 0; i < nparams; i++)
  {
    if(0 == strcmp(nvpairs[i].name, "SmsEventReportingMode"))
    {
      smsEventReportingMode = &nvpairs[i];
    }
    else if(0 == strcmp(nvpairs[i].name, "SmsEventReportingMT"))
    {
      smsEventReportingMT = &nvpairs[i];
    }
    else if(0 == strcmp(nvpairs[i].name, "SmsEventReportingBM"))
    {
      smsEventReportingBM = &nvpairs[i];
    }
    else if(0 == strcmp(nvpairs[i].name, "SmsEventReportingDS"))
    {
      smsEventReportingDS = &nvpairs[i];
    }
    else if(0 == strcmp(nvpairs[i].name, "SmsEventReportingBFR"))
    {
      smsEventReportingBFR = &nvpairs[i];
    }
  }

  // The configuration of each sms event reporting parameter is optional.
  // If some sms event reporting parameter are not set, the last configuration of the modem is used for this parameters.  
  SmsEventReportingConfigInit(&reportingConfig);
  result = GetSmsEventReporting(&reportingConfig);
  AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);

  const gint mode = (smsEventReportingMode != NULL) ? atoi(smsEventReportingMode->value) : reportingConfig.mode;
  const gint mt = (smsEventReportingMT != NULL) ? atoi(smsEventReportingMT->value) : reportingConfig.mt;
  const gint bm = (smsEventReportingBM != NULL) ? atoi(smsEventReportingBM->value) : reportingConfig.bm;
  const gint ds = (smsEventReportingDS != NULL) ? atoi(smsEventReportingDS->value) : reportingConfig.ds;
  const gint bfr = (smsEventReportingBFR != NULL) ? atoi(smsEventReportingBFR->value) : reportingConfig.bfr;

  //Only set the sms event reporting parameters, if any parameter changed
  if(    (mode != reportingConfig.mode)
      || (mt != reportingConfig.mt)
      || (bm != reportingConfig.bm)
      || (ds != reportingConfig.ds)
      || (bfr != reportingConfig.bfr))
  {
    result = SetSmsEventReportingConfig(mode, mt, bm, ds, bfr);
    AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);
  }
}

static void TriggerModemReset(void)
{
  gint result = ModemReset();
  AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);
}

static void TriggerCompleteReset(void)
{
  TriggerModemReset();
  char const szCommand[] = "/etc/init.d/mdmd restart";
  int const result = system(szCommand);
  AssertCondition(result == 0, SYSTEM_CALL_ERROR, "Failed to restart mdmd environment");
}

static void ChangePortParams(tNameValuePair *nvpairs, gint nparams)
{
  gint state;
  gint result;
  gint i;
  tNameValuePair *statename = NULL;
  for (i = 0; i < nparams; i++)
  {
    if(0 == strcmp(nvpairs[i].name, "PortState"))
    {
      statename = &nvpairs[i];
    }
  }
  AssertCondition(statename != NULL, MISSING_PARAMETER, "PortState");

  MapStringToInt(_mapState, statename->value, &state);
  AssertCondition(state != -1, INVALID_PARAMETER, "PortState");

  result = SetPortState(state);
  AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);
}


static void ChangeLogging(tNameValuePair *nvpairs, gint nparams)
{
  gint i;
  gint result;
  gint loglevel;
  tNameValuePair *loglevelparam = NULL;
  for (i = 0; i < nparams; i++)
  {
    if(0 == strcmp(nvpairs[i].name, "LogLevel"))
    {
      loglevelparam = &nvpairs[i];
    }
  }
  if(loglevelparam != NULL)
  {
    MapStringToInt(_mapLoglevel, loglevelparam->value, &loglevel);
    AssertCondition(loglevel != -1, INVALID_PARAMETER, "LogLevel");
    result = SetLogLevel(loglevel);
    AssertCondition(result == 0, SYSTEM_CALL_ERROR, _mdmDBusClient.lastError);
  }
}


int main(int argc, char **argv)
{
  tCommand command;
  tOption option;
  gint nparams;
  char * const szDeviceConfig = typelabel_QUICK_GetValue("DEVCONF");
  gint64 deviceConfig;
  gint64 maskModemDevice = 0x0010; //Bit 5 in device configuration indicates modem device

#if !GLIB_CHECK_VERSION(2,36,0)
    /* g_type_init has been deprecated since version 2.36 and should not be used in newly-written code.
       The type system is now initialised automatically. */
    g_type_init ();
#endif
  _programmName = g_strdup(basename(argv[0]));

  AssertCondition((szDeviceConfig != NULL), SYSTEM_CALL_ERROR, "Get Typelabel DEVCONF failed");
  deviceConfig = g_ascii_strtoll(szDeviceConfig, NULL, 0);
  typelabel_QUICK_FreeValue(szDeviceConfig);
  //AssertCondition(((deviceConfig & maskModemDevice) != 0), SYSTEM_CALL_ERROR, "No modem device");
  if ((deviceConfig & maskModemDevice) == 0)
  { //no modem device
    ExitProgramm(0);
  }

  if (argc <= 1)
  {
    printf(info_text);
    ExitProgramm(0);
  }

  MdmDBusClientInit(CT_DBUS_MDMD_NAME,
                    CT_DBUS_MDMD_PATH,
                    CT_DBUS_MDMD_INTERFACE,
                    190000 /*msec*/);

  log_EVENT_Init("config_mdmd");

  command = (tCommand)GetOption(argv[1], _commands);
  switch (command)
  {
    case CMD_GET:
      AssertCondition(argc > 2, MISSING_PARAMETER, "Missing get-option");
      option = (tOption)GetOption(argv[2], _commandOptions);
      switch(option)
      {
        case OPT_DEVICE:
          PrintDeviceParams(FMT_DEFAULT);
          break;
        case OPT_NETWORK:
          PrintNetworkParams(FMT_DEFAULT);
          break;
        case OPT_NETWORKLIST:
          PrintNetworkList(FMT_DEFAULT);
          break;
        case OPT_GPRSACCESS:
          PrintGprsAccessParams(FMT_DEFAULT);
          break;
        case OPT_SIM:
          PrintSimParams(FMT_DEFAULT);
          break;
        case OPT_PORT:
          PrintPortParams(FMT_DEFAULT);
          break;
        case OPT_IDENTITY:
          PrintIdentity(FMT_DEFAULT);
          break;
        case OPT_SIGNAL_QUALITY:
          PrintSignalQuality(FMT_DEFAULT);
          break;
        case OPT_SMS_STORAGE:
          PrintSmsStorageConfig(FMT_DEFAULT);
          break;
        case OPT_SMS_EVENT_REPORTING:
          PrintSmsEventReportingConfig(FMT_DEFAULT);
          break;
        case OPT_MESSAGE_LIST:
          PrintMessageList(FMT_DEFAULT);
          break;
        case OPT_LOGGING:
          PrintLogging(FMT_DEFAULT);
          break;
        default:
          ExitWithError(INVALID_PARAMETER, "Illegal get-option");
      }
      break;
    case CMD_GET_JSON:
      AssertCondition(argc > 2, MISSING_PARAMETER, "Missing get-option");
      option = (tOption)GetOption(argv[2], _commandOptions);
      switch(option)
      {
        case OPT_DEVICE:
          PrintDeviceParams(FMT_JSON);
          break;
        case OPT_NETWORK:
          PrintNetworkParams(FMT_JSON);
          break;
        case OPT_NETWORKLIST:
          PrintNetworkList(FMT_JSON);
          break;
        case OPT_GPRSACCESS:
          PrintGprsAccessParams(FMT_JSON);
          break;
        case OPT_SIM:
          PrintSimParams(FMT_JSON);
          break;
        case OPT_PORT:
          PrintPortParams(FMT_JSON);
          break;
        case OPT_IDENTITY:
          PrintIdentity(FMT_JSON);
          break;
        case OPT_SIGNAL_QUALITY:
          PrintSignalQuality(FMT_JSON);
          break;
        case OPT_SMS_STORAGE:
          PrintSmsStorageConfig(FMT_JSON);
          break;
        case OPT_SMS_EVENT_REPORTING:
          PrintSmsEventReportingConfig(FMT_JSON);
          break;
        case OPT_MESSAGE_LIST:
          PrintMessageList(FMT_JSON);
          break;
        case OPT_LOGGING:
          PrintLogging(FMT_JSON);
          break;
        default:
          ExitWithError(INVALID_PARAMETER, "Illegal get-option");
      }
      break;
    case CMD_SET:
    {
      tNameValuePair *nvpairs;
      AssertCondition(argc > 2, MISSING_PARAMETER, "Missing set-option");
      option = (tOption)GetOption(argv[2], _commandOptions);
      nparams = argc-3;
      nvpairs = (tNameValuePair*)calloc(nparams, sizeof(tNameValuePair));
      AssertCondition(nvpairs != NULL, SYSTEM_CALL_ERROR, "No memory for parameter check");
      GetCommandParams(nparams, &argv[3], nvpairs);
      switch(option)
      {
        case OPT_NETWORK:
          ChangeNetworkParams(nvpairs, nparams);
          break;
        case OPT_GPRSACCESS:
          ChangeGprsAccessParams(nvpairs, nparams);
          break;
        case OPT_SIM:
          ChangeSimParams(nvpairs, nparams);
          break;
        case OPT_PORT:
          ChangePortParams(nvpairs, nparams);
          break;
        case OPT_SMS_STORAGE:
          ChangeSmsStorageConfig(nvpairs, nparams);
          break;
        case OPT_SMS_EVENT_REPORTING:
          ChangeSmsEventReporting(nvpairs, nparams);
          break;
        case OPT_LOGGING:
          ChangeLogging(nvpairs, nparams);
          break;
        default:
          ExitWithError(INVALID_PARAMETER, "Illegal set-option");
      }
      free(nvpairs);
    } break;
    case CMD_RESET:
      TriggerModemReset();
      break;
    case CMD_RESETALL:
      TriggerCompleteReset();
      break;
    case CMD_HELP:
      printf(usage_text);
      break;
    case CMD_VERSION:
      PrintVersion();
      break;
    case CMD_CHECK:
      PrintCheck();
      break;
    default:
      ExitWithError(INVALID_PARAMETER, "Illegal command");
  }
  ExitProgramm(0);
}

