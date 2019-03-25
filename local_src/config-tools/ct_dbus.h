//------------------------------------------------------------------------------
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     ct_dbus.h
///
///  \version  $Revision: 18092 $1
///
///  \brief    <short description of the file contents>
///
///  \author   <u015614> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef CT_DBUS_H_
#define CT_DBUS_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
#define CT_DBUS_METHOD_POSTWRITE          "postwrite"
#define CT_DBUS_METHOD_STATISTICS         "statistics"

#define CT_DBUS_NAME_MODBUS               "wago.modbus"

#define CT_DBUS_PATH_MODBUS               "/wago/modbus"
#define CT_DBUS_PATH_MODBUS_TCP_ENABLED   "/wago/modbus/tcp/enabled"
#define CT_DBUS_PATH_MODBUS_TCP_PORT      "/wago/modbus/tcp/port"
#define CT_DBUS_PATH_MODBUS_UDP_ENABLED   "/wago/modbus/udp/enabled"
#define CT_DBUS_PATH_MODBUS_UDP_PORT      "/wago/modbus/udp/port"
#define CT_DBUS_PATH_MODBUS_STAT          "/wago/modbus/stat"

#define CT_DBUS_MDMD_NAME                 "de.wago.mdmd"
#define CT_DBUS_MDMD_PATH                 "/de/wago/mdmd"
#define CT_DBUS_MDMD_INTERFACE            "de.wago.mdmd"

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------


#endif /* CT_DBUS_H_ */
