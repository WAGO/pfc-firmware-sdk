//------------------------------------------------------------------------------
/// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
/*
 * icm_device_description.h
 *
 *  Created on: 28.11.2012
 *      Author: u010716
 */

#ifndef DEVICE_DESCRIPTION_H_
#define DEVICE_DESCRIPTION_H_

#include <profibus_master.h>



#define ICM_MAX_DEVICE_NAME_SIZE  50

/* device specific definition */


#define PROFIBUS_SLAVE            1
#define PROFIBUS_MASTER           2
#define CANOPEN_SLAVE             3
#define CANOPEN_MASTER            4
#define CANLAYER2                 5
#define MODBUS                    6
#define ETHERNET_IP               7
#define KBUS                      8

typedef struct stDeviceName
{
  const char *description;
  uint8_t item;
  uint8_t open;
  uint8_t wlock;
  uint8_t rlock;
}tDeviceType;


typedef struct stDevice
{
  tDeviceType type[20];
  size_t NumberOfDevices;
  tDeviceInfo info[20];
}tDevice;



tDevice *device_GetTypeInterface(void);
void device_InitSystem(void);
int32_t device_getIdFromDeviceName(const char* deviceName);





#endif /* ICM_DEVICEDEFINITION_H_ */
