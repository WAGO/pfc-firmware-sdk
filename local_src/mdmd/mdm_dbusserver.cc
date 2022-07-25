// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG


#include "mdm_dbusserver.h"
#include "mdm_statemachine.h"
#include <sys/time.h>
#include "mdmd_log.h"
#include "gerror_exception.h"

static const gchar introspection_xml[] =
"<node>"
"  <interface name='de.wago.mdmd'>"
"    <method name='GetModemInfo'>"
"      <arg type='s' name='manufacturer' direction='out'/>"
"      <arg type='s' name='model' direction='out'/>"
"      <arg type='s' name='revision' direction='out'/>"
"    </method>"
"    <method name='GetModemIdentity'>"
"      <arg type='s' name='imei' direction='out'/>"
"    </method>"
"    <method name='GetSimState'>"
"      <arg type='i' name='state' direction='out'/>"
"      <arg type='i' name='attempts' direction='out'/>"
"    </method>"
"    <method name='SetSimPin'>"
"      <arg type='s' name='pin' direction='in'/>"
"    </method>"
"    <method name='SetSimPuk'>"
"      <arg type='s' name='puk' direction='in'/>"
"      <arg type='s' name='pin' direction='in'/>"
"    </method>"
"    <method name='GetSimAutoActivation'>"
"      <arg type='s' name='iccid' direction='out'/>"
"      <arg type='s' name='pin' direction='out'/>"
"    </method>"
"    <method name='SetSimAutoActivation'>"
"      <arg type='s' name='iccid' direction='in'/>"
"      <arg type='s' name='pin' direction='in'/>"
"    </method>"
"    <method name='GetOperState'>"
"      <arg type='i' name='regstate' direction='out'/>"
"      <arg type='i' name='selmode' direction='out'/>"
"      <arg type='i' name='operid' direction='out'/>"
"      <arg type='i' name='act' direction='out'/>"
"      <arg type='i' name='quality' direction='out'/>"
"      <arg type='s' name='name' direction='out'/>"
"      <arg type='s' name='lac' direction='out'/>"
"      <arg type='s' name='cid' direction='out'/>"
"    </method>"
"    <method name='GetSignalQuality'>"
"      <arg type='i' name='rssi' direction='out'/>"
"      <arg type='i' name='ber' direction='out'/>"
"    </method>"
"    <method name='GetOperList'>"
"      <arg type='a(iiiis)' name='oper_list' direction='out'/>"
"    </method>"
"    <method name='SetOper'>"
"      <arg type='i' name='selmode' direction='in'/>"
"      <arg type='i' name='operid' direction='in'/>"
"      <arg type='i' name='act' direction='in'/>"
"    </method>"
"    <method name='GetGprsAccess'>"
"      <arg type='i' name='regstate' direction='out'/>"
"      <arg type='s' name='apn' direction='out'/>"
"      <arg type='i' name='auth' direction='out'/>"
"      <arg type='s' name='user' direction='out'/>"
"      <arg type='s' name='pass' direction='out'/>"
"    </method>"
"    <method name='GetGprsAccess2'>"
"      <arg type='i' name='regstate' direction='out'/>"
"      <arg type='s' name='apn' direction='out'/>"
"      <arg type='i' name='auth' direction='out'/>"
"      <arg type='s' name='user' direction='out'/>"
"      <arg type='s' name='pass' direction='out'/>"
"      <arg type='i' name='connectivity' direction='out'/>"
"      <arg type='s' name='pdpaddr' direction='out'/>"
"    </method>"
"    <method name='SetGprsAccess'>"
"      <arg type='s' name='apn' direction='in'/>"
"      <arg type='i' name='auth' direction='in'/>"
"      <arg type='s' name='user' direction='in'/>"
"      <arg type='s' name='pass' direction='in'/>"
"    </method>"
"    <method name='SetGprsAccess2'>"
"      <arg type='s' name='apn' direction='in'/>"
"      <arg type='i' name='auth' direction='in'/>"
"      <arg type='s' name='user' direction='in'/>"
"      <arg type='s' name='pass' direction='in'/>"
"      <arg type='i' name='connectivity' direction='in'/>"
"    </method>"
"    <method name='ModemReset'>"
"    </method>"
"    <method name='GetPortState'>"
"      <arg type='i' name='state' direction='out'/>"
"    </method>"
"    <method name='GetPortState2'>"
"      <arg type='i' name='state' direction='out'/>"
"      <arg type='i' name='open' direction='out'/>"
"    </method>"
"    <method name='SetPortState'>"
"      <arg type='i' name='state' direction='in'/>"
"    </method>"
"    <method name='GetSmsFormat'>"
"      <arg type='i' name='mode' direction='out'/>"
"    </method>"
"    <method name='SetSmsFormat'>"
"      <arg type='i' name='mode' direction='in'/>"
"    </method>"
"    <method name='GetSmsReportConfig'>"
"      <arg type='i' name='mode' direction='out'/>"
"      <arg type='i' name='mt' direction='out'/>"
"      <arg type='i' name='bm' direction='out'/>"
"      <arg type='i' name='ds' direction='out'/>"
"      <arg type='i' name='bfr' direction='out'/>"
"    </method>"
"    <method name='SetSmsReportConfig'>"
"      <arg type='i' name='mode' direction='in'/>"
"      <arg type='i' name='mt' direction='in'/>"
"      <arg type='i' name='bm' direction='in'/>"
"      <arg type='i' name='ds' direction='in'/>"
"      <arg type='i' name='bfr' direction='in'/>"
"    </method>"
"    <method name='GetSmsStorage'>"
"      <arg type='s' name='mem1' direction='out'/>"
"      <arg type='i' name='used1' direction='out'/>"
"      <arg type='i' name='total1' direction='out'/>"
"      <arg type='s' name='mem2' direction='out'/>"
"      <arg type='i' name='used2' direction='out'/>"
"      <arg type='i' name='total2' direction='out'/>"
"      <arg type='s' name='mem3' direction='out'/>"
"      <arg type='i' name='used3' direction='out'/>"
"      <arg type='i' name='total3' direction='out'/>"
"    </method>"
"    <method name='SetSmsStorage'>"
"      <arg type='s' name='mem1' direction='in'/>"
"      <arg type='s' name='mem2' direction='in'/>"
"      <arg type='s' name='mem3' direction='in'/>"
"    </method>"
"    <method name='ListSms'>"
"      <arg type='i' name='state' direction='in'/>"
"      <arg type='a(iiis)' name='sms_list' direction='out'/>"
"    </method>"
"    <method name='ReadSms'>"
"      <arg type='i' name='index' direction='in'/>"
"      <arg type='i' name='state' direction='out'/>"
"      <arg type='i' name='length' direction='out'/>"
"      <arg type='s' name='pdu' direction='out'/>"
"    </method>"
"    <method name='DeleteSms'>"
"      <arg type='i' name='index' direction='in'/>"
"      <arg type='i' name='delflag' direction='in'/>"
"    </method>"
"    <method name='SendSms'>"
"      <arg type='i' name='length' direction='in'/>"
"      <arg type='s' name='pdu' direction='in'/>"
"      <arg type='i' name='msgref' direction='out'/>"
"    </method>"
"    <method name='GetVersion'>"
"      <arg type='s' name='version' direction='out'/>"
"    </method>"
"    <method name='GetLogLevel'>"
"      <arg type='i' name='loglevel' direction='out'/>"
"    </method>"
"    <method name='SetLogLevel'>"
"      <arg type='i' name='loglevel' direction='in'/>"
"    </method>"
"  </interface>"
"</node>";

