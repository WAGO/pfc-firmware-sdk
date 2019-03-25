//------------------------------------------------------------------------------
/// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     icm_device_description.C
///
///  \version  $Revision: 17216 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <stdbool.h>
#include <adi_application_interface.h>
#include <menu.h>
#include <sdi.h>

#include "device_type.h"


//------------------------------------------------------------------------------
// Local macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// External variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local typedefs
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Local variables
//------------------------------------------------------------------------------
tDevice device;




//-- Function: icm_GetDeviceTypeInterface --------------------------------------------------------------------
///
///  Sets description interface of the devices.
///
///  \param  none
///
///  \return  Pointer to the interface
///
//------------------------------------------------------------------------------------------

tDevice *device_GetTypeInterface(void)
{
   return &device;
}


void device_InitSystem(void)
{
  uint32_t i;

  for(i = 0; i < device.NumberOfDevices; i++)
  {
    if(strncmp(device.info[i].DeviceName,"libdps",strlen(device.info[i].DeviceName)) == ZERO)
    {
      device.type[i].description =  "PROFIBUS_SLAVE";
      device.type[i].item =  PROFIBUS_SLAVE;
    }
    if(strncmp(device.info[i].DeviceName,"libpbdpm",strlen(device.info[i].DeviceName)) == ZERO)
    {
      device.type[i].description =  "PROFIBUS_MASTER";
      device.type[i].item =  PROFIBUS_MASTER;
    }
    else if(strncmp(device.info[i].DeviceName,"libpackbus",strlen(device.info[i].DeviceName)) == ZERO)
    {
      device.type[i].description =  "KBUS";
      device.type[i].item  = KBUS;
    }
    else if(strncmp(device.info[i].DeviceName,"libmbs",strlen(device.info[i].DeviceName)) == ZERO)
    {
      device.type[i].description =  "MODBUS";
      device.type[i].item =  MODBUS;
    }
    else if(strncmp(device.info[i].DeviceName,"libcanopenm",strlen(device.info[i].DeviceName)) == ZERO)
    {
      device.type[i].description =  "CANOPEN_MASTER";
      device.type[i].item = CANOPEN_MASTER;
    }
    else if(strncmp(device.info[i].DeviceName,"libcanopens",strlen(device.info[i].DeviceName)) == ZERO)
    {
      device.type[i].description =  "CANOPEN_SLAVE";
      device.type[i].item = CANOPEN_SLAVE;
    }
    else if(strncmp(device.info[i].DeviceName,"libcanlayer2",strlen(device.info[i].DeviceName)) == ZERO)
    {
      device.type[i].description =  "CANLAYER2";
      device.type[i].item = CANLAYER2;
    }
  }

}

int32_t device_getIdFromDeviceName(const char* deviceName)
{
  tDevice *device = device_GetTypeInterface();
  size_t i;

  for ( i = 0; i < device->NumberOfDevices; i++)
  {
    if(strncmp(device->info[i].DeviceName,deviceName,strlen(deviceName)) == ZERO)
    {
      return device->info[i].DeviceId;
    }
  }
  return -1;
}


//---- End of source file ------------------------------------------------------