MdmDBusServer::MdmDBusServer(MdmStatemachine *statemachine)
    : DBusServer( "de.wago.mdmd", "/de/wago/mdmd", introspection_xml )
    , _statemachine(statemachine)
{
    mdmd_Log(MD_LOG_INF, "DBus Server %s: created\n", _bus_name.c_str());
}

MdmDBusServer::~MdmDBusServer()
{ }

void
MdmDBusServer::method_call (MethodInvocation invocation)
{
    //mdmd_Log(MD_LOG_DBG, "method %s\n", invocation.method_name().c_str() );
    _statemachine->do_dbus(invocation, false);
}

void
MdmDBusServer::name_acquired(GDBusConnection *connection, const std::string &name)
{
    (void)connection; //unused parameter
    _online = true;
    mdmd_Log(MD_LOG_INF, "DBus Server %s: connected\n", name.c_str());
    _statemachine->do_string("DBus_Connection_up");
}

void
MdmDBusServer::name_lost(GDBusConnection *connection, const std::string &name)
{
    _online = false;
    if (connection) {
      _statemachine->do_string("DBus_Connection_down");
    }
    else {
      mdmd_Log(MD_LOG_WRN, "DBus Server %s: could not aquire dbus connection\n", name.c_str());
    }
}


void
MdmDBusServer::emit_diag_signal(unsigned int id, bool set, int intparam)
{
    char id_str[16];
    GVariant *params;
    struct timeval tv;

    if (!online()) {
      mdmd_Log(MD_LOG_WRN, "Failed to send diagnose signal 0x%08x: dbus offline\n", id);
      return;
    }

    gettimeofday( &tv, nullptr );

    params = g_variant_new( "(iisbi)", tv.tv_sec, tv.tv_usec, "mdmd", set, intparam );

    snprintf( id_str, sizeof(id_str), "ID%.8X", id );

    broadcast_signal("/wago/diagnose/ID", "wago.diagnose", id_str, params);
}
